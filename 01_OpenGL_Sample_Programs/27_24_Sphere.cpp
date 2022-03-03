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

enum
{
    LIGHTING_PER_VERTEX = 0,
    LIGHTING_PER_FRAGMENT
} lightingType;

enum axia
{
    Xaxis,
    Yaxis,
    Zaxis,
}rotationAxis;

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
GLuint lightingType_uniform;

GLuint La_Uniform;
GLuint Ld_Uniform;
GLuint Ls_Uniform;
GLuint light_Position_uniform;

GLuint Ka_Uniform;
GLuint Kd_Uniform;
GLuint Ks_Uniform;
GLuint Kshine_Uniform;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumVertices;
unsigned int gNumElements;

mat4 perspectiveProjectionMatrix;
mat4 roatationMatrix;

float angleRed = 0.0f;
float angleGreen = 2.8f;
float angleBlue = 0.0f;
float radius = 150.0f;

GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_position[] = { 0.0f, 0.0f, 2.0f, 1.0f };

GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f;

float materials[24][13] = { { 0.0215f, 0.1745f, 0.0215f, 1.0f, 0.07568f, 0.61424f, 0.07568f, 1.0f, 0.633f, 0.727811f, 0.633f, 1.0f, 0.6f * 128 },
{ 0.135f, 0.2225f, 0.1575f, 1.0f, 0.54f, 0.89f, 0.63f, 1.0f, 0.316228f, 0.316228f, 0.316228f, 1.0f, 0.1f * 128 },
{ 0.05375f, 0.05f, 0.06625f, 1.0f, 0.18275f, 0.17f, 0.22525f, 1.0f, 0.332741f, 0.328634f, 0.346435f, 1.0f, 0.3f * 128 },
{ 0.25f, 0.20725f, 0.20725f, 1.0f, 1.0f, 0.829f, 0.829f, 1.0f, 0.296648f, 0.296648f, 0.296648f, 1.0f, 0.088f * 128 },
{ 0.1745f, 0.01175f, 0.01175f, 1.0f, 0.61424f, 0.04136f, 0.04136f, 1.0f, 0.727811f, 0.626959f, 0.626959f, 1.0f, 0.6 * 128 },
{ 0.1f, 0.18725f, 0.1745f, 1.0f, 0.396f, 0.74151f, 0.69102f, 1.0f, 0.297254f, 0.30829f, 0.306678f, 1.0f, 0.1 * 128 },
{ 0.329412f, 0.223529f, 0.027451f, 1.0f, 0.780392f, 0.568627f, 0.113725f, 1.0f, 0.992157f, 0.941176f, 0.807843f, 1.0f, 0.21794872 * 128 },
{ 0.2125f, 0.1275f, 0.054f, 1.0f, 0.714f, 0.4284f, 0.18144f, 1.0f, 0.393548f, 0.271906f, 0.166721f, 1.0f, 0.2 * 128 },
{ 0.25f, 0.25f, 0.25f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f, 0.774597f, 0.774597f, 0.774597f, 1.0f, 0.6 * 128 },
{ 0.19125f, 0.0735f, 0.0225f, 1.0f, 0.7038f, 0.27048f, 0.0828f, 1.0f, 0.256777f, 0.137622f, 0.086014f, 1.0f, 0.1 * 128 },
{ 0.24725f, 0.1995f, 0.0745f, 1.0f, 0.75164f, 0.60648f, 0.22648f, 1.0f, 0.628281f, 0.555802f, 0.366065f, 1.0f, 0.4 * 128 },
{ 0.19225f, 0.19225f, 0.19225f, 1.0f, 0.50754f, 0.50754f, 0.50754f, 1.0f, 0.508273f, 0.508273f, 0.508273f, 1.0f, 0.4 * 128 },
{ 0.0f, 0.0f, 0.0f, 1.0f, 0.01f, 0.01f, 0.01f, 1.0f, 0.50f, 0.50f, 0.50f, 1.0f, 0.25 * 128 },
{ 0.0f, 0.1f, 0.06f, 1.0f, 0.0f, 0.50980392f, 0.50980392f, 1.0f, 0.50196078f, 0.50196078f, 0.50196078f, 1.0f, 0.25 * 128 },
{ 0.0f, 0.0f, 0.0f, 1.0f, 0.1f, 0.35f, 0.1f, 1.0f, 0.45f, 0.55f, 0.45f, 1.0f, 0.25f * 128 },
{ 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.7f, 0.6f, 0.6f, 1.0f, 0.25f * 128 },
{ 0.0f, 0.0f, 0.0f, 1.0f, 0.55f, 0.55f, 0.55f, 1.0f, 0.70f, 0.70f, 0.70f, 1.0f, 0.25f * 128 },
{ 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.60f, 0.60f, 0.50f, 1.0f, 0.25f * 128 },
{ 0.02f, 0.02f, 0.02f, 1.0f, 0.01f, 0.01f, 0.01f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f, 0.078125f * 128 },
{ 0.0f, 0.05f, 0.05f, 1.0f, 0.4f, 0.5f, 0.5f, 1.0f, 0.04f, 0.7f, 0.7f, 1.0f, 0.078125f * 128 },
{ 0.0f, 0.05f, 0.0f, 1.0f, 0.4f, 0.5f, 0.4f, 1.0f, 0.04f, 0.7f, 0.04f, 1.0f, 0.078125f * 128 },
{ 0.05f, 0.0f, 0.0f, 1.0f, 0.5f, 0.4f, 0.4f, 1.0f, 0.7f, 0.04f, 0.04f, 1.0f, 0.078125f * 128 },
{ 0.05f, 0.05f, 0.05f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.078125f * 128 },
{ 0.05f, 0.05f, 0.0f, 1.0f, 0.5f, 0.5f, 0.4f, 1.0f, 0.7f, 0.7f, 0.04f, 1.0f, 0.078125f * 128 } };


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

        case 0x56:  //for 'V'
            lightingType = LIGHTING_PER_VERTEX;
            break;

        case 0x50:  //for 'P'
            lightingType = LIGHTING_PER_FRAGMENT;
            break;


        case 0x58:  //for 'X'
            rotationAxis = Xaxis;
            gbAnimation = true;
            break;

        case 0x59:  //for 'Y'
            rotationAxis = Yaxis;
            gbAnimation = true;
            break;

        case 0x5A:  //for 'Z'
            rotationAxis = Zaxis;
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
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_CULL_FACE);

    gbAnimation = false;
    gbLight = false;
    lightingType = LIGHTING_PER_FRAGMENT;

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
        "uniform int u_lighting_enabled;" \
        "uniform int u_lighting_type;" \
        "uniform vec3 u_La[1];" \
        "uniform vec3 u_Ld[1];" \
        "uniform vec3 u_Ls[1];" \
        "uniform vec4 u_light_position[1];" \
        "uniform vec3 u_Ka;" \
        "uniform vec3 u_Kd;" \
        "uniform vec3 u_Ks;" \
        "uniform float u_material_shininess;" \

        "out vec3 out_color;" \
        "out vec3 transformed_normals;" \
        "out vec3 light_direction[1];" \
        "out vec3 viewer_vector;" \

        "void main(void)" \
        "{" \
        "   if(u_lighting_enabled == 1)" \
        "   {" \
        "       for(int index = 0; index < 1; index++)"\
        "       {" \
        "           vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
        "           transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
        "           light_direction[index] = vec3(u_light_position[index]) - eye_coordinates.xyz;" \
        "           viewer_vector = -eye_coordinates.xyz;" \
        "           if(u_lighting_type == 0)" \
        "           {" \
        "               transformed_normals = normalize(transformed_normals);" \
        "               light_direction[index] = normalize(light_direction[index]);" \
        "               viewer_vector = normalize(viewer_vector);" \
        "               float tn_dot_ld = max(dot(transformed_normals, light_direction[index]),0.0);" \
        "               vec3 ambient = u_La[index] * u_Ka;" \
        "               vec3 diffuse = u_Ld[index] * u_Kd * tn_dot_ld;" \
        "               vec3 reflection_vector = reflect(-light_direction[index], transformed_normals);" \
        "               vec3 viewer_vector1 = normalize(-eye_coordinates.xyz);" \
        "               vec3 specular = u_Ls[index] * u_Ks * pow(max(dot(reflection_vector, viewer_vector1), 0.0), u_material_shininess);" \
        "               out_color = out_color + ambient + diffuse + specular;" \
        "           }" \
        "       }" \
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

        "in vec3 out_color;" \
        "in vec3 transformed_normals;" \
        "in vec3 light_direction[1];" \
        "in vec3 viewer_vector;" \

        "uniform vec3 u_La[1];" \
        "uniform vec3 u_Ld[1];" \
        "uniform vec3 u_Ls[1];" \
        "uniform vec3 u_Ka;" \
        "uniform vec3 u_Kd;" \
        "uniform vec3 u_Ks;" \
        "uniform float u_material_shininess;" \
        "uniform int u_lighting_enabled;" \
        "uniform int u_lighting_type;" \

        "out vec4 FragColor;" \

        "void main(void)" \
        "{" \
        "   vec3 phong_ads_color;" \
        "   if(u_lighting_enabled == 1)" \
        "   {" \
        "       if (u_lighting_type == 1)" \
        "       {" \
        "           vec3 normalized_transformed_normals=normalize(transformed_normals);" \
        "           vec3 normalized_viewer_vector=normalize(viewer_vector);" \
        "           for(int index = 0; index < 1; index++)"\
        "           {" \
        "               vec3 normalized_light_direction=normalize(light_direction[index]);" \
        "               vec3 ambient = u_La[index] * u_Ka;" \
        "               float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);" \
        "               vec3 diffuse = u_Ld[index] * u_Kd * tn_dot_ld;" \
        "               vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
        "               vec3 specular = u_Ls[index] * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" \
        "               phong_ads_color= phong_ads_color + ambient + diffuse + specular;" \
        "           }" \
        "       }" \
        "       else" \
        "       {" \
        "           phong_ads_color = out_color;" \
        "       }" \
        "   }" \
        "   else" \
        "   {" \
        "       phong_ads_color = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "   phong_ads_color = min(phong_ads_color, vec3(1.0)) ;"\
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
    lightingType_uniform = glGetUniformLocation(shaderProgramObject, "u_lighting_type");

    La_Uniform = glGetUniformLocation(shaderProgramObject, "u_La");
    Ld_Uniform = glGetUniformLocation(shaderProgramObject, "u_Ld");
    Ls_Uniform = glGetUniformLocation(shaderProgramObject, "u_Ls");

    Ka_Uniform = glGetUniformLocation(shaderProgramObject, "u_Ka");
    Kd_Uniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
    Ks_Uniform = glGetUniformLocation(shaderProgramObject, "u_Ks");
    Kshine_Uniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");

    light_Position_uniform = glGetUniformLocation(shaderProgramObject, "u_light_position");;
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

    perspectiveProjectionMatrix = perspective(10.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

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

    if (gbAnimation)
    {
        switch (rotationAxis)
        {
        case Xaxis:
            light_position[0] = 0;
            light_position[1] = radius*cos(angleRotate);
            light_position[2] = radius*sin(angleRotate);
            break;

        case Yaxis:
            light_position[0] = radius*cos(angleRotate);
            light_position[1] = 0;
            light_position[2] = radius*sin(angleRotate);
            break;

        case Zaxis:
            light_position[0] = radius*cos(angleRotate);
            light_position[1] = radius*sin(angleRotate);
            light_position[2] = -50;
            break;
        }
    }

    if (gbLight)
    {
        glUniform1i(LKeyPressed_uniform, 1);
        glUniform3fv(La_Uniform, 1, (GLfloat *)light_ambient);
        glUniform3fv(Ld_Uniform, 1, (GLfloat *)light_diffuse);
        glUniform3fv(Ls_Uniform, 1, (GLfloat *)light_specular);
        glUniform4fv(light_Position_uniform, 1, (GLfloat *)light_position);

        glUniform1i(lightingType_uniform, lightingType);
    }
    else
    {
        glUniform1i(LKeyPressed_uniform, 0);
    }

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, perspectiveProjectionMatrix);


    for (int row = 0; row < 6; row++)
    {
        for (int column = 0; column < 4; column++)
        {
            GLfloat material_ambient[] = { materials[column * 6 + row][0], materials[column * 6 + row][1], materials[column * 6 + row][2], materials[column * 6 + row][3] };
            glUniform3fv(Ka_Uniform, 1, material_ambient);

            GLfloat material_diffuse[] = { materials[column * 6 + row][4], materials[column * 6 + row][5], materials[column * 6 + row][6], materials[column * 6 + row][7] };
            glUniform3fv(Kd_Uniform, 1, material_diffuse);

            GLfloat material_specular[] = { materials[column * 6 + row][8], materials[column * 6 + row][9], materials[column * 6 + row][10], materials[column * 6 + row][11] };
            glUniform3fv(Ks_Uniform, 1, material_specular);

            glUniform1f(Kshine_Uniform, materials[column * 6 + row][12]);

            modelMatrix = translate(((GLfloat)column / 0.32f) - 4.8f, ((GLfloat)(6 - row) / 0.8f) - 4.5f, -50.0f);

            glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);

            glBindVertexArray(vao_sphere);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
            glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
            glBindVertexArray(0);
        }
    }

    glUseProgram(0);
    SwapBuffers(gHdc);
}

void Update()
{
    if (angleRotate > 360.0f)
        angleRotate = 0.0f;
    if (gbAnimation)
        angleRotate += 0.001f;
}
