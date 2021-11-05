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
		UINT		width					= 0;
		UINT		height					= 0;
		DXGI_FORMAT	render_target_format	= DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT	depth_stencil_format	= DXGI_FORMAT_R24G8_TYPELESS;
		BOOL		is_cube_map				= FALSE;
	};
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	render_target_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	depth_stencil_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		original_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		original_depth_stencil_view;
	D3D11_VIEWPORT										viewport;
	D3D11_VIEWPORT										original_viewport[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	UINT												num_views;
	CONFIG												config;
protected:
	void createRenderTarget(ID3D11Device* device);
	void createDepthStencil(ID3D11Device* device);
	void createRenderTargetCube(ID3D11Device* device);
	void createDepthStencilCube(ID3D11Device* device);
public:
	FrameBuffer(ID3D11Device* device, const CONFIG& config);

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
	void active(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[�����ɖ߂��܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	void deactive(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�Ɋ֘A�t����ꂽ
	/// �V�F�[�_�[���\�[�X�r���[���擾���܂��B
	/// </summary>
	ID3D11ShaderResourceView* getRenderTargetShaderResourceView()
	{
		return render_target_shader_resource_view.Get();
	}

	/// <summary>
	/// �f�v�X�X�e���V���r���[�Ɋ֘A�t����ꂽ
	/// �V�F�[�_�[���\�[�X�r���[���擾���܂��B
	/// </summary>
	ID3D11ShaderResourceView* getDepthStencilShaderResourceView()
	{
		return depth_stencil_shader_resource_view.Get();
	}

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[���擾���܂��B
	/// </summary>
	ID3D11RenderTargetView* getRenderTargetView()
	{
		return render_target_view.Get();
	}

	/// <summary>
	/// �f�v�X�X�e���V���r���[���擾���܂��B
	/// </summary>
	ID3D11DepthStencilView* getDepthStencilView()
	{
		return depth_stencil_view.Get();
	}

	/// <summary>
	/// �I�t�X�N���[���̍\�����擾���܂��B
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
		UINT		width								= 0;
		UINT		height								= 0;
		DXGI_FORMAT	render_target_formats[SCREEN_COUNT] = { DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM };
		DXGI_FORMAT	depth_stencil_format				= DXGI_FORMAT_R24G8_TYPELESS;
		BOOL		is_cube_map							= FALSE;
	};
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	render_target_shader_resource_views[SCREEN_COUNT];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	depth_stencil_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_target_views[SCREEN_COUNT];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		original_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		original_depth_stencil_view;
	D3D11_VIEWPORT										viewport;
	D3D11_VIEWPORT										original_viewport[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	UINT												num_views;
	CONFIG												config;
protected:
	void createRenderTargets(ID3D11Device* device);
	void createDepthStencil(ID3D11Device* device);
	void createRenderTargetsCube(ID3D11Device* device);
	void createDepthStencilCube(ID3D11Device* device);
public:
	GeometryBuffer(ID3D11Device* device, const CONFIG& config);

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
	void active(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[�����ɖ߂��܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	void deactive(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�Ɋ֘A�t����ꂽ
	/// �V�F�[�_�[���\�[�X�r���[���擾���܂��B
	/// </summary>
	std::vector<ID3D11ShaderResourceView*> getRenderTargetShaderResourceViews()
	{
		std::vector<ID3D11ShaderResourceView*> _render_target_shader_resource_views{};
		for (auto rtsrv : render_target_shader_resource_views)
		{
			_render_target_shader_resource_views.push_back(rtsrv.Get());
		}
		return _render_target_shader_resource_views;
	}

	/// <summary>
	/// �f�v�X�X�e���V���r���[�Ɋ֘A�t����ꂽ
	/// �V�F�[�_�[���\�[�X�r���[���擾���܂��B
	/// </summary>
	ID3D11ShaderResourceView* getDepthStencilShaderResourceView()
	{
		return depth_stencil_shader_resource_view.Get();
	}

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[���擾���܂��B
	/// </summary>
	std::vector<ID3D11RenderTargetView*> getRenderTargetViews()
	{
		std::vector<ID3D11RenderTargetView*> _render_target_views{};
		for (auto rtv : render_target_views)
		{
			_render_target_views.push_back(rtv.Get());
		}
		return _render_target_views;
	}

	/// <summary>
	/// �f�v�X�X�e���V���r���[���擾���܂��B
	/// </summary>
	ID3D11DepthStencilView* getDepthStencilView()
	{
		return depth_stencil_view.Get();
	}

	/// <summary>
	/// �I�t�X�N���[���̍\�����擾���܂��B
	/// </summary>
	const CONFIG& getConfig()const { return config; }

};

using GB_CONFIG = GeometryBuffer::CONFIG;

class Peinter
{
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
public:
	Peinter(ID3D11Device* device, const char* ps_filename = NULL);

	/// <summary>
	/// �S��ʂ�SRV��`�悵�܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="shader_resource_view">�V�F�[�_�[���\�[�X�r���[</param>
	/// <param name="external_pixel_shader">
	/// �K�p����s�N�Z���V�F�[�_�[
	/// NULL�ŋK��̃V�F�[�_�[
	/// </param>
	void blit(ID3D11DeviceContext* immediate_context,
		ID3D11ShaderResourceView* shader_resource_view,
		ID3D11PixelShader** external_pixel_shader = NULL)const;

	/// <summary>
	/// �S��ʂ�SRV��`�悵�܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="shader_resource_views">�V�F�[�_�[���\�[�X�r���[�z��</param>
	/// <param name="shader_resource_views_count">�V�F�[�_�[���\�[�X�r���[�z��̗v�f��</param>
	/// <param name="external_pixel_shader">
	/// �K�p����s�N�Z���V�F�[�_�[
	/// NULL�ŋK��̃V�F�[�_�[
	/// </param>
	void blit(ID3D11DeviceContext* immediate_context,
		ID3D11ShaderResourceView** shader_resource_views,
		UINT shader_resource_views_count,
		ID3D11PixelShader** external_pixel_shader = NULL)const;
};