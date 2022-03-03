#pragma once

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <gl\glew.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

void displayCubeMap(glm::mat4 viewMatrix, glm::mat4 PerspectiveProjectionMatrix, int cubeMapIndex);

void initializeCubeMap();
