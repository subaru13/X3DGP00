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
#include "function/framework.h"
#include "function/sprite.h"
#include "function/sprite_batch.h"
#include "function/ConstantBuffer.h"
#include "function/ASCII_Font.h"
#include "function/Arithmetic.h"
#include "function/SimpleSound.h"
#include "function/geometry_primitive.h"
#include "function/SceneConstantBuffer.h"
#include "function/skinned_mesh.h"
#include "function/SceneManager.h"