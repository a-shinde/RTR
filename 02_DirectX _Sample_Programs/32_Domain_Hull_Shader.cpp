//Header files.

#include <windows.h>
#include <fstream>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma warning( disable: 4838 )
#include <XNAMath\xnamath.h>

#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"D3dcompiler.lib")

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
HRESULT resize(int, int);
HRESULT initializeData();

char gszLogFileName[] = "Logs.txt";

float clearColor[4]; // RGBA
IDXGISwapChain *gpI_DXGI_SwapChain = NULL;
ID3D11Device *gpI_D3D11_Device = NULL;
ID3D11DeviceContext *gpI_D3D11_DeviceContext = NULL;
ID3D11RenderTargetView *gpI_D3D11_RenderTargetView = NULL;

ID3D11VertexShader *gpI_D3D11_VertexShader = NULL;
ID3D11PixelShader *gpI_D3D11_PixelShader = NULL;
ID3D11HullShader * gpI_D3D11_HullShader = NULL;
ID3D11DomainShader *gpI_D3D11_DomainShader = NULL;
ID3D11Buffer *gpI_D3D11_Buffer_PositionBuffer = NULL;
ID3D11InputLayout *gpI_D3D11_InputLayout = NULL;
ID3D11Buffer *gpI_D3D11_Buffer_Hull_ConstantBuffer = NULL;
ID3D11Buffer *gpI_D3D11_Buffer_Domain_ConstantBuffer = NULL;
ID3D11Buffer *gpI_D3D11_Buffer_Pixel_ConstantBuffer = NULL;

ID3DBlob *gpI_D3DBlob_VertexShaderCode = NULL;

unsigned int gNumOfLineSegments;


struct CBUFFER_HULL_SHADER
{
    XMVECTOR Hull_Constant_Function_Param;
};

struct CBUFFER_DOMAIN_SHADER
{
    XMMATRIX WorldViewProjectionMatrix;
};

struct CBUFFER_LINE_COLOR
{
    XMVECTOR LineColor;
};

XMMATRIX perspectiveProjectionMatrix;


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

        case VK_UP:
            gNumOfLineSegments++;
            if (gNumOfLineSegments >= 50)
                gNumOfLineSegments = 50;
            break;
        case VK_DOWN:
            gNumOfLineSegments--;
            if (gNumOfLineSegments <= 0)
                gNumOfLineSegments = 1;
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
    dxgi_SwapChainDesc.SampleDesc.Count = 1;
    dxgi_SwapChainDesc.SampleDesc.Quality = 0;
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
        "   float4 position : POSITION;" \
        "}; " \

        "outputVertex main(float2 pos : POSITION)" \
        "{   "\
        "   outputVertex outVec; " \
        "   outVec.position = float4(pos, 0.0, 0.0);" \
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
            outputFile << "D3DCompile() Failed For Vertex Shader : %s." << endl;
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

    //Hull Shader
    const char *hullShaderSourceCode =

        "cbuffer ConstanatBuffer " \
        "{  "\
        "   float4 hull_Constant_Funtion_Param;" \
        "} "\

        "struct outputVertex " \
        "{ " \
        "   float4 position : POSITION;" \
        "}; " \

        "struct hull_ConstantOutput " \
        "{ " \
        "   float edges[2] : SV_TESSFACTOR;" \
        "}; " \

        "hull_ConstantOutput hullConstantFunction(void) " \
        "{ " \
        "   hull_ConstantOutput output;" \
        "   float no_of_strips = hull_Constant_Funtion_Param[0];" \
        "   float no_of_segments = hull_Constant_Funtion_Param[1];" \
        "   output.edges[0] = no_of_strips; " \
        "   output.edges[1] = no_of_segments; " \
        "   return output;" \
        "}; " \

        "struct hull_Output " \
        "{ " \
        "   float4 position : POSITION;" \
        "}; " \

        "[domain(\"isoline\")]" \
        "[partitioning(\"integer\")]" \
        "[outputtopology(\"line\")]" \
        "[outputcontrolpoints(4)]" \
        "[patchconstantfunc(\"hullConstantFunction\")]" \

        "hull_Output main(InputPatch <outputVertex, 4> input_patch, uint i:SV_OUTPUTCONTROLPOINTID) " \
        "{   "\
        "   hull_Output output; " \
        "   output.position = input_patch[i].position;" \
        "   return (output); "\
        "} ";


    ID3DBlob *pI_D3DBlob_HullShaderCode = NULL;
    pI_D3DBlob_Error = NULL;
    hr = D3DCompile(hullShaderSourceCode,
        lstrlenA(hullShaderSourceCode) + 1,
        "HS",
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "hs_5_0",
        0,
        0,
        &pI_D3DBlob_HullShaderCode,
        &pI_D3DBlob_Error);
    if (FAILED(hr))
    {
        if (pI_D3DBlob_Error != NULL)
        {
            outputFile.open(gszLogFileName, std::ofstream::app);
            outputFile << "D3DCompile() Failed For Hull Shader : %s" << (char *)pI_D3DBlob_Error->GetBufferPointer() << endl;
            outputFile.close();
            pI_D3DBlob_Error->Release();
            pI_D3DBlob_Error = NULL;
            return(hr);
        }
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "D3DCompile() Succeeded For Hull Shader." << endl;
        outputFile.close();
    }

    hr = gpI_D3D11_Device->CreateHullShader(pI_D3DBlob_HullShaderCode->GetBufferPointer(), pI_D3DBlob_HullShaderCode->GetBufferSize(), NULL, &gpI_D3D11_HullShader);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateHullShader() Failed." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateHullShader() Succeeded." << endl;
        outputFile.close();
    }
    gpI_D3D11_DeviceContext->HSSetShader(gpI_D3D11_HullShader, 0, 0);
    pI_D3DBlob_HullShaderCode->Release();
    pI_D3DBlob_HullShaderCode = NULL;

    // DOMAIN Shader
    const char *domainShaderSourceCode =

        "cbuffer ConstanatBuffer " \
        "{  "\
        "   float4x4 worldViewProjectionMatrix;" \
        "} "\

        "struct hull_Output " \
        "{ " \
        "   float4 position : POSITION;" \
        "}; " \

        "struct domain_Output " \
        "{ " \
        "   float4 position : SV_POSITION;" \
        "}; " \

        "struct hull_ConstantOutput " \
        "{ " \
        "   float edges[2] : SV_TESSFACTOR;" \
        "}; " \

        "[domain(\"isoline\")]" \

        "domain_Output main(hull_ConstantOutput input, OutputPatch <hull_Output, 4> output_patch, float2 tessCoord : SV_DOMAINLOCATION)"
        "{ " \
        "   domain_Output output;" \
        "   float u = tessCoord.x;" \
        "   float3 p0 = output_patch[0].position.xyz;" \
        "   float3 p1 = output_patch[1].position.xyz;" \
        "   float3 p2 = output_patch[2].position.xyz;" \
        "   float3 p3 = output_patch[3].position.xyz;" \
        "   float u1 = (1.0 - u);" \
        "   float u2 = u * u;" \
        "   float b3 = u2 * u;" \
        "   float b2 = 3.0 * u2 * u1;" \
        "   float b1 = 3.0 * u * u1 * u1;" \
        "   float b0 = u1 * u1 * u1;" \
        "   float3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;" \
        "   output.position = mul(worldViewProjectionMatrix, float4(p, 1.0));"
        "   return output;"
        "}";

    ID3DBlob *pI_D3DBlob_DomainShaderCode = NULL;
    pI_D3DBlob_Error = NULL;
    hr = D3DCompile(domainShaderSourceCode,
        lstrlenA(domainShaderSourceCode) + 1,
        "DS",
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ds_5_0",
        0,
        0,
        &pI_D3DBlob_DomainShaderCode,
        &pI_D3DBlob_Error);
    if (FAILED(hr))
    {
        if (pI_D3DBlob_Error != NULL)
        {
            outputFile.open(gszLogFileName, std::ofstream::app);
            outputFile << "D3DCompile() Failed For Pixel Shader : %s" << (char *)pI_D3DBlob_Error->GetBufferPointer() << endl;
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

    hr = gpI_D3D11_Device->CreateDomainShader(pI_D3DBlob_DomainShaderCode->GetBufferPointer(), pI_D3DBlob_DomainShaderCode->GetBufferSize(), NULL, &gpI_D3D11_DomainShader);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateDomainShader() Failed." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateDomainShader() Succeeded." << endl;
        outputFile.close();
    }
    gpI_D3D11_DeviceContext->DSSetShader(gpI_D3D11_DomainShader, 0, 0);
    pI_D3DBlob_DomainShaderCode->Release();
    pI_D3DBlob_DomainShaderCode = NULL;

    
    // Pixel Shader

    const char *pixelShaderSourceCode =

        "cbuffer LineColor " \
        "{  "\
        "   float4 lineColor;" \
        "} "\

        "float4 main(void) : SV_TARGET"\
        "{ "\
        "   float4 outColot = lineColor;" \
        "   return (outColot);"\
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
            outputFile << "D3DCompile() Failed For Pixel Shader : %s" << (char *)pI_D3DBlob_Error->GetBufferPointer() << endl;
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

    return(S_OK);
}

HRESULT initializeData()
{
    HRESULT hr;
    float vertices[] = { -1.0f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, 1.0f, 1.0f };

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(float) * ARRAYSIZE(vertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = gpI_D3D11_Device->CreateBuffer(&bufferDesc, NULL, &gpI_D3D11_Buffer_PositionBuffer);

    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Failed For Vertex Buffer." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer." << endl;
        outputFile.close();
    }

    // copy vertices into above buffer
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    gpI_D3D11_DeviceContext->Map(gpI_D3D11_Buffer_PositionBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
    memcpy(mappedSubresource.pData, vertices, sizeof(vertices));
    gpI_D3D11_DeviceContext->Unmap(gpI_D3D11_Buffer_PositionBuffer, NULL);

    // define and set the constant buffer for Hull Shader
    D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
    ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
    bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER_HULL_SHADER);
    bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = gpI_D3D11_Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpI_D3D11_Buffer_Hull_ConstantBuffer);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Failed For Hull Constant Buffer." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Succeeded For Hull Constant Buffer." << endl;
        outputFile.close();
    }
    gpI_D3D11_DeviceContext->HSSetConstantBuffers(0, 1, &gpI_D3D11_Buffer_Hull_ConstantBuffer);


    // define and set the constant buffer for Domain Shader
    ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
    bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER_DOMAIN_SHADER);
    bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = gpI_D3D11_Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpI_D3D11_Buffer_Domain_ConstantBuffer);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Failed For Domain Constant Buffer." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Succeeded For Domain Constant Buffer." << endl;
        outputFile.close();
    }
    gpI_D3D11_DeviceContext->DSSetConstantBuffers(0, 1, &gpI_D3D11_Buffer_Domain_ConstantBuffer);


    // define and set the constant buffer for Pixel Shader
    ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
    bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER_LINE_COLOR);
    bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = gpI_D3D11_Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpI_D3D11_Buffer_Pixel_ConstantBuffer);
    if (FAILED(hr))
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Failed For Pixel Constant Buffer." << endl;
        outputFile.close();
        return(hr);
    }
    else
    {
        outputFile.open(gszLogFileName, std::ofstream::app);
        outputFile << "ID3D11Device::CreateBuffer() Succeeded For Pixel Constant Buffer." << endl;
        outputFile.close();
    }
    gpI_D3D11_DeviceContext->PSSetConstantBuffers(0, 1, &gpI_D3D11_Buffer_Pixel_ConstantBuffer);


    // create and set input layout
    D3D11_INPUT_ELEMENT_DESC inputElementDesc;
    inputElementDesc.SemanticName = "POSITION";
    inputElementDesc.SemanticIndex = 0;
    inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDesc.InputSlot = 0;
    inputElementDesc.AlignedByteOffset = 0;
    inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    inputElementDesc.InstanceDataStepRate = 0;

    hr = gpI_D3D11_Device->CreateInputLayout(&inputElementDesc, 1, gpI_D3DBlob_VertexShaderCode->GetBufferPointer(), gpI_D3DBlob_VertexShaderCode->GetBufferSize(), &gpI_D3D11_InputLayout);
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

    gNumOfLineSegments = 1;

    return hr;
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


    gpI_D3D11_DeviceContext->OMSetRenderTargets(1, &gpI_D3D11_RenderTargetView, NULL);

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

    return hr;
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

    if (gpI_D3D11_RenderTargetView)
    {
        gpI_D3D11_RenderTargetView->Release();
        gpI_D3D11_RenderTargetView = NULL;
    }

    if (gpI_DXGI_SwapChain)
    {
        gpI_DXGI_SwapChain->Release();
        gpI_DXGI_SwapChain = NULL;
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

    if (gpI_D3D11_HullShader)
    {
        gpI_D3D11_HullShader->Release();
        gpI_D3D11_HullShader = NULL;
    }

    if (gpI_D3D11_DomainShader)
    {
        gpI_D3D11_DomainShader->Release();
        gpI_D3D11_DomainShader = NULL;
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

    UINT stride = sizeof(float) * 2;
    UINT offset = 0;
    gpI_D3D11_DeviceContext->IASetVertexBuffers(0, 1, &gpI_D3D11_Buffer_PositionBuffer, &stride, &offset);

    gpI_D3D11_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    CBUFFER_HULL_SHADER hullShaderBuffer;
    hullShaderBuffer.Hull_Constant_Function_Param = XMVectorSet(1.0f, gNumOfLineSegments, 0.0f, 0.0f);
    gpI_D3D11_DeviceContext->UpdateSubresource(gpI_D3D11_Buffer_Hull_ConstantBuffer, 0, NULL, &hullShaderBuffer, 0, 0);


    XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);
    XMMATRIX viewMatrix = XMMatrixIdentity();
    XMMATRIX wvpMatrix = worldMatrix * viewMatrix * perspectiveProjectionMatrix;

    CBUFFER_DOMAIN_SHADER constantBuffer;
    constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
    gpI_D3D11_DeviceContext->UpdateSubresource(gpI_D3D11_Buffer_Domain_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

    CBUFFER_LINE_COLOR lineColorBuffer;

    if(gNumOfLineSegments == 1 || gNumOfLineSegments == 50)
        lineColorBuffer.LineColor = XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f);
    else
        lineColorBuffer.LineColor = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

    gpI_D3D11_DeviceContext->UpdateSubresource(gpI_D3D11_Buffer_Pixel_ConstantBuffer, 0, NULL, &lineColorBuffer, 0, 0);


    TCHAR str[255];
    wsprintf(str, TEXT("Tesselataion Window : [ Segments = %d ]"), gNumOfLineSegments);
    SetWindowText(gHwnd, str);

    gpI_D3D11_DeviceContext->Draw(4, 0);

    gpI_DXGI_SwapChain->Present(0, 0);
}
