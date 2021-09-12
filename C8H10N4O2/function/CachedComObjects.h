#pragma once

#include <d3d11.h>
#include <memory>
#include <assert.h>


class CachedComObjects
{
public:
	using Handle = std::unique_ptr<CachedComObjects>;
private:
	template<class T>
	using Com = T*;

	Com<ID3D11InputLayout>			input_layout	= nullptr;
	Com<ID3D11VertexShader>			vertex_shader	= nullptr;
	Com<ID3D11PixelShader>			pixel_shader	= nullptr;
	Com<ID3D11HullShader>			hull_shader		= nullptr;
	Com<ID3D11GeometryShader>		geometry_shader	= nullptr;
	Com<ID3D11DomainShader>			domain_shader	= nullptr;

	Com<ID3D11RenderTargetView>		render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
	Com<ID3D11DepthStencilView>		depth_stencil_view = nullptr;
	Com<ID3D11ShaderResourceView>	shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };

	Com<ID3D11SamplerState>			sampler_states[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { nullptr };
	Com<ID3D11BlendState>			blend_state = nullptr;
	FLOAT							blend_factor[4] = { 1,1,1,1 };
	UINT							sample_mask = 0xffffffff;

	Com<ID3D11RasterizerState>		rasterizer_state = nullptr;
	Com<ID3D11DepthStencilState>	depth_stencil_state = nullptr;
	UINT							stencil_ref = 1;

	D3D_PRIMITIVE_TOPOLOGY			primitive_topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

	D3D11_VIEWPORT					viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
	UINT							number_of_viewport = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

	CachedComObjects() {}
	CachedComObjects(ID3D11DeviceContext* immediate_context)
	{
		immediate_context->IAGetInputLayout(&input_layout);
		immediate_context->VSGetShader(&vertex_shader, nullptr, nullptr);
		immediate_context->PSGetShader(&pixel_shader, nullptr, nullptr);
		immediate_context->HSGetShader(&hull_shader, nullptr, nullptr);
		immediate_context->GSGetShader(&geometry_shader, nullptr, nullptr);
		immediate_context->DSGetShader(&domain_shader, nullptr, nullptr);
		immediate_context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, render_target_views, &depth_stencil_view);
		immediate_context->PSGetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, shader_resource_views);
		immediate_context->PSGetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, sampler_states);
		immediate_context->OMGetBlendState(&blend_state, blend_factor, &sample_mask);
		immediate_context->RSGetState(&rasterizer_state);
		immediate_context->OMGetDepthStencilState(&depth_stencil_state, &stencil_ref);
		immediate_context->IAGetPrimitiveTopology(&primitive_topology);
		immediate_context->RSGetViewports(&number_of_viewport, viewports);
	}

	void pop(ID3D11DeviceContext* immediate_context)const
	{
		immediate_context->IASetInputLayout(input_layout);
		immediate_context->VSSetShader(vertex_shader, nullptr, 0);
		immediate_context->PSSetShader(pixel_shader, nullptr, 0);
		immediate_context->HSSetShader(hull_shader, nullptr, 0);
		immediate_context->GSSetShader(geometry_shader, nullptr, 0);
		immediate_context->DSSetShader(domain_shader, nullptr, 0);
		immediate_context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, render_target_views, depth_stencil_view);
		immediate_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, shader_resource_views);
		immediate_context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, sampler_states);
		immediate_context->OMSetBlendState(blend_state, blend_factor, sample_mask);
		immediate_context->RSSetState(rasterizer_state);
		immediate_context->OMSetDepthStencilState(depth_stencil_state, stencil_ref);
		immediate_context->IASetPrimitiveTopology(primitive_topology);
		immediate_context->RSSetViewports(number_of_viewport, viewports);
	}


	template<class T>
	static void safeRelease(Com<T>& com)
	{
		if (com)com->Release();
	}
	template<class T>
	static void safeCopy(Com<T>& d, Com<T>& s)
	{
		safeRelease(d);
		d = s;
		if (d) d->AddRef();
	}

public:
	~CachedComObjects()
	{
		safeRelease(input_layout);
		safeRelease(vertex_shader);
		safeRelease(pixel_shader);
		safeRelease(hull_shader);
		safeRelease(geometry_shader);
		safeRelease(domain_shader);
		for (Com<ID3D11RenderTargetView>& render_target_view : render_target_views)
		{
			safeRelease(render_target_view);
		}
		safeRelease(depth_stencil_view);
		for (Com<ID3D11ShaderResourceView>& shader_resource_view : shader_resource_views)
		{
			safeRelease(shader_resource_view);
		}
		for (Com<ID3D11SamplerState>& sampler_state : sampler_states)
		{
			safeRelease(sampler_state);
		}
		safeRelease(blend_state);
		safeRelease(rasterizer_state);
		safeRelease(depth_stencil_state);
	}


	/// <summary>
	/// キャッシュされているComオブジェクトを取得します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <returns>
	/// キャッシュされているComオブジェクトの保存ハンドル
	/// </returns>
	friend inline Handle pushCachedComObjects(ID3D11DeviceContext* immediate_context)
	{
		assert(immediate_context && "The context is invalid.");
		return Handle(new CachedComObjects(immediate_context));
	}

	/// <summary>
	/// ハンドルが保存しているキャッシュ情報の復元をします。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="handle">キャッシュされているComオブジェクトの保存ハンドル</param>
	friend inline void popCachedComObjects(ID3D11DeviceContext* immediate_context, Handle& handle)
	{
		assert(immediate_context && "The context is invalid.");
		CachedComObjects* data = handle.release();
		if (data)
		{
			data->pop(immediate_context);
			delete data;
		}
	}


	friend inline Handle copyCachedComObjects(Handle& sauce)
	{
		Handle destination{ new CachedComObjects() };
		safeCopy(destination->input_layout, sauce->input_layout);
		safeCopy(destination->vertex_shader, sauce->vertex_shader);
		safeCopy(destination->pixel_shader, sauce->pixel_shader);
		safeCopy(destination->hull_shader, sauce->hull_shader);
		safeCopy(destination->geometry_shader, sauce->geometry_shader);
		safeCopy(destination->domain_shader, sauce->domain_shader);
		for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		{
			safeCopy(destination->render_target_views[i], sauce->render_target_views[i]);
		}
		safeCopy(destination->depth_stencil_view, sauce->depth_stencil_view);
		for (int i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; i++)
		{
			safeCopy(destination->shader_resource_views[i], sauce->shader_resource_views[i]);
		}
		for (int i = 0; i < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
		{
			safeCopy(destination->sampler_states[i], sauce->sampler_states[i]);
		}
		safeCopy(destination->blend_state, sauce->blend_state);
		safeCopy(destination->rasterizer_state, sauce->rasterizer_state);
		safeCopy(destination->depth_stencil_state, sauce->depth_stencil_state);
		memcpy(destination->blend_factor, sauce->blend_factor, sizeof(FLOAT) * 4);
		memcpy(destination->viewports, sauce->viewports,
			sizeof(D3D11_VIEWPORT) * D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
		destination->sample_mask = sauce->sample_mask;
		destination->stencil_ref = sauce->stencil_ref;
		destination->primitive_topology = sauce->primitive_topology;
		destination->number_of_viewport = sauce->number_of_viewport;
		return destination;
	}
};

using CachedHandle = CachedComObjects::Handle;

