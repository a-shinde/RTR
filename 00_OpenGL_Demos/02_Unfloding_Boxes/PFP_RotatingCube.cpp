#include "PFP_RotatingCube.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdArgs, int cmdShow)
{

    TCHAR className[] = TEXT("OpenGL_Window");
    WNDCLASSEX wndClassEx;
    HWND hWnd;
    MSG msg;

    if (fopen_s(&ptrFile, "log.txt", "w") != 0)
    {
        MessageBox(NULL, TEXT("Log File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST);
        exit(0);
    }
    else
    {
        fprintf(ptrFile, "Log file created successfully.\n");
    }

    wndClassEx.cbSize = sizeof(WNDCLASSEX);
    wndClassEx.cbClsExtra = 0;
    wndClassEx.cbWndExtra = 0;
    wndClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wndClassEx.hInstance = hInstance;
    wndClassEx.lpfnWndProc = WndProc;
    wndClassEx.lpszMenuName = NULL;
    wndClassEx.lpszClassName = className;
    wndClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    int monitorWidth = GetSystemMetrics(SM_CXSCREEN);
    int monitorHeight = GetSystemMetrics(SM_CYSCREEN);

    int x = (monitorWidth / 2) - (WIN_WIDTH / 2);
    int y = (monitorHeight / 2) - (WIN_HEIGHT / 2);

    RegisterClassEx(&wndClassEx);

    hWnd = CreateWindowEx(WS_EX_APPWINDOW, className, TEXT("Unfolding Boxes | programmable Function Pipeline"),
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
                bDone = (BOOL)TRUE;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {

            if (gbActiveWindow == (BOOL)true)
            {
                if (gbEscapeKeyIsPressed == (BOOL)true)
                    bDone = (BOOL)true;
            }
            // Here Game Plays
            Display();
            spin();
        }
    }
    UnInitialize();
    return (int)msg.wParam;
}

void Initialize()
{
    void resize(int, int);
    int LoadGLTextures(GLuint *, TCHAR[]);
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

    //VertexShader
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertexShaderSourceCode =
        "#version 430 core "\
        "\n "\
        "in vec3 vSh_Normal;" \
        "in vec4 vSh_Position; "\

        "in vec2 vTexture0_Coord;" \
        "out vec2 out_texture0_coord;" \

        "uniform mat4 mSh_U_P_Matrix;" \
        "uniform int u_LKeyPressed;" \
        "uniform vec3 u_Ld;" \
        "uniform vec3 u_Kd;" \
        "uniform vec4 u_light_position;" \
        "out vec3 diffuse_light;" \
        "uniform mat4 mSh_U_MV_Matrix; "\
        " "\
        "void main(void) "\
        "{ "\
        "vec4 eyeCoordinates = mSh_U_MV_Matrix * vSh_Position;" \
        "mat3 normal_matrix = mat3(transpose(inverse(mSh_U_MV_Matrix)));" \
        "vec3 tnorm = normalize(normal_matrix * vSh_Normal);" \
        "vec3 s = normalize(vec3(u_light_position - eyeCoordinates));" \
        "diffuse_light = u_Ld * u_Kd * max(dot(s, tnorm), 0.0);" \
        "gl_Position = mSh_U_P_Matrix * mSh_U_MV_Matrix * vSh_Position;" \
        "out_texture0_coord = vTexture0_Coord;"
        "} ";


    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
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
                fprintf(ptrFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                UnInitialize();
                exit(0);
            }
        }
    }


    //FragmentShader
    pixelShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* pixelShaderSourceCode =
        "#version 430 core "\
        "\n"\
        "out vec4 fragColor;"\
        "in vec2 out_texture0_coord;" \
        "in vec3 diffuse_light;" \
        "uniform int u_LKeyPressed;" \
        "uniform sampler2D u_texture0_sampler;" \
        "void main(void)"
        "{ "\
        "fragColor = texture(u_texture0_sampler, out_texture0_coord) * 0.2 + vec4(diffuse_light,1.0) * texture(u_texture0_sampler, out_texture0_coord) * 1.5;"\
        "} ";

    glShaderSource(pixelShaderObject, 1, (const GLchar**)&pixelShaderSourceCode, NULL);
    glCompileShader(pixelShaderObject);


    glGetShaderiv(pixelShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(pixelShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(pixelShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(ptrFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                UnInitialize();
                exit(0);
            }
        }
    }

    //Shader Program 
    shaderProgramObject = glCreateProgram();
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, pixelShaderObject);

    glBindAttribLocation(shaderProgramObject, vSh_Position, "vSh_Position");
    glBindAttribLocation(shaderProgramObject, vSh_Normal, "vSh_Normal");
    glBindAttribLocation(shaderProgramObject, vSh_Texture, "vTexture0_Coord");
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
                fprintf(ptrFile, "Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                UnInitialize();
                exit(0);
            }
        }
    }

    mSh_U_MV_Matrix = glGetUniformLocation(shaderProgramObject, "mSh_U_MV_Matrix");
    mSh_U_P_Matrix = glGetUniformLocation(shaderProgramObject, "mSh_U_P_Matrix");

    gLdUniform = glGetUniformLocation(shaderProgramObject, "u_Ld");
    gKdUniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
    gLightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position");
    gTexture_sampler_uniform = glGetUniformLocation(shaderProgramObject, "u_texture0_sampler");

    glShadeModel(GL_SMOOTH);
    glClearColor(0.15f, 0.15f, 0.15f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    LoadGLTextures(&gTexture_Stone, MAKEINTRESOURCE(IDBITMAP_STONE));
    LoadGLTextures(&gTexture_Background, MAKEINTRESOURCE(IDBITMAP_BACKGROUND));
    glEnable(GL_TEXTURE_2D);

    perspectiveProjectionMatrix = glm::mat4();
    
    InitializePyramids();
    FirstResize();
    PlaySoundTrack(1);    

    const GLubyte * gpuName = glGetString(GL_VENDOR);
    
    if (strstr(((char*)gpuName), "Intel")) 
    {
        Speed *= 2;
    }
    else
    {
        Speed *= 20;
    }
}

void FirstResize()
{
    //TBD.. revisit
    void resize(int, int);
    currentWidth = WIN_WIDTH - GetSystemMetrics(SM_CXSIZEFRAME) * 4;
    currentHeight = WIN_HEIGHT - GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CYSIZEFRAME) * 4;
    resize(currentWidth, currentHeight);
}


int LoadGLTextures(GLuint *texture, TCHAR imageResourceId[])
{
    // variable declarations
    HBITMAP hBitmap;
    BITMAP bmp;
    int iStatus = FALSE;

    // code
    glGenTextures(1, texture);
    hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (hBitmap)
    {
        iStatus = TRUE;
        GetObject(hBitmap, sizeof(bmp), &bmp);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, *texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGB,
            bmp.bmWidth,
            bmp.bmHeight,
            0,
            GL_BGR,
            GL_UNSIGNED_BYTE,
            bmp.bmBits);

        glGenerateMipmap(GL_TEXTURE_2D);

        DeleteObject(hBitmap);
    }
    return(iStatus);
}


void InitializePyramids()
{

    //Initialize pyramid vertises
    // 36 for triangles (3*3 Vertex  x 4 triagles faces) next 12 for Base
    GLfloat BasePyramidVertices[48] = {
        //Front face
        0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
        //Right face
        0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
        //Back face
        0.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
        //Left Face
        0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
        //Base
        -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f
    };


    GLfloat BasePyramidTexcoords[32] =
    {
        0.5, 1.0, 0.0, 0.0, 1.0, 0.0,
        0.5, 1.0, 1.0, 0.0, 0.0, 0.0,
        0.5, 1.0, 1.0, 0.0, 0.0, 0.0,
        0.5, 1.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0
    };


    GLfloat BasePyramidNormals[48] = { 
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    };


    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(BasePyramidVertices), BasePyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(vSh_Position, 3, GL_FLOAT, FALSE, 0, NULL);
    glEnableVertexAttribArray(vSh_Position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(BasePyramidNormals), BasePyramidNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(vSh_Normal, 3, GL_FLOAT, FALSE, 0, NULL);
    glEnableVertexAttribArray(vSh_Normal);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BasePyramidTexcoords), BasePyramidTexcoords, GL_STATIC_DRAW);

    glVertexAttribPointer(vSh_Texture, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(vSh_Texture);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //Background data
    GLfloat backgroundVertices[12] = {
        1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f};

    GLfloat backgroundTexcoords[8] = { 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

    GLfloat backgroundNormals[12] = {
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,0.0f, 0.0f, 1.0f };

    glGenVertexArrays(1, &vaoBackground);
    glBindVertexArray(vaoBackground);

    glGenBuffers(1, &vboBackground);
    glBindBuffer(GL_ARRAY_BUFFER, vboBackground);

    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(vSh_Position, 3, GL_FLOAT, FALSE, 0, NULL);
    glEnableVertexAttribArray(vSh_Position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboBackground);
    glBindBuffer(GL_ARRAY_BUFFER, vboBackground);

    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundNormals), backgroundNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(vSh_Normal, 3, GL_FLOAT, FALSE, 0, NULL);
    glEnableVertexAttribArray(vSh_Normal);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboBackground);
    glBindBuffer(GL_ARRAY_BUFFER, vboBackground);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundTexcoords), backgroundTexcoords, GL_STATIC_DRAW);

    glVertexAttribPointer(vSh_Texture, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(vSh_Texture);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    
    //Set Axis of rotation for each pyramid
    CubeLevel_1.pyramid[0].rotateAxis[0] = 0.0f; CubeLevel_1.pyramid[0].rotateAxis[1] = 0.0f; CubeLevel_1.pyramid[0].rotateAxis[2] = 0.0f;
    CubeLevel_1.pyramid[1].rotateAxis[0] = 0.0f; CubeLevel_1.pyramid[1].rotateAxis[1] = 0.0f; CubeLevel_1.pyramid[1].rotateAxis[2] = 1.0f;
    CubeLevel_1.pyramid[2].rotateAxis[0] = 1.0f; CubeLevel_1.pyramid[2].rotateAxis[1] = 0.0f; CubeLevel_1.pyramid[2].rotateAxis[2] = 0.0f;
    CubeLevel_1.pyramid[3].rotateAxis[0] = 0.0f; CubeLevel_1.pyramid[3].rotateAxis[1] = 0.0f; CubeLevel_1.pyramid[3].rotateAxis[2] = -1.0f;
    CubeLevel_1.pyramid[4].rotateAxis[0] = -1.0f; CubeLevel_1.pyramid[4].rotateAxis[1] = 0.0f; CubeLevel_1.pyramid[4].rotateAxis[2] = 0.0f;
    CubeLevel_1.pyramid[5].rotateAxis[0] = -1.0f; CubeLevel_1.pyramid[5].rotateAxis[1] = 0.0f; CubeLevel_1.pyramid[5].rotateAxis[2] = 0.0f;
    
    //Set Axis of translation for each pyramid
    CubeLevel_1.pyramid[0].translateAxis[0] = 0.0f; CubeLevel_1.pyramid[0].translateAxis[1] = -1.0f; CubeLevel_1.pyramid[0].translateAxis[2] = 0.0f;
    CubeLevel_1.pyramid[1].translateAxis[0] = 1.0f; CubeLevel_1.pyramid[1].translateAxis[1] = 0.0f; CubeLevel_1.pyramid[1].translateAxis[2] = 0.0f;
    CubeLevel_1.pyramid[2].translateAxis[0] = 0.0f; CubeLevel_1.pyramid[2].translateAxis[1] = 0.0f; CubeLevel_1.pyramid[2].translateAxis[2] = -1.0f;
    CubeLevel_1.pyramid[3].translateAxis[0] = -1.0f; CubeLevel_1.pyramid[3].translateAxis[1] = 0.0f; CubeLevel_1.pyramid[3].translateAxis[2] = 0.0f;
    CubeLevel_1.pyramid[4].translateAxis[0] = 0.0f; CubeLevel_1.pyramid[4].translateAxis[1] = 1.0f; CubeLevel_1.pyramid[4].translateAxis[2] = 0.0f;
    CubeLevel_1.pyramid[5].translateAxis[0] = 0.0f; CubeLevel_1.pyramid[5].translateAxis[1] = 0.0f; CubeLevel_1.pyramid[5].translateAxis[2] = 1.0f;

    CubeLevel_1.pyramid[4].rotationMultiplier = 2;

    //Second Level
    for (int index = 0; index < 6; index++)
    {
        intializeCube(CL_2.CL_1[index], 2);
    }

    //Third level
    for (int SecondIndex = 0; SecondIndex < 6; SecondIndex++)
    {
        for (int index = 0; index < 6; index++)
        {
            intializeCube(CL_3.CL_2[SecondIndex].CL_1[index], 3);
        }
    }

}


void intializeCube(Cube &cube, int level)
{
    for (int pyramidIndex = 0; pyramidIndex < 6; pyramidIndex++)
    {
        for (int AxisIndex = 0; AxisIndex < 3; AxisIndex++)
        {
            cube.pyramid[pyramidIndex].translateAxis[AxisIndex] = CubeLevel_1.pyramid[pyramidIndex].translateAxis[AxisIndex];
            cube.pyramid[pyramidIndex].rotateAxis[AxisIndex] = CubeLevel_1.pyramid[pyramidIndex].rotateAxis[AxisIndex];
        }
        cube.pyramid[pyramidIndex].rotationMultiplier = CubeLevel_1.pyramid[pyramidIndex].rotationMultiplier;
        cube.level = level;
        cube.doNextLevelAnimation = false;
    }

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Functions Declarations
    void ToggleFullscreen();
    void resize(int, int);

    switch (message)
    {
    case WM_CREATE:
        //MessageBox(NULL,TEXT("Window is Created"), TEXT("CALLBACK function Info"), MB_OK);
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
            if (gbEscapeKeyIsPressed == (BOOL)false)
                gbEscapeKeyIsPressed = (BOOL)true;
            break;

        case 0x46:  //for 'f'
            if (gbFullscreen == (BOOL)true)
            {
                ToggleFullscreen();
                gbFullscreen = (BOOL)false;
            }
            else
            {
                ToggleFullscreen();
                gbFullscreen = (BOOL)true;
            }
            break;

        case 'R':
        case 'r':
            resetScene();
            break;

        case 'i':
        case 'I':
            Speed *= 1.2f;
            break;

        case 'd':
        case 'D':
            Speed *= 0.8f;
            break;

        case 'u':
        case 'U':
            if (isUnfoldingKey == false)
            {
                isUnfoldingKey = true;
                break;
            }
        }

    case WM_ERASEBKGND:
        return(0);
        break;

    case WM_CLOSE:
        UnInitialize();
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
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

void resize(int width, int height)
{

    if (height == 0)
        height = 1;
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    perspectiveProjectionMatrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void UnInitialize()
{
    if (gbFullscreen == (BOOL)true)
    {
        dwStyle = GetWindowLong(gHwnd, GWL_STYLE);
        SetWindowLong(gHwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(gHwnd, &wpPrev);
        SetWindowPos(gHwnd, HWND_TOP, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }

    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if (vbo)
    {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vaoBackground)
    {
        glDeleteVertexArrays(1, &vaoBackground);
        vaoBackground = 0;
    }

    if (vboBackground)
    {
        glDeleteBuffers(1, &vboBackground);
        vboBackground = 0;
    }

    glDetachShader(shaderProgramObject, vertexShaderObject);
    glDetachShader(shaderProgramObject, pixelShaderObject);

    glDeleteShader(vertexShaderObject);
    vertexShaderObject = 0;
    glDeleteShader(pixelShaderObject);
    pixelShaderObject = 0;
    glDeleteProgram(shaderProgramObject);

    glUseProgram(0);

    wglMakeCurrent(NULL, NULL);

    wglDeleteContext(gHglrc);
    gHglrc = NULL;

    ReleaseDC(gHwnd, gHdc);
    gHdc = NULL;

    DestroyWindow(gHwnd);

    if (ptrFile)
    {
        fprintf(ptrFile, "Log File Is Successfully Closed.\n");
        fclose(ptrFile);
        ptrFile = NULL;
    }
}

void Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 modelViewMatrix, globalModelViewMatrix, tempMatrix = glm::mat4();
    
    DrawBackground();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexture_Stone);
    glUniform1i(gTexture_sampler_uniform, 0);
    DoAnimation(tempMatrix, CubeLevel_1);
    
    if (CubeLevel_1.doTranlation)
        PlaySoundTrack(3);

    if (CL_2.CL_1[0].doTranlation)
        PlaySoundTrack(4);

    if (CubeLevel_1.doNextLevelAnimation)
    {
        for (int index = 0; index < 6; index++)
        {
            DoAnimation(CubeLevel_1.pyramid[index].matrix, CL_2.CL_1[index]);
        }
    }

    if (CL_3.CL_2[0].CL_1[1].doTranlation)
        PlaySoundTrack(5);
    for (int index_CL_2 = 0; index_CL_2 < 6; index_CL_2++)
    {
        if (CL_2.CL_1[index_CL_2].doNextLevelAnimation)
        {
            for (int index = 1; index < 6; index++)
            {
                DoAnimation(CL_2.CL_1[index_CL_2].pyramid[index].matrix, CL_3.CL_2[index_CL_2].CL_1[index]);
                if (index == 4)
                    CL_3.CL_2[index_CL_2].CL_1[index].isPartOfMonilith = true;
            }
        }
    }
    spin();
    SwapBuffers(gHdc);
}

void DrawBackground()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexture_Background);
    glUniform1i(gTexture_sampler_uniform, 0);
    glm::mat4 modelViewMatrix = glm::mat4();
    
    modelViewMatrix = glm::translate(modelViewMatrix, glm::vec3(0.0f, 0.0f, -40.0f));
    modelViewMatrix = glm::scale(modelViewMatrix, glm::vec3(40.0f, 30.0f, 1.0f));

    glUseProgram(shaderProgramObject);
    glUniform3f(gLdUniform, 1.0f, 1.0f, 1.0f);
    glUniform3f(gKdUniform, 0.7f, 0.7f, 0.7f);

    glUniform4fv(gLightPositionUniform, 1, LightPosition);
    glUniformMatrix4fv(mSh_U_MV_Matrix, 1, GL_FALSE, (const float*)glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(mSh_U_P_Matrix, 1, GL_FALSE, (const float*)glm::value_ptr(perspectiveProjectionMatrix));
    
    glBindVertexArray(vaoBackground);

    //Draw Pyramid
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void DoAnimation(glm::mat4 globalViewMatrix, Cube &cube)
{
    glm::mat4 modelViewMatrix, modelViewMatrix_for_Cube = glm::mat4();
    glm::mat4 rotationMatrix = glm::mat4();
    glm::mat4 yRotationMatrix = glm::mat4();

    glm::mat4 initialMatrix = glm::mat4();
    
    if (cube.level == 1)
    {
        initialMatrix = glm::translate(initialMatrix, glm::vec3(0.0f, 0.0f, -radius));
        if (startUnfolding)
        {
            initialMatrix = glm::rotate(initialMatrix, angleTri, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        initialMatrix = glm::rotate(initialMatrix, angleTri, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else
        initialMatrix = globalViewMatrix;

    glUseProgram(shaderProgramObject);
    glUniform3f(gLdUniform, 1.0f, 1.0f, 1.0f);
    glUniform3f(gKdUniform, 0.7f, 0.7f, 0.7f);

    glUniform4fv(gLightPositionUniform, 1, LightPosition);

    int pyramidIndex = 0;

    if (!startUnfolding)
    {
        modelViewMatrix = initialMatrix;
        glUniformMatrix4fv(mSh_U_MV_Matrix, 1, GL_FALSE, (const float*)glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(mSh_U_P_Matrix, 1, GL_FALSE, (const float*)glm::value_ptr(perspectiveProjectionMatrix));
        glBindVertexArray(vao);

        //Draw Pyramid
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    }
    else
    {
        for (int pyramidIndex = 0; pyramidIndex < 6; pyramidIndex++)
        {
            rotationMatrix = glm::mat4();
            yRotationMatrix = glm::mat4();

            modelViewMatrix = modelViewMatrix_for_Cube = initialMatrix;

            if (pyramidIndex != 0)
            {
                //Rotate only one pyramid at a time.
                if (cube.currentRotatingPyramidIndex == pyramidIndex)
                {
                    cube.pyramid[pyramidIndex].currentRotation += Speed;
                    if (cube.pyramid[pyramidIndex].currentRotation >= 1.55f)
                    {
                        cube.pyramid[pyramidIndex].currentRotation = stopAngle;
                        cube.currentRotatingPyramidIndex++;
                    }
                }


                //update rotation Matrix 
                rotationMatrix = glm::rotate(rotationMatrix,
                    (cube.pyramid[pyramidIndex].currentRotation  * cube.pyramid[pyramidIndex].rotationMultiplier),
                    glm::vec3(cube.pyramid[pyramidIndex].rotateAxis[0], cube.pyramid[pyramidIndex].rotateAxis[1], cube.pyramid[pyramidIndex].rotateAxis[2]));

                //To spin while going up
                yRotationMatrix = glm::mat4();
                yRotationMatrix = glm::rotate(yRotationMatrix, cube.pyramid[pyramidIndex].currentRotation, glm::vec3(0.0f, 1.0f, 0.0f));
                rotationMatrix = rotationMatrix * yRotationMatrix;
            }

            //Do Translation
            modelViewMatrix = glm::translate(modelViewMatrix,
                glm::vec3(cube.pyramid[pyramidIndex].translate * cube.pyramid[pyramidIndex].translateAxis[0],
                    cube.pyramid[pyramidIndex].translate * cube.pyramid[pyramidIndex].translateAxis[1],
                    cube.pyramid[pyramidIndex].translate * cube.pyramid[pyramidIndex].translateAxis[2]));

            modelViewMatrix = modelViewMatrix * rotationMatrix;

            //This is needed for next level Cube animation
            cube.pyramid[pyramidIndex].matrix = modelViewMatrix;

            if (is_CL_3_complete && !cube.isPartOfMonilith)
            {
                //continue;
                if (cube.scale < 0.0f)
                    continue;
                modelViewMatrix = glm::scale(modelViewMatrix, glm::vec3(cube.scale, cube.scale, cube.scale));
                modelViewMatrix_for_Cube = glm::scale(modelViewMatrix_for_Cube, glm::vec3(cube.scale, cube.scale, cube.scale));
                cube.scale -= Speed/3;
            }

            //Send to Shader
            glUniformMatrix4fv(mSh_U_MV_Matrix, 1, GL_FALSE, (const float*)glm::value_ptr(modelViewMatrix));
            glUniformMatrix4fv(mSh_U_P_Matrix, 1, GL_FALSE, (const float*)glm::value_ptr(perspectiveProjectionMatrix));
            glBindVertexArray(vao);

            //Draw Pyramid
            glDrawArrays(GL_TRIANGLES, 0, 12);
            glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

            //Draw Pyramid Base which will be face of Cube
            modelViewMatrix_for_Cube = modelViewMatrix_for_Cube * rotationMatrix;
            glUniformMatrix4fv(mSh_U_MV_Matrix, 1, GL_FALSE, (const float*)glm::value_ptr(modelViewMatrix_for_Cube));
            glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

            glBindVertexArray(0);

            if (cube.doTranlation && !cube.isPartOfMonilith)
            {
                if (cube.pyramid[pyramidIndex].translate >= 2.5f)
                {
                    //TBD
                    cube.doNextLevelAnimation = true;
                }
                if (cube.pyramid[pyramidIndex].translate < 6.0f)
                    cube.pyramid[pyramidIndex].translate += (Speed * 0.5f);
                else
                    cube.pyramid[pyramidIndex].translate += (Speed * 0.15f);

            }
        }
    }

    //Start translation
    if (cube.currentRotatingPyramidIndex > 5)
    {
        cube.doTranlation = true;
        cube.currentRotatingPyramidIndex = 0;
    }

    glUseProgram(0);
}

void resetScene()
{

    CubeLevel_1.reset();
    CL_2.reset();
    CL_3.reset();

    angleTri = 0.0f;
    radius = START_RADIUS;
    yEye = START_EYE;
    currentAnimationLevel = 1;
    direction = 0;
    is_CL_3_complete = false;
    currentSoundTrack = 0;
    PlaySoundTrack(1);
}

void PlaySoundTrack(int soundTrack)
{
    if (currentSoundTrack < soundTrack)
    {
        switch (soundTrack)
        {
        case 1:
            PlaySound(TEXT("Music\\1.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
            break;

        case 2:
            //PlaySound(NULL, 0, 0);
            PlaySound(TEXT("Music\\2.wav"), NULL, SND_FILENAME | SND_ASYNC);
            break;

        case 3:
            //PlaySound(NULL, 0, 0);
            PlaySound(TEXT("Music\\3.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
            break;

        case 4:
            //PlaySound(NULL, 0, 0);
            PlaySound(TEXT("Music\\4.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
            break;

        case 5:
            //PlaySound(NULL, 0, 0);
            PlaySound(TEXT("Music\\5.wav"), NULL, SND_FILENAME | SND_ASYNC);
            break;
        }
        currentSoundTrack++;
    }
}


void spin()
{
    angleTri = angleTri + Speed/5.0f;
    if (angleTri >= 6.2831f)
        angleTri = angleTri - 6.2831f;

    float unit = 0.0f;

    if (isUnfoldingKey && !startUnfolding)
    {
        if (((int)(angleTri * 100 / stopAngle) % 100) == 0)
        {
            startUnfolding = true;
            angleTri = 0;
            Speed /= 2;
            PlaySoundTrack(2);
        }
    }

    if (startUnfolding)
    {
        if (direction == 0)
        {
            unit = Speed / 4.0f;
        }
        if (direction == 1)
        {
            unit = Speed / 4.0f;
        }
        else if (direction == 2)
        {
            unit = -(Speed);
        }
        else if (direction == -1)
        {
            unit = Speed / 50.0f;
        }
        radius += unit;

        if (radius > 10.0f && (direction == 0))
        {
            direction = 1;
            Speed *= 1.5;
        }
        if (radius > 20.5f && (direction == 1))
            direction = 2;

        if (radius < 9.0f && (direction == 2))
        {
            direction = -1;
            is_CL_3_complete = true;
        }
    }
}
