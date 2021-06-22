#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "sprite.h"

class OffScreen
{
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	associated_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_traget_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		original_render_traget_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		original_depth_stencil_view;
	std::shared_ptr<Sprite>								renderer;
public:
	OffScreen(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool need_renderer = true);

	void clear(ID3D11DeviceContext* immediate_context,FLOAT4 color = { 0,0,0,0 });

	void active(ID3D11DeviceContext* immediate_context);
	void deactive(ID3D11DeviceContext* immediate_context);

	void quad(ID3D11DeviceContext* immediate_context, ID3D11PixelShader** external_pixel_shader = NULL);

	ID3D11ShaderResourceView* getAssociatedShaderResourceView()
	{
		return associated_shader_resource_view.Get();
	}

	ID3D11RenderTargetView* getRenderTargetView()
	{
		return render_traget_view.Get();
	}

};