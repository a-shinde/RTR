//Header files.

#include <windows.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include <fstream>
#include <vmath.h>
#include "Sphere.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"Sphere.lib")


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
GLuint vbo_element;
GLuint vao_sphere;

GLuint vertexShaderObject;
GLuint fragmentShaderObject;
GLuint shaderProgramObject;

GLuint model_matrix_uniform;
GLuint view_matrix_uniform;
GLuint projection_uniform;
GLuint LKeyPressed_uniform;


GLuint La_Uniform;
GLuint Ld_Uniform;
GLuint Ls_Uniform;
GLuint lightPosition_uniform;

GLuint Ka_Uniform;
GLuint Kd_Uniform;
GLuint Ks_Uniform;
GLuint Kshine_Uniform;

mat4 perspectiveProjectionMatrix;
mat4 roatationMatrix;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumVertices;
unsigned int gNumElements;

GLfloat light_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat light_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat light_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat light_position[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f;


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
                gbAnimation = false;
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

        "uniform mat4 u_model_matrix;" \
        "uniform mat4 u_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform vec4 u_light_position;" \
        "uniform int u_lighting_enabled;" \

        "out vec3 transformed_normals;" \
        "out vec3 light_direction;" \
        "out vec3 viewer_vector;" \

        "void main(void)" \
        "{" \
        "   if(u_lighting_enabled==1)" \
        "   {" \
        "       vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
        "       transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
        "       light_direction = vec3(u_light_position) - eye_coordinates.xyz;" \
        "       viewer_vector = -eye_coordinates.xyz;" \
        "   }" \
        "   gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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

        "in vec3 transformed_normals;" \
        "in vec3 light_direction;" \
        "in vec3 viewer_vector;" \
   
        "uniform vec3 u_La;" \
        "uniform vec3 u_Ld;" \
        "uniform vec3 u_Ls;" \
        "uniform vec3 u_Ka;" \
        "uniform vec3 u_Kd;" \
        "uniform vec3 u_Ks;" \
        "uniform float u_material_shininess;" \
        "uniform int u_lighting_enabled;" \

        "out vec4 FragColor;" \
        
        "void main(void)" \
        "{" \
        "   vec3 phong_ads_color;" \
        "   if(u_lighting_enabled==1)" \
        "   {" \
        "       vec3 normalized_transformed_normals=normalize(transformed_normals);" \
        "       vec3 normalized_light_direction=normalize(light_direction);" \
        "       vec3 normalized_viewer_vector=normalize(viewer_vector);" \
        "       vec3 ambient = u_La * u_Ka;" \
        "       float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);" \
        "       vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
        "       vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
        "       vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" \
        "       phong_ads_color=ambient + diffuse + specular;" \
        "   }" \
        "   else" \
        "   {" \
        "       phong_ads_color = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "   FragColor = vec4(phong_ads_color, 1.0);" \
        "}";
    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
    glCompileShader(fragmentShaderObject);

    iInfoLogLength = 0;
    iShaderCompiledStatus = 0;
    szInfoLog = NULL;
    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
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

    model_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
    view_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
    projection_uniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

    LKeyPressed_uniform = glGetUniformLocation(shaderProgramObject, "u_lighting_enabled");

    La_Uniform = glGetUniformLocation(shaderProgramObject, "u_La");
    Ld_Uniform = glGetUniformLocation(shaderProgramObject, "u_Ld");
    Ls_Uniform = glGetUniformLocation(shaderProgramObject, "u_Ls");

    Ka_Uniform = glGetUniformLocation(shaderProgramObject, "u_Ka");
    Kd_Uniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
    Ks_Uniform = glGetUniformLocation(shaderProgramObject, "u_Ks");
    Kshine_Uniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");

    lightPosition_uniform = glGetUniformLocation(shaderProgramObject, "u_light_position");;
}

void initializeData()
{
    getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

    glGenVertexArrays(1, &vao_sphere);
    glBindVertexArray(vao_sphere);

    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(POSITION_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(POSITION_ATTRIB);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_normal);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(NORMAL_ATTRIB);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

    if (vao_sphere)
    {
        glDeleteVertexArrays(1, &vao_sphere);
        vao_sphere = 0;
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

    if (gbLight)
    {
        glUniform1i(LKeyPressed_uniform, 1);
        glUniform3fv(La_Uniform, 1, light_ambient);
        glUniform3fv(Ld_Uniform, 1, light_diffuse);
        glUniform3fv(Ls_Uniform, 1, light_specular);
        glUniform4fv(lightPosition_uniform, 1, light_position);

        glUniform3fv(Ka_Uniform, 1, material_ambient);
        glUniform3fv(Kd_Uniform, 1, material_diffuse);
        glUniform3fv(Ks_Uniform, 1, material_specular);
        glUniform1f(Kshine_Uniform, material_shininess);
    }
    else
    {
        glUniform1i(LKeyPressed_uniform, 0);
    }

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    modelMatrix = translate(0.0f, 0.0f, -2.0f);
    roatationMatrix = vmath::rotate(angleRotate, angleRotate, angleRotate);
    modelMatrix = modelMatrix * roatationMatrix;

    glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    glBindVertexArray(vao_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
    SwapBuffers(gHdc);
}

void Update()
{
    if (angleRotate > 360.0f)
        angleRotate = 0.0f;
    if (gbAnimation)
        angleRotate += 0.01f;
}
