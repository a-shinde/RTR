#pragma once
#include <Windows.h>
#include <GL\glew.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "Shaders.h"
#include "CubeMap.h"
#include "freeImg\FreeImage.h"
#include "AnimationHelper.h"
#include "ModelEntity.h"


class Animation
{
public:
	Animation();
	~Animation();

	GLfloat speed = 0.02f;
	//for camera
    GLfloat current_time = 0.0f;
	GLfloat delta_time = 0.0f;
	GLfloat last_time = 0.0f;
	GLfloat last_x;
	GLfloat last_y;
	Camera camera;
    ANIMATION_SETTINGS animationSettings;    

	Shaders shader;
	Model model_trinity_animation;
    Model model_dressCode;
    ModelEntity model_trinity_geometries;

	glm::mat4 MVP;
	glm::mat4 viewMatrix;
	glm::mat4 perspectiveProjectionMatrix;
	glm::mat4 modelMatrix;

    GLuint model_matrix_uniform;
    GLuint view_matrix_uniform;
    GLuint projection_uniform;


	void init();
	void update();
	void render();
	void playSound();
    void setLigting();

	static GLuint LoadGLTexturesPNG(GLuint *texture, const char *filePath);	
};

