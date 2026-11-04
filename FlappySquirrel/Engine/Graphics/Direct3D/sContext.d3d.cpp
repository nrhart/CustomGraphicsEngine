// Includes
//=========

#include "../sContext.h"

#include "Includes.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

// Helper Declarations
//====================

namespace
{
	eae6320::cResult CreateDevice( const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight );
}

// Interface
//==========

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::sContext::Initialize( const sInitializationParameters& i_initializationParameters )
{
	auto result = Results::Success;

	windowBeingRenderedTo = i_initializationParameters.mainWindow;

	// Create an interface to a Direct3D device
	if ( !( result = CreateDevice( i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight ) ) )
	{
		EAE6320_ASSERTF( false, "Can't initialize context without device" );
		return result;
	}
	// Create render target view
	{
		ID3D11Texture2D* backBuffer = nullptr;
		const auto hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
		if (FAILED(hr) || !backBuffer)
		{
			EAE6320_ASSERT(false);
			return eae6320::Results::Failure;
		}
		const auto hrRTV = direct3dDevice->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
		backBuffer->Release();
		if (FAILED(hrRTV))
		{
			EAE6320_ASSERT(false);
			return eae6320::Results::Failure;
		}
	}

	// Create depth/stencil buffer and view
	{
		D3D11_TEXTURE2D_DESC depthDesc{};
		depthDesc.Width = i_initializationParameters.resolutionWidth;
		depthDesc.Height = i_initializationParameters.resolutionHeight;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ID3D11Texture2D* depthBuffer = nullptr;
		const auto hrDB = direct3dDevice->CreateTexture2D(&depthDesc, nullptr, &depthBuffer);
		if (FAILED(hrDB))
		{
			EAE6320_ASSERT(false);
			return eae6320::Results::Failure;
		}
		const auto hrDSV = direct3dDevice->CreateDepthStencilView(depthBuffer, nullptr, &depthStencilView);
		depthBuffer->Release();
		if (FAILED(hrDSV))
		{
			EAE6320_ASSERT(false);
			return eae6320::Results::Failure;
		}
	}

	// Bind RTV + DSV
	direct3dImmediateContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	{
		D3D11_VIEWPORT vp{};
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = static_cast<FLOAT>(i_initializationParameters.resolutionWidth);
		vp.Height = static_cast<FLOAT>(i_initializationParameters.resolutionHeight);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		direct3dImmediateContext->RSSetViewports(1, &vp);
	}

	return result;
}

eae6320::cResult eae6320::Graphics::sContext::CleanUp()
{
	auto result = Results::Success;

	if (renderTargetView) 
	{ 
		renderTargetView->Release(); 
		renderTargetView = nullptr; 
	}
	if (depthStencilView) 
	{ 
		depthStencilView->Release(); 
		depthStencilView = nullptr; 
	}
	if ( direct3dImmediateContext )
	{
		direct3dImmediateContext->Release();
		direct3dImmediateContext = nullptr;
	}
	if ( direct3dDevice )
	{
		direct3dDevice->Release();
		direct3dDevice = nullptr;
	}
	if ( swapChain )
	{
		swapChain->Release();
		swapChain = nullptr;
	}

	windowBeingRenderedTo = NULL;

	return result;
}


// -------- Clear / Present --------

void eae6320::Graphics::sContext::ClearBackBuffer(float r, float g, float b, float a)
{
	EAE6320_ASSERT(direct3dImmediateContext && renderTargetView);
	const float clearColor[4] = { r, g, b, a };
	direct3dImmediateContext->ClearRenderTargetView(renderTargetView, clearColor);
}

void eae6320::Graphics::sContext::ClearDepth()
{
	EAE6320_ASSERT(direct3dImmediateContext && depthStencilView);
	constexpr float depth = 1.0f;
	constexpr uint8_t stencil = 0;
	direct3dImmediateContext->ClearDepthStencilView(
		depthStencilView, D3D11_CLEAR_DEPTH, depth, stencil);
}

void eae6320::Graphics::sContext::SwapBuffers()
{
	EAE6320_ASSERT(swapChain);
	constexpr unsigned int swapImmediately = 0;
	constexpr unsigned int presentNextFrame = 0;
	swapChain->Present(swapImmediately, presentNextFrame);
}


// Helper Definitions
//===================

namespace
{
	eae6320::cResult CreateDevice(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
	{
		auto& g_context = eae6320::Graphics::sContext::g_context;

		IDXGIAdapter* const useDefaultAdapter = nullptr;
		constexpr D3D_DRIVER_TYPE useHardwareRendering = D3D_DRIVER_TYPE_HARDWARE;
		constexpr HMODULE dontUseSoftwareRendering = NULL;
		constexpr unsigned int flags = D3D11_CREATE_DEVICE_SINGLETHREADED
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
			| D3D11_CREATE_DEVICE_DEBUG
#endif
			;
		constexpr D3D_FEATURE_LEVEL* const useDefaultFeatureLevels = nullptr;
		constexpr unsigned int requestedFeatureLevelCount = 0;
		constexpr unsigned int sdkVersion = D3D11_SDK_VERSION;

		DXGI_SWAP_CHAIN_DESC scd{};
		scd.BufferDesc.Width = i_resolutionWidth;
		scd.BufferDesc.Height = i_resolutionHeight;
		scd.BufferDesc.RefreshRate.Numerator = 0;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = g_context.windowBeingRenderedTo;
		scd.Windowed = TRUE;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = 0;

		D3D_FEATURE_LEVEL highestSupportedFeatureLevel;
		const auto result_create = D3D11CreateDeviceAndSwapChain(
			useDefaultAdapter, useHardwareRendering, dontUseSoftwareRendering,
			flags, useDefaultFeatureLevels, requestedFeatureLevelCount, sdkVersion,
			&scd, &g_context.swapChain, &g_context.direct3dDevice,
			&highestSupportedFeatureLevel, &g_context.direct3dImmediateContext);

		if (SUCCEEDED(result_create))
			return eae6320::Results::Success;
		else
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError(
				"Direct3D failed to create a device, HRESULT = %#010x", result_create);
			return eae6320::Results::Failure;
		}
	}
}