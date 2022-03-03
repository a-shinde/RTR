#include <windows.h>
#include <stdio.h>
#include <Mmsystem.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include <string.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp" 
#include "glm/vec4.hpp" 
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "My_OGL.h"



#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "Winmm.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define SPEED 0.0003f
#define START_RADIUS 3.5f
#define START_EYE 3.0f

//Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Initialize();
void UnInitialize();
void Display();
void spin();
void InitializePyramids();
void resetScene();
void FirstResize();
void DrawBackground();
void PlaySoundTrack(int);

HWND gHwnd;
HDC gHdc;
HGLRC gHglrc;

BOOL gbFullscreen;
BOOL gbEscapeKeyIsPressed;
BOOL gbActiveWindow;
BOOL bDone;
bool startCameraOut = false;
bool is_CL_3_complete = false;
bool startUnfolding = false;
bool isUnfoldingKey = false;
int currentSoundTrack = 0;

WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
DWORD dwStyle;
GLfloat angleTri = 0.0f;
GLfloat Speed = SPEED;
GLfloat stopAngle = 1.5707963268;
int direction = 0;
int currentWidth;
int currentHeight;
int currentAnimationLevel = 1;

FILE *ptrFile = NULL;

enum
{
    vSh_Position = 0,
    vSh_Color,
    vSh_Texture,
    vSh_Normal,
};

GLuint vertexShaderObject;
GLuint pixelShaderObject;
GLuint shaderProgramObject;

GLuint vao;
GLuint vbo;
GLuint vaoBackground;
GLuint vboBackground;
GLuint mSh_U_MV_Matrix;
GLuint mSh_U_P_Matrix;
GLuint gLdUniform, gKdUniform, gLightPositionUniform;
GLuint gTexture_sampler_uniform;
GLuint gTexture_Stone;
GLuint gTexture_Background;

glm::mat4 perspectiveProjectionMatrix;

bool lightOn = false;
GLfloat AmbientLight[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat DiffusedLight[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat LightPosition[] = { 4.0f, 4.0f, 4.0f, 0.0f };

GLfloat radius = START_RADIUS;
GLfloat xEye, xLookAt, yEye = 2.5f, yLookAt, zEye, zLookat;

GLfloat BasePyramidFaces[36];

struct Pyramid
{
    GLfloat rotateAxis[3];
    GLfloat rotationMultiplier;
    GLfloat currentRotation;
    GLfloat translate;
    GLfloat translateAxis[3];
    glm::mat4 matrix;
    Pyramid()
    {
        rotationMultiplier = 1;
        matrix = glm::mat4();
    }
};

struct Cube
{
    Pyramid pyramid[6];
    int currentRotatingPyramidIndex = 1;
    bool doNextLevelAnimation = false;
    bool doTranlation;
    bool doFinalRoationY;
    GLfloat angleFinalRotateY;
    bool isPartOfMonilith;
    int level;
    float scale;

    Cube()
    {
        currentRotatingPyramidIndex = 1;
        doNextLevelAnimation = false;
        doTranlation = false;
        doFinalRoationY = false;
        angleFinalRotateY = 0.0f;
        isPartOfMonilith = false;
        level = 1;
        scale = 1.0f;
    }

    void reset()
    {
        for (int pyramidIndex = 0; pyramidIndex < 6; pyramidIndex++)
        {
            pyramid[pyramidIndex].translate = 0.0f;
            pyramid[pyramidIndex].currentRotation = 0.0f;
            pyramid[pyramidIndex].matrix = glm::mat4();
        }
        currentRotatingPyramidIndex = 1;
        doNextLevelAnimation = false;
        doTranlation = false;
        doFinalRoationY = false;
        angleFinalRotateY = 0.0f;
        isPartOfMonilith = false;
        scale = 1.0f;
    }
}CubeLevel_1; 

struct SecondLevelCube
{
    Cube CL_1[6];

    void reset()
    {
        for (int CubeIndex = 0; CubeIndex < 6; CubeIndex++)
        {
            CL_1[CubeIndex].reset();
        }
    }
}CL_2;

struct ThirdLevelCube
{
    SecondLevelCube CL_2[6];

    void reset()
    {
        for (int index = 0; index < 6; index++)
        {
            CL_2[index].reset();
        }
    }
}CL_3;


void DoAnimation(glm::mat4, Cube&);
void intializeCube(Cube &, int);
