#pragma once

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#endif

#include <memory>
#include <Windows.h>

//DefaultFunctions
#include "function/Framework.h"
#include "function/MakeInterface.h"
#include "function/GeometryPrimitive.h"
#include "function/ASCIIFont.h"
#include "function/Arithmetic.h"
#include "function/SimpleSound.h"
#include "function/SceneConstantBuffer.h"
#include "function/SceneManager.h"
#include "function/CreateComObjectHelpar.h"
#include "function/KeyInput.h"
#include "function/CameraControl.h"