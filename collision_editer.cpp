#include "collision_editer.h"
#include <fstream>
#include <filesystem>

#undef max
#undef min

#define __min_(a,b) ((a) < (b)) ? (a) : (b)
#define __max_(a,b) ((a) > (b)) ? (a) : (b)

#define S_FILE_PASS "_collision_editer"
#define I_FILE_PASS "collision_editer"

FLOAT3 ld{ 0,-1,1 };

bool col_line_aabb(const FLOAT3& s, const FLOAT3& e, const M_AABB& box, FLOAT3& hitPoint)
{
	float       enter = 0.0f;
	float       exit = 1.0f;
	FLOAT3    dir = e - s;
	auto Line_AABB_1d = [](float start, float dir, float min, float max, float& enter, float& exit)->bool
	{
		if (fabsf(dir) < 1.0E-8) return (start >= min && start <= max);
		float   ooDir = 1.0f / dir;
		float   t0 = (min - start) * ooDir;
		float   t1 = (max - start) * ooDir;
		if (t0 > t1)
		{
			float cope = t0;
			t0 = t1;
			t1 = cope;
		}
		if (t0 > exit || t1 < enter)return false;
		if (t0 > enter) enter = t0;
		if (t1 < exit) exit = t1;
		return true;
	};
	if (!Line_AABB_1d(s.x, dir.x, box.min.x, box.max.x, enter, exit)) return false;
	if (!Line_AABB_1d(s.y, dir.y, box.min.y, box.max.y, enter, exit))return false;
	if (!Line_AABB_1d(s.z, dir.z, box.min.z, box.max.z, enter, exit))return false;
	hitPoint = s + dir * enter;
	return true;
}

M_AABB& M_AABB::operator~()
{
	FLOAT3 new_min, new_max;
	new_min.x = __min_(min.x, max.x);
	new_min.y = __min_(min.y, max.y);
	new_min.z = __min_(min.z, max.z);
	new_max.x = __max_(min.x, max.x);
	new_max.y = __max_(min.y, max.y);
	new_max.z = __max_(min.z, max.z);
	memcpy(&min, &new_min, sizeof(FLOAT3));
	memcpy(&max, &new_max, sizeof(FLOAT3));
	return *this;
}

void M_AABB::operator>>(FLOAT3* c)const
{
	FLOAT3 center;
	center.x = (min.x + max.x) * 0.5f;
	center.y = (min.y + max.y) * 0.5f;
	center.z = (min.z + max.z) * 0.5f;
	memcpy(c, &center, sizeof(FLOAT3));
}

void M_AABB::operator<<(FLOAT3* s) const
{
	FLOAT3 size;
	size.x = max.x - min.x;
	size.y = max.y - min.y;
	size.z = max.z - min.z;
	memcpy(s, &size, sizeof(FLOAT3));
}


COLLIDER& COLLIDER::operator~() { ~area; return *this; }

void collision_editer::save_collider_data() const
{
	std::filesystem::path path = file_pass;
	path.replace_extension("collision");
	std::ofstream ofs{ path ,std::ios::binary };
	cereal::BinaryOutputArchive serialization{ ofs };
	serialization(collider_data);
	ofs.close();
}

void collision_editer::load_collider_data()
{
	std::filesystem::path path = file_pass;
	path.replace_extension("collision");
	if (std::filesystem::exists(path))
	{
		std::ifstream ifs{ path ,std::ios::binary };
		cereal::BinaryInputArchive deserialization{ ifs };
		deserialization(collider_data);
		ifs.close();
	}
}

collision_editer::collision_editer(ID3D11Device* device)
	: SceneBase(device), bg(new Sprite(device)),
	stage_mesh(new SkinnedMesh(device, "data/ExampleStage.fbx")),
	player_mesh(new SkinnedMesh(device, "data/Slime.fbx")),
	collider_mesh(new GeometryPrimitive(device, GP_CONFIG{ GP_SHAPE::GP_CUBE ,32 })),
	file_pass(), scene_constant(new SceneConstant(device)), can_input(true),is_w(true), __1(VK_XBUTTON1),
	depth_shadow(device, 256, 256)
{
	strcpy_s(file_pass, S_FILE_PASS);
	load_collider_data();
	std::filesystem::path path = file_pass;
	path.replace_extension("cf");
	if (std::filesystem::exists(path))
	{
		std::ifstream ifs{ path ,std::ios::binary };
		cereal::BinaryInputArchive deserialization{ ifs };
		deserialization(
			file_pass,
			bg_color,
			material_colors[0],
			material_colors[1],
			material_colors[2],
			is_w, can_input,
			(*camera.getPos()),
			(*camera.getTarget()));
		ifs.close();
		camera.fit();
	}
	else strcpy_s(file_pass, I_FILE_PASS);
}

void collision_editer::gui()
{
	ImGui::Begin("control_panel");
	if (ImGui::Button("exit"))QUIT_MAIN_LOOP;
	ImGui::SameLine();
	if (ImGui::Button("preview"))preview_init();
	ImGui::Text("color edit ===========================================");
	ImGui::ColorEdit3("background color", &bg_color.x);
	ImGui::ColorEdit3((std::string("material_colors[") + std::string(type_label[0]) + std::string("]")).c_str(), &material_colors[0].x);
	ImGui::ColorEdit3((std::string("material_colors[") + std::string(type_label[1]) + std::string("]")).c_str(), &material_colors[1].x);
	ImGui::ColorEdit3((std::string("material_colors[") + std::string(type_label[2]) + std::string("]")).c_str(), &material_colors[2].x);
	ImGui::Text("buttons ==============================================");
	ImGui::Checkbox("camera control",&can_input);
	ImGui::SameLine();
	ImGui::Checkbox("wireframe", &is_w);
	ImGui::InputText("file_pass", file_pass, ARRAYSIZE(file_pass));
	if (ImGui::Button("save")) { save_collider_data(); }
	ImGui::SameLine();
	if (ImGui::Button("load")) { load_collider_data(); }
	if (ImGui::Button("+"))
	{
		COLLIDER & c = collider_data.emplace_back();
		c.area.max.x = 0.5f;
		c.area.max.y = 0.5f;
		c.area.max.z = 0.5f;
		c.area.min.x = -0.5f;
		c.area.min.y = -0.5f;
		c.area.min.z = -0.5f;
		c.type = 0;
	}
	ImGui::SameLine();
	if (ImGui::Button("-") && collider_data.size())
	{
		collider_data.erase(collider_data.end() - 1);
	}
	ImGui::SameLine();
	if (ImGui::Button("clear")) { collider_data.clear(); }
	ImGui::BeginChild("collider_datas =======================================", {},true);
	for (auto& it = collider_data.begin(); it != collider_data.end(); )
	{
		std::stringstream label;
		label << "collider[" << std::hex << PtrToInt(&(*it)) << "]";
		if (ImGui::TreeNode(label.str().c_str()))
		{
			if (ImGui::Button("erase"))
			{
				it = collider_data.erase(it);
				ImGui::TreePop();
				continue;
			}
			ImGui::SameLine();
			if (ImGui::Button("focus"))
			{
				it->area >> camera.getTarget();
				camera.fit();
			}
			ImGui::Combo("type", &it->type, type_label, ARRAYSIZE(type_label));

			FLOAT3 p;
			FLOAT3 s;

			if (ImGui::ButtonEx("px+", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area.max.x += 0.1f;
				it->area.min.x += 0.1f;
			}
			ImGui::SameLine();
			if (ImGui::ButtonEx("px-", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area.max.x -= 0.1f;
				it->area.min.x -= 0.1f;
			}

			if (ImGui::ButtonEx("py+", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area.max.y += 0.1f;
				it->area.min.y += 0.1f;
			}
			ImGui::SameLine();
			if (ImGui::ButtonEx("py-", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area.max.y -= 0.1f;
				it->area.min.y -= 0.1f;
			}

			if (ImGui::ButtonEx("pz+", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area.max.z += 0.1f;
				it->area.min.z += 0.1f;
			}
			ImGui::SameLine();
			if (ImGui::ButtonEx("pz-", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area.max.z -= 0.1f;
				it->area.min.z -= 0.1f;
			}

			/********************************************************************************/

			if (ImGui::ButtonEx("sx+", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area >> (&p);it->area << (&s);
				s.x += 0.1f;s.x *= 0.5f;
				it->area.max.x = p.x + s.x;
				it->area.min.x = p.x - s.x;
			}
			ImGui::SameLine();
			if (ImGui::ButtonEx("sx-", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area >> (&p);it->area << (&s);
				s.x -= 0.1f;s.x *= 0.5f;
				it->area.max.x = p.x + s.x;
				it->area.min.x = p.x - s.x;
			}

			if (ImGui::ButtonEx("sy+", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area >> (&p);it->area << (&s);
				s.y += 0.1f;s.y *= 0.5f;
				it->area.max.y = p.y + s.y;
				it->area.min.y = p.y - s.y;
			}
			ImGui::SameLine();
			if (ImGui::ButtonEx("sy-", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area >> (&p);it->area << (&s);
				s.y -= 0.1f;s.y *= 0.5f;
				it->area.max.y = p.y + s.y;
				it->area.min.y = p.y - s.y;
			}

			if (ImGui::ButtonEx("sz+", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area >> (&p);it->area << (&s);
				s.z += 0.1f;s.z *= 0.5f;
				it->area.max.z = p.z + s.z;
				it->area.min.z = p.z - s.z;
			}
			ImGui::SameLine();
			if (ImGui::ButtonEx("sz-", {}, ImGuiButtonFlags_::ImGuiButtonFlags_Repeat))
			{
				it->area >> (&p);it->area << (&s);
				s.z -= 0.1f; s.z *= 0.5f;
				it->area.max.z = p.z + s.z;
				it->area.min.z = p.z - s.z;
			}

			ImGui::InputFloat("max::x", &it->area.max.x,0.5f, -0.5f);
			ImGui::InputFloat("max::y", &it->area.max.y,0.5f, -0.5f);
			ImGui::InputFloat("max::z", &it->area.max.z,0.5f, -0.5f);
			ImGui::InputFloat("min::x", &it->area.min.x, 0.5f, -0.5f);
			ImGui::InputFloat("min::y", &it->area.min.y, 0.5f, -0.5f);
			ImGui::InputFloat("min::z", &it->area.min.z, 0.5f, -0.5f);
			ImGui::TreePop();
		}
		++it;
	}
	ImGui::EndChild();
	ImGui::End();
	collider_data.shrink_to_fit();
}

void collision_editer::preview_init()
{
	is_preview = true;
	position = {};
	posture = {};
	preview_camera = {};
	camera_d = 15.0f;
	camera_h = 5.0f;
	speed = 10.0f;
}

void collision_editer::preview_update(float e)
{
	FLOAT3 f,mf;

	DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&f),
		DirectX::XMMatrixRotationRollPitchYaw(posture.x, posture.y, posture.z).r[2]);

	//mf.y -= 0.98f * e;

	if (move_key[0].hold())
	{
		mf.x += f.x * speed * e;
		mf.y += f.y * speed * e;
		mf.z += f.z * speed * e;
	}
	if (move_key[1].hold())
	{
		mf.x -= f.x * speed * e;
		mf.y -= f.y * speed * e;
		mf.z -= f.z * speed * e;
	}
	if (move_key[2].hold())
	{
		posture.y -= 3.14f * e;
	}
	if (move_key[3].hold())
	{
		posture.y += 3.14f * e;
	}


	FLOAT3 r_s = position;
	r_s.y += 1.5f;
	FLOAT3 r_e = position;
	r_e.y -= 1.5f;

	bool is_h = false;
	FLOAT3 ne = { FLT_MAX,FLT_MAX ,FLT_MAX };
	FLOAT3 hp;
	for (auto& c : collider_data)
	{
		if (c.type == wall)continue;
		if (col_line_aabb(r_s, r_e, c.area, hp))
		{
			is_h = true;
			if (vec3Length(position - hp) < vec3Length(position - ne))
			{
				memcpy(&ne, &hp, sizeof(FLOAT3));
			}
		}
	}

	if (is_h)
	{
		memcpy(&position, &ne, sizeof(FLOAT3));
	}
	else
	{
		position.y += mf.y;
	}

	is_h = false;
	ne = { FLT_MAX,FLT_MAX ,FLT_MAX };
	r_s = position;
	r_e = position;
	r_e.x += mf.x;
	r_e.z += mf.z;
	for (auto& c : collider_data)
	{
		if (c.type == floor)continue;
		if (col_line_aabb(r_s, r_e, c.area, hp))
		{
			is_h = true;
			if (vec3Length(position - hp) < vec3Length(position - ne))
			{
				memcpy(&ne, &hp, sizeof(FLOAT3));
			}
		}
	}
	if (is_h)
	{
		memcpy(&position, &ne, sizeof(FLOAT3));
		position.x -= mf.x * 0.1f;
		position.z -= mf.z * 0.1f;
	}
	else
	{
		position.x += mf.x;
		position.z += mf.z;
	}
	DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&f),
		DirectX::XMMatrixRotationRollPitchYaw(posture.x, posture.y, posture.z).r[2]);
	FLOAT3* c_pos = preview_camera.getPos();
	FLOAT3* c_target = preview_camera.getTarget();

	memcpy(c_target, &position, sizeof(FLOAT3));
	c_pos->x = c_target->x - f.x * camera_d;
	c_pos->y = c_target->y - f.y + camera_h;
	c_pos->z = c_target->z - f.z * camera_d;

	scene_constant->setView(preview_camera.getView());
	FLOAT4 dir{};
	dir.x = c_target->x - c_pos->x;
	dir.y = c_target->y - c_pos->y;
	dir.z = c_target->z - c_pos->z;
	dir.w = 1.0f;
	scene_constant->setLightDirection(dir);
	ImGui::Begin("control_panel");
	if (ImGui::Button("quit_preview"))is_preview = false;
	ImGui::Checkbox("wireframe", &is_w);
	ImGui::SliderFloat("move_speed", &speed, 0.1f, 30.0f);
	ImGui::SliderFloat("camera_d", &camera_d, 0.5f, 30.0f);
	ImGui::SliderFloat("camera_h", &camera_h, -30.0f, 30.0f);
	ImGui::End();
}

void collision_editer::preview_render(ID3D11PixelShader*** pixel_shader)
{
	CHANGE_SETTINGS_3D_RENDERING_LHS;
	DirectX::XMFLOAT4X4 w{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	DirectX::XMMATRIX wm =
		DirectX::XMLoadFloat4x4(&w) *
		DirectX::XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		DirectX::XMMatrixRotationRollPitchYaw(posture.x, posture.y, posture.z) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&w, wm);
	player_mesh->render(immediate_context, pixel_shader[1], w, NULL);
}

void collision_editer::render_collider(FLOAT3* p, FLOAT3* s, int t,ID3D11PixelShader*** pixel_shader) const
{
	DirectX::XMMATRIX wm =
		DirectX::XMMatrixScaling(s->x, s->y, s->z) *
		DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f) *
		DirectX::XMMatrixTranslation(p->x, p->y, p->z);
	DirectX::XMFLOAT4X4 w;
	DirectX::XMStoreFloat4x4(&w, wm);
	collider_mesh->render(immediate_context, pixel_shader[0], w, material_colors[t]);
}

void collision_editer::update(float e)
{
	if (is_preview) { preview_update(e); }
	else
	{
		if (__1.down())can_input = !can_input;
		if (can_input) camera.update(e);
		gui();
		scene_constant->setView(camera.getView());
		FLOAT4 dir{};
		dir.x = camera.getTarget()->x - camera.getPos()->x;
		dir.y = camera.getTarget()->y - camera.getPos()->y;
		dir.z = camera.getTarget()->z - camera.getPos()->z;
		dir.w = 1.3f;
		scene_constant->setLightDirection(dir);
		if (_c[0].hold() && _c[1].down())save_collider_data();
		if (_c[0].hold() && _c[2].down())load_collider_data();
	}
}

void collision_editer::render(ID3D11DeviceContext* dc, float)
{
	immediate_context = dc;
	CHANGE_SETTINGS_2D_RENDERING;
	bg->quad(immediate_context, NULL, {}, {}, bg_color);
	scene_constant->send(immediate_context);

	DirectX::XMFLOAT4X4 w{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	DirectX::XMMATRIX wm =
		DirectX::XMLoadFloat4x4(&w) *
		DirectX::XMMatrixScaling(1.0f,1.0f,1.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	DirectX::XMStoreFloat4x4(&w, wm);
	static float bias = depth_shadow.getBias()*10.0f;
	static float dis = 10.0f;
	static FLOAT3 shadow_color = depth_shadow.getShadowColor();
	ImGui::ColorEdit3("shadow_color", &shadow_color.x);
	ImGui::SliderFloat("bias", &bias, 0.001f, 1, "%.6f / 10");
	ImGui::SliderFloat("dis", &dis, 1.f, 300.0f, "%.6f");

	depth_shadow.setShadowColor(shadow_color);
	depth_shadow.setBias(bias / 10.0f);
	depth_shadow.blit(immediate_context,
		[&](ID3D11PixelShader** pixel_shaders[2])
	{
		CHANGE_SETTINGS_3D_RENDERING_LHS;
		stage_mesh->render(immediate_context, pixel_shaders[1], w, NULL);
		if (is_preview) { preview_render(pixel_shaders); }
		if (is_w)Framework::setRasterizerState(RASTERIZER_FILL::WIREFRAME, FALSE);
		for (auto& c : collider_data)
		{
			~c;
			FLOAT3 p;
			FLOAT3 s;
			c.area >> (&p);
			c.area << (&s);
			render_collider(&p, &s, c.type, pixel_shaders);
		}
	}, ld, dis);

}

collision_editer::~collision_editer()
{
	char _file_pass[256];
	strcpy_s(_file_pass, file_pass);
	strcpy_s(file_pass, S_FILE_PASS);
	save_collider_data();
	std::filesystem::path path = file_pass;
	path.replace_extension("cf");
	std::ofstream ofs{ path ,std::ios::binary };
	cereal::BinaryOutputArchive serialization{ ofs };
	serialization(
		_file_pass,
		bg_color,
		material_colors[0],
		material_colors[1],
		material_colors[2],
		is_w, can_input,
		(*camera.getPos()),
		(*camera.getTarget()));
	ofs.close();
}


