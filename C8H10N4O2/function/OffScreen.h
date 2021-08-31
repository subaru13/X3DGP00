#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Arithmetic.h"

struct OFFSCREEN_CONFIG
{
	UINT width;
	UINT height;
	DXGI_FORMAT render_traget_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT depth_stencil_format = DXGI_FORMAT_R24G8_TYPELESS;
};

class OffScreen
{
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	render_traget_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	depth_stencil_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_traget_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		original_render_traget_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		original_depth_stencil_view;
	D3D11_VIEWPORT										viewport;
	D3D11_VIEWPORT										original_viewport[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	UINT												num_views;
	const OFFSCREEN_CONFIG								config;
protected:
	void _render_traget(ID3D11Device* device);
	void _depth_stencil(ID3D11Device* device);
public:
	OffScreen(ID3D11Device* device, const OFFSCREEN_CONFIG& config);

	/// <summary>
	/// レンダーターゲットビューとデプスステンシルビューをクリアします。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="color">塗りつぶし色</param>
	void clear(ID3D11DeviceContext* immediate_context, FLOAT4 color = { 0,0,0,0 });

	/// <summary>
	/// レンダーターゲットビューとデプスステンシルビューを切り替えます。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	void active(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// レンダーターゲットビューとデプスステンシルビューを元に戻します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	void deactive(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// レンダーターゲットビューに関連付けられた
	/// シェーダーリソースビューを取得します。
	/// </summary>
	ID3D11ShaderResourceView* getRenderTragetShaderResourceView()
	{
		return render_traget_shader_resource_view.Get();
	}

	/// <summary>
	/// デプスステンシルビューに関連付けられた
	/// シェーダーリソースビューを取得します。
	/// </summary>
	ID3D11ShaderResourceView* getDepthStencilShaderResourceView()
	{
		return depth_stencil_shader_resource_view.Get();
	}

	/// <summary>
	/// レンダーターゲットビューを取得します。
	/// </summary>
	ID3D11RenderTargetView* getRenderTargetView()
	{
		return render_traget_view.Get();
	}

	/// <summary>
	/// デプスステンシルビューを取得します。
	/// </summary>
	ID3D11DepthStencilView* getDepthStencilView()
	{
		return depth_stencil_view.Get();
	}

	/// <summary>
	/// オフスクリーンの構成を取得します。
	/// </summary>
	const OFFSCREEN_CONFIG& getConfig()const { return config; }

};


class FullScreenQuad
{
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
public:
	FullScreenQuad(ID3D11Device* device, const char* ps_filename = NULL);
	void blit(ID3D11DeviceContext* immediate_context,
		ID3D11ShaderResourceView* shader_resource_view,
		ID3D11PixelShader** external_pixel_shader = NULL)const;

	void blit(ID3D11DeviceContext* immediate_context,
		ID3D11ShaderResourceView** shader_resource_views,
		UINT shader_resource_views_count,
		ID3D11PixelShader** external_pixel_shader = NULL)const;
};