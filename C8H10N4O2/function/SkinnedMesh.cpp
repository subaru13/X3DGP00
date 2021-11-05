#pragma warning(disable : 4099)
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <sstream>
#include <functional>
#include <filesystem>
#include <fstream>
#include <codecvt>
#include "SkinnedMesh.h"
#include "Misc.h"
#include "MyHandy.h"
#include "CreateComObjectHelpar.h"
#include "../FrameworkConfig.h"
using namespace DirectX;
using namespace fbxsdk;


std::u16string utf8_to_utf16(std::string const& src)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
	return converter.from_bytes(src);
}

inline XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
{
	XMFLOAT4X4 xmfloat4x4;
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
	return xmfloat4x4;
}
inline XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
{
	XMFLOAT3 xmfloat3;
	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
	return xmfloat3;
}
inline XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
{
	XMFLOAT4 xmfloat4;
	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
	return xmfloat4;
}

struct bone_influence
{
	uint32_t bone_index;
	float bone_weight;
};
using bone_influences_per_control_point = std::vector<bone_influence>;

void fetch_bone_influences(const FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& bone_influences)
{
	const int control_points_count{ fbx_mesh->GetControlPointsCount() };
	bone_influences.resize(control_points_count);

	const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
	for (int skin_index = 0; skin_index < skin_count; ++skin_index)
	{
		const FbxSkin* fbx_skin
		{ static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index, FbxDeformer::eSkin)) };

		const int cluster_count{ fbx_skin->GetClusterCount() };
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };

			const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
			for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count;
				++control_point_indices_index)
			{
				int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
				double control_point_weight
				{ fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
				bone_influence& bone_influence{ bone_influences.at(control_point_index).emplace_back() };
				bone_influence.bone_index = static_cast<uint32_t>(cluster_index);
				bone_influence.bone_weight = static_cast<float>(control_point_weight);
			}
		}
	}
}

SkinnedMesh::SkinnedMesh(ID3D11Device* device,
	const char* fbx_filename,
	bool triangulate,
	float sampling_rate)
	:constant_buffer(device)
{
	assert(device && "The device is invalid.");

	std::filesystem::path cereal_filename(fbx_filename);
	cereal_filename.replace_extension("skn");
	if (std::filesystem::exists(cereal_filename))
	{
		std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(scene_view, meshes, materials, animation_clips);
	}
	else
	{
		assert(isExistFileA(fbx_filename) && "File does not exist.");
		FbxManager* fbx_manager{ FbxManager::Create() };
		FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };

		FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
		bool import_status{ false };
		import_status = fbx_importer->Initialize(fbx_filename);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		import_status = fbx_importer->Import(fbx_scene);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		FbxGeometryConverter fbx_converter(fbx_manager);
		if (triangulate)
		{
			fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
			fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
		}

		std::function<void(FbxNode*)> traverse
		{
			[&](FbxNode* fbx_node)
			{
				scene::node& node{ scene_view.nodes.emplace_back() };
				node.attribute = fbx_node->GetNodeAttribute() ?
					fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
				node.name = fbx_node->GetName();
				node.unique_id = fbx_node->GetUniqueID();
				node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
				for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
				{
					traverse(fbx_node->GetChild(child_index));
				}
			}
		};
		traverse(fbx_scene->GetRootNode());
		fetchMeshes(fbx_scene, meshes);
		fetchMaterials(fbx_scene, materials);
		std::string directory = std::filesystem::path(fbx_filename).filename().replace_extension("fbm").string();

		for (auto iterator = materials.begin(); iterator != materials.end(); ++iterator)
		{
			if (iterator->second.texture_filenames[0].size() > 0)
			{
				size_t offset = iterator->second.texture_filenames[0].find(directory);
				if (offset != std::string::npos)
				{
					std::string re_path = iterator->second.texture_filenames[0].substr(offset);
					iterator->second.texture_filenames[0] =
						std::filesystem::path(re_path).relative_path().string();
				}
			}
			if (iterator->second.texture_filenames[1].size() > 0)
			{
				size_t offset = iterator->second.texture_filenames[1].find(directory);
				if (offset != std::string::npos)
				{
					std::string re_path = iterator->second.texture_filenames[1].substr(offset);
					iterator->second.texture_filenames[1] =
						std::filesystem::path(re_path).relative_path().string();
				}
			}
		}
		fetchAnimations(fbx_scene, animation_clips, sampling_rate);

		fbx_manager->Destroy();

		std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization(scene_view, meshes, materials, animation_clips);
	}
	createComObjects(device, fbx_filename);
}

void SkinnedMesh::render(ID3D11DeviceContext* immediate_context,
	ID3D11PixelShader** external_pixel_shader,
	const FLOAT4X4& world,
	const Animation::keyframe* keyframe,
	const FLOAT4& material_color)
{
	assert(immediate_context && "The context is invalid.");
	for (mesh& mesh : meshes)
	{
		uint32_t stride{ sizeof(vertex) };
		uint32_t offset{ 0 };
		if (mesh.vertex_buffer == nullptr)continue;
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		if (mesh.index_buffer == nullptr)continue;
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		immediate_context->IASetInputLayout(input_layout.Get());
		immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
		if (external_pixel_shader)
		{
			immediate_context->PSSetShader((*external_pixel_shader), nullptr, 0);
		}
		else
		{
			immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
		}

		constants data;
		if (keyframe && keyframe->nodes.size() > 0)
		{
			const Animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));

			const size_t bone_count{ mesh.bind_pose.bones.size() };
			_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

			for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
				const Animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
				XMStoreFloat4x4(&data.bone_transforms[bone_index],
					XMLoadFloat4x4(&bone.offset_transform) *
					XMLoadFloat4x4(&bone_node.global_transform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
				);
			}
		}
		else
		{
			XMStoreFloat4x4(&data.world,
				XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				data.bone_transforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			}
		}
		for (const mesh::subset& subset : mesh.subsets)
		{
			const material& material{ materials.at(subset.material_unique_id) };
			XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));
			constant_buffer = data;
			constant_buffer.send(immediate_context, 0, true, true);

			immediate_context->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
			immediate_context->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());

			immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
		}
	}
}

void SkinnedMesh::updateKeyframe(Animation::keyframe& keyframe)const
{
	size_t node_count{ keyframe.nodes.size() };
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		Animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
		XMMATRIX S{ XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z) };
		XMMATRIX R{ XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation)) };
		XMMATRIX T{ XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };

		int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
		XMMATRIX P{ parent_index < 0 ? XMMatrixIdentity() :
		XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };

		XMStoreFloat4x4(&node.global_transform, S * R * T * P);
	}
}

bool SkinnedMesh::appendAnimations(const char* animation_filename, float sampling_rate)
{
	if (!isExistFileA(animation_filename))return false;
	FbxManager* fbx_manager{ FbxManager::Create() };
	FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };
	FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
	bool import_status{ false };
	import_status = fbx_importer->Initialize(animation_filename);
	_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
	import_status = fbx_importer->Import(fbx_scene);
	_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
	fetchAnimations(fbx_scene, animation_clips, sampling_rate);
	fbx_manager->Destroy();
	return true;
}

void SkinnedMesh::blendAnimations(const Animation::keyframe* keyframes[2], float factor, Animation::keyframe& keyframe)const
{
	size_t node_count{ keyframes[0]->nodes.size() };
	keyframe.nodes.resize(node_count);
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		XMVECTOR S[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(node_index).scaling),XMLoadFloat3(&keyframes[1]->nodes.at(node_index).scaling) };
		XMStoreFloat3(&keyframe.nodes.at(node_index).scaling, XMVectorLerp(S[0], S[1], factor));

		XMVECTOR R[2]{ XMLoadFloat4(&keyframes[0]->nodes.at(node_index).rotation),XMLoadFloat4(&keyframes[1]->nodes.at(node_index).rotation) };
		XMStoreFloat4(&keyframe.nodes.at(node_index).rotation, XMQuaternionSlerp(R[0], R[1], factor));

		XMVECTOR T[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(node_index).translation),XMLoadFloat3(&keyframes[1]->nodes.at(node_index).translation) };
		XMStoreFloat3(&keyframe.nodes.at(node_index).translation, XMVectorLerp(T[0], T[1], factor));
	}
	updateKeyframe(keyframe);
}

void SkinnedMesh::fetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
	for (const scene::node& node : scene_view.nodes)
	{
		if (node.attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}

		FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
		FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

		mesh& mesh{ meshes.emplace_back() };
		mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
		mesh.name = fbx_mesh->GetNode()->GetName();
		mesh.node_index = scene_view.indexof(mesh.unique_id);
		mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

		std::vector<bone_influences_per_control_point> bone_influences;
		fetch_bone_influences(fbx_mesh, bone_influences);
		fetchSkeleton(fbx_mesh, mesh.bind_pose);
		std::vector<mesh::subset>& subsets{ mesh.subsets };
		const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
		subsets.resize(material_count > 0 ? material_count : 1);
		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
			subsets.at(material_index).material_name = fbx_material->GetName();
			subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
		}
		if (material_count > 0)
		{
			const int polygon_count{ fbx_mesh->GetPolygonCount() };
			for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
			{
				const int material_index
				{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
				subsets.at(material_index).index_count += 3;
			}
			uint32_t offset{ 0 };
			for (mesh::subset& subset : subsets)
			{
				subset.start_index_location = offset;
				offset += subset.index_count;
				// This will be used as counter in the following procedures, reset to zero
				subset.index_count = 0;
			}
		}
		const int polygon_count{ fbx_mesh->GetPolygonCount() };
		mesh.vertices.resize(polygon_count * 3LL);
		mesh.indices.resize(polygon_count * 3LL);

		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);
		const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
		for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
		{
			const int material_index{ material_count > 0 ?
			fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
			mesh::subset& subset{ subsets.at(material_index) };
			const uint32_t offset{ subset.start_index_location + subset.index_count };

			for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
			{
				const int vertex_index{ polygon_index * 3 + position_in_polygon };

				vertex vertex;
				const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
				vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
				vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
				vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

				const bone_influences_per_control_point& influences_per_control_point{ bone_influences.at(polygon_vertex) };
				for (size_t influence_index = 0; influence_index < influences_per_control_point.size(); ++influence_index)
				{
					if (influence_index < MAX_BONE_INFLUENCES)
					{
						vertex.bone_weights[influence_index] = influences_per_control_point.at(influence_index).bone_weight;
						vertex.bone_indices[influence_index] = influences_per_control_point.at(influence_index).bone_index;
					}
				}

				if (fbx_mesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				if (fbx_mesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
						uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				if (fbx_mesh->GenerateTangentsData(0, false))
				{
					const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
				}

				mesh.vertices.at(vertex_index) = std::move(vertex);
				mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
				subset.index_count++;
			}
		}
	}
}

void SkinnedMesh::createComObjects(ID3D11Device* device, const char* fbx_filename)
{
	for (mesh& mesh : meshes)
	{
		if (mesh.vertices.empty())continue;
		HRESULT hr{ S_OK };
		D3D11_BUFFER_DESC buffer_desc{};
		D3D11_SUBRESOURCE_DATA subresource_data{};
		if (size_t size = mesh.vertices.size(); size > 0)
		{
			buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * size);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = mesh.vertices.data();
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;
			hr = device->CreateBuffer(&buffer_desc, &subresource_data,
				mesh.vertex_buffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		if (size_t size = mesh.indices.size(); size > 0)
		{
			buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * size);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			subresource_data.pSysMem = mesh.indices.data();
			hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.index_buffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
#if 1
		mesh.vertices.clear();
		mesh.indices.clear();
		mesh.vertices.shrink_to_fit();
		mesh.indices.shrink_to_fit();
#endif
	}

	HRESULT hr = S_OK;

	std::string cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "skinned_mesh_ps.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadPixelShader(device, cso_pass, pixel_shader.ReleaseAndGetAddressOf());
	}
	else
	{
		std::string ps =
			"struct VS_IN \n"
			"{ \n"
			"	float4 position : POSITION; \n"
			"	float4 normal : NORMAL; \n"
			"	float4 tangent : TANGENT; \n"
			"	float2 texcoord : TEXCOORD; \n"
			"	float4 bone_weights : WEIGHTS; \n"
			"	uint4 bone_indices : BONES; \n"
			"}; \n"
			"struct VS_OUT \n"
			"{ \n"
			"	float4 sv_position : SV_POSITION; \n"
			"	float4 position : POSITION; \n"
			"	float4 world_position : WORLD_POSITION; \n"
			"	float4 world_normal : NORMAL; \n"
			"	float4 world_tangent : TANGENT; \n"
			"	float2 texcoord : TEXCOORD; \n"
			"	float4 color : COLOR; \n"
			"}; \n"
			"static const int MAX_BONES = 256; \n"
			"cbuffer object_constant : register(b0) \n"
			"{ \n"
			"	row_major float4x4 world; \n"
			"	float4 material_color; \n"
			"	row_major float4x4 bone_transforms[MAX_BONES]; \n"
			"}; \n"
			"cbuffer Scene : register(b7) \n"
			"{\n"
			"	row_major float4x4 view_projection;\n"
			"	float4 light_direction;\n"
			"	float4 camera_position;\n"
			"};\n"
			"SamplerState samplerstate : register(s0);\n"
			"Texture2D texture_maps[4] : register(t0);\n"
			"float4 main(VS_OUT pin) : SV_TARGET\n"
			"{\n"
			"	float4 color = texture_maps[0].Sample(samplerstate, pin.texcoord);\n"
			"	float3 N = normalize(pin.world_normal.xyz);\n"
			"	float3 L = normalize(-light_direction.xyz);\n"
			"	float3 diffuse = color.rgb * max(max(0, light_direction.w), dot(N, L));\n"
			"	return float4(diffuse, color.a) * pin.color;\n"
			"}\n";
		hr = createPixelShader(device, ps, pixel_shader.ReleaseAndGetAddressOf());
	}

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TANGENT",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "WEIGHTS",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "BONES",		0,	DXGI_FORMAT_R32G32B32A32_UINT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
	};
	cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "skinned_mesh_vs.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadVertexShader(device, cso_pass, vertex_shader.ReleaseAndGetAddressOf(),
			input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	}
	else
	{
		std::string vs =
			"struct VS_IN \n"
			"{ \n"
			"	float4 position : POSITION; \n"
			"	float4 normal : NORMAL; \n"
			"	float4 tangent : TANGENT; \n"
			"	float2 texcoord : TEXCOORD; \n"
			"	float4 bone_weights : WEIGHTS; \n"
			"	uint4 bone_indices : BONES; \n"
			"}; \n"
			"struct VS_OUT \n"
			"{ \n"
			"	float4 sv_position : SV_POSITION; \n"
			"	float4 position : POSITION; \n"
			"	float4 world_position : WORLD_POSITION; \n"
			"	float4 world_normal : NORMAL; \n"
			"	float4 world_tangent : TANGENT; \n"
			"	float2 texcoord : TEXCOORD; \n"
			"	float4 color : COLOR; \n"
			"}; \n"
			"static const int MAX_BONES = 256; \n"
			"cbuffer object_constant : register(b0) \n"
			"{ \n"
			"	row_major float4x4 world; \n"
			"	float4 material_color; \n"
			"	row_major float4x4 bone_transforms[MAX_BONES]; \n"
			"}; \n"
			"cbuffer Scene : register(b7) \n"
			"{\n"
			"	row_major float4x4 view_projection;\n"
			"	float4 light_direction;\n"
			"	float4 camera_position;\n"
			"};\n"
			"VS_OUT main(VS_IN vin)\n"
			"{\n"
			"	float sigma = vin.tangent.w;\n"
			"	vin.tangent.w = 0;\n"
			"	vin.normal.w = 0;\n"
			"	float4 blended_position = { 0, 0, 0, 1 };\n"
			"	float4 blended_normal = { 0, 0, 0, 0 };\n"
			"	float4 blended_tangent = { 0, 0, 0, 0 };\n"
			"	for (int bone_index = 0; bone_index < 4; ++bone_index)\n"
			"	{\n"
			"		blended_position += vin.bone_weights[bone_index]\n"
			"			* mul(vin.position, bone_transforms[vin.bone_indices[bone_index]]);\n"
			"		blended_normal += vin.bone_weights[bone_index]\n"
			"			* mul(vin.normal, bone_transforms[vin.bone_indices[bone_index]]);\n"
			"		blended_tangent += vin.bone_weights[bone_index]\n"
			"			* mul(vin.tangent, bone_transforms[vin.bone_indices[bone_index]]);\n"
			"	}\n"
			"	vin.position = float4(blended_position.xyz, 1.0f);\n"
			"	vin.normal = float4(blended_normal.xyz, 0.0f);\n"
			"	vin.tangent = float4(blended_tangent.xyz, 0.0f);\n"
			"	VS_OUT vout;\n"
			"	vout.sv_position = mul(vin.position, mul(world, view_projection));\n"
			"	vout.position = vout.sv_position;\n"
			"	vout.world_position = mul(vin.position, world);\n"
			"	vin.normal.w = 0;\n"
			"	vout.world_normal = normalize(mul(vin.normal, world));\n"
			"	vout.world_tangent = normalize(mul(vin.tangent, world));\n"
			"	vout.world_tangent.w = sigma;\n"
			"	vout.texcoord = vin.texcoord;\n"
			"	vout.color = material_color;\n"
			"	return vout;\n"
			"};\n";
		hr = createVertexShader(device, vs, vertex_shader.ReleaseAndGetAddressOf(),
			input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	}

	for (std::unordered_map<uint64_t, material>::iterator iterator = materials.begin();
		iterator != materials.end(); ++iterator)
	{
		if (iterator->second.texture_filenames[0].size() > 0)
		{
			std::filesystem::path path(fbx_filename);
			std::filesystem::path texpass(utf8_to_utf16(iterator->second.texture_filenames[0]));
			path.replace_filename(texpass.filename());
			if (!std::filesystem::exists(path))
			{
				path.replace_filename(texpass);
			}
			D3D11_TEXTURE2D_DESC texture2d_desc;
			loadTextureFromFile(device, path.c_str(),
				iterator->second.shader_resource_views[0].ReleaseAndGetAddressOf(), &texture2d_desc);
		}
		else
		{
			makeDummyTexture(device, iterator->second.shader_resource_views[0].ReleaseAndGetAddressOf());
		}
		if (iterator->second.texture_filenames[1].size() > 0)
		{
			std::filesystem::path path(fbx_filename);
			std::filesystem::path texpass(utf8_to_utf16(iterator->second.texture_filenames[1]));
			path.replace_filename(texpass.filename());
			if (!std::filesystem::exists(path))
			{
				path.replace_filename(texpass);
			}
			D3D11_TEXTURE2D_DESC texture2d_desc;
			loadTextureFromFile(device, path.c_str(),
				iterator->second.shader_resource_views[1].ReleaseAndGetAddressOf(), &texture2d_desc);
		}
		else
		{
			makeDummyTexture(device, iterator->second.shader_resource_views[1].ReleaseAndGetAddressOf());
		}
	}
}

void SkinnedMesh::fetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
{
	const size_t node_count{ scene_view.nodes.size() };
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		const scene::node& node{ scene_view.nodes.at(node_index) };
		const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

		const int material_count{ fbx_node->GetMaterialCount() };
		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

			material material;
			material.name = fbx_material->GetName();
			material.unique_id = fbx_material->GetUniqueID();
			FbxProperty fbx_property;
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (fbx_property.IsValid())
			{
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.Kd.x = static_cast<float>(color[0]);
				material.Kd.y = static_cast<float>(color[1]);
				material.Kd.z = static_cast<float>(color[2]);
				material.Kd.w = 1.0f;

				const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[0] = fbx_texture ? fbx_texture->GetFileName() : "";
			}
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
			if (fbx_property.IsValid())
			{
				const FbxFileTexture* file_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[1] = file_texture ? file_texture->GetFileName() : "";
			}
			materials.emplace(material.unique_id, std::move(material));
		}
	}
	if (materials.empty())
	{
		material material;
		material.name = "";
		material.unique_id = 0;
		materials.emplace(material.unique_id, std::move(material));
	}
}

void SkinnedMesh::fetchSkeleton(FbxMesh* fbx_mesh, skeleton& bind_pose)
{
	const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
		const int cluster_count = skin->GetClusterCount();
		bind_pose.bones.resize(cluster_count);
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			FbxCluster* cluster = skin->GetCluster(cluster_index);

			skeleton::bone& bone{ bind_pose.bones.at(cluster_index) };
			bone.name = cluster->GetLink()->GetName();
			bone.unique_id = cluster->GetLink()->GetUniqueID();
			bone.parent_index = bind_pose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());

			bone.node_index = scene_view.indexof(bone.unique_id);

			//'reference_global_init_position' is used to convert from local space of model(mesh) to
			// global space of scene.
			FbxAMatrix reference_global_init_position;
			cluster->GetTransformMatrix(reference_global_init_position);

			// 'cluster_global_init_position' is used to convert from local space of bone to
			// global space of scene.
			FbxAMatrix cluster_global_init_position;
			cluster->GetTransformLinkMatrix(cluster_global_init_position);

			// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation
			// (translation, rotation and scale), the last row of the matrix represents the translation part of
			// the transformation.
			// Compose 'bone.offset_transform' matrix that trnasforms position from mesh space to bone space.
			// This matrix is called the offset matrix.
			bone.offset_transform = to_xmfloat4x4(cluster_global_init_position.Inverse() * reference_global_init_position);
		}
	}
}

void SkinnedMesh::fetchAnimations(FbxScene* fbx_scene, std::vector<Animation>& animation_clips, float sampling_rate)
{
	FbxArray<FbxString*> animation_stack_names;
	fbx_scene->FillAnimStackNameArray(animation_stack_names);
	const int animation_stack_count{ animation_stack_names.GetCount() };
	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		Animation& animation_clip{ animation_clips.emplace_back() };
		animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();

		FbxAnimStack* animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.name.c_str()) };
		fbx_scene->SetCurrentAnimationStack(animation_stack);

		const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
		FbxTime one_second;
		one_second.SetTime(0, 0, 1, 0, 0, time_mode);
		animation_clip.sampling_rate = sampling_rate > 0 ?
			sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));
		const FbxTime sampling_interval
		{ static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate) };
		const FbxTakeInfo* take_info{ fbx_scene->GetTakeInfo(animation_clip.name.c_str()) };
		const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };
		const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };
		for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
		{
			Animation::keyframe& keyframe{ animation_clip.sequence.emplace_back() };

			const size_t node_count{ scene_view.nodes.size() };
			keyframe.nodes.resize(node_count);
			for (size_t node_index = 0; node_index < node_count; ++node_index)
			{
				FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
				if (fbx_node)
				{
					Animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
					// 'global_transform' is a transformation matrix of a node with respect to
					// the scene's global coordinate system.
					node.global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform(time));
					// 'local_transform' is a transformation matrix of a node with respect to
					// its parent's local coordinate system.
					const FbxAMatrix& local_transform{ fbx_node->EvaluateLocalTransform(time) };
					node.scaling = to_xmfloat3(local_transform.GetS());
					node.rotation = to_xmfloat4(local_transform.GetQ());
					node.translation = to_xmfloat3(local_transform.GetT());
				}
			}
		}
	}
	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		delete animation_stack_names[animation_stack_index];
	}
}

bool Keyframe::_canChange(std::shared_ptr<SkinnedMesh> _owner, int _clip)const
{
	if (_clip < 0)return false;
	const std::vector<Animation>& animations{ _owner->getAnimationClips() };
	return _clip < animations.size();
}

Keyframe::Keyframe(std::shared_ptr<SkinnedMesh> owner)
	:owner(owner), keyframe(), clip_index(-1),
	animation_tick(0.0f), loop_flg(false), end_flg(true)
{
	//assert(this->owner.expired() == false && "Owner is absent.");
}

bool Keyframe::setOwner(std::shared_ptr<SkinnedMesh> owner)
{
	if (this->owner.expired())
	{
		this->owner = owner;
		change(-1);
		return true;
	}
	return false;
}

bool Keyframe::change(int clip, bool loop)
{
	if (owner.expired())return false;
	if (clip < 0)
	{
		clip_index = -1;
		keyframe.nodes.clear();
		keyframe.nodes.shrink_to_fit();
		loop_flg = false;
		end_flg = true;
		return true;
	}
	std::shared_ptr<SkinnedMesh> ownerPtr = owner.lock();
	bool result;
	if (result = _canChange(ownerPtr, clip))
	{
		clip_index = clip;
		animation_tick = 0.0f;
		end_flg = false;
		loop_flg = loop;
		update(0);
	}
	return result;
}

void Keyframe::resetAnimationTick()
{
	animation_tick = 0.0f;
}

bool Keyframe::blend(int clip, int frame, float factor)
{
	if (owner.expired())return false;
	if (clip_index < 0)return false;
	std::shared_ptr<SkinnedMesh> ownerPtr = owner.lock();
	bool result;
	if (result = _canChange(ownerPtr, clip))
	{
		const std::vector<Animation>& animations = ownerPtr->getAnimationClips();
		int frame_index{ 0 };

		const Animation* animation[2] = { &animations.at(clip_index),&animations.at(clip) };
		frame_index = static_cast<int>(animation_tick * animation[0]->sampling_rate);
		if (frame_index > animation[0]->sequence.size() - 1)
		{
			if (loop_flg)
			{
				frame_index = 0;
				animation_tick = 0;
			}
			else
			{
				frame_index = static_cast<int>(animation[0]->sequence.size() - 1);
				end_flg = true;
			}
		}
		const Animation::keyframe* keyframes[2]
		{
			&animation[0]->sequence.at(frame_index),
			&animation[1]->sequence.at(frame % animation[1]->sequence.size())
		};

		ownerPtr->blendAnimations(keyframes, factor, keyframe);
		ownerPtr->updateKeyframe(keyframe);
	}
	return result;
}

bool Keyframe::blend(int clip1, int frame1, int clip2, int frame2, float factor)
{
	if (owner.expired())return false;
	std::shared_ptr<SkinnedMesh> ownerPtr = owner.lock();
	if (!_canChange(ownerPtr, clip1))return false;
	clip_index = clip1;
	const std::vector<Animation>& animations = ownerPtr->getAnimationClips();
	const Animation& animation1{ animations.at(clip_index) };
	int frame_index{ frame1 % (int)animation1.sequence.size() };
	animation_tick = static_cast<float>(frame_index) / animation1.sampling_rate;
	keyframe = animation1.sequence.at(frame_index);
	bool result;
	if (result = _canChange(ownerPtr, clip2))
	{
		const Animation& animation_brend = { animations.at(clip2) };
		const Animation::keyframe* keyframes[2]
		{
			&keyframe,
			&animation_brend.sequence.at(frame2 % animation_brend.sequence.size())
		};

		ownerPtr->blendAnimations(keyframes, factor, keyframe);
		ownerPtr->updateKeyframe(keyframe);
	}
	return result;
}

void Keyframe::update(float elapsed_time)
{
	if (owner.expired())return;
	if (end_flg)return;
	if (clip_index < 0)return;
	std::shared_ptr<SkinnedMesh> ownerPtr = owner.lock();
	const std::vector<Animation>& animations = ownerPtr->getAnimationClips();
	int frame_index{ 0 };
	const Animation& animation{ animations.at(clip_index) };
	frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
	if (frame_index > animation.sequence.size() - 1)
	{
		if (loop_flg)
		{
			frame_index = 0;
			animation_tick = 0;
		}
		else
		{
			frame_index = static_cast<int>(animation.sequence.size() - 1);
			end_flg = true;
		}
	}
	else
	{
		animation_tick += elapsed_time;
	}
	keyframe = animation.sequence.at(frame_index);
}

void Keyframe::updateAndBlend(float elapsed_time, int clip, int frame, float factor)
{
	if (owner.expired())return;
	if (end_flg)return;
	if (clip_index < 0)return;
	std::shared_ptr<SkinnedMesh> ownerPtr = owner.lock();
	const std::vector<Animation>& animations = ownerPtr->getAnimationClips();
	int frame_index{ 0 };
	const Animation& animation{ animations.at(clip_index) };
	frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
	if (frame_index > animation.sequence.size() - 1)
	{
		if (loop_flg)
		{
			frame_index = 0;
			animation_tick = 0;
		}
		else
		{
			frame_index = static_cast<int>(animation.sequence.size() - 1);
			end_flg = true;
		}
	}
	else
	{
		animation_tick += elapsed_time;
	}
	keyframe = animation.sequence.at(frame_index);
	if (_canChange(ownerPtr, clip))
	{
		const Animation& animation_brend = { animations.at(clip) };
		const Animation::keyframe* keyframes[2]
		{
			&keyframe,
			&animation_brend.sequence.at(frame % animation_brend.sequence.size())
		};

		ownerPtr->blendAnimations(keyframes, factor, keyframe);
		ownerPtr->updateKeyframe(keyframe);
	}
}

void Keyframe::reset()
{
	change(-1);
	owner.reset();
}