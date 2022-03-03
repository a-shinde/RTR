//Header files.

#include <windows.h>
#include <fstream>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma warning( disable: 4838 )
#include <XNAMath\xnamath.h>
#include <WICTextureLoader.h>

#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"D3dcompiler.lib")
#pragma comment (lib,"DirectXTK.lib")

using namespace std;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

int currentWidth;
int currentHeight;

HWND gHwnd;
HDC gHdc;
HGLRC gHglrc;

bool gbFullscreen;
bool gbEscapeKeyIsPressed;
bool gbActiveWindow;
bool bDone;

WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
DWORD dwStyle;
ofstream outputFile;

//Fun Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Initialize();
void FirstResize();
void Uninitialize();
void Display();
void Update();
HRESULT resize(int, int);
HRESULT initializeData();
HRESULT LoadD3DTexture(const wchar_t *, ID3D11ShaderResourceView **);

char gszLogFileName[] = "Logs.txt";

float clearColor[4]; // RGBA
IDXGISwapChain *gpI_DXGI_SwapChain = NULL;
ID3D11Device *gpI_D3D11_Device = NULL;
ID3D11DeviceContext *gpI_D3D11_DeviceContext = NULL;

ID3D11RenderTargetView *gpI_D3D11_RenderTargetView = NULL;
ID3D11RasterizerState *gpI_D3D11_RasterizeState = NULL;
ID3D11DepthStencilView *gpI_D3D11_Depth_Stencil_View = NULL;

ID3D11VertexShader *gpI_D3D11_VertexShader = NULL;
ID3D11PixelShader *gpI_D3D11_PixelShader = NULL;

ID3D11Buffer *gpI_D3D11_Buffer_Pyramid_PositionBuffer = NULL;
ID3D11Buffer *gpI_D3D11_Buffer_Pyramid_TexcoordBuffer = NULL;

ID3D11Buffer *gpI_D3D11_Buffer_Cube_PositionBuffer = NULL;
ID3D11Buffer *gpI_D3D11_Buffer_Cube_TexcoordBuffer = NULL;

ID3D11ShaderResourceView *gpI_D3D11_ShaderResourceView_Texture_Pyramid = NULL;
ID3D11SamplerState *gpI_D3D11_SamplerState_Texture_Pyramid = NULL;
ID3D11ShaderResourceView *gpI_D3D11_ShaderResourceView_Texture_Cube = NULL;
ID3D11SamplerState *gpI_D3D11_SamplerState_Texture_Cube = NULL;

ID3D11InputLayout *gpI_D3D11_InputLayout = NULL;
ID3D11Buffer *gpI_D3D11_Buffer_ConstantBuffer = NULL;

ID3DBlob *gpI_D3DBlob_VertexShaderCode = NULL;

struct CBUFFER
{
    XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX perspectiveProjectionMatrix;
float angleRotate;


//Code
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdArgs, int cmdShow)
{

    TCHAR className[] = TEXT("DirectX_Window");
    WNDCLASSEX wndClassEx;
    HWND hWnd;
    MSG msg;

    outputFile.open(gszLogFileName, std::ofstream::out);
    if (outputFile.is_open())
    {
        outputFile << "------------------------------" << endl;
        outputFile << "----     Amol Shinde      ----" << endl;
        outputFile << "------------------------------" << endl;
        outputFile << "Log File Successfully Opened" << endl;

    }
    outputFile.close();

    wndClassEx.cbSize = sizeof(WNDCLASSEX);
    wndClassEx.cbClsExtra = 0;
    wndClassEx.cbWndExtra = 0;
    wndClassEx.hbrBackground = CreateSolidBrush(RGB(0.0f, 0.0f, 0.0f));
    wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wndClassEx.hInstance = hInstance;
    wndClassEx.lpfnWndProc = WndProc;
    wndClassEx.lpszMenuName = NULL;
    wndClassEx.lpszClassName = className;
    wndClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    RegisterClassEx(&wndClassEx);

    int monitorWidth = GetSystemMetrics(SM_CXSCREEN);
    int monitorHeight = GetSystemMetrics(SM_CYSCREEN);

    int x = (monitorWidth / 2) - (WIN_WIDTH / 2);
    int y = (monitorHeight / 2) - (WIN_HEIGHT / 2);


    hWnd = CreateWindowEx(WS_EX_APPWINDOW, className, TEXT("DirextX 3D"),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
        x, y, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

    gHwnd = hWnd;

    HRESULT hr;
    hr = Initialize();
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "initialize() Failed. Exitting Now ..." << endl;
        outputFile.close();
        DestroyWindow(hWnd);
        hWnd = NULL;
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "initialize() Succeeded." << endl;
        outputFile.close();
    }


    ShowWindow(hWnd, cmdShow);
    //Game Loop
    while (bDone == false)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                bDone = TRUE;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {

            if (gbActiveWindow == true)
            {
                if (gbEscapeKeyIsPressed == true)
                    bDone = true;
            }
            Display();
            Update();
        }
    }
    Uninitialize();
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Functions Declarations
    void ToggleFullscreen();

    switch (message)
    {
    case WM_CREATE:
        break;

    case WM_ACTIVATE:
        if (HIWORD(wParam) == 0)
            gbActiveWindow = true;
        else
            gbActiveWindow = false;
        break;

    case WM_LBUTTONDOWN:
        break;

    case WM_RBUTTONDOWN:
        break;

    case WM_SIZE:
        if (gpI_D3D11_DeviceContext)
        {
            HRESULT hr;
            hr = resize(LOWORD(lParam), HIWORD(lParam));
            if (FAILED(hr))
            {
                outputFile.open(gszLogFileName, std::ofstream::app);
                outputFile << "resize() Failed" << endl;
                outputFile.close();
                return(hr);
            }
            else
            {
                outputFile.open(gszLogFileName, std::ofstream::app);
                outputFile << "resize() Successded" << endl;
                outputFile.close();
            }
        }
        break;

    case WM_PAINT:
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            if (gbEscapeKeyIsPressed == false)
                gbEscapeKeyIsPressed = true;
            break;

        case 0x46:  //for 'f'
            if (gbFullscreen == true)
            {
                ToggleFullscreen();
                gbFullscreen = false;
            }
            else
            {
                ToggleFullscreen();
                gbFullscreen = true;
            }
            break;

        case 0x4C:  //for 'l'
            break;

        }
        break;

    case WM_CHAR:

        break;

    case WM_ERASEBKGND:
        return(0);
        break;

    case WM_CLOSE:
        Uninitialize();
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT Initialize()
{
    HRESULT hr;
    D3D_DRIVER_TYPE d3d_DriverType;
    D3D_DRIVER_TYPE d3d_DriverTypes[] = { D3D_DRIVER_TYPE_HARDWARE,D3D_DRIVER_TYPE_WARP,D3D_DRIVER_TYPE_REFERENCE, };
    D3D_FEATURE_LEVEL d3d_FeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL d3d_FeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;
    UINT createDeviceFlags = 0;
    UINT num_DriverTypes = 0;
    UINT num_FeatureLevels = 1;

    num_DriverTypes = sizeof(d3d_DriverTypes) / sizeof(d3d_DriverTypes[0]);

    DXGI_SWAP_CHAIN_DESC dxgi_SwapChainDesc;
    ZeroMemory((void *)&dxgi_SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    dxgi_SwapChainDesc.BufferCount = 1;
    dxgi_SwapChainDesc.BufferDesc.Width = WIN_WIDTH;
    dxgi_SwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
    dxgi_SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxgi_SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    dxgi_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    dxgi_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    dxgi_SwapChainDesc.OutputWindow = gHwnd;
    dxgi_SwapChainDesc.SampleDesc.Count = 4;
    dxgi_SwapChainDesc.SampleDesc.Quality = 0.99;
    dxgi_SwapChainDesc.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < num_DriverTypes; driverTypeIndex++)
    {
        d3d_DriverType = d3d_DriverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(
            NULL,                          // Adapter
            d3d_DriverType,                 // Driver Type
            NULL,                          // Software
            createDeviceFlags,             // Flags
            &d3d_FeatureLevel_required,     // Feature Levels
            num_FeatureLevels,              // Num Feature Levels
            D3D11_SDK_VERSION,             // SDK Version
            &dxgi_SwapChainDesc,            // Swap Chain Desc
            &gpI_DXGI_SwapChain,             // Swap Chain
            &gpI_D3D11_Device,               // Device
            &d3d_FeatureLevel_acquired,     // Feature Level
            &gpI_D3D11_DeviceContext);       // Device Context
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "D3D11CreateDeviceAndSwapChain() Failed" << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "D3D11CreateDeviceAndSwapChain() Succeeded" << endl;
        outputFile << "The Chosen Driver Is Of" << endl;
        if (d3d_DriverType == D3D_DRIVER_TYPE_HARDWARE)
        {
            outputFile << "Hardware Type." << endl;
        }
        else if (d3d_DriverType == D3D_DRIVER_TYPE_WARP)
        {
            outputFile << "Warp Type." << endl;
        }
        else if (d3d_DriverType == D3D_DRIVER_TYPE_REFERENCE)
        {
            outputFile << "Reference Type." << endl;
        }
        else
        {
            outputFile << "Unknown Type." << endl;
        }

        outputFile << "The Supported Highest Feature Level Is" << endl;
        if (d3d_FeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
        {
            outputFile << "11.0" << endl;
        }
        else if (d3d_FeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
        {
            outputFile << "10.1" << endl;
        }
        else if (d3d_FeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
        {
            outputFile << "10.0" << endl;
        }
        else
        {
            outputFile << "Unknown" << endl;
        }

        outputFile.close();
    }

    const char *vertexShaderSourceCode =
        "struct outputVertex " \
        "{ " \
        "   float4 position : SV_POSITION;" \
        "   float2 texcoord : TEXCOORD;" \
        "}; " \
        "cbuffer ConstanatBuffer " \
        "{  "\
        "   float4x4 worldViewProjectionMatrix;" \
        "} "\
        "  "\
        "outputVertex main(float4 pos : POSITION, float2 tex : TEXCOORD )" \
        "{   "\
        "   outputVertex outVec; " \
        "   outVec.position = mul(worldViewProjectionMatrix, pos);" \
        "   outVec.texcoord = tex;" \
        "   return (outVec); "\
        "} ";

    ID3DBlob *pI_D3DBlob_Error = NULL;

    hr = D3DCompile(vertexShaderSourceCode,
        lstrlenA(vertexShaderSourceCode) + 1,
        "VS",
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        0,
        0,
        &gpI_D3DBlob_VertexShaderCode,
        &pI_D3DBlob_Error);

    if (FAILED(hr))
    {
        if (pI_D3DBlob_Error != NULL)
        {
            outputFile.open(gszLogFileName, std::ofstream::app);
            outputFile << "D3DCompile() Failed For Vertex Shader :" << (char *)pI_D3DBlob_Error->GetBufferPointer() << endl;
            outputFile.close();
            pI_D3DBlob_Error->Release();
            pI_D3DBlob_Error = NULL;
            return(hr);
        }
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "D3DCompile() Succeeded For Vertex Shader." << endl;
        outputFile.close();
    }

    hr = gpI_D3D11_Device->CreateVertexShader(gpI_D3DBlob_VertexShaderCode->GetBufferPointer(), gpI_D3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpI_D3D11_VertexShader);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateVertexShader() Failed." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateVertexShader() Succeeded." << endl;
        outputFile.close();
    }
    gpI_D3D11_DeviceContext->VSSetShader(gpI_D3D11_VertexShader, 0, 0);

    const char *pixelShaderSourceCode =
        "Texture2D myTexture2D; "\
        "SamplerState mySamplerState; "\
        "float4 main( float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET"\
        "{ "\
        "   float4 outColor = myTexture2D.Sample(mySamplerState,texcoord);" \
        "   return (outColor);"\
        "}";

    ID3DBlob *pI_D3DBlob_PixelShaderCode = NULL;
    pI_D3DBlob_Error = NULL;
    hr = D3DCompile(pixelShaderSourceCode,
        lstrlenA(pixelShaderSourceCode) + 1,
        "PS",
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_5_0",
        0,
        0,
        &pI_D3DBlob_PixelShaderCode,
        &pI_D3DBlob_Error);
    if (FAILED(hr))
    {
        if (pI_D3DBlob_Error != NULL)
        {
            outputFile.open(gszLogFileName, std::ofstream::app);
            outputFile << "D3DCompile() Failed For Pixel Shader :" << (char *)pI_D3DBlob_Error->GetBufferPointer() << endl;
            outputFile.close();
            pI_D3DBlob_Error->Release();
            pI_D3DBlob_Error = NULL;
            return(hr);
        }
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "D3DCompile() Succeeded For Pixel Shader." << endl;
        outputFile.close();
    }

    hr = gpI_D3D11_Device->CreatePixelShader(pI_D3DBlob_PixelShaderCode->GetBufferPointer(), pI_D3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpI_D3D11_PixelShader);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreatePixelShader() Failed." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreatePixelShader() Succeeded." << endl;
        outputFile.close();
    }
    gpI_D3D11_DeviceContext->PSSetShader(gpI_D3D11_PixelShader, 0, 0);
    pI_D3DBlob_PixelShaderCode->Release();
    pI_D3DBlob_PixelShaderCode = NULL;

    clearColor[0] = 0.0f;
    clearColor[1] = 0.0f;
    clearColor[2] = 0.0f;
    clearColor[3] = 1.0f;

    hr = initializeData();
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "initializeData() Failed" << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "initializeData() Successded" << endl;
        outputFile.close();
    }

    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;

    gpI_D3D11_Device->CreateRasterizerState(&rasterizerDesc, &gpI_D3D11_RasterizeState);
    gpI_D3D11_DeviceContext->RSSetState(gpI_D3D11_RasterizeState);

    hr = resize(WIN_WIDTH, WIN_HEIGHT);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "resize() Failed" << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "resize() Successded" << endl;
        outputFile.close();
    }

    hr = LoadD3DTexture(L"Stone.bmp", &gpI_D3D11_ShaderResourceView_Texture_Pyramid);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "LoadD3DTexture(Stone) Failed" << endl;
        outputFile.close();        
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "LoadD3DTexture(Stone) Successded" << endl;
        outputFile.close();
    }

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    hr = gpI_D3D11_Device->CreateSamplerState(&samplerDesc, &gpI_D3D11_SamplerState_Texture_Pyramid);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "CreateSamplerState() Failed" << endl;
        outputFile.close();
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "CreateSamplerState() Successded" << endl;
        outputFile.close();
    }

    hr = LoadD3DTexture(L"Vijay_Kundali.bmp", &gpI_D3D11_ShaderResourceView_Texture_Cube);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "LoadD3DTexture(Kundli) Failed" << endl;
        outputFile.close();
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "LoadD3DTexture(Kundli) Successded" << endl;
        outputFile.close();
    }

    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    hr = gpI_D3D11_Device->CreateSamplerState(&samplerDesc, &gpI_D3D11_SamplerState_Texture_Cube);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "CreateSamplerState() Failed" << endl;
        outputFile.close();
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "CreateSamplerState() Successded" << endl;
        outputFile.close();
    }

    return(S_OK);
}

HRESULT initializeData()
{
    HRESULT hr;

    //--------------------------
    // Pyramid Data
    float pyramid_positions[] =
    {
        //front
        0.0f,1.0f,0.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,

        //Right face
        0.0f,1.0f,0.0f,
        1.0f,-1.0f,1.0f,
        1.0f,-1.0f,-1.0f,

        //Back face
        0.0f,1.0f,0.0f,
        -1.0f,-1.0f,1.0f,
        1.0f,-1.0f,1.0f,

        //right face
        0.0f,1.0f,0.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,1.0f,
    };

    float pyramid_texcoord[] =
    {
        0.5, 1.0,
        1.0, 0.0,
        0.0, 0.0,

        0.5, 1.0,
        0.0, 0.0,
        1.0, 0.0,

        0.5, 1.0,
        0.0, 0.0,
        1.0, 0.0,

        0.5, 1.0,
        1.0, 0.0,
        0.0, 0.0,
    };

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(float) * ARRAYSIZE(pyramid_positions);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = gpI_D3D11_Device->CreateBuffer(&bufferDesc, NULL, &gpI_D3D11_Buffer_Pyramid_PositionBuffer);

    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Failed For Position Buffer." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Succeeded For Position Buffer." << endl;
        outputFile.close();
    }

    // copy vertices into above buffer
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    gpI_D3D11_DeviceContext->Map(gpI_D3D11_Buffer_Pyramid_PositionBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
    memcpy(mappedSubresource.pData, pyramid_positions, sizeof(pyramid_positions));
    gpI_D3D11_DeviceContext->Unmap(gpI_D3D11_Buffer_Pyramid_PositionBuffer, NULL);

    //Copy Color Data
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(float) * ARRAYSIZE(pyramid_texcoord);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = gpI_D3D11_Device->CreateBuffer(&bufferDesc, NULL, &gpI_D3D11_Buffer_Pyramid_TexcoordBuffer);

    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Failed For Color Buffer." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Succeeded For Color Buffer." << endl;
        outputFile.close();
    }

    // copy vertices into above buffer
    ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    gpI_D3D11_DeviceContext->Map(gpI_D3D11_Buffer_Pyramid_TexcoordBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
    memcpy(mappedSubresource.pData, pyramid_texcoord, sizeof(pyramid_texcoord));
    gpI_D3D11_DeviceContext->Unmap(gpI_D3D11_Buffer_Pyramid_TexcoordBuffer, NULL);

    // Cube Data
    float cube_positions[] =
    {
        //Front
        -1.0f,1.0f,-1.0f,
        1.0f,1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,

        //Right
        1.0f,1.0f,-1.0f,
        1.0f,1.0f,1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f,1.0f,1.0f,
        1.0f,-1.0f,1.0f,

        //Back
        1.0f,1.0f,1.0f,
        -1.0f,1.0f,1.0f,
        1.0f,-1.0f,1.0f,
        1.0f,-1.0f,1.0f,
        -1.0f,1.0f,1.0f,
        -1.0f,-1.0f,1.0f,

        //Left
        -1.0f,1.0f,1.0f,
        -1.0f,1.0f,-1.0f,
        -1.0f,-1.0f,1.0f,
        -1.0f,-1.0f,1.0f,
        -1.0f,1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,

        //Top
        -1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        -1.0f,1.0f,-1.0f,
        -1.0f,1.0f,-1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,-1.0f,

        //Bottom
        -1.0f,-1.0f,1.0f,
        1.0f,-1.0f,1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,1.0f,
        1.0f,-1.0f,-1.0f,
    };

    float cube_texcoord[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    };

    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(float) * ARRAYSIZE(cube_positions);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = gpI_D3D11_Device->CreateBuffer(&bufferDesc, NULL, &gpI_D3D11_Buffer_Cube_PositionBuffer);

    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Failed For Position Buffer." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Succeeded For Position Buffer." << endl;
        outputFile.close();
    }

    // copy vertices into above buffer
    ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    gpI_D3D11_DeviceContext->Map(gpI_D3D11_Buffer_Cube_PositionBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
    memcpy(mappedSubresource.pData, cube_positions, sizeof(cube_positions));
    gpI_D3D11_DeviceContext->Unmap(gpI_D3D11_Buffer_Cube_PositionBuffer, NULL);

    //Copy Color Data
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(float) * ARRAYSIZE(cube_texcoord);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = gpI_D3D11_Device->CreateBuffer(&bufferDesc, NULL, &gpI_D3D11_Buffer_Cube_TexcoordBuffer);

    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Failed For Color Buffer." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Succeeded For Color Buffer." << endl;
        outputFile.close();
    }

    // copy vertices into above buffer
    ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    gpI_D3D11_DeviceContext->Map(gpI_D3D11_Buffer_Cube_TexcoordBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
    memcpy(mappedSubresource.pData, cube_texcoord, sizeof(cube_texcoord));
    gpI_D3D11_DeviceContext->Unmap(gpI_D3D11_Buffer_Cube_TexcoordBuffer, NULL);

    //----------------------------------
    // define and set the constant buffer
    D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
    ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
    bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
    bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = gpI_D3D11_Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpI_D3D11_Buffer_ConstantBuffer);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Failed For Constant Buffer." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Succeeded For Constant Buffer." << endl;
        outputFile.close();
    }
    gpI_D3D11_DeviceContext->VSSetConstantBuffers(0, 1, &gpI_D3D11_Buffer_ConstantBuffer);

    // create and set input layout
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
    inputElementDesc[0].SemanticName = "POSITION";
    inputElementDesc[0].SemanticIndex = 0;
    inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDesc[0].InputSlot = 0;
    inputElementDesc[0].AlignedByteOffset = 0;
    inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    inputElementDesc[0].InstanceDataStepRate = 0;

    inputElementDesc[1].SemanticName = "TEXCOORD";
    inputElementDesc[1].SemanticIndex = 0;
    inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDesc[1].InputSlot = 1;
    inputElementDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    inputElementDesc[1].InstanceDataStepRate = 0;

    hr = gpI_D3D11_Device->CreateInputLayout(inputElementDesc, _ARRAYSIZE(inputElementDesc), gpI_D3DBlob_VertexShaderCode->GetBufferPointer(), gpI_D3DBlob_VertexShaderCode->GetBufferSize(), &gpI_D3D11_InputLayout);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateInputLayout() Failed." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateInputLayout() Succeeded." << endl;
        outputFile.close();
    }
    gpI_D3D11_DeviceContext->IASetInputLayout(gpI_D3D11_InputLayout);
    gpI_D3DBlob_VertexShaderCode->Release();
    gpI_D3DBlob_VertexShaderCode = NULL;

    perspectiveProjectionMatrix = XMMatrixIdentity();
}

HRESULT LoadD3DTexture(const wchar_t *textureFileName, ID3D11ShaderResourceView **ppID3D11ShaderResourceView)
{
    HRESULT hr;

    hr = DirectX::CreateWICTextureFromFile(gpI_D3D11_Device, gpI_D3D11_DeviceContext, textureFileName, NULL, ppID3D11ShaderResourceView);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "CreateWICTextureFromFile() Failed." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "CreateWICTextureFromFile Succeeded." << endl;
        outputFile.close();
    }
    return(hr);
}


void FirstResize()
{
    currentWidth = WIN_WIDTH - GetSystemMetrics(SM_CXSIZEFRAME) * 4;
    currentHeight = WIN_HEIGHT - GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CYSIZEFRAME) * 4;
    resize(currentWidth, currentHeight);
}

HRESULT resize(int width, int height)
{

    HRESULT hr = S_OK;

    if (gpI_D3D11_RenderTargetView)
    {
        gpI_D3D11_RenderTargetView->Release();
        gpI_D3D11_RenderTargetView = NULL;
    }

    gpI_DXGI_SwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

    // again get back buffer from swap chain
    ID3D11Texture2D *pI_D3D11_Texture2D_BackBuffer;
    gpI_DXGI_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pI_D3D11_Texture2D_BackBuffer);

    hr = gpI_D3D11_Device->CreateRenderTargetView(pI_D3D11_Texture2D_BackBuffer, NULL, &gpI_D3D11_RenderTargetView);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateRenderTargetView() Failed" << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateRenderTargetView() Succeeded" << endl;
        outputFile.close();
    }
    pI_D3D11_Texture2D_BackBuffer->Release();
    pI_D3D11_Texture2D_BackBuffer = NULL;

    if (gpI_D3D11_Depth_Stencil_View)
    {
        gpI_D3D11_Depth_Stencil_View->Release();
        gpI_D3D11_Depth_Stencil_View = NULL;
    }

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    textureDesc.Height = height;
    textureDesc.Width = width;
    textureDesc.ArraySize = 1;
    textureDesc.MipLevels = 1;
    textureDesc.SampleDesc.Count = 4;
    textureDesc.SampleDesc.Quality = 0.99;
    textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    ID3D11Texture2D *pI_D3D11_Texture2D = NULL;
    gpI_D3D11_Device->CreateTexture2D(&textureDesc, NULL, &pI_D3D11_Texture2D);

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;


    hr = gpI_D3D11_Device->CreateDepthStencilView(pI_D3D11_Texture2D, &depthStencilViewDesc, &gpI_D3D11_Depth_Stencil_View);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateDepthStencilView() Failed" << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateDepthStencilView() Succeeded" << endl;
        outputFile.close();
    }

    gpI_D3D11_DeviceContext->OMSetRenderTargets(1, &gpI_D3D11_RenderTargetView, gpI_D3D11_Depth_Stencil_View);

    D3D11_VIEWPORT d3d_ViewPort;
    d3d_ViewPort.TopLeftX = 0;
    d3d_ViewPort.TopLeftY = 0;
    d3d_ViewPort.Width = (float)width;
    d3d_ViewPort.Height = (float)height;
    d3d_ViewPort.MinDepth = 0.0f;
    d3d_ViewPort.MaxDepth = 1.0f;
    gpI_D3D11_DeviceContext->RSSetViewports(1, &d3d_ViewPort);

    // set orthographic matrix
    perspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
}

void ToggleFullscreen()
{
    MONITORINFO mi = { sizeof(MONITORINFO) };

    if (!gbFullscreen)
    {
        // Make fullscreen
        dwStyle = GetWindowLong(gHwnd, GWL_STYLE);
        if (dwStyle & WS_OVERLAPPEDWINDOW)
        {
            if (GetWindowPlacement(gHwnd, &wpPrev) && GetMonitorInfo(
                MonitorFromWindow(gHwnd, MONITORINFOF_PRIMARY), &mi))
            {
                SetWindowLong(gHwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(gHwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                    mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
                    SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        ShowCursor(FALSE);
    }
    else
    {
        SetWindowLong(gHwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(gHwnd, &wpPrev);
        SetWindowPos(gHwnd, HWND_TOP, 0, 0, 0, 0,
            SWP_NOMOVE |
            SWP_NOSIZE |
            SWP_NOOWNERZORDER |
            SWP_NOZORDER |
            SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }
}

void Uninitialize()
{
    if (gbFullscreen == true)
    {
        dwStyle = GetWindowLong(gHwnd, GWL_STYLE);
        SetWindowLong(gHwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(gHwnd, &wpPrev);
        SetWindowPos(gHwnd, HWND_TOP, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }

    if (gpI_D3D11_Depth_Stencil_View)
    {
        gpI_D3D11_Depth_Stencil_View->Release();
        gpI_D3D11_Depth_Stencil_View = NULL;
    }


    if (gpI_D3D11_RasterizeState)
    {
        gpI_D3D11_RasterizeState->Release();
        gpI_D3D11_RasterizeState = NULL;
    }

    if (gpI_D3D11_RenderTargetView)
    {
        gpI_D3D11_RenderTargetView->Release();
        gpI_D3D11_RenderTargetView = NULL;
    }

    if (gpI_D3D11_Buffer_ConstantBuffer)
    {
        gpI_D3D11_Buffer_ConstantBuffer->Release();
        gpI_D3D11_Buffer_ConstantBuffer = NULL;
    }

    if (gpI_D3D11_InputLayout)
    {
        gpI_D3D11_InputLayout->Release();
        gpI_D3D11_InputLayout = NULL;
    }

    if (gpI_D3D11_Buffer_Pyramid_TexcoordBuffer)
    {
        gpI_D3D11_Buffer_Pyramid_TexcoordBuffer->Release();
        gpI_D3D11_Buffer_Pyramid_TexcoordBuffer = NULL;
    }

    if (gpI_D3D11_Buffer_Pyramid_PositionBuffer)
    {
        gpI_D3D11_Buffer_Pyramid_PositionBuffer->Release();
        gpI_D3D11_Buffer_Pyramid_PositionBuffer = NULL;
    }

    if (gpI_D3D11_PixelShader)
    {
        gpI_D3D11_PixelShader->Release();
        gpI_D3D11_PixelShader = NULL;
    }

    if (gpI_D3D11_VertexShader)
    {
        gpI_D3D11_VertexShader->Release();
        gpI_D3D11_VertexShader = NULL;
    }

    if (gpI_DXGI_SwapChain)
    {
        gpI_DXGI_SwapChain->Release();
        gpI_DXGI_SwapChain = NULL;
    }

    if (gpI_D3D11_DeviceContext)
    {
        gpI_D3D11_DeviceContext->Release();
        gpI_D3D11_DeviceContext = NULL;
    }

    if (gpI_D3D11_Device)
    {
        gpI_D3D11_Device->Release();
        gpI_D3D11_Device = NULL;
    }

    ReleaseDC(gHwnd, gHdc);
    gHdc = NULL;

    outputFile.open(gszLogFileName, std::ofstream::app);
    if (outputFile.is_open())
    {
        outputFile << "Log File Successfully Closed.";
        outputFile.close();
    }
    DestroyWindow(gHwnd);
}

void Display()
{
    gpI_D3D11_DeviceContext->ClearRenderTargetView(gpI_D3D11_RenderTargetView, clearColor);
    gpI_D3D11_DeviceContext->ClearDepthStencilView(gpI_D3D11_Depth_Stencil_View, D3D11_CLEAR_DEPTH, 1.0f, 0);

    UINT stride = sizeof(float) * 3;
    UINT offset = 0;

    //Draw Pyramid
    gpI_D3D11_DeviceContext->IASetVertexBuffers(0, 1, &gpI_D3D11_Buffer_Pyramid_PositionBuffer, &stride, &offset);
    stride = sizeof(float) * 2;
    gpI_D3D11_DeviceContext->IASetVertexBuffers(1, 1, &gpI_D3D11_Buffer_Pyramid_TexcoordBuffer, &stride, &offset);
    gpI_D3D11_DeviceContext->PSSetShaderResources(0, 1, &gpI_D3D11_ShaderResourceView_Texture_Pyramid);
    gpI_D3D11_DeviceContext->PSSetSamplers(0, 1, &gpI_D3D11_SamplerState_Texture_Pyramid);

    gpI_D3D11_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    XMMATRIX worldMatrix = XMMatrixTranslation(-1.5f, 0.0f, 6.0f);
    worldMatrix = XMMatrixRotationY(XMConvertToRadians(angleRotate)) * worldMatrix;
    XMMATRIX viewMatrix = XMMatrixIdentity();
    XMMATRIX wvpMatrix = worldMatrix * viewMatrix * perspectiveProjectionMatrix;

    CBUFFER constantBuffer;
    constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
    gpI_D3D11_DeviceContext->UpdateSubresource(gpI_D3D11_Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

    gpI_D3D11_DeviceContext->Draw(12, 0);

    // Draw Cube
    stride = sizeof(float) * 3;
    gpI_D3D11_DeviceContext->IASetVertexBuffers(0, 1, &gpI_D3D11_Buffer_Cube_PositionBuffer, &stride, &offset);
    stride = sizeof(float) * 2;
    gpI_D3D11_DeviceContext->IASetVertexBuffers(1, 1, &gpI_D3D11_Buffer_Cube_TexcoordBuffer, &stride, &offset);
    gpI_D3D11_DeviceContext->PSSetShaderResources(0, 1, &gpI_D3D11_ShaderResourceView_Texture_Cube);
    gpI_D3D11_DeviceContext->PSSetSamplers(0, 1, &gpI_D3D11_SamplerState_Texture_Cube);

    XMMATRIX translationMatrix = XMMatrixTranslation(1.5f, 0.0f, 6.0f);
    XMMATRIX scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);

    XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(angleRotate));
    rotationMatrix = XMMatrixRotationY(XMConvertToRadians(angleRotate)) * rotationMatrix;
    rotationMatrix = XMMatrixRotationZ(XMConvertToRadians(angleRotate)) * rotationMatrix;

    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    wvpMatrix = worldMatrix * viewMatrix * perspectiveProjectionMatrix;

    constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
    gpI_D3D11_DeviceContext->UpdateSubresource(gpI_D3D11_Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

    gpI_D3D11_DeviceContext->Draw(6, 0);
    gpI_D3D11_DeviceContext->Draw(6, 6);
    gpI_D3D11_DeviceContext->Draw(6, 12);
    gpI_D3D11_DeviceContext->Draw(6, 18);
    gpI_D3D11_DeviceContext->Draw(6, 24);
    gpI_D3D11_DeviceContext->Draw(6, 30);

    gpI_DXGI_SwapChain->Present(0, 0);
}

void Update()
{
    if (angleRotate > 360.0f)
        angleRotate = 0.0f;

    angleRotate += 0.02f;
}