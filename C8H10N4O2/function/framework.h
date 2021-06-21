#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <assert.h>

#include "misc.h"
#include "high_resolution_timer.h"
#include "pBlendState.h"
#include "pDepthStencilState.h"
#include "pRasterizerState.h"
#include "ConstantBuffer.h"
#include "Arithmetic.h"
#include "../FrameworkConfig.h"

#if USE_IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif


using namespace Microsoft::WRL;

enum SAMPLER_STATE { SS_POINT, SS_LINEAR, SS_ANISOTROPIC };

class Framework final
{

private:
	static Framework*						instance;

	HWND hwnd;
	
	ComPtr<ID3D11Device>				d3d11_device;
	ComPtr<ID3D11DeviceContext>			d3d11_context;
	ComPtr<ID3D11RenderTargetView>		d3d11_render_view;
	ComPtr<ID3D11DepthStencilView>		d3d11_depth_view;
	std::unique_ptr<BlendStates>		d3d11_blend_states;
	std::unique_ptr<DepthStencilStates>	d3d11_depth_stencil_states;
	std::unique_ptr<RasterizerStates>	d3d11_rasterizer_states;
	ComPtr<ID3D11SamplerState>			d3d11_sampler_states[3];
	ComPtr<IDXGISwapChain>				idxgi_swapchain;

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;
public:

	Framework(HWND hwnd);
	~Framework();

	/// <summary>
	/// デバイスを取得します。
	/// </summary>
	static ID3D11Device* getDevice()
	{
		assert(instance != nullptr && "No Instance.");
		return instance->d3d11_device.Get();
	}

	/// <summary>
	/// コンテキストを取得します。
	/// </summary>
	static ID3D11DeviceContext* getDeviceContext()
	{
		assert(instance != nullptr && "No Instance.");
		return instance->d3d11_context.Get();
	}

	/// <summary>
	/// ウィンドウを取得します。
	/// </summary>
	static HWND getHwnd()
	{
		assert(instance != nullptr && "No Instance.");
		return instance->hwnd;
	}

	/// <summary>
	/// レンダーターゲットビューを取得します。
	/// </summary>
	static ID3D11RenderTargetView* getRenderTargetView()
	{
		assert(instance != nullptr && "No Instance.");
		return instance->d3d11_render_view.Get();
	}

	/// <summary>
	/// デプスステンシルビューを取得します。
	/// </summary>
	static ID3D11DepthStencilView* getDepthStencilView()
	{
		assert(instance != nullptr && "No Instance.");
		return instance->d3d11_depth_view.Get();
	}

	/// <summary>
	/// ブレンドステートを設定します。
	/// </summary>
	/// <param name="type">ブレンドステートの種類</param>
	static void setBlendState(BLEND_STATE type)
	{
		assert(instance != nullptr && "No Instance.");
		instance->d3d11_context->OMSetBlendState(instance->d3d11_blend_states->at(type), nullptr, 0xFFFFFFFF);
	}

	/// <summary>
	/// デプスステンシルステートを設定します。
	/// </summary>
	/// <param name="type">デプスステンシルステートの種類</param>
	static void setDepthStencilState(DEPTH_STENCIL_STATE type)
	{
		assert(instance != nullptr && "No Instance.");
		instance->d3d11_context->OMSetDepthStencilState(instance->d3d11_depth_stencil_states->at(type), 1);
	}

	/// <summary>
	/// サンプラーステートを設定します。
	/// </summary>
	/// <param name="type">サンプラーステートの種類</param>
	/// <param name="slot">セットするスロット</param>
	static void setSamplerState(SAMPLER_STATE type, UINT slot = 0)
	{
		assert(instance != nullptr && "No Instance.");
		instance->d3d11_context->PSSetSamplers(slot, 1, instance->d3d11_sampler_states[type].GetAddressOf());
	}

	/// <summary>
	/// ラスタライザーステートを設定します。
	/// </summary>
	/// <param name="type">フィルモードの種類</param>
	/// <param name="clockwise">時計回りを表とみるか</param>
	static void setRasterizerState(RASTERIZER_FILL type, BOOL clockwise = FALSE)
	{
		assert(instance != nullptr && "No Instance.");
		instance->d3d11_context->RSSetState(instance->d3d11_rasterizer_states->at(type, clockwise));
	}

	int run()
	{
		MSG msg{};

		if (!initialize())
		{
			return 0;
		}

#if USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", IMGUI_FONT_SIZE, nullptr, glyphRangesJapanese);
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(d3d11_device.Get(), d3d11_context.Get());
		ImGui::StyleColorsDark();
#endif

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				calculate_frame_stats();
				update();
				render();
			}
		}

		d3d11_context->ClearState();
		d3d11_context->Flush();

#if USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif


		BOOL fullscreen = 0;
		idxgi_swapchain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			idxgi_swapchain->SetFullscreenState(FALSE, 0);
		}


		return uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#if USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	void update();
	void render();
	bool uninitialize();

private:
	high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };
	void calculate_frame_stats()
	{
		if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
		{
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << L"【"<<APPLICATION_NAME << L"】" 
				<< L"	FPS : " << fps << L" / " 
				<< L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames = 0;
			elapsed_time += 1.0f;
		}
	}
};

#define QuitMainLoop		PostQuitMessage(0)