#pragma once
#include <windows.h> 
#include <math.h>
#include <vector>
#include <iostream>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "time.h"
#include "Resources.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#define IDT_CURTAIN 1
#define IDT_CAMERA 2
#define SLOWDOWN 0.01f 
#define SPEED 0.25f
#define REFINE_ITERATIONS 15


void showCurtain();
void SetLights();
void showChairs();
void SetCamera();
void UpdateCamera();
void UpdateCurtain();
void Theater_Initialize();
void Scene_Theater();
int LoadGLTexturesRepeat(GLuint *, TCHAR[], bool);
void showNatak();
void bindNatakTextue(int);


