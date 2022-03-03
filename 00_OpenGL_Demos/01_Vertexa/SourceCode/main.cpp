#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <gl\glew.h>
#include <gl\GL.h>
#include <Mmsystem.h>
#include "Animation.h"
#include "Utilities.h"

//Link required lib files
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"opengl32.lib")

#pragma comment(lib,"res/lib/x64/Debug/glew32d.lib")
#pragma comment(lib,"res/lib/x64/Debug/assimp-vc140-mt.lib")
#pragma comment(lib,"res/lib/x64/Debug/FreeImage.lib")
#pragma comment(lib,"res/lib/x64/Debug/xerces-c_3.lib")
#pragma comment(lib,"winmm.lib")

/*AWS services libs are linked in respective implementation files*/

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
bool isProgramTerminating = false; //To handle multithreaded cross components gracefull termination
std::mutex gmIsProgramTerminatingMutex;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool gbIsFullScreen = false;
HWND ghwnd;
HDC ghdc;
HGLRC ghrc;
bool gbActiveWindow = false;
bool gbEscapeKeyPressed =false;
std::ofstream g_log_file; //For verbose logging for all components. TODO: Add a macro for exclusive write to this file
GLuint winWidth, winHeight;

//Manual control over our model
GLfloat xRot = -90.0f, yRot = 0.0f, zRot = 0.0f;
GLfloat xTrans = 0.0f, yTrans = -12.0f, zTrans = 0.0f;

Animation animation;
DWORD frameStart, frameTime;
int cubeMapIndex = 0;
//FPS
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
//OpenCV img capture
extern std::mutex opencv_img_capture_mtx;
extern bool gbKeepCapturingImg;

//From DMO_rec
extern std::mutex dmo_rec_ask_to_stop_rec_mtx;
//extern bool isAskedToStopRecording;
//extern map<std::string, std::string> faceMap;
//extern bool gbIsPlaybackInProgress;
//extern std::mutex is_playback_in_progress_mutex;
//extern bool gbIsImageCaptureInProgress;
//extern std::mutex is_image_capture_in_progress_mutex;
const  std::string g_temp_audio_file_name = "sample_out.pcm";

//bool gbMimicFacialExpressions = false;
//std::mutex mimic_facial_expressions_mutex;
//LexResponse voiceInputLexResponse;

void handleShutdown();

#define INTENT_PROCESSED 1
#define INTENT_PLAY_AUDIO_USING_POLLY 2
#define INTENT_HYBRID_ACTION_AND_AUDIO 3 

//int processLexIntent(LexResponse& lexResponse);

//extern Aws::Utils::Array<unsigned char> unIdentifiedPersonPhotoData;
//extern std::string previouslyRekognizedFaceId;
bool readParseAndRenderSpeechMarksFromFile(const std::string& speechMarksFilepath);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	void initialize();
	void uninitialize();
	void update();
	void display();
	int iScreenWidth, iScreenHeight;
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Model Loading and Animation");
	bool bDone = false;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wndclass)) {
		MessageBox(NULL, TEXT("Failed to register wndclass. Exiting"), TEXT("Error"), MB_OK);
		exit(EXIT_FAILURE);
	}
	iScreenWidth = GetSystemMetrics(0);
	iScreenHeight = GetSystemMetrics(1);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName,
		szAppName,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		((iScreenWidth / 2) - 400), ((iScreenHeight / 2) - 300),
		800, 600,
		NULL,
		NULL,
		hInstance,
		NULL);
	ghwnd = hwnd;
	ShowWindow(hwnd, nCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	g_log_file.open("Log.txt", std::ios::out);
	if (!g_log_file.is_open())
	{
		std::cout << "Failed to open log new file" << std::endl;
		uninitialize();
	}
	else
		g_log_file << "Log file successfully created!" << std::endl;

	initialize();
	/*int ret = initialize_opencv();
	if (ret != OCV_SUCCESS)
	{
		g_log_file << "OpenCV initialization failed" << std::endl;
	}
	else
		g_log_file << "OpenCV initialization successfull." << std::endl;*/
	
	/* Initialize AWS Lex*/
	/*ret = initialize_lex();
	if (ret != LEX_SUCCESS)
	{
		g_log_file << "AWS Lex client initialization failed!" << std::endl;
		uninitialize();
		return 0;
	}
	else
		g_log_file << "AWS Lex client initialization successfull" << std::endl;*/


	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			display();
			update();
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyPressed == true)
				{
					bDone = true;
				}
			}
		}
	}
	uninitialize();
	return((int)msg.wParam);
}

void initialize()
{
	void update();
	void resize(int, int);
	void uninitialize();
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;
	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	GLenum glew_error = glewInit();	//Turn ON all graphic card extension
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		uninitialize();
	}


	const GLubyte *glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
	g_log_file << "GLSL version is:" << glsl_version << std::endl;

	glShadeModel(GL_SMOOTH);
	// set-up depth buffer
	glClearDepth(1.0f);
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	// depth test to do
	glDepthFunc(GL_LEQUAL);
	// set really nice percpective calculations ?
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// set background color to which it will display even if it will empty. THIS LINE CAN BE IN drawRect().
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f); // blue

	//Initialization of timer and triagle(load model)	
	frameStart = GetTickCount();

	//Intitialize the Animation
	animation.init();

	winWidth = WIN_WIDTH;
	winHeight = WIN_HEIGHT;
	resize(winWidth, winHeight);

	//updateFaceMapFromPropertyFile(&faceMap); //fill the map of faceId and faceNames


    animation.camera.currentScene = 0;
    animation.camera.start();
	PlaySound(TEXT("res\\audio\\entry.wav"), NULL, SND_FILENAME | SND_ASYNC);

	update();
	animation.animationSettings.action.actionSequence = ACTION_OPENING_SCENE;
	animation.animationSettings.action.startActionPlayback(animation.current_time);
	animation.camera.currentScene = ACTION_OPENING_SCENE;
	animation.camera.start();
}

//FUNCTION DEFINITIONS
void resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    animation.perspectiveProjectionMatrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 500.0f);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// Here we render our final animation
	animation.render();

	SwapBuffers(ghdc);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;
	DWORD dwStyle;
	static WINDOWPLACEMENT wpPrev;
	BOOL isWp;
	HMONITOR hMonitor;
	MONITORINFO monitorInfo;
	BOOL isMonitorInfo;
	int ret;
	
	bool keepCapturingImgs = false;

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE:
		winWidth = LOWORD(lParam);
		winHeight = HIWORD(lParam);
		resize(winWidth, winHeight);
		break;
	case WM_CHAR:
		switch (LOWORD(wParam)) {
		case 'p': //Sample key to test audio and speech marks rendering locally
	/*		voiceInputLexResponse.intent = "AboutTheCource";
			ret = processLexIntent(voiceInputLexResponse);
			if (ret == INTENT_PROCESSED)
			{
				g_log_file << "Process 'AboutTheCource' successfully!" << std::endl;
			}*/
			break;
			//Start recording. Press 'r'
        case 'i': //Sample key to test audio and speech marks rendering locally
    /*        voiceInputLexResponse.intent = "SelfIntroduction";
            ret = processLexIntent(voiceInputLexResponse);
            if (ret == INTENT_PROCESSED)
            {
                g_log_file << "Process 'SelfIntroduction' successfully!" << std::endl;
            }*/
            break;
      

        case 'a':
            animation.animationSettings.action.actionSequence = ACTION_OPENING_SCENE;
            animation.animationSettings.action.startActionPlayback(animation.current_time);
            animation.camera.currentScene = ACTION_OPENING_SCENE;
            animation.camera.start();
            break;

        case ';':
            PlaySound(NULL, NULL, NULL);
            animation.animationSettings.action.actionSequence = ACTION_IDEL_POSITION;
            animation.animationSettings.action.startActionPlayback(animation.current_time);
            animation.camera.currentScene = ACTION_IDEL_POSITION;
            animation.camera.start();
            break;

        case 'g':
            //animation.animationSettings.action.actionSequence = ACTION_WALK;
            //animation.animationSettings.action.startActionPlayback(animation.current_time);
            //animation.camera.currentScene = ACTION_WALK;
            //animation.camera.start();
            break;

        case 'h':
            PlaySound(TEXT("res\\audio\\happy.wav"), NULL, SND_FILENAME | SND_ASYNC);
            animation.animationSettings.action.actionSequence = ACTION_AEROPLANE;
            animation.animationSettings.action.startActionPlayback(animation.current_time);
            animation.camera.currentScene = ACTION_AEROPLANE;
            animation.camera.start();
            break;

        case 'j':
			PlaySound(TEXT("res\\audio\\IndianClassicalDheemTaDare.wav"), NULL, SND_FILENAME | SND_ASYNC);
            animation.animationSettings.action.actionSequence = ACTION_INDIAN_DANCE;
            animation.animationSettings.action.startActionPlayback(animation.current_time);
            animation.camera.currentScene = ACTION_INDIAN_DANCE;
            animation.camera.start();
            break;

        case 'k':
            PlaySound(TEXT("res\\audio\\moonwalk.wav"), NULL, SND_FILENAME | SND_ASYNC);
            animation.animationSettings.action.actionSequence = ACTION_MOONWALK;
            animation.animationSettings.action.startActionPlayback(animation.current_time);
            animation.camera.currentScene = ACTION_MOONWALK;
            animation.camera.start();
            break;

        case 'l':
			if (readParseAndRenderSpeechMarksFromFile("res\\audio\\speech\\SayHelloToAll.marks"))
			{
				PlaySound(TEXT("res\\audio\\speech\\SayHelloToAll.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
            animation.animationSettings.action.actionSequence = ACTION_HELLO;
            animation.animationSettings.action.startActionPlayback((GLfloat)GetTickCount());
            animation.camera.currentScene = ACTION_HELLO;
            animation.camera.start();
            break;

        case 'b':
            if (readParseAndRenderSpeechMarksFromFile("res\\audio\\speech\\temp.marks"))
            {
            }

        case 'n':
            if(animation.animationSettings.modelIndex == 1)
                animation.animationSettings.modelIndex = 0;
            else
                animation.animationSettings.modelIndex = 1;
            break;


        case 'A':            
            break;

		case 'x':
			xRot += 1.0f;
			break;
		case 'X':
			xRot -= 1.0f;
			break;
		case 'y':
			yRot += 1.0f;
			break;
		case 'Y':
			yRot -= 1.0f;
		case 'z':
			zRot += 1.0f;
			break;
		case 'Z':
			zRot -= 1.0f;
			break;
		case 'q': //x
			xTrans += 0.2f;
			break;
		case 'Q':
			xTrans -= 0.2f;
			break;
		case 'w':
			yTrans += 0.2f;
			break;
		case 'W':
			yTrans -= 0.2f;
			break;
		case 'e':
			zTrans += 0.2f;
			break;
		case 'E':
			zTrans -= 0.2f;
			break;

        case 'M':
			//mimic_facial_expressions_mutex.lock();
			//gbMimicFacialExpressions = !gbMimicFacialExpressions;//toggle
			//mimic_facial_expressions_mutex.unlock();
            break;

        case 'v': 
            animation.animationSettings.environment.cubeMapIndex++;
            if (animation.animationSettings.environment.cubeMapIndex > animation.animationSettings.environment.maxCubeMaps)
                animation.animationSettings.environment.cubeMapIndex = 0;
            break;

        case '0':
            animation.animationSettings.expression.emotion = EXPRESSION_NEUTRAL;
            animation.animationSettings.expression.factor = 1;
            break;

        case '1':
            animation.animationSettings.expression.emotion = EXPRESSION_ANGREE;
            animation.animationSettings.expression.factor = 1;
            break;

        case '2':
            animation.animationSettings.expression.emotion = EXPRESSION_DISGUST;
            animation.animationSettings.expression.factor = 1;
            break;

        case '3':
            animation.animationSettings.expression.emotion = EXPRESSION_DOUBT;
            animation.animationSettings.expression.factor = 1;
            break;

        case '4':
            animation.animationSettings.expression.emotion = EXPRESSION_FEAR;
            animation.animationSettings.expression.factor = 1;
            break;

        case '5':
            animation.animationSettings.expression.emotion = EXPRESSION_SAD;
            animation.animationSettings.expression.factor = 1;
            break;

        case '6':
            animation.animationSettings.expression.emotion = EXPRESSION_SLEEPING;
            animation.animationSettings.expression.factor = 1;
            break;

        case '7':
            animation.animationSettings.expression.emotion = EXPRESSION_SMILE;
            animation.animationSettings.expression.factor = 1;
            break;

        case '8':
            animation.animationSettings.expression.emotion = EXPRESSION_SURPRISE;
            animation.animationSettings.expression.factor = 1;
            break;

        //case 'l':
        //    animation.animationSettings.lipSync.startLipSyncPlayback(animation.current_time);
        //    PlaySound(TEXT("res\\audio\\sample.wav"), NULL, SND_FILENAME | SND_ASYNC);
        //    break;

		default:
			break;
		}

		break;
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			handleShutdown();
			gbEscapeKeyPressed = true;
			break;

		case 0x46:	//f
			dwStyle = GetWindowLong(hwnd, GWL_STYLE);
			if (gbIsFullScreen == false)
			{
				if (dwStyle & WS_OVERLAPPEDWINDOW)
				{
					wpPrev.length = sizeof(WINDOWPLACEMENT);
					isWp = GetWindowPlacement(hwnd, &wpPrev);
					hMonitor = MonitorFromWindow(hwnd, MONITORINFOF_PRIMARY);
					monitorInfo.cbSize = sizeof(MONITORINFO);
					isMonitorInfo = GetMonitorInfo(hMonitor, &monitorInfo);
					if (isWp == TRUE && isMonitorInfo)
					{
						SetWindowLong(hwnd, GWL_STYLE, dwStyle&~WS_OVERLAPPEDWINDOW);
						SetWindowPos(hwnd, HWND_TOP,
							monitorInfo.rcMonitor.left,
							monitorInfo.rcMonitor.top,
							monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
							monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
							SWP_NOZORDER | SWP_FRAMECHANGED);
						ShowCursor(FALSE);
						gbIsFullScreen = true;
					}
				}
			}
			else
			{ //restore
				SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
				SetWindowPlacement(hwnd, &wpPrev);
				SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
				ShowCursor(TRUE);
				gbIsFullScreen = false;
			}
			break;

		default:
			break;
		}
		break;

	case WM_DESTROY:
		handleShutdown();
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
void update()
{
	animation.update();
}

bool readParseAndRenderSpeechMarksFromFile(const std::string& speechMarksFilepath)
{
	std::vector<LIP_SYNC_INPUT> speechMarksOut;
	ifstream infile{ speechMarksFilepath };
	string file_contents{ istreambuf_iterator<char>(infile), istreambuf_iterator<char>() };
	parse_speech_marks(file_contents, speechMarksOut);
    if (animation.animationSettings.startLipSyncPlayback(speechMarksOut))//Set visme for facial expressions
    {
        return true;
    }
    else
        return false;
}


void handleShutdown()
{
	setProgramShuttingDownToTrue();
	//Stop capturing faces from camera
	//opencv_img_capture_mtx.lock();
	//gbKeepCapturingImg = false;
	//opencv_img_capture_mtx.unlock();
	//Update global flag
	//Check is playback in progress
	
	//is_playback_in_progress_mutex.lock();
	//bool isAudioPlaybackInProgress = gbIsPlaybackInProgress;
	//is_playback_in_progress_mutex.unlock();

	//is_image_capture_in_progress_mutex.lock();
	//bool isImageCaptureInProgress = gbIsImageCaptureInProgress;
	//is_image_capture_in_progress_mutex.unlock();

//	while (isAudioPlaybackInProgress)
//	{
//		Sleep(1000);//1 sec
		//Check for update value
	//	is_playback_in_progress_mutex.lock();
	//	isAudioPlaybackInProgress = gbIsPlaybackInProgress;
	//	is_playback_in_progress_mutex.unlock();

		//is_image_capture_in_progress_mutex.lock();
		//isImageCaptureInProgress = gbIsImageCaptureInProgress;
		//is_image_capture_in_progress_mutex.unlock();
	//}
}
void uninitialize(void)
{
	//UNINITIALIZATION CODE
	DWORD dwStyle;
	WINDOWPLACEMENT wpPrev;
	if (gbIsFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);

	}
	/* Uninitialize in reverse order*/
	//AWS Lex
	//uninitialize_lex();
	
	//uninitialize_opencv();

	//OpenGL specific
	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;
	if (g_log_file.is_open())
	{
		g_log_file << "Closing log file";
		g_log_file.close();
	}
	DestroyWindow(ghwnd);
}



