#pragma once
#include "Arithmetic.h"
#include "ConstantBuffer.h"
class SceneConstant
{
private:
	struct data
	{
		FLOAT4X4	view_projection;
		FLOAT4		light_direction;
		FLOAT4		camera_position;
	};

	ConstantBuffer<data> constant_buffer;
	FLOAT4X4	view;
	FLOAT4X4	projection;
public:
	SceneConstant(ID3D11Device* device);
	virtual ~SceneConstant() = default;
	/// <summary>
	/// ライトの向きを設定します。
	/// </summary>
	/// <param name="light_direction">ライトの向き</param>
	void setLightDirection(FLOAT4 light_direction) { constant_buffer.data.light_direction = light_direction; }
	/// <summary>
	/// ビュー行列を設定します。
	/// </summary>
	/// <param name="view">ビュー行列</param>
	void setView(FLOAT4X4 view) { this->view = view; }
	/// <summary>
	/// プロジェクション行列を設定します。
	/// </summary>
	/// <param name="projection">プロジェクション行列</param>
	void setProjection(FLOAT4X4 projection) { this->projection = projection; }
	/// <summary>
	/// カメラの座標を設定します。
	/// </summary>
	/// <param name="camera_position">カメラの座標</param>
	void setCameraPosition(FLOAT4 camera_position) { constant_buffer.data.camera_position = camera_position; }

	/// <summary>
	/// ライトの向きを取得します。
	/// </summary>
	const FLOAT4& getLightDirection()const { return constant_buffer.data.light_direction; }
	/// <summary>
	/// ビュー行列を取得します。
	/// </summary>
	const FLOAT4X4& getView()const { return view; }

	/// <summary>
	/// プロジェクション行列を取得します。
	/// </summary>
	const FLOAT4X4& getProjection()const { return projection; }
	/// <summary>
	/// カメラの座標を取得します。
	/// </summary>
	/// <returns></returns>
	const FLOAT4& setCameraPosition()const { return constant_buffer.data.camera_position; }

	/// <summary>
	/// シーンデータをシェーダーに送信します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	void send(ID3D11DeviceContext* immediate_context);
};

struct View360Degree
{
	FLOAT4X4 view_matrices[6] = {};
	FLOAT4X4 projection_matrix;
	View360Degree() : view_matrices{}, projection_matrix(){setPosition({ 0,0,0 }); }
	void setPosition(const FLOAT3& position);
};
