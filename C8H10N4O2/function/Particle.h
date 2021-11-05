#pragma once
#include "Arithmetic.h"
#include <d3d11.h>
#include <wrl.h>
#include <wchar.h>
#include <functional>
#include <memory>

using namespace Microsoft::WRL;

class Particle
{
public:
	struct innovation_vertex
	{
		FLOAT3	position = {};
		FLOAT2	size = {};
		FLOAT3	velocity = {};
		FLOAT3	acceleration = {};
		FLOAT	alpha = 0.0f;
		FLOAT	attenuation_alpha = 0.0f;
		FLOAT	timer = 0.0f;
		FLOAT	animation_timer = 0.0f;
		INT		chip_num = -1;
	};

private:
	struct vertex
	{
		FLOAT4	position;	//	xyz	->	world_position	w	->	no used
		FLOAT4	color;		//	xy	->	scale	z	->	type	w	->	alpha
	};

	struct textrue_data
	{
		FLOAT2	size;
		FLOAT2	per_chip;
	};

	struct conversion_material
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};

	ComPtr<ID3D11DepthStencilState>			depth_stencil_state;
	ComPtr<ID3D11RasterizerState>			rasterizer_state;
	ComPtr<ID3D11SamplerState>				sampler_state;
	ComPtr<ID3D11BlendState>				blend_state;

	ComPtr<ID3D11PixelShader>				pixel_shader;
	ComPtr<ID3D11GeometryShader>			geometry_shader;
	ComPtr<ID3D11VertexShader>				vertex_shader;
	ComPtr<ID3D11InputLayout>				input_layout;

	const size_t							max_vertex;
	std::shared_ptr<innovation_vertex[]>	vertex_data;
	ComPtr<ID3D11Buffer>					vertex_buffer;

	textrue_data							tex_data;
	ComPtr<ID3D11Buffer>					textrue_constant;

	conversion_material						conversion_data;
	ComPtr<ID3D11Buffer>					conversion_constant;

	ComPtr<ID3D11ShaderResourceView>		shader_resource_view;
	D3D11_TEXTURE2D_DESC					texture2d_desc;


public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="device">有効なデバイス</param>
	/// <param name="textrue_name">テクスチャのパス</param>
	/// <param name="_max_vertex">頂点の最大数</param>
	Particle(ID3D11Device* device, const wchar_t* textrue_name, size_t _max_vertex);

	/// <summary>
	/// テクスチャの1チップ当たりの切り取りサイズを設定します。
	/// </summary>
	/// <param name="per_chip">1チップ当たりの切り取りサイズ</param>
	void setTexturePerChip(FLOAT2 per_chip) { tex_data.per_chip = per_chip; }

	/// <summary>
	/// テクスチャの1チップ当たりの切り取りサイズを取得します。
	/// </summary>
	const FLOAT2& getTexturePerChip()const { return tex_data.per_chip; }

	/// <summary>
	/// テクスチャの大きさを取得します。
	/// </summary>
	const FLOAT2& getTextrueSize()const { return tex_data.size; }


	/// <summary>
	/// ビュー行列を設定します。
	/// </summary>
	/// <param name="view">設定するビュー行列</param>
	void setView(FLOAT4X4 view) { conversion_data.view = view; }

	/// <summary>
	/// プロジェクション行列を設定します。
	/// </summary>
	/// <param name="projection">設定するプロジェクション行列</param>
	void setProjection(FLOAT4X4 projection) { conversion_data.projection = projection; }

	/// <summary>
	/// ビュー行列を取得します。
	/// </summary>
	const FLOAT4X4& getView()const { return conversion_data.view; }

	/// <summary>
	/// プロジェクション行列を取得します。
	/// </summary>
	const FLOAT4X4& getProjection()const { return conversion_data.projection; }

	/// <summary>
	/// 最大頂点数を取得します。
	/// </summary>
	const size_t& getMaxVertex()const { return max_vertex; }

	/// <summary>
	/// 頂点の更新をします。
	/// </summary>
	/// <param name="dt">経過時間</param>
	void update(FLOAT dt);

	/// <summary>
	/// アニメションを実行します。
	/// </summary>
	/// <param name="dt">経過時間</param>
	/// <param name="speed">チップ切り替えの速度</param>
	/// <param name="stop_chip_count">チップの最大数</param>
	void animation(FLOAT dt, FLOAT speed, INT stop_chip_count);

	/// <summary>
	/// 頂点データを設定します。
	/// </summary>
	/// <param name="position">初期座標</param>
	/// <param name="size">大きさ</param>
	/// <param name="velocity">初期速度</param>
	/// <param name="acceleration">加速度</param>
	/// <param name="alpha">初期透明度</param>
	/// <param name="attenuation_alpha">透明度の減衰量</param>
	/// <param name="timer">生存タイマー</param>
	/// <param name="chip_num">使用するチップの番号 0スタート</param>
	/// <returns>登録できたらtrue</returns>
	bool setVertex(innovation_vertex data);

	/// <summary>
	/// 頂点データを設定します。
	/// </summary>
	/// <param name="position">初期座標</param>
	/// <param name="size">大きさ</param>
	/// <param name="velocity">初期速度</param>
	/// <param name="acceleration">加速度</param>
	/// <param name="alpha">初期透明度</param>
	/// <param name="attenuation_alpha">透明度の減衰量</param>
	/// <param name="timer">生存タイマー</param>
	/// <param name="chip_num">使用するチップの番号 0スタート</param>
	/// <returns>登録できたらtrue</returns>
	size_t setVertexs(size_t num, std::function<void(innovation_vertex&)> setter);


	/// <summary>
	/// 描画します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	void render(ID3D11DeviceContext* immediate_context);

};

using InnovationVertex = Particle::innovation_vertex;