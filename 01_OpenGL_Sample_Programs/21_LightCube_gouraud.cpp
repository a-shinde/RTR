//Header files.

#include <windows.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include <fstream>
#include <vmath.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")


using namespace std;
using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

int currentWidth;
int currentHeight;

HWND gHwnd;
HDC gHdc;
HGLRC gHglrc;

bool gbFullscreen;
bool gbLight;
bool gbAnimation;
bool gbEscapeKeyIsPressed;
bool gbActiveWindow;
bool bDone;
float angleRotate;

WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
DWORD dwStyle;
ofstream outputFile;

//Fun Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Initialize();
void FirstResize();
void Uninitialize();
void Display();
void Update();
void resize(int, int);
void initializeShaders();
void initializeData();

enum
{
    POSITION_ATTRIB = 0,
    COLOR_ATTRIB,
    NORMAL_ATTRIB,
    TEXTURE0_ATTRIB
};

GLuint vbo_position;
GLuint vbo_normal;
GLuint vao_cube;

GLuint vertexShaderObject;
GLuint fragmentShaderObject;
GLuint shaderProgramObject;

GLuint mv_matrix_uniform;
GLuint projection_uniform;
GLuint LKeyPressed_uniform;
GLuint Ld_Uniform;
GLuint Kd_Uniform;
GLuint lightPosition_uniform;

mat4 perspectiveProjectionMatrix;
mat4 roatationMatrix;

//Code
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdArgs, int cmdShow)
{

    TCHAR className[] = TEXT("OpenGL_Window");
    WNDCLASSEX wndClassEx;
    HWND hWnd;
    MSG msg;

    outputFile.open("Logs.txt", std::ofstream::out);
    if (outputFile.is_open())
    {
        outputFile << "------------------------------" << endl;
        outputFile << "----     Amol Shinde      ----" << endl;
        outputFile << "------------------------------" << endl;
        outputFile << "Log File Successfully Opened" << endl;

    }

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


    hWnd = CreateWindowEx(WS_EX_APPWINDOW, className, TEXT("OpenGL_PP"),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
        x, y, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

    gHwnd = hWnd;
    Initialize();

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
    void resize(int, int);

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
        resize(LOWORD(lParam), HIWORD(lParam));
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
            if (gbLight == true)
                gbLight = false;
            else
                gbLight = true;
            break;

        case 0x41:  //for 'A'
            if (gbAnimation == true)
                gbAnimation= false;
            else
                gbAnimation = true;
            break;

        default:
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

void Initialize()
{
    PIXELFORMATDESCRIPTOR pfd;

    int iPixelFormatIndex;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nVersion = 1;
    pfd.cColorBits = 32;
    pfd.cGreenBits = 8;
    pfd.cRedBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 32;
    pfd.cDepthBits = 32;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);

    gHdc = GetDC(gHwnd);

    iPixelFormatIndex = ChoosePixelFormat(gHdc, &pfd);
    if (iPixelFormatIndex == 0)
    {
        ReleaseDC(gHwnd, gHdc);
        gHdc = NULL;
    }

    if (SetPixelFormat(gHdc, iPixelFormatIndex, &pfd) == false)
    {
        ReleaseDC(gHwnd, gHdc);
        gHdc = NULL;
    }

    gHglrc = wglCreateContext(gHdc);
    if (gHglrc == NULL)
    {
        ReleaseDC(gHwnd, gHdc);
        gHdc = NULL;
    }

    if (wglMakeCurrent(gHdc, gHglrc) == NULL)
    {
        wglDeleteContext(gHglrc);
        gHglrc = NULL;
        ReleaseDC(gHwnd, gHdc);
        gHdc = NULL;
    }

    GLenum gl_Error = glewInit();
    if (gl_Error != GLEW_OK)
    {
        wglDeleteContext(gHglrc);
        gHglrc = NULL;
        ReleaseDC(gHwnd, gHdc);
        gHdc = NULL;
    }

    initializeShaders();
    initializeData();

    perspectiveProjectionMatrix = mat4::identity();
    roatationMatrix = mat4::identity();

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_CULL_FACE);

    gbAnimation = false;
    gbLight = false;

    FirstResize();
}

void initializeShaders()
{

    //----------Vertex Shader--------------
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertextShaderSourceCode =
        "#version 430 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \

        "uniform mat4 u_model_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform int u_LKeyPressed;" \
        "uniform vec3 u_Ld;" \
        "uniform vec3 u_Kd;" \
        "uniform vec4 u_light_position;" \

        "out vec3 diffuse_light;" \
        "void main(void)" \
        "{" \
        "   if (u_LKeyPressed == 1)" \
        "   {" \
        "       vec4 eyeCoordinates = u_model_view_matrix * vPosition;" \
        "       vec3 tnorm = normalize(mat3(u_model_view_matrix) * vNormal);" \
        "       vec3 s = normalize(vec3(u_light_position - eyeCoordinates));" \
        "       diffuse_light = u_Ld * u_Kd * max(dot(s, tnorm), 0.0);" \
        "   }" \
        "   gl_Position = u_projection_matrix * u_model_view_matrix * vPosition;" \
        "}";

    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertextShaderSourceCode, NULL);
    glCompileShader(vertexShaderObject);

    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char *szInfoLog = NULL;
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject, iInfoLogLength, &written, szInfoLog);
                outputFile << "Vertex Shader Compilation Log :" << szInfoLog << endl;
                free(szInfoLog);
                Uninitialize();
                exit(0);
            }
        }
    }

    //----------Fragment Shader--------------
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragmentShaderSourceCode =
        "#version 430 core" \
        "\n" \
        "in vec3 diffuse_light;" \
        "out vec4 FragColor;" \
        "uniform int u_LKeyPressed;" \
        "void main(void)" \
        "{" \
        "   vec4 color;" \
        "   if (u_LKeyPressed == 1)" \
        "   {" \
        "       color = vec4(diffuse_light,1.0);" \
        "   }" \
        "   else" \
        "   {" \
        "       color = vec4(1.0, 1.0, 1.0, 1.0);" \
        "   }" \
        "   FragColor = color;" \
        "}";

    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
    glCompileShader(fragmentShaderObject);

    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject, iInfoLogLength, &written, szInfoLog);
                outputFile << "Fragment Shader Compilation Log :" << szInfoLog << endl;
                free(szInfoLog);
                Uninitialize();
                exit(0);
            }
        }
    }

    //-------------Create Program---------------
    shaderProgramObject = glCreateProgram();
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    glBindAttribLocation(shaderProgramObject, POSITION_ATTRIB, "vPosition");
    glBindAttribLocation(shaderProgramObject, NORMAL_ATTRIB, "vNormal");


    glLinkProgram(shaderProgramObject);

    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength>0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject, iInfoLogLength, &written, szInfoLog);
                outputFile << "Shader Program Link Log :" << szInfoLog << endl;
                free(szInfoLog);
                Uninitialize();
                exit(0);
            }
        }
    }

    mv_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_model_view_matrix");
    projection_uniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

    LKeyPressed_uniform = glGetUniformLocation(shaderProgramObject, "u_LKeyPressed");

    Ld_Uniform = glGetUniformLocation(shaderProgramObject, "u_Ld");
    Kd_Uniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
    lightPosition_uniform = glGetUniformLocation(shaderProgramObject, "u_light_position");;
}

void initializeData()
{
    const GLfloat cubeCord[] =
    {
        //Front Face
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        //Right Face
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        //Back Face
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        //Left Face
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        //Top face
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        //Bottom face
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f };

    const GLfloat cubeNormal[] =
    {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,

        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f
    };

    glGenVertexArrays(1, &vao_cube);
    glBindVertexArray(vao_cube);

    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeCord), cubeCord, GL_STATIC_DRAW);
    glVertexAttribPointer(POSITION_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(POSITION_ATTRIB);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_normal);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormal), cubeNormal, GL_STATIC_DRAW);
    glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(NORMAL_ATTRIB);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void FirstResize()
{
    currentWidth = WIN_WIDTH - GetSystemMetrics(SM_CXSIZEFRAME) * 4;
    currentHeight = WIN_HEIGHT - GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CYSIZEFRAME) * 4;
    resize(currentWidth, currentHeight);
}

void resize(int width, int height)
{
    if (height == 0)
        height = 1;
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

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

    if (vao_cube)
    {
        glDeleteVertexArrays(1, &vao_cube);
        vao_cube = 0;
    }

    // destroy vbo
    if (vbo_position)
    {
        glDeleteBuffers(1, &vbo_position);
        vbo_position = 0;
    }
    if (vbo_normal)
    {
        glDeleteBuffers(1, &vbo_normal);
        vbo_normal = 0;
    }

    glDetachShader(shaderProgramObject, vertexShaderObject);
    glDetachShader(shaderProgramObject, fragmentShaderObject);

    glDeleteShader(vertexShaderObject);
    vertexShaderObject = 0;
    glDeleteShader(fragmentShaderObject);
    fragmentShaderObject = 0;

    glDeleteProgram(shaderProgramObject);
    shaderProgramObject = 0;

    glUseProgram(0);

    wglMakeCurrent(NULL, NULL);

    wglDeleteContext(gHglrc);
    gHglrc = NULL;

    ReleaseDC(gHwnd, gHdc);
    gHdc = NULL;

    if (outputFile.is_open())
    {
        outputFile << "Log File Successfully Closed.";
        outputFile.close();
    }
    DestroyWindow(gHwnd);
}

void Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramObject);

    mat4 modelViewMatrix = mat4::identity();
    mat4 mvpMatrix = mat4::identity();

    modelViewMatrix = translate(0.0f, 0.0f, -4.0f);
    modelViewMatrix = modelViewMatrix * scale(0.75f, 0.75f, 0.75f);
    roatationMatrix = vmath::rotate(angleRotate, angleRotate, angleRotate);
    modelViewMatrix = modelViewMatrix * roatationMatrix;

    if (gbLight)
    {
        glUniform1i(LKeyPressed_uniform, 1);
        glUniform3f(Ld_Uniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(Kd_Uniform, 0.5f, 0.5f, 0.5f);

        GLfloat light_position[] = { 0.0f, 0.0f, 2.0f, 1.0f };
        glUniform4fv(lightPosition_uniform, 1, light_position);
    }
    else
    {
        glUniform1i(LKeyPressed_uniform, 0);
    }

    glUniformMatrix4fv(mv_matrix_uniform, 1, GL_FALSE, modelViewMatrix);
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, perspectiveProjectionMatrix);
        
    glBindVertexArray(vao_cube);
    for (int i = 0; i < 6; i++)
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    glBindVertexArray(0);

    glUseProgram(0);
    SwapBuffers(gHdc);
}

void Update()
{
    if (angleRotate > 360.0f)
        angleRotate = 0.0f;
    if(gbAnimation)
        angleRotate += 0.01f;
}
