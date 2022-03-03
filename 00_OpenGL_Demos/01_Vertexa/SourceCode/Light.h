#pragma once
#include <gl\glew.h>

class Light
{
public:
    GLenum lightIndex;

    bool isLightEnabled;
    GLuint u_isLightEnabled;

    GLfloat ambient[4];
    GLuint u_ambient;
    GLfloat diffuse[4];
    GLuint u_diffuse;
    GLfloat specular[4];
    GLuint u_specular;
    GLfloat position[4];
    GLuint u_position;

    bool is_attenuation_on = false;
    GLuint u_is_attenuation_on;
    GLfloat constant_attenuation;
    GLuint u_constant_attenuation;
    GLfloat linear_attenuation;
    GLuint u_linear_attenuation;
    GLfloat quadratic_attenuation;
    GLuint u_quadratic_attenuation;

    bool is_spot_light = false;
    GLfloat spot_direction[3];
    GLfloat spot_cutOff;
    GLfloat spot_exponent;

    Light()
    {
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
            if (is_attenuation_on)
            {

            }

            if (is_spot_light)
            {

            }
        }
        else
        {

        }
    }
};

class Lighting
{
public:
    Light lights[10];
    int maxLight;
    bool doLightAnimation = false;
    int currentSequence = 0;
    float sequence1_Counter = 0;
    float sequence2_Counter = 0;
    float sequence3_Counter = 0;
    float sequence4_Counter = 0;

    void updateLighting()
    {
        maxLight = 10;
        for (int index = 0; index < 8; index++)
        {
            lights[index].updateLight();
        }
    }

    Lighting()
    {
        initialize_sequence1();
        updateLighting();
    }

    void initialize_sequence1()
    {
       
        // First Light
        lights[0].lightIndex = 0;
        lights[0].isLightEnabled = true;
        lights[0].setPosition(0.0f, 0.0f, 3.0f);
       
        lights[0].setAmbient(0.1f, 0.1f, 0.1f, 1.0f);
        lights[0].setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
        lights[0].setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        lights[0].is_attenuation_on = true;
        lights[0].constant_attenuation = 1.0f;
        lights[0].linear_attenuation = 0.007f;
        lights[0].quadratic_attenuation = 0.0002f;

        lights[0].is_spot_light = false;
        
        // Second Light
        lights[1].isLightEnabled = false;
        
    }
};