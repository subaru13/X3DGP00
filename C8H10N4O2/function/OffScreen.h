#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "sprite.h"


class OffScreen
{
public:
	enum class LINK_DESTINATION { RENDER_TARGET, DEPTH_STENCIL};
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	associated_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_traget_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		original_render_traget_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		original_depth_stencil_view;
	std::shared_ptr<Sprite>								renderer;
	const LINK_DESTINATION								link_destination;
	void _render_traget(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format);
	void _depth_stencil(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format);

public:
	OffScreen(ID3D11Device* device, LINK_DESTINATION link_destination,UINT w, UINT h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool need_renderer = true);

	/// <summary>
	/// レンダーターゲットビューとデプスステンシルビューをクリアします。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="color">塗りつぶし色</param>
	void clear(ID3D11DeviceContext* immediate_context,FLOAT4 color = { 0,0,0,0 });

	/// <summary>
	/// レンダーターゲットビューとデプスステンシルビューを切り替えます。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="usu_depth">デプスステンシルビューを設定するか</param>
	void active(ID3D11DeviceContext* immediate_context, bool usu_depth = true);

	/// <summary>
	/// レンダーターゲットビューとデプスステンシルビューを元に戻します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	void deactive(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// レンダーターゲットビューまたはデプスステンシルビューに関連付けられた
	/// シェーダーリソースビューを全画面に描画します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="external_pixel_shader">
	/// 適用するピクセルシェーダー
	/// NULLで規定のシェーダー
	/// </param>
	void quad(ID3D11DeviceContext* immediate_context, ID3D11PixelShader** external_pixel_shader = NULL);

	/// <summary>
	/// レンダーターゲットビューまたはデプスステンシルビューに関連付けられた
	/// シェーダーリソースビューを取得します。
	/// </summary>
	ID3D11ShaderResourceView* getAssociatedShaderResourceView()
	{
		return associated_shader_resource_view.Get();
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
	/// シェーダーリソースビューのリンク先を取得します。
	/// </summary>
	const LINK_DESTINATION& getLinkDestination()const
	{
		return link_destination;
	}

};