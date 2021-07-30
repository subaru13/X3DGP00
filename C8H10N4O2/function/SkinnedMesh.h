#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

#include "ConstantBuffer.h"
#include "Arithmetic.h"

namespace DirectX
{
	template < class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template < class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template < class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template < class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}

struct Animation
{
	struct keyframe
	{
		struct node
		{
			// 'global_transform' is used to convert from local space of node to global space of scene.
			DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
			DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 }; // Rotation quaternion
			DirectX::XMFLOAT3 translation{ 0, 0, 0 };

			template < class T>
			void serialize(T& archive)
			{
				archive(global_transform, scaling, rotation, translation);
			}
		};
		std::vector<node> nodes;

		template < class T>
		void serialize(T& archive)
		{
			archive(nodes);
		}
	};

	std::string name;
	float sampling_rate{ 0 };
	std::vector<keyframe> sequence;

	template < class T>
	void serialize(T& archive)
	{
		archive(name, sampling_rate, sequence);
	}
};

class SkinnedMesh
{
public:
	static const int MAX_BONE_INFLUENCES{ 4 };
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 tangent;
		DirectX::XMFLOAT2 texcoord;
		float bone_weights[MAX_BONE_INFLUENCES]{ 1, 0, 0, 0 };
		uint32_t bone_indices[MAX_BONE_INFLUENCES]{};
		template < class T>
		void serialize(T& archive)
		{
			archive(position, normal, tangent, texcoord, bone_weights, bone_indices);
		}
	};
	static const int MAX_BONES{ 256 };
	struct constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{ { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
	};
	struct scene
	{
		struct node
		{
			uint64_t unique_id{ 0 };
			std::string name;
			FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
			int64_t parent_index{ -1 };
			template<class T>
			void serialize(T& archive)
			{
				archive(unique_id, name, attribute, parent_index);
			}
		};
		int64_t indexof(uint64_t unique_id) const
		{
			int64_t index{ 0 };
			for (const node& node : nodes)
			{
				if (node.unique_id == unique_id)
				{
					return index;
				}
				++index;
			}
			return -1;
		}
		std::vector<node> nodes;
		template<class T>
		void serialize(T& archive)
		{
			archive(nodes);
		}
	};
	struct skeleton
	{
		struct bone
		{
			uint64_t unique_id{ 0 };
			std::string name;
			// 'parent_index' is index that refers to the parent bone's position in the array that contains itself.
			int64_t parent_index{ -1 }; // -1 : the bone is orphan
			// 'node_index' is an index that refers to the node array of the scene.
			int64_t node_index{ 0 };

			// 'offset_transform' is used to convert from model(mesh) space to bone(node) scene.
			DirectX::XMFLOAT4X4 offset_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

			bool is_orphan() const { return parent_index < 0; };
			template < class T>
			void serialize(T& archive)
			{
				archive(unique_id, name, parent_index, node_index, offset_transform);
			}
		};
		int64_t indexof(uint64_t unique_id) const
		{
			int64_t index{ 0 };
			for (const bone& bone : bones)
			{
				if (bone.unique_id == unique_id)
				{
					return index;
				}
				++index;
			}
			return -1;
		}
		std::vector<bone> bones;
		template < class T>
		void serialize(T& archive)
		{
			archive(bones);
		}
	};
	struct mesh
	{
		uint64_t unique_id{ 0 };
		std::string name;
		// 'node_index' is an index that refers to the node array of the scene.
		int64_t node_index{ 0 };
		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;
		struct subset
		{
			uint64_t material_unique_id{ 0 };
			std::string material_name;
			uint32_t start_index_location{ 0 };
			uint32_t index_count{ 0 };

			template < class T>
			void serialize(T& archive)
			{
				archive(material_unique_id, material_name, start_index_location, index_count);
			}
		};
		std::vector<subset> subsets;
		DirectX::XMFLOAT4X4 default_global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		skeleton bind_pose;
		template < class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, node_index, subsets, default_global_transform, bind_pose, vertices, indices);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		friend class SkinnedMesh;
	};
	struct material
	{
		uint64_t unique_id{ 0 };
		std::string name;

		DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };

		std::string texture_filenames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[4];
		template < class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, Ka, Kd, Ks, texture_filenames);
		}
	};

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	ConstantBuffer<constants> constant_buffer;
public:
	SkinnedMesh(ID3D11Device* device, const char* fbx_filename, bool triangulate = false, float sampling_rate = 0.0f);
	virtual ~SkinnedMesh() = default;
	/// <summary>
	/// �X�L���h���b�V����`�悵�܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="external_pixel_shader">
	/// �K�p����s�N�Z���V�F�[�_�[
	/// NULL�ŋK��̃V�F�[�_�[
	/// </param>
	/// <param name="world">���[���h�ϊ��s��</param>
	/// <param name="keyframe">�L�[�t���[��</param>
	/// <param name="material_color">�F</param>
	void render(ID3D11DeviceContext* immediate_context,
		ID3D11PixelShader** external_pixel_shader,
		const FLOAT4X4& world,
		const Animation::keyframe* keyframe = nullptr,
		const FLOAT4& material_color = { 1,1,1,1 });
	/// <summary>
	/// �L�[�t���[���̏����X�V���܂��B
	/// </summary>
	/// <param name="keyframe">�L�[�t���[��</param>
	void updateKeyframe(Animation::keyframe& keyframe)const;
	/// <summary>
	/// �A�j���V�����f�[�^��ǉ����܂��B
	/// </summary>
	/// <param name="animation_filename">�A�j���V�����f�[�^�̃t�@�C���p�X</param>
	/// <param name="sampling_rate">�T���v�����O���[�g</param>
	/// <returns>����������true</returns>
	bool appendAnimations(const char* animation_filename, float sampling_rate = 0.0f);
	/// <summary>
	/// ���[�V�����u�����h�����s���܂��B
	/// </summary>
	/// <param name="keyframes">�u�����h����2�̃L�[�t���[��</param>
	/// <param name="factor">
	/// �u�����h�̊���
	/// 0�ɋ߂��ق�1�ڂ̃L�[�t���[�����g�p����܂��B
	/// </param>
	/// <param name="keyframe">���������L�[�t���[��</param>
	void blendAnimations(const Animation::keyframe* keyframes[2], float factor, Animation::keyframe& keyframe);
	/// <summary>
	/// �V�[���r���[�̃f�[�^���擾���܂��B
	/// </summary>
	const scene& getSceneView()const { return scene_view; }
	/// <summary>
	/// �}�e���A���̃f�[�^���擾���܂��B
	/// </summary>
	const std::unordered_map<uint64_t, material>& getMaterials()const { return materials; }
	/// <summary>
	/// ���b�V���f�[�^���擾���܂��B
	/// </summary>
	const std::vector<mesh>& getMeshes()const { return meshes; }
	/// <summary>
	/// �A�j���V�����f�[�^���擾���܂��B
	/// </summary>
	const std::vector<Animation>& getAnimationClips()const { return animation_clips; }

protected:
	void fetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);
	void fetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials);
	void fetchSkeleton(FbxMesh* fbx_mesh, skeleton& bind_pose);
	void fetchAnimations(FbxScene* fbx_scene,
		std::vector<Animation>& animation_clips,
		float sampling_rate /*If this value is 0, the animation data will be sampled at the default frame rate.*/);
	void createComObjects(ID3D11Device* device, const char* fbx_filename);
protected:
	scene scene_view;
	std::unordered_map<uint64_t, material> materials;
	std::vector<mesh> meshes;
	std::vector<Animation> animation_clips;
};

class Keyframe
{
private:
	std::weak_ptr<SkinnedMesh>  owner;
	Animation::keyframe			keyframe;
	int							clip_index;
	float						animation_tick;
	bool						loop_flg;
	bool						end_flg;

	bool _canChange(std::shared_ptr<SkinnedMesh> _owner, int _clip)const;
public:
	Keyframe(std::shared_ptr<SkinnedMesh> owner = nullptr);
	/// <summary>
	/// �I�[�i�[���s�݂̏ꍇ�̂݃I�[�i�[��ݒ肵�܂��B
	/// </summary>
	/// <param name="owner">�I�[�i�[</param>
	/// <returns>�ݒ�o������true</returns>
	bool setOwner(std::shared_ptr<SkinnedMesh> owner);

	/// <summary>
	/// �A�j���[�V������؂�ւ��܂�
	/// </summary>
	/// <param name="clip">
	/// �؂�ւ���̃A�j���[�V�����ԍ�
	/// 0�����ŃA�j���V�����f�[�^�̏�����
	/// </param>
	/// <param name="loop">���[�v�ݒ�</param>
	/// <returns>
	/// �؂�ւ��o������true
	/// </returns>
	bool change(int clip, bool loop = false);

	/// <summary>
	/// ���݂̃��[�V�����ɕʂ̃��[�V�������u�����h���܂��B
	/// </summary>
	/// <param name="clip">�u�����h����A�j���[�V�����ԍ�</param>
	/// <param name="frame">�u�����h����t���[����</param>
	/// <param name="factor">
	/// �u�����h�̊���
	/// 0�ɋ߂��ق�1�ڂ̃L�[�t���[�����g�p����܂��B
	/// </param>
	/// <returns>�u�����h�ɐ���������true</returns>
	bool blend(int clip, int frame, float factor);

	/// <summary>
	/// �u�����h����2�̃A�j���[�V�����ԍ��ƃt���[�������w�肵�ău�����h���܂��B
	/// </summary>
	/// <param name="clip1">�u�����h����A�j���[�V�����ԍ�1��</param>
	/// <param name="frame1">�u�����h����t���[����1��</param>
	/// <param name="clip2">�u�����h����A�j���[�V�����ԍ�2��</param>
	/// <param name="frame2">�u�����h����t���[����2��</param>
	/// <param name="factor">
	/// �u�����h�̊���
	/// 0�ɋ߂��ق�1�ڂ̃L�[�t���[�����g�p����܂��B
	/// </param>
	/// <returns>�u�����h�ɐ���������true</returns>
	bool blend(int clip1, int frame1, int clip2, int frame2, float factor);

	/// <summary>
	/// �A�j���[�V�������X�V���܂��B
	/// </summary>
	/// <param name="elapsed_time">�o�ߎ���</param>
	void update(float elapsed_time);

	/// <summary>
	///	�A�j���[�V�������X�V���ʂ̃��[�V�������u�����h���܂��B
	/// �u�����h�Ɏ��s�����ꍇ�X�V�̂ݎ��s���܂��B
	/// </summary>
	/// <param name="elapsed_time">�o�ߎ���</param>
	/// <param name="clip">�u�����h����A�j���[�V�����ԍ�</param>
	/// <param name="frame">�u�����h����t���[����</param>
	/// <param name="factor">
	/// �u�����h�̊���
	/// 0�ɋ߂��ق�1�ڂ̃L�[�t���[�����g�p����܂��B
	/// </param>
	void updateAndBlend(float elapsed_time, int clip, int frame, float factor);

	/// <summary>
	/// �A�j���[�V�����e�B�b�N�����Z�b�g���܂��B
	/// </summary>
	void resetAnimationTick();

	/// <summary>
	/// ���[�v�t���O��ݒ肵�܂��B
	/// </summary>
	/// <param name="loop">���[�v�ݒ�</param>
	void setLoopFlg(bool loop) { loop_flg = loop; end_flg = false; }

	/// <summary>
	/// �Đ����̃A�j���[�V�����ԍ����擾���܂��B
	/// </summary>
	int getClipIndex()const { return owner.expired() ? -1 : clip_index; }
	/// <summary>
	/// �A�j���[�V�����e�B�b�N���擾���܂��B
	/// </summary>
	float getAnimationTick()const { return owner.expired() ? 0.0f : animation_tick; }
	/// <summary>
	/// �A�j���[�V�������I�����Ă��邩�̃t���O���擾���܂��B
	/// </summary>
	bool getEndFlg()const { return owner.expired() ? true : end_flg; }
	/// <summary>
	/// ���݂̃��[�v�ݒ���擾���܂��B
	/// </summary>
	bool getLoopFlg()const { return owner.expired() ? false : loop_flg; }
	/// <summary>
	/// ���݂̃L�[�t���[�����擾���܂��B
	/// </summary>
	const Animation::keyframe* getData()const { return owner.expired() ? nullptr : &keyframe; }

	/// <summary>
	/// �I�[�i�[�̊J���Ȃǂ����܂��B
	/// </summary>
	void reset();

	operator bool()const { return owner.expired(); }
	const Animation::keyframe* operator&()const { return owner.expired() ? nullptr : &keyframe; }
};