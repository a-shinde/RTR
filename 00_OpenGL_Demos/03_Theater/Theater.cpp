//Header files.
#pragma once
#include "Theater.h"

bool scene_theater = true;
bool announcement = true;
bool doAnimation = false;
bool drawCurtain = true;
bool curtainanimatonComplete = false;
bool FirstNandi = true;
bool SecondNandi = false;
bool stopMusic = false;


float slowDownCurtain = 1.0f;
float xLightPosition = 0.0f;
float yLightPosition = 1.5f;
float zLigntPosition = 0.0f;
float xCamera = 0.0f;
float yCamera = -0.5f;
float zCamera = 2.5f;
float xRotation = 0.0f;
float yRotation = 0.0f;
float zRotation = 0.0f;

GLuint texture_curtain;
GLuint texture_stage_floor;
GLuint texture_stage_lower_edge;
GLuint texture_stage_upper_edge;
GLuint texture_chair;
GLuint texture_black;
GLuint texture_theater_back;
GLuint texture_theater_side;
GLuint texture_theater_floor;

GLuint texture_1_Pratibimba;
GLuint texture_2_Ghashiram;
GLuint texture_3_Teen;
GLuint texture_4_itihaas;
GLuint texture_5_bot;
GLuint texture_6_TheEscape;
GLuint texture_7_chafa;
GLuint texture_8_Sunila;
GLuint texture_9_pravah;
GLuint texture_10_AlADin;
GLuint texture_11_Sankraman;
GLuint texture_12_TheDeath;

class Light
{
public:
    GLenum lightIndex;
    bool is_ambient_on = false;
    GLfloat ambient[4];
    bool is_diffuse_on = false;
    GLfloat diffuse[4];
    bool is_specular_on = false;
    GLfloat specular[4];
    GLfloat position[4];

    bool is_attenuation_on = false;
    GLfloat constant_attenuation;
    GLfloat linear_attenuation;
    GLfloat quadratic_attenuation;
    
    bool is_spot_light = false;
    GLfloat spot_direction[3];
    GLfloat spot_cutOff;
    GLfloat spot_exponent;
    
    bool isLightEnabled;

    Light()
    {
        isLightEnabled = false;
    }

    void enableLight()
    {
        glEnable(lightIndex);
        isLightEnabled = true;
    }

    void disableLight()
    {
        glDisable(lightIndex);
        isLightEnabled = false;
    }

    void setAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
        ambient[0] = r;
        ambient[1] = g;
        ambient[2] = b;
        ambient[3] = a;
    }

    void setDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
        diffuse[0] = r;
        diffuse[1] = g;
        diffuse[2] = b;
        diffuse[3] = a;
    }

    void setSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
        specular[0] = r;
        specular[1] = g;
        specular[2] = b;
        specular[3] = a;
    }

    void setPosition(GLfloat x, GLfloat y, GLfloat z)
    {
        position[0] = x;
        position[1] = y;
        position[2] = z;
        position[3] = 1.0;
    }

    void setSpot_direction(GLfloat x, GLfloat y, GLfloat z)
    {
        spot_direction[0] = x;
        spot_direction[1] = y;
        spot_direction[2] = z;
    }

    void updateLight()
    {
        if (isLightEnabled)
        {
            glEnable(lightIndex);
        }
        else
            glDisable(lightIndex);

        if (is_ambient_on)
            glLightfv(lightIndex, GL_AMBIENT, ambient);

        if (is_diffuse_on)
            glLightfv(lightIndex, GL_DIFFUSE, diffuse);

        if (is_specular_on)
            glLightfv(lightIndex, GL_SPECULAR, specular);

        glLightfv(lightIndex, GL_POSITION, position);

        if (is_attenuation_on)
        {
            glLightf(lightIndex, GL_CONSTANT_ATTENUATION, constant_attenuation);
            glLightf(lightIndex, GL_LINEAR_ATTENUATION, linear_attenuation);
            glLightf(lightIndex, GL_QUADRATIC_ATTENUATION, quadratic_attenuation);
        }
        else
        {
            glLightf(lightIndex, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(lightIndex, GL_LINEAR_ATTENUATION, 0);
            glLightf(lightIndex, GL_QUADRATIC_ATTENUATION, 0);
        }


        if (is_spot_light)
        {
            glLightf(lightIndex, GL_SPOT_CUTOFF, spot_cutOff);
            glLightfv(lightIndex, GL_SPOT_DIRECTION, spot_direction);
            glLightf(lightIndex, GL_SPOT_EXPONENT, spot_exponent);
        }
    }
};

class Lighting
{
public:
    Light lights[8];
    bool doLightAnimation = false;
    int currentSequence = 0;
    float sequence1_Counter = 0;
    float sequence2_Counter = 0;
    float sequence3_Counter = 0;
    float sequence4_Counter = 0;

    Lighting()
    {
        initialize_sequence1();
    }

    void initialize_sequence1()
    {
        float const_atten = 0.9f;
        float lin_atten = 0.05f;
        float quad_atten = 0.05f;

        // First Light
        lights[0].lightIndex = GL_LIGHT0;
        lights[0].isLightEnabled = false;
        lights[0].setPosition(4.4f, 1.5f, 7.8f);

        lights[0].is_ambient_on = true;
        lights[0].setAmbient(0.05f, 0.05f, 0.05f, 1.0f);

        lights[0].is_diffuse_on = true;
        lights[0].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[0].is_specular_on = true;
        lights[0].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[0].is_attenuation_on = true;
        lights[0].constant_attenuation = const_atten;
        lights[0].linear_attenuation = lin_atten;
        lights[0].quadratic_attenuation = quad_atten;

        lights[0].is_spot_light = false;
        lights[0].setSpot_direction(0.0f, 0.0f, -1.0f);
        lights[0].spot_cutOff = 45.0f;
        lights[0].spot_exponent = 2.0f;

        // Second Light
        lights[1].lightIndex = GL_LIGHT1;
        lights[1].isLightEnabled = false;
        lights[1].setPosition(-4.4f, 1.5f, 7.8f);

        lights[1].is_ambient_on = true;
        lights[0].setAmbient(0.05f, 0.05f, 0.05f, 1.0f);

        lights[1].is_diffuse_on = true;
        lights[1].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[1].is_specular_on = true;
        lights[1].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[1].is_attenuation_on = true;
        lights[1].constant_attenuation = const_atten;
        lights[1].linear_attenuation = lin_atten;
        lights[1].quadratic_attenuation = quad_atten;

        lights[1].is_spot_light = false;
        lights[1].setSpot_direction(0.0f, 0.0f, -1.0f);
        lights[1].spot_cutOff = 45.0f;
        lights[1].spot_exponent = 2.0f;

        // Third Light
        lights[2].lightIndex = GL_LIGHT2;
        lights[2].isLightEnabled = false;
        lights[2].setPosition(-4.4f, 1.5f, 4.0f);

        lights[2].is_ambient_on = true;
        lights[0].setAmbient(0.05f, 0.05f, 0.05f, 1.0f);

        lights[2].is_diffuse_on = true;
        lights[2].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[2].is_specular_on = true;
        lights[2].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[2].is_attenuation_on = true;
        lights[2].constant_attenuation = const_atten;
        lights[2].linear_attenuation = lin_atten;
        lights[2].quadratic_attenuation = quad_atten;

        lights[2].is_spot_light = false;
        lights[2].setSpot_direction(0.0f, 0.0f, -1.0f);
        lights[2].spot_cutOff = 45.0f;
        lights[2].spot_exponent = 2.0f;

        // Forth Light
        lights[3].lightIndex = GL_LIGHT3;
        lights[3].isLightEnabled = false;
        lights[3].setPosition(4.4f, 1.5f, 4.0f);

        lights[3].is_ambient_on = true;
        lights[0].setAmbient(0.05f, 0.05f, 0.05f, 1.0f);

        lights[3].is_diffuse_on = true;
        lights[3].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[3].is_specular_on = true;
        lights[3].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[3].is_attenuation_on = true;
        lights[3].constant_attenuation = const_atten;
        lights[3].linear_attenuation = lin_atten;
        lights[3].quadratic_attenuation = quad_atten;

        lights[3].is_spot_light = false;
        lights[3].setSpot_direction(0.0f, 0.0f, -1.0f);
        lights[3].spot_cutOff = 45.0f;
        lights[3].spot_exponent = 2.0f;

        // Fifth Light
        lights[4].lightIndex = GL_LIGHT4;
        lights[4].isLightEnabled = false;
        lights[4].setPosition(-4.4f, 1.5f, 0.0f);

        lights[4].is_ambient_on = true;
        lights[0].setAmbient(0.05f, 0.05f, 0.05f, 1.0f);

        lights[4].is_diffuse_on = true;
        lights[4].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[4].is_specular_on = true;
        lights[4].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[4].is_attenuation_on = true;
        lights[4].constant_attenuation = const_atten;
        lights[4].linear_attenuation = lin_atten;
        lights[4].quadratic_attenuation = quad_atten;

        lights[4].is_spot_light = false;
        lights[4].setSpot_direction(0.0f, 0.0f, -1.0f);
        lights[4].spot_cutOff = 45.0f;
        lights[4].spot_exponent = 2.0f;

        // Sixth Light
        lights[5].lightIndex = GL_LIGHT5;
        lights[5].isLightEnabled = false;
        lights[5].setPosition(4.4f, 1.5f, 0.0f);

        lights[5].is_ambient_on = true;
        lights[0].setAmbient(0.05f, 0.05f, 0.05f, 1.0f);

        lights[5].is_diffuse_on = true;
        lights[5].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[5].is_specular_on = true;
        lights[5].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[5].is_attenuation_on = true;
        lights[5].constant_attenuation = const_atten;
        lights[5].linear_attenuation = lin_atten;
        lights[5].quadratic_attenuation = quad_atten;

        lights[5].is_spot_light = false;
        lights[5].setSpot_direction(0.0f, 0.0f, -1.0f);
        lights[5].spot_cutOff = 45.0f;
        lights[5].spot_exponent = 2.0f;
    }

    void LightSequence2()
    {
        float const_atten = 0.9f;
        float lin_atten = 0.05f;
        float quad_atten = 0.05f;

        // First Light
        lights[0].lightIndex = GL_LIGHT0;
        lights[0].isLightEnabled = true;
        lights[0].setPosition(0.0f, -1.5f, -3.0f);

        lights[0].is_ambient_on = true;
        lights[0].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[0].is_diffuse_on = true;
        lights[0].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[0].is_specular_on = true;
        lights[0].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[0].is_attenuation_on = true;
        lights[0].constant_attenuation = 1.0;
        lights[0].linear_attenuation = 0.0f;
        lights[0].quadratic_attenuation = 0.0f;

        lights[0].is_spot_light = true;
        lights[0].setSpot_direction(-0.4f, 0.3f, -1.0f);
        lights[0].spot_cutOff = 15.0f;
        lights[0].spot_exponent = 10.0f;

        // Second Light
        lights[1].lightIndex = GL_LIGHT1;
        lights[1].isLightEnabled = true;
        lights[1].setPosition(-2.0f, 1.5f, -3.0f);

        lights[1].is_ambient_on = true;
        lights[1].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[1].is_diffuse_on = true;
        lights[1].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[1].is_specular_on = true;
        lights[1].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[1].is_attenuation_on = true;
        lights[1].constant_attenuation = 1.0;
        lights[1].linear_attenuation = 0.0f;
        lights[1].quadratic_attenuation = 0.0f;

        lights[1].is_spot_light = true;
        lights[1].setSpot_direction(0.0f, -0.4f, -1.0f);
        lights[1].spot_cutOff = 15.0f;
        lights[1].spot_exponent = 10.0f;

        // Third Light
        lights[2].lightIndex = GL_LIGHT2;
        lights[2].isLightEnabled = true;
        lights[2].setPosition(2.0f, 1.5f, -3.0f);

        lights[2].is_ambient_on = true;
        lights[2].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[2].is_diffuse_on = true;
        lights[2].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[2].is_specular_on = true;
        lights[2].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[2].is_attenuation_on = true;
        lights[2].constant_attenuation = 1.0;
        lights[2].linear_attenuation = 0.0f;
        lights[2].quadratic_attenuation = 0.0f;

        lights[2].is_spot_light = true;
        lights[2].setSpot_direction(-0.8f, -0.4f, -1.0f);
        lights[2].spot_cutOff = 15.0f;
        lights[2].spot_exponent = 10.0f;



        lights[3].lightIndex = GL_LIGHT3;
        lights[3].isLightEnabled = true;
        lights[3].setPosition(0.0f, 7.5f, -5.0f);

        lights[3].is_ambient_on = true;
        lights[3].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[3].is_diffuse_on = true;
        lights[3].setDiffuse(0.4f, 0.4f, 0.4f, 1.0f);

        lights[3].is_specular_on = true;
        lights[3].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[3].is_attenuation_on = true;
        lights[3].constant_attenuation = const_atten;
        lights[3].linear_attenuation = 0.0f;
        lights[3].quadratic_attenuation = 0.0f;

        lights[3].is_spot_light = false;
        lights[3].setSpot_direction(0.0f, -1.0f, 0.0f);
        lights[3].spot_cutOff = 40.0f;
        lights[3].spot_exponent = 2.0f;

        lights[4].lightIndex = GL_LIGHT4;
        lights[4].isLightEnabled = true;
        lights[4].setPosition(0.0f, 0.0f, 0.5f);

        lights[4].is_ambient_on = true;
        lights[4].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[4].is_diffuse_on = true;
        lights[4].setDiffuse(0.5f, 0.5f, 0.5f, 1.0f);

        lights[4].is_specular_on = true;
        lights[4].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[4].is_attenuation_on = true;
        lights[4].constant_attenuation = 1.0f;
        lights[4].linear_attenuation = 0.0f;
        lights[4].quadratic_attenuation = 0.0f;

        lights[4].is_spot_light = false;
        lights[4].setSpot_direction(0.0f, 0.0f, -1.0f);
        lights[4].spot_cutOff = 45.0f;
        lights[4].spot_exponent = 2.0f;

        lights[5].isLightEnabled = false;
    }

    void LightSequence3()
    {
        float const_atten = 0.9f;
        float lin_atten = 0.05f;
        float quad_atten = 0.05f;

        // First Light
        lights[0].lightIndex = GL_LIGHT0;
        lights[0].isLightEnabled = true;
        lights[0].setPosition(0.0f, -1.5f, -3.0f);

        lights[0].is_ambient_on = true;
        lights[0].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[0].is_diffuse_on = true;
        lights[0].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[0].is_specular_on = true;
        lights[0].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[0].is_attenuation_on = true;
        lights[0].constant_attenuation = 1.0;
        lights[0].linear_attenuation = 0.0f;
        lights[0].quadratic_attenuation = 0.0f;

        lights[0].is_spot_light = true;
        lights[0].setSpot_direction(0.0f, 0.3f, -1.0f);
        lights[0].spot_cutOff = 15.0f;
        lights[0].spot_exponent = 10.0f;

        // Second Light
        lights[1].lightIndex = GL_LIGHT1;
        lights[1].isLightEnabled = true;
        lights[1].setPosition(-2.0f, 1.5f, -3.0f);

        lights[1].is_ambient_on = true;
        lights[1].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[1].is_diffuse_on = true;
        lights[1].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[1].is_specular_on = true;
        lights[1].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[1].is_attenuation_on = true;
        lights[1].constant_attenuation = 1.0;
        lights[1].linear_attenuation = 0.0f;
        lights[1].quadratic_attenuation = 0.0f;

        lights[1].is_spot_light = true;
        lights[1].setSpot_direction(0.4f, -0.4f, -1.0f);
        lights[1].spot_cutOff = 15.0f;
        lights[1].spot_exponent = 10.0f;

        // Third Light
        lights[2].lightIndex = GL_LIGHT2;
        lights[2].isLightEnabled = true;
        lights[2].setPosition(2.0f, 1.5f, -3.0f);

        lights[2].is_ambient_on = true;
        lights[2].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[2].is_diffuse_on = true;
        lights[2].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[2].is_specular_on = true;
        lights[2].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[2].is_attenuation_on = true;
        lights[2].constant_attenuation = 1.0;
        lights[2].linear_attenuation = 0.0f;
        lights[2].quadratic_attenuation = 0.0f;

        lights[2].is_spot_light = true;
        lights[2].setSpot_direction(-0.4f, -0.4f, -1.0f);
        lights[2].spot_cutOff = 15.0f;
        lights[2].spot_exponent = 10.0f;




        lights[3].lightIndex = GL_LIGHT3;
        lights[3].isLightEnabled = true;
        lights[3].setPosition(0.0f, 7.5f, -5.0f);

        lights[3].is_ambient_on = true;
        lights[3].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[3].is_diffuse_on = true;
        lights[3].setDiffuse(0.4f, 0.4f, 0.4f, 1.0f);

        lights[3].is_specular_on = true;
        lights[3].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[3].is_attenuation_on = true;
        lights[3].constant_attenuation = const_atten;
        lights[3].linear_attenuation = 0.0f;
        lights[3].quadratic_attenuation = 0.0f;

        lights[3].is_spot_light = false;
        lights[3].setSpot_direction(0.0f, -1.0f, 0.0f);
        lights[3].spot_cutOff = 40.0f;
        lights[3].spot_exponent = 2.0f;

        lights[4].lightIndex = GL_LIGHT4;
        lights[4].isLightEnabled = true;
        lights[4].setPosition(0.0f, 0.0f, 0.5f);

        lights[4].is_ambient_on = true;
        lights[4].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[4].is_diffuse_on = true;
        lights[4].setDiffuse(0.5f, 0.5f, 0.5f, 1.0f);

        lights[4].is_specular_on = true;
        lights[4].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[4].is_attenuation_on = true;
        lights[4].constant_attenuation = 1.0f;
        lights[4].linear_attenuation = 0.0f;
        lights[4].quadratic_attenuation = 0.0f;

        lights[4].is_spot_light = false;
        lights[4].setSpot_direction(0.0f, 0.0f, -1.0f);
        lights[4].spot_cutOff = 45.0f;
        lights[4].spot_exponent = 2.0f;

        lights[5].isLightEnabled = false;

    }
    
    void LightSequence4()
    {
        float const_atten = 0.9f;
        float lin_atten = 0.05f;
        float quad_atten = 0.05f;

        // First Light
        lights[0].lightIndex = GL_LIGHT0;
        lights[0].isLightEnabled = true;
        lights[0].setPosition(0.0f, -1.5f, -3.0f);

        lights[0].is_ambient_on = true;
        lights[0].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[0].is_diffuse_on = true;
        lights[0].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[0].is_specular_on = true;
        lights[0].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[0].is_attenuation_on = true;
        lights[0].constant_attenuation = 1.0;
        lights[0].linear_attenuation = 0.0f;
        lights[0].quadratic_attenuation = 0.0f;

        lights[0].is_spot_light = true;
        lights[0].setSpot_direction(0.45f, 0.3f, -1.0f);
        lights[0].spot_cutOff = 15.0f;
        lights[0].spot_exponent = 10.0f;

        // Second Light
        lights[1].lightIndex = GL_LIGHT1;
        lights[1].isLightEnabled = true;
        lights[1].setPosition(-2.0f, 1.5f, -3.0f);

        lights[1].is_ambient_on = true;
        lights[1].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[1].is_diffuse_on = true;
        lights[1].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[1].is_specular_on = true;
        lights[1].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[1].is_attenuation_on = true;
        lights[1].constant_attenuation = 1.0;
        lights[1].linear_attenuation = 0.0f;
        lights[1].quadratic_attenuation = 0.0f;

        lights[1].is_spot_light = true;
        lights[1].setSpot_direction(0.8f, -0.4f, -1.0f);
        lights[1].spot_cutOff = 15.0f;
        lights[1].spot_exponent = 10.0f;

        // Third Light
        lights[2].lightIndex = GL_LIGHT2;
        lights[2].isLightEnabled = true;
        lights[2].setPosition(2.0f, 1.5f, -3.0f);

        lights[2].is_ambient_on = true;
        lights[2].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[2].is_diffuse_on = true;
        lights[2].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

        lights[2].is_specular_on = true;
        lights[2].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[2].is_attenuation_on = true;
        lights[2].constant_attenuation = 1.0;
        lights[2].linear_attenuation = 0.0f;
        lights[2].quadratic_attenuation = 0.0f;

        lights[2].is_spot_light = true;
        lights[2].setSpot_direction(0.1f, -0.4f, -1.0f);
        lights[2].spot_cutOff = 15.0f;
        lights[2].spot_exponent = 10.0f;




        lights[3].lightIndex = GL_LIGHT3;
        lights[3].isLightEnabled = true;
        lights[3].setPosition(0.0f, 7.5f, -5.0f);

        lights[3].is_ambient_on = true;
        lights[3].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[3].is_diffuse_on = true;
        lights[3].setDiffuse(0.4f, 0.4f, 0.4f, 1.0f);

        lights[3].is_specular_on = true;
        lights[3].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[3].is_attenuation_on = true;
        lights[3].constant_attenuation = const_atten;
        lights[3].linear_attenuation = 0.0f;
        lights[3].quadratic_attenuation = 0.0f;

        lights[3].is_spot_light = false;
        lights[3].setSpot_direction(0.0f, -1.0f, 0.0f);
        lights[3].spot_cutOff = 40.0f;
        lights[3].spot_exponent = 2.0f;

        lights[4].lightIndex = GL_LIGHT4;
        lights[4].isLightEnabled = true;
        lights[4].setPosition(0.0f, 0.0f, 0.5f);

        lights[4].is_ambient_on = true;
        lights[4].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

        lights[4].is_diffuse_on = true;
        lights[4].setDiffuse(0.5f, 0.5f, 0.5f, 1.0f);

        lights[4].is_specular_on = true;
        lights[4].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[4].is_attenuation_on = true;
        lights[4].constant_attenuation = 1.0f;
        lights[4].linear_attenuation = 0.0f;
        lights[4].quadratic_attenuation = 0.0f;

        lights[4].is_spot_light = false;
        lights[4].setSpot_direction(0.0f, 0.0f, -1.0f);
        lights[4].spot_cutOff = 45.0f;
        lights[4].spot_exponent = 2.0f;

        lights[5].isLightEnabled = false;



    }

    void disableLighting()
    {
        for (int index = 0; index < 8; index++)
        {

            lights[index].disableLight();
            lights[index].isLightEnabled = false;
            lights[index].setPosition(0.0f, 0.0f, 0.0f);

            lights[index].is_ambient_on = true;
            lights[index].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

            lights[index].is_diffuse_on = true;
            lights[index].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

            lights[index].is_specular_on = true;
            lights[index].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

            lights[index].is_attenuation_on = false;
            lights[index].constant_attenuation = 1.0f;
            lights[index].linear_attenuation = 0.0f;
            lights[index].quadratic_attenuation = 0.0f;

            lights[index].is_spot_light = false;
            lights[index].setSpot_direction(0.0f, 0.0f, -1.0f);
            lights[index].spot_cutOff = 45.0f;
            lights[index].spot_exponent = 2.0f;
        }
        glDisable(GL_LIGHTING);
    }

    void updateLighting()
    {
        for (int index = 0; index < 8; index++)
        {
            lights[index].updateLight();
        }
    }

    void enableLighting()
    {
        glEnable(GL_LIGHTING);
    }
};

class Vertex
{
public:
    float cord[3];

    Vertex(float x, float y, float z)
    {
        cord[0] = x;
        cord[1] = y;
        cord[2] = z;
    }

    Vertex() {}

    float length()
    {
        return sqrt(cord[0] * cord[0] + cord[1] * cord[1] + cord[2] * cord[2]);
    }

    Vertex normalized()
    {
        float len = length();
        return Vertex(cord[0] / len, cord[1] / len, cord[2] / len);
    }

    void operator+= (const Vertex &v)
    {
        cord[0] += v.cord[0];
        cord[1] += v.cord[1];
        cord[2] += v.cord[2];
    }

    Vertex operator/ (const float &a)
    {
        return Vertex(cord[0] / a, cord[1] / a, cord[2] / a);
    }

    Vertex operator- (const Vertex &v)
    {
        return Vertex(cord[0] - v.cord[0], cord[1] - v.cord[1], cord[2] - v.cord[2]);
    }

    Vertex operator+ (const Vertex &v)
    {
        return Vertex(cord[0] + v.cord[0], cord[1] + v.cord[1], cord[2] + v.cord[2]);
    }

    Vertex operator* (const float &a)
    {
        return Vertex(cord[0] * a, cord[1] * a, cord[2] * a);
    }

    Vertex operator-()
    {
        return Vertex(-cord[0], -cord[1], -cord[2]);
    }

    Vertex cross(const Vertex &v)
    {
        //TBD
        return Vertex(cord[1] * v.cord[2] - cord[2] * v.cord[1], cord[2] * v.cord[0] - cord[0] * v.cord[2], cord[0] * v.cord[1] - cord[1] * v.cord[0]);
    }

    float dot(const Vertex &v)
    {
        return cord[0] * v.cord[0] + cord[1] * v.cord[1] + cord[2] * v.cord[2];
    }
};

class CameraSequence
{
public:
    Vertex initialPosition;
    Vertex currentPosition;
    Vertex finalPosition;
    Vertex initialRotation;
    Vertex currentRotation;
    Vertex finalRotation;
    float PosIncrementer;
    float RotIncrementer;
    float prePosDiff;
    float PreRotDiff;
    bool isSequenceFinished;

    CameraSequence(Vertex cPos, Vertex fPos, Vertex cRot, Vertex fRot, float pIncr, float rIncer)
    {
        initialPosition = cPos;
        currentPosition = cPos;
        finalPosition = fPos;
        initialRotation = cRot;
        currentRotation = cRot;
        finalRotation = fRot;
        PosIncrementer = pIncr;
        RotIncrementer = rIncer;
        isSequenceFinished = false;

        prePosDiff = (finalPosition - initialPosition).length();
        PreRotDiff = (finalRotation - initialRotation).length();
    }

    void moveCamera()
    {
        if (!isSequenceFinished)
        {
            prePosDiff = (finalPosition - currentPosition).length();
            currentPosition = currentPosition + (finalPosition - initialPosition)* PosIncrementer;
        }
    }

    void rotateCamera()
    {
        if (!isSequenceFinished)
        {
            PreRotDiff = (finalRotation - currentRotation).length();
            currentRotation = currentRotation + (finalRotation - initialRotation)* RotIncrementer;
        }
    }


    bool isMotionFinished()
    {
        float posDiff = (finalPosition - currentPosition).length();
        float rotDiff = (finalRotation - currentRotation).length();

        if ((posDiff >= prePosDiff)
            && (rotDiff >= PreRotDiff))
        {
            isSequenceFinished = true;
            return true;
        }
        else
            return false;
    }

};

class Partical
{
public:
    bool isMovable;
    float wight;
    Vertex pos;
    Vertex old_pos;
    Vertex acceleration;
    Vertex total_normal;

    Partical(Vertex position)
    {
        pos = position;
        old_pos = position;
        acceleration = Vertex(0, 0, 0);
        wight = 1;
        isMovable = true;
        total_normal = Vertex(0, 0, 0);
    }

    Partical()
    {}

    void addForce(Vertex force)
    {
        acceleration += force / wight;
    }

    void Update()
    {
        if (isMovable)
        {
            Vertex temp = pos;
            pos = pos + (pos - old_pos)*(1.0f - SLOWDOWN) + acceleration*SPEED;
            old_pos = temp;
            acceleration = Vertex(0, 0, 0);
        }
    }

    Vertex& getPos()
    {
        return pos;
    }

    void resetAcceleration()
    {
        acceleration = Vertex(0, 0, 0);
    }

    void offsetPos(Vertex v)
    {
        if (isMovable)
            pos += v;
    }

    void Make_Hook()
    {
        isMovable = false;
    }

    void Un_Hook()
    {
        isMovable = true;
    }

    void addToNormal(Vertex normal)
    {
        total_normal += normal.normalized();
    }

    Vertex& getNormal()
    {
        return total_normal;
    }

    void resetNormal()
    {
        total_normal = Vertex(0, 0, 0);
    }

};

class Constraint
{

public:
    float resting_length;
    Partical *p1, *p2;

    Constraint(Partical *partical1, Partical *partical2)
    {
        p1 = partical1;
        p2 = partical2;
        Vertex vec = (*p1).pos - (*p2).pos;
        resting_length = vec.length();
    }

    void MakeCorrection()
    {
        Vertex p1_to_p2 = (*p2).pos - (*p1).pos;
        float current_distance = p1_to_p2.length();
        Vertex correctionVector = p1_to_p2 * (1 - resting_length / current_distance);
        Vertex correctionVectorHalf = correctionVector*0.5f;
        (*p1).offsetPos(correctionVectorHalf);
        (*p2).offsetPos(-correctionVectorHalf);
    }
};

class Curtain
{
public:
    float width;
    float height;
    int num_Horizontal_Particals;
    int num_verticle_Particals;
    int constraintLevel;
    int hookWidth = 3;
    bool isCurtainDrawn;
    bool isCurtainUnDrawn;


    std::vector<Partical> Particals;
    std::vector<Constraint> constraints;

    Partical& getPartical(int x, int y)
    {
        return Particals[y*num_Horizontal_Particals + x];
    }

    void SetConstraint(Partical &p1, Partical &p2)
    {
        constraints.push_back(Constraint(&p1, &p2));
    }

    Vertex calcTriangleNormal(Partical &p1, Partical &p2, Partical &p3)
    {
        Vertex pos1 = p1.pos;
        Vertex pos2 = p2.pos;
        Vertex pos3 = p3.pos;

        Vertex v1 = pos2 - pos1;
        Vertex v2 = pos3 - pos1;

        return v1.cross(v2);
    }

    void addWindForcesForTriangle(Partical &p1, Partical &p2, Partical &p3, const Vertex direction)
    {
        Vertex normal = calcTriangleNormal(p1, p2, p3);
        Vertex d = normal.normalized();
        Vertex force = normal * (d.dot(direction));
        p1.addForce(force);
        p2.addForce(force);
        p3.addForce(force);
    }

    void drawTriangle(int x, int y)
    {
        glTexCoord2f(((GLfloat)x) / num_Horizontal_Particals, (1 - ((GLfloat)y) / num_verticle_Particals));
        glNormal3fv((GLfloat *) &(getPartical(x, y).getNormal().normalized()));
        glVertex3fv((GLfloat *) &(getPartical(x, y).pos));
    }

    void makeHooks()
    {
        for (int x = 0; x<num_Horizontal_Particals; x += hookWidth)
        {
            if ((((x / hookWidth) % 2) == 0) && (x + hookWidth) <= num_Horizontal_Particals)
            {
                float zHook = getPartical(x + 1, 0).pos.cord[0] - getPartical(x, 0).pos.cord[0];
                //Hook Particals
                if (((x / (hookWidth * 2)) % 2) == 0)
                {
                    getPartical(x, 0).offsetPos(Vertex(zHook / 2, 0, -zHook));
                    getPartical(x + 2, 0).offsetPos(Vertex(-zHook / 2, 0, zHook));
                }
                else
                {
                    getPartical(x, 0).offsetPos(Vertex(zHook / 2, 0, zHook));
                    getPartical(x + 2, 0).offsetPos(Vertex(-zHook / 2, 0, -zHook));
                }
                getPartical(x, 0).Make_Hook();
                getPartical(x + 1, 0).Make_Hook();
                getPartical(x + 2, 0).Make_Hook();
            }
        }
    }

    Curtain(float curtainWidth, float curtainHeight, int num_H_Particals, int num_v_Particals, int cLevel)
    {
        width = curtainWidth;
        height = curtainHeight;
        num_Horizontal_Particals = num_H_Particals;
        num_verticle_Particals = num_v_Particals;
        isCurtainDrawn = false;
        isCurtainUnDrawn = true;
        Particals.resize(num_Horizontal_Particals*num_verticle_Particals);
        hookWidth = 3;
        constraintLevel = cLevel;

        for (int x = 0; x<num_Horizontal_Particals; x++)
        {
            for (int y = 0; y<num_verticle_Particals; y++)
            {
                Vertex pos = Vertex(width * (x / (float)num_Horizontal_Particals),
                    -height * (y / (float)num_verticle_Particals), 0);

                Particals[y*num_Horizontal_Particals + x] = Partical(pos);
            }
        }

        for (int x = 0; x<num_Horizontal_Particals; x++)
        {
            for (int y = 0; y<num_verticle_Particals; y++)
            {
                if (x<num_Horizontal_Particals - 1)
                    SetConstraint(getPartical(x, y), getPartical(x + 1, y));
                if (y<num_verticle_Particals - 1)
                    SetConstraint(getPartical(x, y), getPartical(x, y + 1));
                if (x<num_Horizontal_Particals - 1 && y<num_verticle_Particals - 1)
                    SetConstraint(getPartical(x, y), getPartical(x + 1, y + 1));
                if (x<num_Horizontal_Particals - 1 && y<num_verticle_Particals - 1)
                    SetConstraint(getPartical(x + 1, y), getPartical(x, y + 1));
            }
        }


        for (int x = 0; x<num_Horizontal_Particals; x++)
        {
            for (int y = 0; y<num_verticle_Particals; y++)
            {
                if (y<num_verticle_Particals - 2)
                    SetConstraint(getPartical(x, y), getPartical(x, y + 2));

                if (constraintLevel > 0)
                {
                    if (x<num_Horizontal_Particals - 2) SetConstraint(getPartical(x, y), getPartical(x + 2, y));
                    if (x<num_Horizontal_Particals - 2 && y<num_verticle_Particals - 2) SetConstraint(getPartical(x, y), getPartical(x + 2, y + 2));
                    if (x<num_Horizontal_Particals - 2 && y<num_verticle_Particals - 2) SetConstraint(getPartical(x + 2, y), getPartical(x, y + 2));
                }
            }
        }
    }



    void showCurtain()
    {
        std::vector<Partical>::iterator Partical;
        for (Partical = Particals.begin(); Partical != Particals.end(); Partical++)
        {
            (*Partical).resetNormal();
        }

        for (int x = 0; x<num_Horizontal_Particals - 1; x++)
        {
            for (int y = 0; y<num_verticle_Particals - 1; y++)
            {
                Vertex normal = calcTriangleNormal(getPartical(x + 1, y), getPartical(x, y), getPartical(x, y + 1));
                getPartical(x + 1, y).addToNormal(normal);
                getPartical(x, y).addToNormal(normal);
                getPartical(x, y + 1).addToNormal(normal);

                normal = calcTriangleNormal(getPartical(x + 1, y + 1), getPartical(x + 1, y), getPartical(x, y + 1));
                getPartical(x + 1, y + 1).addToNormal(normal);
                getPartical(x + 1, y).addToNormal(normal);
                getPartical(x, y + 1).addToNormal(normal);
            }
        }

        for (int x = 0; x<num_Horizontal_Particals - 1; x++)
        {
            for (int y = 0; y<num_verticle_Particals - 1; y++)
            {
                glBegin(GL_TRIANGLES);
                drawTriangle(x + 1, y);
                drawTriangle(x, y);
                drawTriangle(x, y + 1);
                drawTriangle(x + 1, y + 1);
                drawTriangle(x + 1, y);
                drawTriangle(x, y + 1);
                glEnd();
            }
        }
    }

    void wrapCurtain()
    {   
        for (int x = 0; x<num_Horizontal_Particals - 1; x++)
        {
            for (int y = 0; y<num_verticle_Particals - 1; y++)
            {
                float randX = (((rand() % 2000) / 2000.0f) - 0.5f)* 0.001f;
                float randY = (((rand() % 2000) / 2000.0f) - 0.5f)* 0.001f;
                float randZ = 0.0f;

                getPartical(x + 1, y).pos = getPartical(x + 1, y).pos + Vertex(randX, randY, randZ);
                getPartical(x, y).pos = getPartical(x, y).pos + Vertex(randX, randY, randZ);
                getPartical(x, y + 1).pos = getPartical(x, y + 1).pos + Vertex(randX, randY, randZ);
                getPartical(x + 1, y + 1).pos = getPartical(x + 1, y + 1).pos + Vertex(randX, randY, randZ);
                getPartical(x + 1, y).pos = getPartical(x + 1, y).pos + Vertex(randX, randY, randZ);
                getPartical(x, y + 1).pos = getPartical(x, y + 1).pos + Vertex(randX, randY, randZ);

            }
        }
    }



    void Update()
    {
        std::vector<Constraint>::iterator constraint;
        for (int i = 0; i<REFINE_ITERATIONS; i++)
        {
            for (constraint = constraints.begin(); constraint != constraints.end(); constraint++)
            {
                (*constraint).MakeCorrection();
            }
        }

        std::vector<Partical>::iterator Partical;
        for (Partical = Particals.begin(); Partical != Particals.end(); Partical++)
        {
            (*Partical).Update();
        }
    }

    void addForce(const Vertex direction)
    {
        std::vector<Partical>::iterator Partical;
        for (Partical = Particals.begin(); Partical != Particals.end(); Partical++)
        {
            (*Partical).addForce(direction);
        }
    }

    void windForce(const Vertex direction)
    {
        for (int x = 0; x<num_Horizontal_Particals - 1; x++)
        {
            for (int y = 0; y<num_verticle_Particals - 1; y++)
            {
                addWindForcesForTriangle(getPartical(x + 1, y), getPartical(x, y), getPartical(x, y + 1), direction);
                addWindForcesForTriangle(getPartical(x + 1, y + 1), getPartical(x + 1, y), getPartical(x, y + 1), direction);
            }
        }
    }


    void drawCurtain(int curtainEdge, Vertex movement, int action)
    {
        if ((action == 1 && !isCurtainDrawn) || (action == -1 && !isCurtainUnDrawn))
        {
            for (int x = 0; x < num_Horizontal_Particals; x += hookWidth)
            {
                if ((((x / hookWidth) % 2) == 0) && (x + hookWidth) <= num_Horizontal_Particals)
                {
                    float zHook = getPartical(x + 1, 0).pos.cord[0] - getPartical(x, 0).pos.cord[0];
                    //Hook Particals
                    for (int i = 0; i < hookWidth; i++)
                    {
                        getPartical(x + i, 0).Un_Hook();
                        // 1 means right edge, 0 means left edge

                        if (curtainEdge)
                            getPartical(x + i, 0).offsetPos(((movement*0.2f) + movement *((GLfloat)x / num_Horizontal_Particals))*action);
                        else
                            getPartical(x + i, 0).offsetPos(((movement*0.2f) + movement *(1.0f - ((GLfloat)x / num_Horizontal_Particals)))*action);
                        getPartical(x + i, 0).Make_Hook();
                    }
                }
            }
            if (action == 1)
            {
                isCurtainUnDrawn = false;
                float distance;
                if (curtainEdge)
                {
                    distance = getPartical((num_Horizontal_Particals - 2), 0).pos.cord[0] - getPartical((num_Horizontal_Particals - 2) - hookWidth, 0).pos.cord[0];
                }
                else
                {
                    distance = getPartical(hookWidth, 0).pos.cord[0] - getPartical(0, 0).pos.cord[0];

                }
                if (distance < 0.01f)
                    isCurtainDrawn = true;
            }
            else
            {
                isCurtainDrawn = false;
                float distance;
                if (curtainEdge)
                {
                    distance = getPartical((num_Horizontal_Particals - 2), 0).pos.cord[0] - getPartical((num_Horizontal_Particals - 2) - hookWidth, 0).pos.cord[0];
                }
                else
                {
                    distance = getPartical(hookWidth, 0).pos.cord[0] - getPartical(0, 0).pos.cord[0];

                }
                if (distance > ((float)width * 3 / num_Horizontal_Particals))
                    isCurtainUnDrawn = true;
            }
        }
    }
};

class Chairs
{
public:

    GLUnurbsObj *nurb1;
    GLUnurbsObj *nurb2;
    GLfloat chair_back[9][9][3];
    GLfloat chair_back_Normals[9 * 9 * 2];
    GLfloat chair_seat[9][9][3];
    GLfloat chair_seat_Normals[9 * 9 * 2];
    GLfloat chair_handle[9][9][3];
    GLfloat chair_handle_Normals[9 * 9 * 2];
    GLfloat tempChairRotation = 0.0f;
    GLfloat multiplyFactor = 0.5f;
    GLfloat xunit = 0.13f * multiplyFactor;
    GLfloat yunit = 0.2f * multiplyFactor;
    GLfloat zunit = 0.13f * multiplyFactor;
    GLfloat panelWidth = 0.05f;
    int rowCount = 11;
    int columnCount = 13;

    GLfloat knots[13] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.4f, 0.5f, 0.6f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f };


    void initializeBack()
    {
        GLfloat chair_depth_Pattern[9] = { 1.0f, 1.2f, 0.7f, 0.5f, 1.0f, 1.2f, 1.2f, 1.2f, 1.0f };
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if ((i == 0) || (i == 8) || (j == 0) || (j == 8))
                {
                    chair_back[i][j][0] = xunit * (j - 4);
                    chair_back[i][j][1] = yunit * (4 - i);
                    chair_back[i][j][2] = xunit * (-1.0f);
                }
                else
                {
                    chair_back[i][j][0] = xunit * (j - 4);
                    chair_back[i][j][1] = yunit * (4 - i);
                    chair_back[i][j][2] = xunit* chair_depth_Pattern[i];
                }
            }
        };
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (i == 0)
                    chair_back[i][j][1] -= yunit;
                if (i == 8)
                    chair_back[i][j][1] += yunit;
                if (j == 0)
                    chair_back[i][j][0] += xunit;
                if (j == 8)
                    chair_back[i][j][0] -= xunit;
            }
        };

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                chair_back_Normals[((i * 18) + j * 2) + 0] = (float)j / 9.0f;
                chair_back_Normals[((i * 18) + j * 2) + 1] = 1.0f - ((float)i / 9.0f);
            }
        };

    }

    void initializeSeat()
    {
        GLfloat chair_depth_Pattern[9] = { 1.0f, 1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.8f, 0.8f, 1.0f };
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if ((i == 0) || (i == 8) || (j == 0) || (j == 8))
                {
                    chair_seat[i][j][0] = xunit * (4 - i);//xunit * (j - 4);
                    chair_seat[i][j][1] = zunit*(-1);//yunit * (4 - i);
                    chair_seat[i][j][2] = zunit * (j - 4);//xunit*(-1);
                }
                else
                {
                    chair_seat[i][j][0] = xunit * (4 - i);//xunit * (j - 4);
                    chair_seat[i][j][1] = zunit* chair_depth_Pattern[j];//yunit * (4 - i);
                    chair_seat[i][j][2] = zunit * (j - 4);//xunit* chair_depth_Pattern[i];
                }
            }
        };
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (i == 0)
                    chair_seat[i][j][0] -= xunit;
                if (i == 8)
                    chair_seat[i][j][0] += xunit;
                if (j == 0)
                    chair_seat[i][j][2] += zunit;
                if (j == 8)
                    chair_seat[i][j][2] -= zunit;

            }
        };

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                chair_seat_Normals[((i * 18) + j * 2) + 0] = (float)j / 9.0f;
                chair_seat_Normals[((i * 18) + j * 2) + 1] = 1.0f - ((float)i / 9.0f);
            }
        };
    }

    void drawTriangle(int x, int y)
    {
        glTexCoord2f(chair_back_Normals[(x * 9) + y], chair_back_Normals[(x * 9) + y + 1]);
        //glNormal3fv((GLfloat *) &(getPartical(x, y).getNormal().normalized()));
        glVertex3f(chair_back[x][y][0], chair_back[x][y][1], chair_back[x][y][2]);
    }

    void drawSingleChair()
    {
        // glRotatef(tempChairRotation += 0.001f, 0.0f, 1.0f, 0.0f);
        glPushMatrix();
        glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

        //back panel
        drawBackPanel();

        /*gluBeginSurface(nurb1);
        gluNurbsSurface(nurb1,
        13, knots, 13, knots,
        9 * 3, 3, &chair_back[0][0][0],
        4, 4, GL_MAP2_VERTEX_3);

        gluNurbsSurface(nurb1,
        13, knots, 13, knots,
        9 * 2, 2, &chair_back_Normals[0],
        4, 4, GL_MAP2_TEXTURE_COORD_2);
        gluEndSurface(nurb1);*/

        glBegin(GL_QUADS);
        for (int x = 0; x < 8; x++)
        {
            for (int y = 0; y < 8; y++)
            {

                glTexCoord2f((float)(x + 1) / 8.0f, (1.0f - (float)(y) / 8.0f));
                glVertex3fv((GLfloat *)chair_back[x + 1][y]);

                glTexCoord2f((float)(x) / 8.0f, (1.0f - (float)(y) / 8.0f));
                glVertex3fv((GLfloat *)chair_back[x][y]);

                glTexCoord2f((float)(x) / 8.0f, (1.0f - (float)(y + 1) / 8.0f));
                glVertex3fv((GLfloat *)chair_back[x][y + 1]);

                glTexCoord2f((float)(x + 1) / 8.0f, (1.0f - (float)(y + 1) / 8.0f));
                glVertex3fv((GLfloat *)chair_back[x + 1][y + 1]);
            }
        }
        glEnd();
        glPopMatrix();

        //seat

        gluBeginSurface(nurb2);
        glTranslatef(0.0f, -0.25f, -0.25f);
        drawSeatPanel();

        //gluBeginSurface(nurb2);
        //gluNurbsSurface(nurb2,
        //    13, knots, 13, knots,
        //    9 * 3, 3, &chair_seat[0][0][0],
        //    4, 4, GL_MAP2_VERTEX_3);

        //gluNurbsSurface(nurb2,
        //    13, knots, 13, knots,
        //    9 * 2, 2, &chair_seat_Normals[0],
        //    4, 4, GL_MAP2_TEXTURE_COORD_2);

        //gluEndSurface(nurb2);

        glBegin(GL_QUADS);
        for (int x = 0; x < 8; x++)
        {
            for (int y = 0; y < 8; y++)
            {

                glTexCoord2f((float)(x + 1) / 8.0f, (1.0f - (float)(y) / 8.0f));
                glVertex3fv((GLfloat *)chair_seat[x + 1][y]);

                glTexCoord2f((float)(x) / 8.0f, (1.0f - (float)(y) / 8.0f));
                glVertex3fv((GLfloat *)chair_seat[x][y]);

                glTexCoord2f((float)(x) / 8.0f, (1.0f - (float)(y + 1) / 8.0f));
                glVertex3fv((GLfloat *)chair_seat[x][y + 1]);

                glTexCoord2f((float)(x + 1) / 8.0f, (1.0f - (float)(y + 1) / 8.0f));
                glVertex3fv((GLfloat *)chair_seat[x + 1][y + 1]);
            }
        }
        glEnd();

        drawStand();
    }

    void drawBackPanel()
    {
        GLfloat unit = 0.02f;
        glBegin(GL_QUADS);
        //Front
        glVertex3f(chair_back[0][0][0] - unit, chair_back[0][0][1] + unit, chair_back[0][0][2]);
        glVertex3f(chair_back[0][8][0] + unit, chair_back[0][8][1] + unit, chair_back[0][8][2]);
        glVertex3f(chair_back[8][8][0] + unit, chair_back[8][8][1] - unit, chair_back[8][8][2]);
        glVertex3f(chair_back[8][0][0] - unit, chair_back[8][0][1] - unit, chair_back[8][0][2]);

        //Back
        glVertex3f(chair_back[0][0][0] - unit, chair_back[0][0][1] + unit, (chair_back[0][0][2] + panelWidth));
        glVertex3f(chair_back[0][8][0] + unit, chair_back[0][8][1] + unit, (chair_back[0][8][2] + panelWidth));
        glVertex3f(chair_back[8][8][0] + unit, chair_back[8][8][1] - unit, (chair_back[8][8][2] + panelWidth));
        glVertex3f(chair_back[8][0][0] - unit, chair_back[8][0][1] - unit, (chair_back[8][0][2] + panelWidth));

        //Right
        glVertex3f(chair_back[0][8][0] + unit, chair_back[0][8][1] + unit, (chair_back[0][8][2] + panelWidth));
        glVertex3f(chair_back[0][8][0] + unit, chair_back[0][8][1] + unit, chair_back[0][8][2]);
        glVertex3f(chair_back[8][8][0] + unit, chair_back[8][8][1] - unit, chair_back[8][8][2]);
        glVertex3f(chair_back[8][8][0] + unit, chair_back[8][8][1] - unit, (chair_back[8][8][2] + panelWidth));

        //Left
        glVertex3f(chair_back[0][0][0] - unit, chair_back[0][0][1] + unit, (chair_back[0][0][2] + panelWidth));
        glVertex3f(chair_back[0][0][0] - unit, chair_back[0][0][1] + unit, chair_back[0][0][2]);
        glVertex3f(chair_back[8][0][0] - unit, chair_back[8][0][1] - unit, chair_back[8][0][2]);
        glVertex3f(chair_back[8][0][0] - unit, chair_back[8][0][1] - unit, (chair_back[8][0][2] + panelWidth));

        //Top
        glVertex3f(chair_back[0][8][0] + unit, chair_back[0][8][1] + unit, (chair_back[0][8][2] + panelWidth));
        glVertex3f(chair_back[0][0][0] - unit, chair_back[0][0][1] + unit, (chair_back[0][0][2] + panelWidth));
        glVertex3f(chair_back[0][0][0] - unit, chair_back[0][0][1] + unit, chair_back[0][0][2]);
        glVertex3f(chair_back[0][8][0] + unit, chair_back[0][8][1] + unit, chair_back[0][8][2]);

        //Bottom
        glVertex3f(chair_back[8][8][0] + unit, chair_back[8][8][1] - unit, (chair_back[8][8][2] + panelWidth));
        glVertex3f(chair_back[8][0][0] - unit, chair_back[8][0][1] - unit, (chair_back[8][0][2] + panelWidth));
        glVertex3f(chair_back[8][0][0] - unit, chair_back[8][0][1] - unit, chair_back[8][0][2]);
        glVertex3f(chair_back[8][8][0] + unit, chair_back[8][8][1] - unit, chair_back[8][8][2]);

        glEnd();
    }

    void drawSeatPanel()
    {
        GLfloat unit = -0.02f;
        glBegin(GL_QUADS);

        //Top
        glVertex3f(chair_seat[0][0][0] - unit, chair_seat[0][0][1], chair_seat[0][0][2] + unit);
        glVertex3f(chair_seat[0][8][0] - unit, chair_seat[0][8][1], chair_seat[0][8][2] - unit);
        glVertex3f(chair_seat[8][8][0] + unit, chair_seat[8][8][1], chair_seat[8][8][2] - unit);
        glVertex3f(chair_seat[8][0][0] + unit, chair_seat[8][0][1], chair_seat[8][0][2] + unit);

        //Bottom
        glVertex3f(chair_seat[0][0][0] - unit, chair_seat[0][0][1] - panelWidth, chair_seat[0][0][2] + unit);
        glVertex3f(chair_seat[0][8][0] - unit, chair_seat[0][8][1] - panelWidth, chair_seat[0][8][2] - unit);
        glVertex3f(chair_seat[8][8][0] + unit, chair_seat[8][8][1] - panelWidth, chair_seat[8][8][2] - unit);
        glVertex3f(chair_seat[8][0][0] + unit, chair_seat[8][0][1] - panelWidth, chair_seat[8][0][2] + unit);

        //right
        glVertex3f(chair_seat[0][8][0] - unit, chair_seat[0][8][1], chair_seat[0][8][2] - unit);
        glVertex3f(chair_seat[8][8][0] + unit, chair_seat[8][8][1], chair_seat[8][8][2] - unit);
        glVertex3f(chair_seat[8][8][0] + unit, chair_seat[8][8][1] - panelWidth, chair_seat[8][8][2] - unit);
        glVertex3f(chair_seat[0][8][0] - unit, chair_seat[0][8][1] - panelWidth, chair_seat[0][8][2] - unit);

        //left
        glVertex3f(chair_seat[0][0][0] - unit, chair_seat[0][0][1], chair_seat[0][0][2] + unit);
        glVertex3f(chair_seat[8][0][0] + unit, chair_seat[8][0][1], chair_seat[8][0][2] + unit);
        glVertex3f(chair_seat[8][0][0] + unit, chair_seat[8][0][1] - panelWidth, chair_seat[8][0][2] + unit);
        glVertex3f(chair_seat[0][0][0] - unit, chair_seat[0][0][1] - panelWidth, chair_seat[0][0][2] + unit);

        //Back
        glVertex3f(chair_seat[0][8][0] - unit, chair_seat[8][8][1], (chair_seat[8][8][2] - unit));
        glVertex3f(chair_seat[0][0][0] - unit, chair_seat[8][0][1], (chair_seat[8][0][2] + unit));
        glVertex3f(chair_seat[0][0][0] - unit, chair_seat[8][0][1] - panelWidth, chair_seat[8][0][2] + unit);
        glVertex3f(chair_seat[0][8][0] - unit, chair_seat[8][8][1] - panelWidth, chair_seat[8][8][2] - unit);

        //Front
        glVertex3f(chair_seat[8][8][0] + unit, chair_seat[8][8][1], (chair_seat[8][8][2] + unit));
        glVertex3f(chair_seat[8][0][0] + unit, chair_seat[8][0][1], (chair_seat[8][0][2] - unit));
        glVertex3f(chair_seat[8][0][0] + unit, chair_seat[8][0][1] - panelWidth, chair_seat[8][0][2] - unit);
        glVertex3f(chair_seat[8][8][0] + unit, chair_seat[8][8][1] - panelWidth, chair_seat[8][8][2] + unit);

        glEnd();
    }

    void drawStand()
    {
        GLfloat unit = 0.03f;
        GLfloat stand_unit = 6.0f;
        glBegin(GL_QUADS);

        //right
        glVertex3f(chair_seat[0][8][0], chair_seat[0][8][1], chair_seat[0][8][2]);
        glVertex3f(chair_seat[0][8][0] + unit, chair_seat[0][8][1], chair_seat[0][8][2]);
        glVertex3f(chair_seat[0][8][0] + unit, chair_seat[0][8][1] - panelWidth*stand_unit, chair_seat[0][8][2]);
        glVertex3f(chair_seat[0][8][0], chair_seat[0][8][1] - panelWidth*stand_unit, chair_seat[0][8][2]);

        //left
        glVertex3f(chair_seat[0][0][0], chair_seat[0][0][1], chair_seat[0][0][2]);
        glVertex3f(chair_seat[0][0][0] + unit, chair_seat[0][0][1], chair_seat[0][0][2]);
        glVertex3f(chair_seat[0][0][0] + unit, chair_seat[0][0][1] - panelWidth*stand_unit, chair_seat[0][0][2]);
        glVertex3f(chair_seat[0][0][0], chair_seat[0][0][1] - panelWidth*stand_unit, chair_seat[0][0][2]);

        //Back
        glVertex3f(chair_seat[0][8][0], chair_seat[8][8][1], (chair_seat[0][8][2]));
        glVertex3f(chair_seat[0][0][0], chair_seat[8][0][1], (chair_seat[0][0][2]));
        glVertex3f(chair_seat[0][0][0], chair_seat[8][0][1] - panelWidth*stand_unit, chair_seat[0][0][2]);
        glVertex3f(chair_seat[0][8][0], chair_seat[8][8][1] - panelWidth*stand_unit, chair_seat[0][8][2]);

        //Front
        glVertex3f(chair_seat[0][8][0] + unit, chair_seat[8][8][1], (chair_seat[0][8][2]));
        glVertex3f(chair_seat[0][0][0] + unit, chair_seat[8][0][1], (chair_seat[0][0][2]));
        glVertex3f(chair_seat[0][0][0] + unit, chair_seat[8][0][1] - panelWidth*stand_unit, chair_seat[0][0][2]);
        glVertex3f(chair_seat[0][8][0] + unit, chair_seat[8][8][1] - panelWidth*stand_unit, chair_seat[0][8][2]);

        glEnd();

        glBegin(GL_QUADS);

        //right
        glVertex3f(chair_seat[8][8][0], chair_seat[0][8][1], chair_seat[0][8][2]);
        glVertex3f(chair_seat[8][8][0] + unit, chair_seat[0][8][1], chair_seat[0][8][2]);
        glVertex3f(chair_seat[8][8][0] + unit, chair_seat[0][8][1] - panelWidth*stand_unit, chair_seat[0][8][2]);
        glVertex3f(chair_seat[8][8][0], chair_seat[0][8][1] - panelWidth*stand_unit, chair_seat[0][8][2]);

        //left
        glVertex3f(chair_seat[8][0][0], chair_seat[0][0][1], chair_seat[0][0][2]);
        glVertex3f(chair_seat[8][0][0] + unit, chair_seat[0][0][1], chair_seat[0][0][2]);
        glVertex3f(chair_seat[8][0][0] + unit, chair_seat[0][0][1] - panelWidth*stand_unit, chair_seat[0][0][2]);
        glVertex3f(chair_seat[8][0][0], chair_seat[0][0][1] - panelWidth*stand_unit, chair_seat[0][0][2]);

        //Back
        glVertex3f(chair_seat[8][8][0], chair_seat[8][8][1], (chair_seat[0][8][2]));
        glVertex3f(chair_seat[8][0][0], chair_seat[8][0][1], (chair_seat[0][0][2]));
        glVertex3f(chair_seat[8][0][0], chair_seat[8][0][1] - panelWidth*stand_unit, chair_seat[0][0][2]);
        glVertex3f(chair_seat[8][8][0], chair_seat[8][8][1] - panelWidth*stand_unit, chair_seat[0][8][2]);

        //Front
        glVertex3f(chair_seat[8][8][0] + unit, chair_seat[8][8][1], (chair_seat[0][8][2]));
        glVertex3f(chair_seat[8][0][0] + unit, chair_seat[8][0][1], (chair_seat[0][0][2]));
        glVertex3f(chair_seat[8][0][0] + unit, chair_seat[8][0][1] - panelWidth*stand_unit, chair_seat[0][0][2]);
        glVertex3f(chair_seat[8][8][0] + unit, chair_seat[8][8][1] - panelWidth*stand_unit, chair_seat[0][8][2]);

        glEnd();

    }

    void InitializeChair()
    {
        nurb1 = gluNewNurbsRenderer();
        nurb2 = gluNewNurbsRenderer();
        gluNurbsProperty(nurb1, GLU_SAMPLING_TOLERANCE, 30.0f);
        gluNurbsProperty(nurb1, GLU_DISPLAY_MODE, GLU_FILL);
        gluNurbsProperty(nurb1, GLU_CULLING, GL_TRUE);
        gluNurbsProperty(nurb2, GLU_SAMPLING_TOLERANCE, 30.0f);
        gluNurbsProperty(nurb2, GLU_DISPLAY_MODE, GLU_FILL);
        gluNurbsProperty(nurb2, GLU_CULLING, GL_TRUE);
        initializeBack();
        initializeSeat();
    }

    void drawAllChairs()
    {
        GLfloat theaterX = 7.0f;
        GLfloat theaterY = 7.0f;
        for (int row = 0; row < rowCount; row++)
        {
            for (int column = 0; column < columnCount; column++)
            {
                if (column == (columnCount - 1) / 2 && (column % 2 == 0))
                {
                    continue;
                }
                glPushMatrix();
                glTranslatef(theaterX * (0.5f - (GLfloat)column / columnCount), -(GLfloat)row * 0.10f, theaterY * (0.5f - (GLfloat)row / rowCount));
                drawSingleChair();
                glPopMatrix();
            }
        }
    }
};

class Camera
{
public:
    std::vector<CameraSequence> cameraSequence;
    int currentSequence = 0;
    int sequenceCount = 0;
    bool sceneFinished = false;
    bool cameraPasue = true;
    float cameraSpeed = 0.0012f;

    Camera()
    {
        cameraSequence.push_back(
            CameraSequence(Vertex(0.0f, -0.5f, 2.5f),
                Vertex(0.0f, -1.0f, -2.0f),
                Vertex(0.0f, 180.0f, 0.0f),
                Vertex(20.0f, 180.0f, 0.0f),
                cameraSpeed, cameraSpeed));

        sequenceCount++;


        cameraSequence.push_back(
            CameraSequence(Vertex(0.0f, -1.0f, -2.0f),
                Vertex(0.0f, -0.6f, -6.0f),
                Vertex(20.0f, 180.0f, 0.0f),
                Vertex(-20.0f, 180.0f, 0.0f),
                cameraSpeed, cameraSpeed));

        sequenceCount++;


        cameraSequence.push_back(
            CameraSequence(Vertex(0.0f, -0.6f, -6.0f),
                Vertex(0.0f, 0.0f, -5.0f),
                Vertex(-20.0f, 180.0f, 0.0f),
                Vertex(20.0f, 0.0f, 0.0f),
                cameraSpeed, cameraSpeed));

        sequenceCount++;


        cameraSequence.push_back(
            CameraSequence(Vertex(0.0f, 0.0f, -5.0f),
                Vertex(0.0f, -0.4f, -3.0f),
                Vertex(20.0f, 0.0f, 0.0f),
                Vertex(0.0f, 0.0f, 0.0f),
                cameraSpeed, cameraSpeed));

        sequenceCount++;

        cameraSequence.push_back(
            CameraSequence(Vertex(0.0f, -0.4f, -3.0f),
                Vertex(0.0f, -0.4f, -2.4f),
                Vertex(0.0f, 0.0f, 0.0f),
                Vertex(-4.0f, 0.0f, 0.0f),
                cameraSpeed  * 0.8f, cameraSpeed * 0.8f));

        sequenceCount++;

        //For Curtain
        sequenceCount++;
    }
};

class NatakPati
{
public:
    Curtain Pati = Curtain(2.5f, 2.5f, 120, 120, 1);
    GLuint textureIndex;
    Vertex StarPos;
    Vertex CurrentPos;
    Vertex finalPos;
    Vertex displacemnt;
    Vertex splitingDirection;
    bool isVisible = false;
    bool dospliting = false;
    float PosIncrementer;
    float prePosDiff;
    bool isMovementComplete = false;
    bool isAnimationComplete = false;
    int splitCounter = 0;
    int movCouner = 0;


    NatakPati(Vertex StarPosition, Vertex finalPosition, Vertex splitingDir, float incrementer, GLuint texture)
    {
        StarPos = StarPosition;
        CurrentPos = StarPosition;
        finalPos = finalPosition;
        splitingDirection = splitingDir;
        PosIncrementer = incrementer;
        textureIndex = texture;
        prePosDiff = (finalPosition - StarPosition).length();
        displacemnt = Vertex((((rand() % 2000) / 2000.0f) - 0.5f),
            (((rand() % 2000) / 2000.0f) - 0.5f),
            (((rand() % 2000) / 2000.0f) - 0.5f));
    }

    void movePati()
    {
        glTranslatef(CurrentPos.cord[0], CurrentPos.cord[1], CurrentPos.cord[2]);
        if (!isMovementComplete)
        {
            prePosDiff = (finalPos - CurrentPos).length();
            CurrentPos = CurrentPos + (finalPos - StarPos)* PosIncrementer;

            float posDiff = (finalPos - CurrentPos).length();
            if (posDiff >= prePosDiff)
            {
                isMovementComplete = true;
                dospliting = true;
            }

            if (movCouner < 30)
            {
           //     Pati.wrapCurtain();
            }
            else
                Pati.showCurtain();

            movCouner++;
              
        }
        if (dospliting)
        {
            if (splitCounter > 200)
            {
                isAnimationComplete = true;
            }
            // Pati.Update();
            glTranslatef(
                displacemnt.cord[0] * splitCounter*0.0001f,
                displacemnt.cord[0] * splitCounter*0.00015f,
                0.0f);
            glRotatef(displacemnt.cord[0] * splitCounter*0.02f, 0.0f, 0.0f, 1.0f);
            Pati.showCurtain();
            splitCounter++;
        }
    }

};

class NatakSequence
{
public:
    float Speed = 0.008f;
    bool doPatiAnimation = false;
    int currentPati = 0;
    std::vector<NatakPati> natakNames;

    NatakSequence()
    {
        natakNames.push_back(NatakPati(
            Vertex(-1.0f, 0.3f, -40.0f),
            Vertex(-1.0f, 1.2f, -6.0f),
            Vertex(0.0f, 0.0f, 0.0f),
            Speed, texture_1_Pratibimba
        ));

        natakNames.push_back(NatakPati(
            Vertex(-3.0f, 0.3f, -40.0f),
            Vertex(-2.5f,  1.2f, -6.0f),
            Vertex(-1.0f, 0.0f, 0.0f),
            Speed, texture_2_Ghashiram
        ));

        natakNames.push_back(NatakPati(
            Vertex(-1.0f, 0.3f, -40.0f),
            Vertex(-1.0f, 1.2f, -6.0f),
            Vertex(0.0f, -1.0f, 0.0f),
            Speed, texture_3_Teen
        ));


        natakNames.push_back(NatakPati(
            Vertex(0.5f, 0.3f, -40.0f),
            Vertex(0.5f, 1.2f, -6.0f),
            Vertex(1.0f, 0.0f, 0.0f),
            Speed, texture_4_itihaas
        ));

        natakNames.push_back(NatakPati(
            Vertex(-2.5f, 0.3f, -40.0f),
            Vertex(-2.5f, 1.2f, -6.0f),
            Vertex(0.0f, 1.0f, 0.0f),
            Speed, texture_5_bot
        ));

        natakNames.push_back(NatakPati(
            Vertex(-1.0f, 0.3f, -40.0f),
            Vertex(-1.0f, 1.2f, -6.0f),
            Vertex(0.0f, -1.0f, 0.0f),
            Speed, texture_6_TheEscape
        ));

        natakNames.push_back(NatakPati(
            Vertex(0.5f, 0.3f, -40.0f),
            Vertex(0.5f, 1.2f, -6.0f),
            Vertex(1.0f, 0.0f, 0.0f),
            Speed, texture_7_chafa
        ));

        natakNames.push_back(NatakPati(
            Vertex(-2.5f, 0.3f, -40.0f),
            Vertex(-2.5f, 1.2f, -6.0f),
            Vertex(-1.0f, 0.0f, 0.0f),
            Speed, texture_8_Sunila
        ));

        natakNames.push_back(NatakPati(
            Vertex(-1.0f, 0.3f, -40.0f),
            Vertex(-1.0f, 1.2f, -6.0f),
            Vertex(0.0f, -1.0f, 0.0f),
            Speed, texture_9_pravah
        ));

        natakNames.push_back(NatakPati(
            Vertex(0.5f, 0.3f, -40.0f),
            Vertex(0.5f, 1.2f, -6.0f),
            Vertex(1.0f, 0.0f, 0.0f),
            Speed, texture_10_AlADin

        ));

        natakNames.push_back(NatakPati(
            Vertex(-2.5f, 0.3f, -40.0f),
            Vertex(-2.5f, 1.2f, -6.0f),
            Vertex(-1.0f, 0.0f, 0.0f),
            Speed, texture_11_Sankraman
        ));


        natakNames.push_back(NatakPati(
            Vertex(-1.0f, 0.3f, -40.0f),
            Vertex(-1.0f, 1.2f, -6.0f),
            Vertex(0.0f, -1.0f, 0.0f),
            Speed, texture_12_TheDeath
        ));
    }
};

class Theater
{
public:
    //Lights
    Lighting lighting;

    //Chairs
    Chairs chairs;

    //Camera
    Camera camera;

    //Natak
    NatakSequence natak;

    float theater_length = 4.5f;
    float theater_height = 2.5f;
    float theater_depth = 8.0f;

    float stage_length = 3.0f;
    float stage_height = 1.5f;
    float stage_depth = 1.5f;

    void ShowThreater()
    {
        //Theater
        glTranslatef(0.0f, 0.0f, 0.0f);


        glBindTexture(GL_TEXTURE_2D, texture_theater_side);
        glBegin(GL_QUADS);
        //Right Face
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(theater_length, theater_height, theater_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(theater_length, theater_height, -theater_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(theater_length, -theater_height, -theater_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(theater_length, -theater_height, theater_depth);

        //Left Face
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-theater_length, theater_height, -theater_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-theater_length, theater_height, theater_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-theater_length, -theater_height, theater_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-theater_length, -theater_height, -theater_depth);
        glEnd();

        // glBindTexture(GL_TEXTURE_2D, texture_theater_back);
        glBegin(GL_QUADS);
        //Back Face
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-theater_length, theater_height, theater_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(theater_length, theater_height, theater_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(theater_length, -theater_height, theater_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-theater_length, -theater_height, theater_depth);
        glEnd();

        glBegin(GL_QUADS);
        //Top face
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(theater_length, theater_height, theater_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-theater_length, theater_height, theater_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-theater_length, theater_height, -theater_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(theater_length, theater_height, -theater_depth);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texture_theater_floor);
        glBegin(GL_QUADS);
        //Bottom face
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(theater_length, -theater_height, -theater_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-theater_length, -theater_height, -theater_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-theater_length, -theater_height, theater_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(theater_length, -theater_height, theater_depth);

        glNormal3f(0.0f, 1.0f, 0.4f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(theater_length, -theater_height, theater_depth*0.1f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-theater_length, -theater_height, theater_depth*0.1f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-theater_length, -theater_height/1.5f, theater_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(theater_length, -theater_height/1.5f, theater_depth);
        glEnd();


    }

    void ShowStage()
    {
        glTranslatef(0.0f, 0.0f, -4.0f);
        glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, texture_black);

        //stage
        glBegin(GL_QUADS);
        //Right Face
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(stage_length, stage_height, stage_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(stage_length, stage_height, -stage_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(stage_length, -stage_height, -stage_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(stage_length, -stage_height, stage_depth);

        //Left Face
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-stage_length, stage_height, -stage_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-stage_length, stage_height, stage_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-stage_length, -stage_height, stage_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-stage_length, -stage_height, -stage_depth);
        glEnd();


        glBegin(GL_QUADS);
        //Top face
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(stage_length, stage_height, stage_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-stage_length, stage_height, stage_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-stage_length, stage_height, -stage_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(stage_length, stage_height, -stage_depth);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texture_stage_floor);
        glBegin(GL_QUADS);

        //Bottom face
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(stage_length, -stage_height, -stage_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-stage_length, -stage_height, -stage_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-stage_length, -stage_height, stage_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(stage_length, -stage_height, stage_depth);
        glEnd();


        //Stage Edges
        glBindTexture(GL_TEXTURE_2D, texture_stage_lower_edge);

        //Left Edge
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 0.6f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-stage_length, theater_height, stage_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-theater_length, theater_height, stage_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-theater_length, -theater_height, stage_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-stage_length, -theater_height, stage_depth);
        glEnd();

        //Right Edge
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 0.6f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(theater_length, theater_height, stage_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(stage_length, theater_height, stage_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(stage_length, -theater_height, stage_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(theater_length, -theater_height, stage_depth);
        glEnd();

        //Bottom Edge
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 0.6f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(stage_length, -stage_height, stage_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-stage_length, -stage_height, stage_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-stage_length, -theater_height, stage_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(stage_length, -theater_height, stage_depth);
        glEnd();
        
        glBindTexture(GL_TEXTURE_2D, texture_stage_upper_edge);
        glBegin(GL_QUADS);
        //Top Edge
        glNormal3f(0.0f, 0.0f, 0.6f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(stage_length, theater_height, stage_depth);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-stage_length, theater_height, stage_depth);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-stage_length, stage_height, stage_depth);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(stage_length, stage_height, stage_depth);
        glEnd();
     
    }
};

Theater theater;

Curtain Curtain1(3.5f, 3.0f, 57, 45, 0);
Curtain Curtain2(3.5f, 3.0f, 57, 45, 0);

void Theater_Initialize()
{
    glEnable(GL_LIGHTING);
    LoadGLTexturesRepeat(&texture_curtain, MAKEINTRESOURCE(IDBITMAP_CURTAIN), false);
    LoadGLTexturesRepeat(&texture_stage_floor, MAKEINTRESOURCE(IDBITMAP_STAGE_FLOOR), false);
    LoadGLTexturesRepeat(&texture_stage_lower_edge, MAKEINTRESOURCE(IDBITMAP_STAGE_LOWER_EDGE), false);
    LoadGLTexturesRepeat(&texture_stage_upper_edge, MAKEINTRESOURCE(IDBITMAP_STAGE_UPPER_EDGE), false);

    LoadGLTexturesRepeat(&texture_chair, MAKEINTRESOURCE(IDBITMAP_CHAIR), false);
    LoadGLTexturesRepeat(&texture_black, MAKEINTRESOURCE(IDBITMAP_BLACK), false);
    LoadGLTexturesRepeat(&texture_theater_floor, MAKEINTRESOURCE(IDBITMAP_THEATER_FLOOR), true);
    LoadGLTexturesRepeat(&texture_theater_side, MAKEINTRESOURCE(IDBITMAP_THEATER_SIDE), true);
    LoadGLTexturesRepeat(&texture_theater_back, MAKEINTRESOURCE(IDBITMAP_THEATER_BACK), true);

    LoadGLTexturesRepeat(&texture_1_Pratibimba, MAKEINTRESOURCE(IDBITMAP_PRATIBIMBA), false);
    LoadGLTexturesRepeat(&texture_2_Ghashiram, MAKEINTRESOURCE(IDBITMAP_GHASHIRAM), false);
    LoadGLTexturesRepeat(&texture_3_Teen, MAKEINTRESOURCE(IDBITMAP_TEEN), false);
    LoadGLTexturesRepeat(&texture_4_itihaas, MAKEINTRESOURCE(IDBITMAP_ITIHAAS), false);
    LoadGLTexturesRepeat(&texture_5_bot, MAKEINTRESOURCE(IDBITMAP_BOT), false);
    LoadGLTexturesRepeat(&texture_6_TheEscape, MAKEINTRESOURCE(IDBITMAP_THEESCAPE), false);
    LoadGLTexturesRepeat(&texture_7_chafa, MAKEINTRESOURCE(IDBITMAP_CHAFA), false);
    LoadGLTexturesRepeat(&texture_8_Sunila, MAKEINTRESOURCE(IDBITMAP_SUNILA), false);
    LoadGLTexturesRepeat(&texture_9_pravah, MAKEINTRESOURCE(IDBITMAP_PRAVAH), false);
    LoadGLTexturesRepeat(&texture_10_AlADin, MAKEINTRESOURCE(IDBITMAP_ALADIN), false);
    LoadGLTexturesRepeat(&texture_11_Sankraman, MAKEINTRESOURCE(IDBITMAP_SANKRAMAN), false);
    LoadGLTexturesRepeat(&texture_12_TheDeath, MAKEINTRESOURCE(IDBITMAP_THEDETH), false);

    glEnable(GL_TEXTURE_2D);

    //To remove afterwords
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    time_t t;
    srand((unsigned)time(&t));

    theater.chairs.InitializeChair();
    Curtain1.makeHooks();
    Curtain2.makeHooks();
    //theater.lighting.disableLighting();
    theater.lighting.enableLighting();
}

void SetLights()
{
    SetCamera();
    
    if (theater.lighting.currentSequence == 0)
    {
        if (theater.lighting.sequence1_Counter > 10)
        {
            theater.lighting.lights[0].enableLight();
            theater.lighting.lights[1].enableLight();
            if (theater.lighting.sequence1_Counter > 30)
            {
                if (announcement)
                {
                    announcement = false;
                }
                theater.lighting.lights[2].enableLight();
                theater.lighting.lights[3].enableLight();
                if (theater.lighting.sequence1_Counter > 60)
                {
                    theater.lighting.lights[4].enableLight();
                    theater.lighting.lights[5].enableLight();
                    if (theater.lighting.sequence1_Counter > 70)
                    {
                        if (FirstNandi)
                        {
                            //PlaySound first nandi
                            PlaySound(MAKEINTRESOURCE(IDBWAVW_NANDI), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
                            FirstNandi = false;
                        }
                        theater.camera.cameraPasue = false;
                    }
                }
            }
        }
        theater.lighting.sequence1_Counter += 0.1;
    }

    if (theater.lighting.currentSequence == 2)
    {
        theater.lighting.LightSequence2();
        theater.lighting.sequence1_Counter = 0.0f;
    }
    if (theater.lighting.currentSequence == 3)
    {
        theater.lighting.LightSequence3();
    }
    if (theater.lighting.currentSequence == 4)
    {
        theater.lighting.LightSequence4();
    }
    if (theater.lighting.currentSequence == 5)
    {
        theater.lighting.LightSequence3();
        if (theater.lighting.sequence1_Counter > 10)
        {
            theater.lighting.lights[4].disableLight();
            if (theater.lighting.sequence1_Counter > 20)
            {
                theater.lighting.lights[3].disableLight();
                if (theater.lighting.sequence1_Counter > 30)
                {
                    if (!stopMusic)
                    {
                        PlaySound(NULL, NULL, NULL);
                        stopMusic = true;
                    }
                    theater.lighting.lights[2].disableLight();
                    if (theater.lighting.sequence1_Counter > 40)
                    {
                        theater.lighting.lights[1].disableLight();
                        if (theater.lighting.sequence1_Counter > 50)
                        {
                            theater.lighting.lights[0].disableLight();
                            if (theater.lighting.sequence1_Counter > 55)
                            {
                                scene_theater = false;
                                theater.lighting.disableLighting();
                            }
                        }
                    }
                }
            }
        }
        theater.lighting.sequence1_Counter += 0.1;
    }
    theater.lighting.updateLighting();
}

int LoadGLTexturesRepeat(GLuint *texture, TCHAR imageResourceId[], bool repeat)
{
    //variable declarations
    HBITMAP hBitmap;
    BITMAP bmp;
    int iStatus = FALSE;

    //code
    glGenTextures(1, texture); //1 image
    hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (hBitmap) //if bitmap exists ( means hBitmap is not null )
    {
        iStatus = TRUE;
        GetObject(hBitmap, sizeof(bmp), &bmp);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //pixel storage mode (word alignment/4 bytes)
        glBindTexture(GL_TEXTURE_2D, *texture); //bind texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        if (repeat)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        //generate mipmapped texture (3 bytes, width, height & data from bmp)
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

        DeleteObject(hBitmap); //delete unwanted bitmap handle
    }
    return(iStatus);
}

void Scene_Theater()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    SetLights();

    SetCamera();
    UpdateCurtain();
    showCurtain();

    SetCamera();
    theater.ShowStage();
    
    SetCamera();
    theater.ShowThreater();
    
    SetCamera();
    showChairs();
    
    SetCamera();
    showNatak();

    SetCamera();
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glPointSize(10);
    glBegin(GL_POINTS);
    
    //for (int index = 0; index < 8; index++)
    //{
    //    if(theater.lighting.lights[index].isLightEnabled)
    //        glVertex3f(theater.lighting.lights[index].position[0],
    //            theater.lighting.lights[index].position[1],
    //            theater.lighting.lights[index].position[2]);
    //}

    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

void showCurtain()
{
    SetCamera();
    glBindTexture(GL_TEXTURE_2D, texture_curtain);

    if (curtainanimatonComplete)
    {
        if (drawCurtain)
        {
            if (!Curtain1.isCurtainDrawn)
            {
                Curtain1.drawCurtain(1, Vertex(-0.0014f, 0.0f, 0.0f), 1);
                Curtain2.drawCurtain(0, Vertex(0.0014f, 0.0f, 0.0f), 1);
            }

            else
                curtainanimatonComplete = false;
        }
        else
        {
            if (!Curtain1.isCurtainUnDrawn)
            {
                Curtain1.drawCurtain(1, Vertex(-0.0014f, 0.0f, 0.0f), -1);
                Curtain2.drawCurtain(0, Vertex(0.0014f, 0.0f, 0.0f), -1);
            }

            else
                curtainanimatonComplete = false;
        }
    }
    glTranslatef(-3.4f, 1.55f, -3.0f);
    Curtain1.showCurtain();
    glTranslatef(3.4f, 0.0f, 0.0f);
    Curtain2.showCurtain();
}

void showChairs()
{
    SetCamera();
    glBindTexture(GL_TEXTURE_2D, texture_chair);
    glTranslatef(-0.25f, -0.7f, 5.0f);
    theater.chairs.drawAllChairs();
}

void SetCamera()
{
    glLoadIdentity();
    glRotatef(xRotation, 1.0f, 0.0f, 0.0f);
    glRotatef(yRotation, 0.0f, 1.0f, 0.0f);
    glRotatef(zRotation, 0.0f, 0.0f, 1.0f);
    glTranslatef(xCamera, -yCamera, zCamera);
}

void UpdateCamera()
{
    if (!theater.camera.sceneFinished)
    {
            if (theater.camera.currentSequence == 5)
            {
                // Curtain Animation
                if (curtainanimatonComplete == false)
                {
                    // Natak Animation
                    theater.camera.currentSequence++;
                    slowDownCurtain = 0.5f;
                    theater.natak.doPatiAnimation = true;
                }

            }
            else if (!theater.camera.cameraSequence[theater.camera.currentSequence].isSequenceFinished)
            {
                if (!theater.camera.cameraPasue)
                {
                    theater.camera.cameraSequence[theater.camera.currentSequence].moveCamera();
                    theater.camera.cameraSequence[theater.camera.currentSequence].rotateCamera();
                    theater.camera.cameraSequence[theater.camera.currentSequence].isMotionFinished();
                }
                xCamera = theater.camera.cameraSequence[theater.camera.currentSequence].currentPosition.cord[0];
                yCamera = theater.camera.cameraSequence[theater.camera.currentSequence].currentPosition.cord[1];
                zCamera = theater.camera.cameraSequence[theater.camera.currentSequence].currentPosition.cord[2];
                xRotation = theater.camera.cameraSequence[theater.camera.currentSequence].currentRotation.cord[0];
                yRotation = theater.camera.cameraSequence[theater.camera.currentSequence].currentRotation.cord[1];
                zRotation = theater.camera.cameraSequence[theater.camera.currentSequence].currentRotation.cord[2];
            }
            else
            {
                theater.camera.currentSequence++;

                if (theater.camera.currentSequence == 5)
                {
                    PlaySound(MAKEINTRESOURCE(IDBWAVW_NANDI2), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
            	    curtainanimatonComplete = true;
                }
                if (theater.camera.currentSequence == 4)
                {
                    // Ghanti
                    PlaySound(MAKEINTRESOURCE(IDBWAVW_GHANTA), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
                }
            }

            if (theater.camera.currentSequence == theater.camera.sequenceCount)
                theater.camera.sceneFinished = true;
    }
}

void UpdateCurtain()
{
    Curtain1.addForce(Vertex(0.0f, -0.2f, 0.0f)*SPEED);
    Curtain1.windForce(Vertex(0.2f, 0.0f, 0.2f)*SPEED*slowDownCurtain);
    Curtain1.Update();

    Curtain2.addForce(Vertex(0.0f, -0.2f, 0.0f)*SPEED);
    Curtain2.windForce(Vertex(0.2f, 0.0f, 0.2f)*SPEED*slowDownCurtain);
    Curtain2.Update();
}

void showNatak()
{
    if (theater.natak.doPatiAnimation)
    {
        if (theater.natak.currentPati < theater.natak.natakNames.size())
        {
            bindNatakTextue(theater.natak.currentPati);
            if ((theater.lighting.currentSequence == 0) || 
                (theater.natak.currentPati + 1) == theater.natak.natakNames.size())
            {
                theater.lighting.currentSequence = 3;
            }
            else
            {
                theater.lighting.currentSequence = ((theater.natak.currentPati-1) % 3) + 2;
            }
            theater.natak.natakNames[theater.natak.currentPati].movePati();
            if (theater.natak.natakNames[theater.natak.currentPati].isMovementComplete)
            {
                if (theater.natak.natakNames[theater.natak.currentPati].isAnimationComplete)
                {
                    theater.natak.currentPati++;
                }
            }
        }
        else
        {
            bindNatakTextue(theater.natak.natakNames.size() - 1);
            theater.natak.natakNames[theater.natak.natakNames.size() - 1].movePati();
            theater.lighting.currentSequence = 5;
        }
    }
}

void bindNatakTextue(int index)
{
    switch (index)
    {
    case 0:
        glBindTexture(GL_TEXTURE_2D, texture_1_Pratibimba);
        break;

    case 1:
        glBindTexture(GL_TEXTURE_2D, texture_2_Ghashiram);
        break;

    case 2:
        glBindTexture(GL_TEXTURE_2D, texture_3_Teen);
        break;

    case 3:
        glBindTexture(GL_TEXTURE_2D, texture_4_itihaas);
        break;

    case 4:
        glBindTexture(GL_TEXTURE_2D, texture_5_bot);
        break;

    case 5:
        glBindTexture(GL_TEXTURE_2D, texture_6_TheEscape);
        break;

    case 6:
        glBindTexture(GL_TEXTURE_2D, texture_7_chafa);
        break;

    case 7:
        glBindTexture(GL_TEXTURE_2D, texture_8_Sunila);
        break;

    case 8:
        glBindTexture(GL_TEXTURE_2D, texture_9_pravah);
        break;

    case 9:
        glBindTexture(GL_TEXTURE_2D, texture_10_AlADin);
        break;

    case 10:
        glBindTexture(GL_TEXTURE_2D, texture_11_Sankraman);
        break;

    case 11:
        glBindTexture(GL_TEXTURE_2D, texture_12_TheDeath);
        break;
    }

}