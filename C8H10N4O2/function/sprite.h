#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include "Arithmetic.h"
#include "ConstantBuffer.h"

using namespace Microsoft::WRL;

class Sprite
{
public:
	struct vertex
	{
		FLOAT3 position;
		FLOAT2 texcoord;
	};
private:
	ComPtr<ID3D11PixelShader>			pixel_shader;
	ComPtr<ID3D11VertexShader>			vertex_shader;
	ComPtr<ID3D11InputLayout>			input_layout;
	ComPtr<ID3D11Buffer>				vertex_buffer;
	ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	ConstantBuffer<FLOAT4>				constant_buffer;
	FLOAT2								textrue_size;

	Sprite(Sprite&) = delete;
	Sprite& operator =(Sprite&) = delete;
public:

	Sprite(ID3D11Device* device, const wchar_t* file_name = L"\0");
	Sprite(ID3D11Device* device, ID3D11ShaderResourceView* new_shader_resource_view);
	virtual ~Sprite() = default;

	/// <summary>
	/// シェーダーリソースビューを結びつけます
	/// </summary>
	/// <param name="new_shader_resource_view">シェーダーリソースビュー</param>
	/// <returns>成功すればtrue</returns>
	bool attach(ID3D11ShaderResourceView* new_shader_resource_view);

	/// <summary>
	/// シェーダーリソースビューを取得します。
	/// </summary>
	ID3D11ShaderResourceView* getShaderResourceView() { return shader_resource_view.Get(); }

	/// <summary>
	/// スプライトを表示します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="external_pixel_shader">
	/// 適用するピクセルシェーダー
	/// NULLで規定のシェーダー
	/// </param>
	/// <param name="pos">表示座標</param>
	/// <param name="size">表示サイズ</param>
	/// <param name="texpos">テクスチャの切り取り開始位置</param>
	/// <param name="texsize">テクスチャの切り取りサイズ</param>
	/// <param name="angle">回転角度(ラジアン)</param>
	/// <param name="color">表示色</param>
	void render(ID3D11DeviceContext* immediate_context,
		ID3D11PixelShader** external_pixel_shader,
		FLOAT2 pos,
		FLOAT2 size,
		FLOAT2 texpos = {},
		FLOAT2 texsize = {},
		float angle = 0.0f,
		FLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });

	/// <summary>
	/// 画面全体に描画します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="external_pixel_shader">
	/// 適用するピクセルシェーダー
	/// NULLで規定のシェーダー
	/// </param>
	/// <param name="texpos">テクスチャの切り取り開始位置</param>
	/// <param name="texsize">テクスチャの切り取りサイズ</param>
	/// <param name="color">表示色</param>
	void quad(ID3D11DeviceContext* immediate_context,
		ID3D11PixelShader** external_pixel_shader,
		FLOAT2 texpos = {},
		FLOAT2 texsize = {},
		FLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });

	/// <summary>
	/// テクスチャの大きさを取得します。
	/// </summary>
	const FLOAT2& getTextrueSize()const { return textrue_size; }

	/// <summary>
	/// テクスチャをシェーダーに送信します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="slot">送信スロット番号</param>
	/// <param name="use_vs">頂点シェーダーに使用するか</param>
	/// <param name="use_ps">ピクセルシェーダーに使用するか</param>
	/// <param name="use_ds">ドメインシェーダーに使用するか</param>
	/// <param name="use_hs">ハルシェーダーに使用するか</param>
	/// <param name="use_gs">ジオメトリシェーダーに使用するか</param>
	void sendTextrue(
		ID3D11DeviceContext* immediate_context,
		UINT slot,
		bool use_vs = true,
		bool use_ps = true,
		bool use_ds = true,
		bool use_hs = true,
		bool use_gs = true)const;
};