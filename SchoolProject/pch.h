#pragma once
#define NOMINMAX

// Windows
#include <windows.h>
#include <windowsx.h>

// Standard
#include <string>
#include <exception>
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <io.h>
#include <memory>
#include <assert.h> 
#include <unordered_map>
#include <vector>
#include <array>
#include <set>

// DirectX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>
#include <SimpleMath.h>

namespace sm = DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

// ImGUI
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


