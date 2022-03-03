#pragma once

#include <iostream>
#include <GL\glew.h>

class Shaders
{

public:

    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;

	const GLchar* ReadShaderFromFile(const char* filename);

	bool MakeProgramObject(const char* vertex, const char* fragment);

    void DeleteProgramObject();
};

