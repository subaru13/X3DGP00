#pragma once
#include "C8H10N4O2/Include.h"
#include "C8H10N4O2/function/DepthShadow.h"
#include <vector>

class collision_editer;

struct M_AABB
{
	FLOAT3 min;
	FLOAT3 max;
	M_AABB& operator ~ ();
	void operator>> (FLOAT3*)const;
	void operator<< (FLOAT3*)const;
};

struct COLLIDER
{
	M_AABB area;
	int type;
	template<class T>void serialize(T& archive)
	{
		archive(area.min, area.max, type);
	}
	COLLIDER& operator ~ ();
};

class collision_editer : public SceneBase
{
private:
	ID3D11DeviceContext* immediate_context;
	std::unique_ptr<Sprite> bg;
	std::unique_ptr<SkinnedMesh> stage_mesh;
	std::unique_ptr<SkinnedMesh> player_mesh;
	std::unique_ptr<GeometryPrimitive> collider_mesh;
	const char* type_label[3] = { "floor", "wall", "floor + wall", };
	enum { floor, wall, floor_wall };
	FLOAT4 material_colors[3] = { {1,0,0,1},{0,1,0,1},{0,0,1,1} };
	FLOAT4 bg_color = { 1,1,1,1 };
	std::vector<COLLIDER> collider_data;
	std::unique_ptr<SceneConstant> scene_constant;
	CameraControl camera;
	char file_pass[256];
	void save_collider_data()const;
	void load_collider_data();
	bool can_input;
	bool is_w;

	bool is_preview = false;
	FLOAT3 position, posture;
	CameraControl preview_camera;
	float speed = 10.0f;
	float camera_h = 5.0f;
	float camera_d = 15.0f;

	void gui();

	Key __1;
	Key _c[3] = { {VK_CONTROL},{'S'},{'L'} };

	void preview_init();
	void preview_update(float e);
	void preview_render(ID3D11PixelShader***);

	Key move_key[4] = { 'W','S','A','D' };
	DepthShadow depth_shadow;
public:
	collision_editer(ID3D11Device* device);
	void render_collider(FLOAT3*, FLOAT3*, int, ID3D11PixelShader***)const;
	virtual void update(float)override;
	virtual void render(ID3D11DeviceContext*, float)override;

	~collision_editer();
};

