//Header files.

#include <windows.h>
#include <fstream>
#include <d3d11.h>

#pragma comment (lib,"d3d11.lib")


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
void initializeShaders();
void initializeData();

char gszLogFileName[] = "Logs.txt";


enum
{
    POSITION_ATTRIB = 0,
    COLOR_ATTRIB,
    NORMAL_ATTRIB,
    TEXTURE0_ATTRIB
};

float clearColor[4]; // RGBA
IDXGISwapChain *gpI_DXGI_SwapChain = NULL;
ID3D11Device *gpI_D3D11_Device = NULL;
ID3D11DeviceContext *gpI_D3D11_DeviceContext = NULL;
ID3D11RenderTargetView *gpI_D3D11_RenderTargetView = NULL;

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


    clearColor[0] = 0.0f;
    clearColor[1] = 0.0f;
    clearColor[2] = 1.0f;
    clearColor[3] = 1.0f;

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

void initializeShaders()
{
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

    gpI_DXGI_SwapChain->Present(0, 0);
}
