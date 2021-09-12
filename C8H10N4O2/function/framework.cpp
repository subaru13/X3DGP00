#include "Framework.h"
#include "SceneManager.h"
#include "KeyInput.h"
#include <DirectXTK/ScreenGrab.h>
#include <wincodec.h>
#include <direct.h>
#include <filesystem>
Framework* Framework::instance = nullptr;

void ClearComObjectCache();
void initializationProcessBeforeLoop(ID3D11Device*);
void updateProcess(float);
void drawingProcess(ID3D11DeviceContext*, float);
void endProcessAfterTheLoopEnds();

#ifdef _DEBUG
void debugModeInitializationProcessBeforeLoop(ID3D11Device*);
void debugModeUpdateProcess(float);
void debugModeDrawingProcess(ID3D11DeviceContext*, float);
void debugModeEndProcessAfterTheLoopEnds();
#endif

Framework::Framework(HWND hwnd)
	:hwnd(hwnd), d3d11_device(nullptr), d3d11_context(nullptr),
	d3d11_render_traget_view(nullptr), d3d11_depth_stencil_view(nullptr),
	idxgi_swapchain(nullptr), d3d11_blend_states(nullptr), screenshot_key(nullptr)
{
	assert(instance == nullptr && "No more instances can be created.");
	instance = this;

	HRESULT hr{ S_OK };

	hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

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
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
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
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	//Creating a render target view
	{
		ComPtr<ID3D11Texture2D> back_buffer{};
		hr = idxgi_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer.ReleaseAndGetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		hr = d3d11_device->CreateRenderTargetView(back_buffer.Get(), NULL, d3d11_render_traget_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
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
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = texture2d_desc.Format;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;
		hr = d3d11_device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, d3d11_depth_stencil_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
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
	d3d11_context->RSSetState(d3d11_rasterizer_states->at(RASTERIZER_FILL::BACK_SOLID, FALSE));
	d3d11_sampler_states = std::make_unique<SamplerStates>(d3d11_device.Get());
	d3d11_context->PSSetSamplers(0, 1, instance->d3d11_sampler_states->at(SAMPLER_STATE::SS_POINT));
#if  USE_SCREEN_SHOT
	screenshot_key = std::make_unique<Key>(VK_SNAPSHOT);
#endif //  USE_SCREEN_SHOT

}

void Framework::saveJpeg(const WCHAR* file_name, ID3D11Resource* resource)
{
	assert(instance != nullptr && "No Instance.");
	HRESULT hr = SaveWICTextureToFile(instance->d3d11_context.Get(), resource, GUID_ContainerFormatJpeg, file_name);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void Framework::saveJpeg(const WCHAR* file_name, ID3D11RenderTargetView* render_target_view)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	render_target_view->GetResource(resource.ReleaseAndGetAddressOf());
	Framework::saveJpeg(file_name, resource.Get());
}

void Framework::saveJpeg(const WCHAR* file_name, ID3D11DepthStencilView* depth_stencil_view)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	depth_stencil_view->GetResource(resource.ReleaseAndGetAddressOf());
	Framework::saveJpeg(file_name, resource.Get());
}

void Framework::saveJpeg(const WCHAR* file_name, ID3D11ShaderResourceView* shader_resource_view)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	shader_resource_view->GetResource(resource.ReleaseAndGetAddressOf());
	Framework::saveJpeg(file_name, resource.Get());
}

bool Framework::initialize()
{
	initializationProcessBeforeLoop(d3d11_device.Get());
#ifdef _DEBUG
	debugModeInitializationProcessBeforeLoop(d3d11_device.Get());
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
	KeyManager::instance()->update();
	Mouse::instance()->update(hwnd);
#if  USE_SCREEN_SHOT
	if (screenshot_key->down())
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		d3d11_render_traget_view->GetResource(resource.ReleaseAndGetAddressOf());
		time_t t = time(NULL);
		tm time_stamp{};
		errno_t err = localtime_s(&time_stamp, &t);
		int mkr = _mkdir("./ScreenShot//");
		std::wstringstream name;
		name << L"./ScreenShot//" << time_stamp.tm_year + 1900 << L"_" <<
			time_stamp.tm_mon + 1 << L"_" << time_stamp.tm_mday
			<< L"_" << time_stamp.tm_hour << L"_" <<
			time_stamp.tm_min << L"_" << time_stamp.tm_sec << L".jpg";
		HRESULT hr = SaveWICTextureToFile(d3d11_context.Get(), resource.Get(), GUID_ContainerFormatJpeg, name.str().c_str());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}
#endif //  USE_SCREEN_SHOT

	float elapsed_time = tictoc.timeInterval();
	SceneManager::getInstance()->edit(d3d11_device.Get(), elapsed_time);
	updateProcess(elapsed_time);
#ifdef _DEBUG
	debugModeUpdateProcess(elapsed_time);
#endif
}

void Framework::render()
{
	HRESULT hr{ S_OK };

	d3d11_context->ClearRenderTargetView(d3d11_render_traget_view.Get(), FILL_COLOR);
	d3d11_context->ClearDepthStencilView(d3d11_depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	d3d11_context->OMSetRenderTargets(1, d3d11_render_traget_view.GetAddressOf(), d3d11_depth_stencil_view.Get());

	ID3D11ShaderResourceView* null_srv[16] = { nullptr };

	d3d11_context->PSSetShaderResources(0, 16, null_srv);

	float elapsed_time = tictoc.timeInterval();
	SceneManager::getInstance()->render(d3d11_context.Get(), elapsed_time);
	drawingProcess(d3d11_context.Get(), elapsed_time);
#ifdef _DEBUG
	debugModeDrawingProcess(d3d11_context.Get(), elapsed_time);
#endif

#if USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	hr = idxgi_swapchain->Present(SYNC_INTERVAL, 0);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

bool Framework::uninitialize()
{
	SceneManager::getInstance()->release();
	endProcessAfterTheLoopEnds();
#ifdef _DEBUG
	debugModeEndProcessAfterTheLoopEnds();
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