#include "pch.h"
#include "D3D11Core.h"


//--------------------------------------------------------------------------------------
D3D11Core::D3D11Core(Window * pWindow)
    : pWindow(pWindow)
{
    assert(pWindow != nullptr);
	this->InitializeDirectX();
}





//--------------------------------------------------------------------------------------
void D3D11Core::InitializeDirectX()
{
    // Initialize DeviceAndSwapChain.
    if (!this->createDeviceAndSwapChain())
    {
        std::cout << "ERROR::D3D11Core::CreateDeviceAndSwapChain()::Could not initialize DeviceAndSwapChain." << std::endl;
    }

    // Initialize RenderTargetView.
    if (!this->createRenderTargetView())
    {
        std::cout << "ERROR::D3D11Core::CreateRenderTargetView()::Could not initialize RenderTargetView." << std::endl;
    }

    // Initialize DepthStencilBuffer.
    if (!this->createDepthStencilTexture())
    {
        std::cout << "ERROR::D3D11Core::CreateDepthStencilBuffer()::Could not initialize DepthStencilBuffer." << std::endl;
    }

    // Initialize DepthStencilState.
    if (!this->createDepthStencilState())
    {
        std::cout << "ERROR::D3D11Core::CreateDepthStencilState()::Could not initialize DepthStencilState." << std::endl;
    }

    // Initialize DepthStencilView.
    if (!this->createDepthStencilView())
    {
        std::cout << "ERROR::D3D11Core::CreateDepthStencilView()::Could not initialize DepthStencilView." << std::endl;
    }

    // Initialize RasterizerStates.
    if (!this->createRasterizerStates())
    {
        std::cout << "ERROR::D3D11Core::CreateRasterizerState()::Could not initialize RasterizerState." << std::endl;
    }
    // Initialize SamplerStates.
    if (!this->createSamplerStates())
    {
        std::cout << "ERROR::D3D11Core::CreateSamplerState()::Could not initialize SamplerStates." << std::endl;
    }

    // Set Viewport.
    this->setViewport();

    // Initialize UnorderedAccesView.
    if (!this->createUnorderedAccesView())
    {
        std::cout << "ERROR::D3D11Core::CreateUnorderedAccesView()::Could not initialize UnorderedAccesView." << std::endl;
    }
}





//--------------------------------------------------------------------------------------
bool D3D11Core::createDeviceAndSwapChain()
{
    // Compute the exact client dimensions.
    RECT clientRect;
    GetClientRect(this->pWindow->getHwnd(), &clientRect);
    const UINT clientWidth = clientRect.right - clientRect.left;
    const UINT clientHeight = clientRect.bottom - clientRect.top;

    // Initialize the swap chain description.
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = this->pWindow->getHwnd();

    // Set to windowed mode.
    swapChainDesc.Windowed = TRUE;

    // Set the client dimensions of the back buffer.
    swapChainDesc.BufferDesc.Width = clientWidth;
    swapChainDesc.BufferDesc.Height = clientHeight;

    // Set to a duo back buffer.
    swapChainDesc.BufferCount = 2;

    // Set the refresh rate of the back buffer.
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

    // Set regular 32-bit surface for the back buffer.
    swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.Format = swapChainFormat;

    // Set the scan line ordering and scaling to unspecified.                             
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;

    // Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Discard the contents of the back buffer after IDXGISwapChain1::Present() call.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    // Set this flag to enable an application to switch modes by calling IDXGISwapChain::ResizeTarget.
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // Creates a device that supports the debug layer. 
    UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    #ifdef _DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif
    // Set the feature level to dx 11.
    D3D_FEATURE_LEVEL featureLevels[]
    {
        D3D_FEATURE_LEVEL_11_0
    };

    // Create the device, swap chain and device context.
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
        &swapChainDesc, this->swapChain.GetAddressOf(), this->device.GetAddressOf(), nullptr, this->deviceContext.GetAddressOf());

    return !FAILED(hr);
}





//--------------------------------------------------------------------------------------
bool D3D11Core::createRenderTargetView()
{
    ID3D11Texture2D* pBackBuffer = nullptr;

    // Get the pointer to the back buffer.
    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer))))
        return false;

    // Create the render target View with the back buffer pointer.
    HRESULT hr = device->CreateRenderTargetView(pBackBuffer, nullptr, this->renderTargetView.GetAddressOf());

    // Release pointer to the back buffer.
    pBackBuffer->Release();
    pBackBuffer = 0;

    return !FAILED(hr);
}





//--------------------------------------------------------------------------------------
bool D3D11Core::createDepthStencilTexture()
{
    // Initialize the description of the depth buffer.
    D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
    ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

    // Set up the description of the depth buffer.
    depthStencilBufferDesc.Width = this->pWindow->getWidth();
    depthStencilBufferDesc.Height = this->pWindow->getHeight();
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.ArraySize = 1;
    depthStencilBufferDesc.SampleDesc.Count = 1;
    depthStencilBufferDesc.SampleDesc.Quality = 0;
    depthStencilBufferDesc.CPUAccessFlags = 0;
    depthStencilBufferDesc.MiscFlags = 0;
    depthStencilBufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    depthStencilBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;

    // Create the texture for the depth buffer using the filled out description.
    HRESULT hr = device->CreateTexture2D(&depthStencilBufferDesc, nullptr, this->depthStencilTexture.GetAddressOf());

    return !FAILED(hr);
}





//--------------------------------------------------------------------------------------
bool D3D11Core::createDepthStencilState()
{
    // Initialize the description of the stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state.
    HRESULT hr = this->device->CreateDepthStencilState(&depthStencilDesc, this->depthStencilState.GetAddressOf());

    if (SUCCEEDED(hr))
    {
        // Set the default depth stencil state.
        this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 1);
    }
    return !FAILED(hr);
}





//--------------------------------------------------------------------------------------
bool D3D11Core::createDepthStencilView()
{
    // Initialize the depth stencil View.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil View description, setting applied for Deferred Rendering.
    depthStencilViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil View.
    HRESULT hr = this->device->CreateDepthStencilView(this->depthStencilTexture.Get(), &depthStencilViewDesc, this->depthStencilView.GetAddressOf());

    if (SUCCEEDED(hr))
    {
        // Bind the render target View and depth stencil buffer to the output render pipeline.
        this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
    }

    return !FAILED(hr);
}





//--------------------------------------------------------------------------------------
bool D3D11Core::createRasterizerStates()
{
    // Initialize the description of the rasterizer state.
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

    // Setup the raster description: how and what polygons will be drawn.
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.AntialiasedLineEnable = false;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.MultisampleEnable = false;
    rasterizerDesc.ScissorEnable = false;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    // Backface culling refers to the process of discarding back-facing triangles from the pipeline. 
    // This can potentially reduce the amount of triangles that need to be processed by half, hence it will be set as default.
    rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

    // Create the rasterizer state from the description we just filled out.
    HRESULT hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());

    if (SUCCEEDED(hr))
    {
        // Set default rasterizer state.
        this->deviceContext->RSSetState(this->rasterizerState.Get());
    }

    // Setup a raster description with no back face culling.
    rasterizerDesc.CullMode = D3D11_CULL_NONE;

    // Create the no culling rasterizer state.
    hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterStateNoCulling.GetAddressOf());
    if (FAILED(hr))
        return false;

    // Setup a raster description which enables wire frame rendering.
    rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

    // Create the wire frame rasterizer state.
    hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterStateWireframe.GetAddressOf());

    return !FAILED(hr);
}






// Point: Takes nearest neighbor pixel and copies the color for harsh mini/magnification.
// Linear: Takes nearest neighborS and avarages out the color for smooth mini/magnification.
//--------------------------------------------------------------------------------------
bool D3D11Core::createSamplerStates()
{
    // Create a sampler state description.
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

    // Setup for Linear SamplerState
    samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MinLOD = 0;

    HRESULT hr = this->device->CreateSamplerState(&samplerDesc, this->linearSamplerState.GetAddressOf());
    if (FAILED(hr))
        return false;

    // Setup for Point SamplerState
    samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.MinLOD = 0;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;

    hr = this->device->CreateSamplerState(&samplerDesc, this->pointSamplerState.GetAddressOf());

    return !FAILED(hr);
}





//--------------------------------------------------------------------------------------
bool D3D11Core::createUnorderedAccesView()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer))))
        return false;

    HRESULT hr = device->CreateUnorderedAccessView(pBackBuffer, nullptr, this->backBufferUAV.GetAddressOf());

    pBackBuffer->Release();
    pBackBuffer = 0;

    return !FAILED(hr);
}





//--------------------------------------------------------------------------------------
void D3D11Core::setViewport()
{
    // Initialize the viewport to occupy the entire client area.
    RECT clientRect;
    GetClientRect(this->pWindow->getHwnd(), &clientRect);
    const LONG clientWidth = clientRect.right - clientRect.left;
    const LONG clientHeight = clientRect.bottom - clientRect.top;

    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    // Setup the viewport for rendering.
    viewport.Width = static_cast<FLOAT>(clientWidth);
    viewport.Height = static_cast<FLOAT>(clientHeight);
    viewport.TopLeftX = 0.f;
    viewport.TopLeftY = 0.f;

    // Direct3D uses a depth buffer range of 0 to 1, hence:
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;

    // Set viewport.
    deviceContext->RSSetViewports(1, &this->viewport);
}


