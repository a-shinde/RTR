#include "CubeMap.h"

extern std::ofstream g_log_file;

enum
{
    ATTRIBUTE_VERTEX = 0,
    ATTRIBUTE_COLOR,
    ATTRIBUTE_NORMAL,
    ATTRIBUTE_TEXTURE0,
};

GLuint gTexture_sampler_uniform;
GLuint gTexture_CubeMap;

GLfloat gAangle = 0.0f;

GLuint gCubeMapVertexShaderObject;
GLuint gCubeMapFragmentShaderObject;
GLuint gCubeMapShaderProgramObject;

GLuint gVaoCubeMap;
GLuint gVboCubeMap;
GLuint gMVPUniformCubeMap;

//*************

int LoadGLTexturesCubeMapBmp(GLuint *texture, char *textureNames[])
{
    HBITMAP hBitmap[6];
    BITMAP bmp;
    int iStatus = FALSE;

    glGenTextures(1, texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *texture);

    for (unsigned int i = 0; i < 6; i++)
    {
        //wchar_t str=textureNames[i];
        size_t length = strlen(textureNames[i]) + 1;
        wchar_t wtext[200];
        mbstowcs(wtext, textureNames[i], length);
        LPWSTR p_texture = wtext;
        memset((void*)&bmp, sizeof(bmp), 0);
        hBitmap[i] = (HBITMAP)LoadImage(GetModuleHandle(NULL), p_texture, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
        //fprintf(gpFile, "%s\n", textureNames[i].c);
        if (hBitmap[i])
        {
            iStatus = TRUE;
            GetObject(hBitmap[i], sizeof(bmp), &bmp);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                bmp.bmWidth,
                bmp.bmHeight,
                0,
                GL_BGR,
                GL_UNSIGNED_BYTE,
                bmp.bmBits);

        }
        else
        {
            return FALSE;
        }
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //pixel storage mode (word alignment/4 bytes)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //create mipmap for this texture for better image quality
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    DeleteObject(hBitmap); //delete unwanted bitmap handle

    return TRUE;
}

void initializeCubeMap() {

    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char* szInfoLog = NULL;
  
    //*****************Cubemap*******************************

    //*** Vertex Shader ***
    // Create Shader
    gCubeMapVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    //Provide source code to shader
    const GLchar *CubeMapvertexShaderSourceCode =
        "#version 430 core" \
        "\n" \
        "in vec3 vPosition;" \
        "out vec3 TexCoords;" \
        "uniform mat4 u_mvp_matrix;" \
        "void main(void)" \
        "{"\
        "TexCoords = vPosition;" \
        "gl_Position  = u_mvp_matrix * vec4(vPosition, 1.0);" \
        "}";
    glShaderSource(gCubeMapVertexShaderObject, 1, (const GLchar **)&CubeMapvertexShaderSourceCode, NULL);

    // compile shader
    glCompileShader(gCubeMapVertexShaderObject);
    iInfoLogLength = 0;
    GLint iShaderCompliedStatus = 0;
    szInfoLog = NULL;
    glGetShaderiv(gCubeMapVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompliedStatus);
    if (iShaderCompliedStatus == GL_FALSE)
    {
        char *szInfoLog = NULL;
        glGetShaderiv(gCubeMapVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gCubeMapVertexShaderObject, iInfoLogLength, &written, szInfoLog);
                g_log_file << "Vertex Shader Compilation Log :" << szInfoLog << std::endl;
                free(szInfoLog);
                exit(0);
            }
        }
    }


    //*** Fragment Shader ***
    // Create Shader
    gCubeMapFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    //Provide source code to shader
    const GLchar *CubeMapfragmentShaderSourceCode =
        "#version 430 core" \
        "\n" \
        "out vec4 FragColor;" \
        "in vec3 TexCoords;" \
        "uniform samplerCube u_texture0_sampler;" \
        "void main(void)" \
        "{" \
        "FragColor = texture(u_texture0_sampler, TexCoords);" \
        "}";
    glShaderSource(gCubeMapFragmentShaderObject, 1, (const GLchar **)&CubeMapfragmentShaderSourceCode, NULL);

    // compile shader
    glCompileShader(gCubeMapFragmentShaderObject);
    glGetShaderiv(gCubeMapFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompliedStatus);
    if (iShaderCompliedStatus == GL_FALSE)
    {
        char *szInfoLog = NULL;
        glGetShaderiv(gCubeMapFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gCubeMapFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
                g_log_file << "Fragment Shader Compilation Log :" << szInfoLog << std::endl;
                free(szInfoLog);
                exit(0);
            }
        }
    }

    //Shader program
    //create
    gCubeMapShaderProgramObject = glCreateProgram();

    //attach vertex shader to shader program
    glAttachShader(gCubeMapShaderProgramObject, gCubeMapVertexShaderObject);

    //attach fragment shader to shader program
    glAttachShader(gCubeMapShaderProgramObject, gCubeMapFragmentShaderObject);

    //Pre-link binding of shader program object with vertex shader position attribute
    glBindAttribLocation(gCubeMapShaderProgramObject, ATTRIBUTE_VERTEX, "vPosition");

    //Link shader
    glLinkProgram(gCubeMapShaderProgramObject);
    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(gCubeMapShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE)
    {
        char *szInfoLog = NULL;
        glGetProgramiv(gCubeMapShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(gCubeMapShaderProgramObject, iInfoLogLength, &written, szInfoLog);
                g_log_file << "Shader Program Link Log :" << szInfoLog << std::endl;
                free(szInfoLog);
                exit(0);
            }
        }
    }

    //get MVP uniform location
    gMVPUniformCubeMap = glGetUniformLocation(gCubeMapShaderProgramObject, "u_mvp_matrix");
    gTexture_sampler_uniform = glGetUniformLocation(gCubeMapShaderProgramObject, "u_texture0_sampler");

    // *** vertices, Colors, Shader Attribs, vbo, vao initializations

    float cubeMapSize = 100.0f;
    const GLfloat cubeMapVertices[] =
    {
        -cubeMapSize,  cubeMapSize, -cubeMapSize,
        -cubeMapSize, -cubeMapSize, -cubeMapSize,
        cubeMapSize, -cubeMapSize, -cubeMapSize,
        cubeMapSize, -cubeMapSize, -cubeMapSize,
        cubeMapSize,  cubeMapSize, -cubeMapSize,
        -cubeMapSize,  cubeMapSize, -cubeMapSize,

        -cubeMapSize, -cubeMapSize,  cubeMapSize,
        -cubeMapSize, -cubeMapSize, -cubeMapSize,
        -cubeMapSize,  cubeMapSize, -cubeMapSize,
        -cubeMapSize,  cubeMapSize, -cubeMapSize,
        -cubeMapSize,  cubeMapSize,  cubeMapSize,
        -cubeMapSize, -cubeMapSize,  cubeMapSize,

        cubeMapSize, -cubeMapSize, -cubeMapSize,
        cubeMapSize, -cubeMapSize,  cubeMapSize,
        cubeMapSize,  cubeMapSize,  cubeMapSize,
        cubeMapSize,  cubeMapSize,  cubeMapSize,
        cubeMapSize,  cubeMapSize, -cubeMapSize,
        cubeMapSize, -cubeMapSize, -cubeMapSize,

        -cubeMapSize, -cubeMapSize,  cubeMapSize,
        -cubeMapSize,  cubeMapSize,  cubeMapSize,
        cubeMapSize,  cubeMapSize,  cubeMapSize,
        cubeMapSize,  cubeMapSize,  cubeMapSize,
        cubeMapSize, -cubeMapSize,  cubeMapSize,
        -cubeMapSize, -cubeMapSize,  cubeMapSize,

        -cubeMapSize,  cubeMapSize, -cubeMapSize,
        cubeMapSize,  cubeMapSize, -cubeMapSize,
        cubeMapSize,  cubeMapSize,  cubeMapSize,
        cubeMapSize,  cubeMapSize,  cubeMapSize,
        -cubeMapSize,  cubeMapSize,  cubeMapSize,
        -cubeMapSize,  cubeMapSize, -cubeMapSize,

        -cubeMapSize, -cubeMapSize, -cubeMapSize,
        -cubeMapSize, -cubeMapSize,  cubeMapSize,
        cubeMapSize, -cubeMapSize, -cubeMapSize,
        cubeMapSize, -cubeMapSize, -cubeMapSize,
        -cubeMapSize, -cubeMapSize,  cubeMapSize,
        cubeMapSize, -cubeMapSize,  cubeMapSize
    };

    glGenVertexArrays(1, &gVaoCubeMap);  // recording of casatte
    glBindVertexArray(gVaoCubeMap);

    glGenBuffers(1, &gVboCubeMap);
    glBindBuffer(GL_ARRAY_BUFFER, gVboCubeMap);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeMapVertices), cubeMapVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ATTRIBUTE_VERTEX);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // passing zero is unbinding
    glBindVertexArray(0);

  

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //White Color

                                          //For bmp image file
    //char* cubeMapTextures_ice[] = {
    //    "res/texture/cubeMap/ice/posx.bmp",
    //    "res/texture/cubeMap/ice/negx.bmp",
    //    "res/texture/cubeMap/ice/posy.bmp",
    //    "res/texture/cubeMap/ice/negy.bmp",
    //    "res/texture/cubeMap/ice/posz.bmp",
    //    "res/texture/cubeMap/ice/negz.bmp"
    //};

    //char* cubeMapTextures_garden[] = {
    //    "Garden_posx.bmp",
    //    "Garden_negx.bmp",
    //    "Garden_posy.bmp",
    //    "Garden_negy.bmp",
    //    "Garden_posz.bmp",
    //    "Garden_negz.bmp"
    //};
    char* cubeMapTextures_cloudtop2[] = {
        "res/texture/cubeMap/cloudtop2/cloudtop_rt.bmp",
        "res/texture/cubeMap/cloudtop2/cloudtop_lf.bmp",
        "res/texture/cubeMap/cloudtop2/cloudtop_up.bmp",
        "res/texture/cubeMap/cloudtop2/cloudtop_dn.bmp",
        "res/texture/cubeMap/cloudtop2/cloudtop_bk.bmp",
        "res/texture/cubeMap/cloudtop2/cloudtop_ft.bmp",
    };

    LoadGLTexturesCubeMapBmp(&gTexture_CubeMap, cubeMapTextures_cloudtop2);

    //LoadGLTexturesCubeMapBmp(&gTexture_CubeMap, cubeMapTextures_ice);

    //	LoadGLTexturesCubeMapBmp(&gTexture_CubeMap, cubeMapTextures_garden);

    
}

void displayCubeMap(glm::mat4 viewMatrix, glm::mat4 PerspectiveProjectionMatrix, int cubeMapIndex)
{
   
    //======================= Cubemap drawing ==============

    glUseProgram(gCubeMapShaderProgramObject);

    //Set model view and modelviewprojection matrices to identity
    glm::mat4 modelViewMatrix = glm::mat4();
    glm::mat4 modelViewProjectionMatrix = glm::mat4();
    glm::mat4 RotationMatrix = glm::mat4();

    //Translate model view matrix
    modelViewMatrix = glm::translate(modelViewMatrix, glm::vec3(0.0f, 0.0f, 0.0f));

    RotationMatrix = RotationMatrix * glm::rotate(RotationMatrix, glm::radians(gAangle), glm::vec3(0.0f, 1.0f, 0.0f));

    modelViewMatrix = modelViewMatrix * RotationMatrix;

    //multiple the modelview and perspective matrix to get modelview projection matrix
    modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix * viewMatrix; // ORDER IS IMPORTANT

                                                                                //pass above modelViewProjectionMatrix to the vertexshader in u_mvp_matrix shader variable
                                                                                // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
    glUniformMatrix4fv(gMVPUniformCubeMap, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

    //glDepthMask(GL_FALSE);

    //*** bind vao ***
    glBindVertexArray(gVaoCubeMap);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gTexture_CubeMap);
    glUniform1i(gTexture_sampler_uniform, 0);

    // *** draw eigther by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLES, 0, 36); // 3 (each with its x y z) vertices in triangleVertices array

                                       //glDepthMask(GL_TRUE);

                                       //*** unbind vao ***
    glBindVertexArray(0);
    //====================================

    //Stop using OpenGL program object
    glUseProgram(0);
}
