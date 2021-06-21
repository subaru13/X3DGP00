#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <vector>
#include "Arithmetic.h"
#include "ConstantBuffer.h"

using namespace Microsoft::WRL;

class SpriteBatch final
{
public:
	struct vertex
	{
		FLOAT3 position;
		FLOAT2 texcoord;
		FLOAT4 color;
	};
private:
	ComPtr<ID3D11PixelShader>			pixel_shader;
	ComPtr<ID3D11VertexShader>			vertex_shader;
	ComPtr<ID3D11InputLayout>			input_layout;
	ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	FLOAT2								textrue_size;
	ComPtr<ID3D11Buffer>				vertex_buffer;
	const size_t						max_vertices;
	std::vector<vertex>					vertices;
	SpriteBatch(SpriteBatch&) = delete;
	SpriteBatch& operator =(SpriteBatch&) = delete;
public:
	SpriteBatch(ID3D11Device* device,size_t max_sprites, const wchar_t* file_name = nullptr);

	/// <summary>
	/// 描画の開始処理
	/// ここでシェーダーとSRVを設定している。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="external_pixel_shader">
	/// 適用するピクセルシェーダー
	/// NULLで規定のシェーダー
	/// </param>
	void begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader** external_pixel_shader = NULL);
	/// <summary>
	/// 描画するデータの書き込み
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="pos">表示座標</param>
	/// <param name="size">表示サイズ</param>
	/// <param name="texpos">テクスチャの切り取り開始位置</param>
	/// <param name="texsize">テクスチャの切り取りサイズ</param>
	/// <param name="angle">回転角度(ラジアン)</param>
	/// <param name="color">表示色</param>
	void render(ID3D11DeviceContext* immediate_context,
		FLOAT2 pos, 
		FLOAT2 size,
		FLOAT2 texpos = {},
		FLOAT2 texsize = {},
		float angle = 0.0f,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });
	/// <summary>
	/// 実際に描画している場所
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	void end(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// テクスチャのサイズを取得します。
	/// </summary>
	/// <returns></returns>
	const FLOAT2& getTextrueSize()const { return textrue_size; }

	virtual ~SpriteBatch() = default;
};
