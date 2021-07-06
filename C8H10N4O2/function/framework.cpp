#include "framework.h"
#include "SceneManager.h"
#include "key_input.h"
Framework* Framework::instance = nullptr;

void ClearComObjectCache();
void Initialization_Process_Before_Loop(ID3D11Device*);
void Update_Process(float);
void Drawing_Process(ID3D11DeviceContext*, float);
void End_Process_After_The_Loop_ends();

#ifdef _DEBUG
void Debug_Mode_Initialization_Process_Before_Loop(ID3D11Device*);
void Debug_Mode_Update_Process(float);
void Debug_Mode_Drawing_Process(ID3D11DeviceContext*, float);
void Debug_Mode_End_Process_After_The_Loop_ends();
#endif

Framework::Framework(HWND hwnd)
	:hwnd(hwnd), d3d11_device(nullptr), d3d11_context(nullptr),
	d3d11_render_view(nullptr), d3d11_depth_view(nullptr),
	idxgi_swapchain(nullptr), d3d11_blend_states(nullptr)
{
	assert(instance == nullptr && "No more instances can be created.");
	instance = this;

	HRESULT hr{ S_OK };

	hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//Creating a device and swap chain
	{
		UINT create_device_flags{ 0 };
#ifdef _DEBUG
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };
		DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
		swap_chain_desc.BufferCount = 1;
		swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;
		swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.OutputWindow = hwnd;
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
#ifdef _DEBUG
		swap_chain_desc.Windowed = TRUE;
#else
		swap_chain_desc.Windowed = !FULLSCREEN;
#endif
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags,
			&feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc,
			idxgi_swapchain.ReleaseAndGetAddressOf(),
			d3d11_device.ReleaseAndGetAddressOf(),
			NULL, d3d11_context.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//Creating a render target view
	{
		ComPtr<ID3D11Texture2D> back_buffer{};
		hr = idxgi_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer.ReleaseAndGetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = d3d11_device->CreateRenderTargetView(back_buffer.Get(), NULL, d3d11_render_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//Creating a depth stencil view
	{
		ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = SCREEN_WIDTH;
		texture2d_desc.Height = SCREEN_HEIGHT;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = d3d11_device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = texture2d_desc.Format;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;
		hr = d3d11_device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, d3d11_depth_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//Viewport settings
	{
		d3d11_viewport.TopLeftX = 0;
		d3d11_viewport.TopLeftY = 0;
		d3d11_viewport.Width = static_cast<float>(SCREEN_WIDTH);
		d3d11_viewport.Height = static_cast<float>(SCREEN_HEIGHT);
		d3d11_viewport.MinDepth = 0.0f;
		d3d11_viewport.MaxDepth = 1.0f;
		d3d11_context->RSSetViewports(1, &d3d11_viewport);
	}
	d3d11_blend_states = std::make_unique<BlendStates>(d3d11_device.Get());
	d3d11_context->OMSetBlendState(d3d11_blend_states->at(BLEND_STATE::BS_ALPHA), nullptr, 0xFFFFFFFF);
	d3d11_depth_stencil_states = std::make_unique<DepthStencilStates>(d3d11_device.Get());
	d3d11_context->OMSetDepthStencilState(d3d11_depth_stencil_states->at(DEPTH_STENCIL_STATE::DS_FALSE), 1);
	d3d11_rasterizer_states = std::make_unique<RasterizerStates>(d3d11_device.Get());
	d3d11_context->RSSetState(d3d11_rasterizer_states->at(RASTERIZER_FILL::RS_SOLID, FALSE));
	d3d11_sampler_states = std::make_unique<SamplerStates>(d3d11_device.Get());
	d3d11_context->PSSetSamplers(0, 1, instance->d3d11_sampler_states->at(SAMPLER_STATE::SS_POINT));
}

bool Framework::initialize()
{
	Initialization_Process_Before_Loop(d3d11_device.Get());
#ifdef _DEBUG
	Debug_Mode_Initialization_Process_Before_Loop(d3d11_device.Get());
#endif
	return true;
}

void Framework::update()
{
#if USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
	KeyManager::instance()->Update();
	float elapsed_time = tictoc.time_interval();
	SceneManager::getInstance()->edit(d3d11_device.Get(), elapsed_time);
	Update_Process(elapsed_time);
#ifdef _DEBUG
	Debug_Mode_Update_Process(elapsed_time);
#endif
}

void Framework::render()
{
	HRESULT hr{ S_OK };

	d3d11_context->ClearRenderTargetView(d3d11_render_view.Get(), FILL_COLOR);
	d3d11_context->ClearDepthStencilView(d3d11_depth_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	d3d11_context->OMSetRenderTargets(1, d3d11_render_view.GetAddressOf(), d3d11_depth_view.Get());

	ID3D11ShaderResourceView* null_srv[128] = { nullptr };

	d3d11_context->PSSetShaderResources(0, 128, null_srv);

	float elapsed_time = tictoc.time_interval();
	SceneManager::getInstance()->render(d3d11_context.Get(), elapsed_time);
	Drawing_Process(d3d11_context.Get(), elapsed_time);
#ifdef _DEBUG
	Debug_Mode_Drawing_Process(d3d11_context.Get(), elapsed_time);
#endif

#if USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	hr = idxgi_swapchain->Present(SYNC_INTERVAL, 0);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

bool Framework::uninitialize()
{
	SceneManager::getInstance()->release();
	End_Process_After_The_Loop_ends();
#ifdef _DEBUG
	Debug_Mode_End_Process_After_The_Loop_ends();
#endif
	return true;
}

Framework::~Framework()
{
	assert(instance != nullptr && "No Instance.");
	instance = nullptr;
	ClearComObjectCache();
	CoUninitialize();
}