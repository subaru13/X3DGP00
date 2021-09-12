#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include "Arithmetic.h"

class FrameBuffer
{
public:
	struct CONFIG
	{
		UINT width;
		UINT height;
		DXGI_FORMAT render_traget_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT depth_stencil_format = DXGI_FORMAT_R24G8_TYPELESS;
	};
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
	const CONFIG										config;
protected:
	void createRenderTraget(ID3D11Device* device);
	void createDepthStencil(ID3D11Device* device);
public:
	FrameBuffer(ID3D11Device* device, const CONFIG& config);

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
	const CONFIG& getConfig()const { return config; }

};

using FB_CONFIG = FrameBuffer::CONFIG;

class GeometryBuffer
{
public:
	static constexpr UINT SCREEN_COUNT = 3;
	struct CONFIG
	{
		UINT width;
		UINT height;
		DXGI_FORMAT render_traget_formats[SCREEN_COUNT] = { DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM };
		DXGI_FORMAT depth_stencil_format = DXGI_FORMAT_R24G8_TYPELESS;
	};
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	render_traget_shader_resource_views[SCREEN_COUNT];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	depth_stencil_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_traget_views[SCREEN_COUNT];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		original_render_traget_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		original_depth_stencil_view;
	D3D11_VIEWPORT										viewport;
	D3D11_VIEWPORT										original_viewport[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	UINT												num_views;
	const CONFIG										config;
protected:
	void createRenderTragets(ID3D11Device* device);
	void createDepthStencil(ID3D11Device* device);
public:
	GeometryBuffer(ID3D11Device* device, const CONFIG& config);

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
	std::vector<ID3D11ShaderResourceView*> getRenderTragetShaderResourceViews()
	{
		std::vector<ID3D11ShaderResourceView*> _render_traget_shader_resource_views{};
		for (auto rtsrv : render_traget_shader_resource_views)
		{
			_render_traget_shader_resource_views.push_back(rtsrv.Get());
		}
		return _render_traget_shader_resource_views;
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
	std::vector<ID3D11RenderTargetView*> getRenderTargetViews()
	{
		std::vector<ID3D11RenderTargetView*> _render_traget_views{};
		for (auto rtv : render_traget_views)
		{
			_render_traget_views.push_back(rtv.Get());
		}
		return _render_traget_views;
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
	const CONFIG& getConfig()const { return config; }

};

using GB_CONFIG = GeometryBuffer::CONFIG;

class FullScreenQuad
{
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
public:
	FullScreenQuad(ID3D11Device* device, const char* ps_filename = NULL);

	/// <summary>
	/// 全画面にSRVを描画します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="shader_resource_view">シェーダーリソースビュー</param>
	/// <param name="external_pixel_shader">
	/// 適用するピクセルシェーダー
	/// NULLで規定のシェーダー
	/// </param>
	void blit(ID3D11DeviceContext* immediate_context,
		ID3D11ShaderResourceView* shader_resource_view,
		ID3D11PixelShader** external_pixel_shader = NULL)const;

	/// <summary>
	/// 全画面にSRVを描画します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="shader_resource_views">シェーダーリソースビュー配列</param>
	/// <param name="shader_resource_views_count">シェーダーリソースビュー配列の要素数</param>
	/// <param name="external_pixel_shader">
	/// 適用するピクセルシェーダー
	/// NULLで規定のシェーダー
	/// </param>
	void blit(ID3D11DeviceContext* immediate_context,
		ID3D11ShaderResourceView** shader_resource_views,
		UINT shader_resource_views_count,
		ID3D11PixelShader** external_pixel_shader = NULL)const;
};