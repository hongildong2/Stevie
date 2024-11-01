//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <winsdkver.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <sdkddkver.h>

#define _MY_ASSERT(x) if ((x) == false) { __debugbreak(); }

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#define MY_ASSERT(x) _MY_ASSERT(x)
#else
#define MY_ASSERT(x)
#endif




// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl/client.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_6.h>

#include <DirectXMath.h>
#include <DirectXColors.h>


#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <exception>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <tuple>
#include <vector>
#include <unordered_set>


#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include "BufferHelpers.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"


// Mine
#include "SubModules\Render\RenderConfig.h"
#include "SubModules\Render\RenderDefs.h"
#include "SubModules\Render\IRenderer.h"
#include "SubModules\Render\IRenderResource.h"
#include "SubModules\Render\RMaterial.h"
#include "SubModules\Render\RShader.h"
#include "SubModules\Render\RBlendState.h"
#include "SubModules\Render\RTexture.h"
#include "SubModules\Render\RMaterial.h"
#include "SubModules\Render\RMeshGeometry.h"
#include "SubModules\Render\GraphicsCommon1.h"