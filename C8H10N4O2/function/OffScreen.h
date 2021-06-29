#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "sprite.h"

class OffScreen
{
public:
	enum class LINK_DESTINATION { RENDER_TARGET, DEPTH_STENCIL };
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	associated_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_traget_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		original_render_traget_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		original_depth_stencil_view;
	std::shared_ptr<Sprite>								renderer;
	const LINK_DESTINATION								link_destination;
	D3D11_VIEWPORT										viewport;
	D3D11_VIEWPORT										original_viewport[8];
	UINT												num_views;
	void _render_traget(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format);
	void _depth_stencil(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format);

public:
	OffScreen(ID3D11Device* device, LINK_DESTINATION link_destination, UINT w, UINT h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool need_renderer = true);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[���N���A���܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="color">�h��Ԃ��F</param>
	void clear(ID3D11DeviceContext* immediate_context, FLOAT4 color = { 0,0,0,0 });

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[��؂�ւ��܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="usu_depth">�f�v�X�X�e���V���r���[��ݒ肷�邩</param>
	void active(ID3D11DeviceContext* immediate_context, bool usu_depth = true);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[�����ɖ߂��܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	void deactive(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�܂��̓f�v�X�X�e���V���r���[�Ɋ֘A�t����ꂽ
	/// �V�F�[�_�[���\�[�X�r���[��S��ʂɕ`�悵�܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="external_pixel_shader">
	/// �K�p����s�N�Z���V�F�[�_�[
	/// NULL�ŋK��̃V�F�[�_�[
	/// </param>
	void quad(ID3D11DeviceContext* immediate_context, ID3D11PixelShader** external_pixel_shader = NULL);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�܂��̓f�v�X�X�e���V���r���[�Ɋ֘A�t����ꂽ
	/// �V�F�[�_�[���\�[�X�r���[���擾���܂��B
	/// </summary>
	ID3D11ShaderResourceView* getAssociatedShaderResourceView()
	{
		return associated_shader_resource_view.Get();
	}

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[���擾���܂��B
	/// </summary>
	ID3D11RenderTargetView* getRenderTargetView()
	{
		return render_traget_view.Get();
	}

	/// <summary>
	/// �f�v�X�X�e���V���r���[���擾���܂��B
	/// </summary>
	ID3D11DepthStencilView* getDepthStencilView()
	{
		return depth_stencil_view.Get();
	}

	/// <summary>
	/// �V�F�[�_�[���\�[�X�r���[�̃����N����擾���܂��B
	/// </summary>
	const LINK_DESTINATION& getLinkDestination()const
	{
		return link_destination;
	}
};