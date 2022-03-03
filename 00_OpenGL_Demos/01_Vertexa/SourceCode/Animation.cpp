#include <fstream>
#include "Animation.h"
#include "Shaders.h"


extern std::ofstream g_log_file;
extern GLuint winWidth, winHeight;
extern GLfloat xRot, yRot, zRot;
extern GLfloat xTrans, yTrans, zTrans;

Animation::Animation()
{
  
}


Animation::~Animation()
{
	glDeleteProgram(shader.shaderProgramObject);
}


void Animation::init()
{

	// shader for animated model
	shader.MakeProgramObject("shaders/VertexShader.vert", "shaders/FragmentShader.frag");

    //Load Animation Data from Assimp

	model_trinity_animation.initShaders(shader.shaderProgramObject);
	model_trinity_animation.loadModel("models//model//model.dae");
    model_trinity_animation.loadVisemes("models//model//animations//viseme.dae");
    ////model_trinity_animation.loadSecondModel("models//model//dressCode//model.dae");
    vector<string> animationNames;
    animationNames.push_back("models//model//animations//Expression.dae");
    animationNames.push_back("models//model//animations//OpeningScene.dae");
    animationNames.push_back("models//model//animations//idle.dae");
    animationNames.push_back("models//model//animations//walk.dae");
    animationNames.push_back("models//model//animations//Aeroplane.dae");
    animationNames.push_back("models//model//animations//IndianDance.dae");
    animationNames.push_back("models//model//animations//Moonwalk.dae");
    animationNames.push_back("models//model//animations//Hello.dae");
    model_trinity_animation.loadAnimations(animationNames);

    model_dressCode.initShaders(shader.shaderProgramObject);
    model_dressCode.loadModel("models//model//dressCode//model.dae");
    model_dressCode.loadVisemes("models//model//animations//viseme.dae");
    model_dressCode.loadAnimations(animationNames);



    //Load Model From our Parser
    /*model_trinity_geometries.loadModel("models/model/model.dae");
    model_trinity_geometries.initShaders(shader.shaderProgramObject);
    model_trinity_geometries.setBoneInfo(model_trinity_animation.getBoneMapping());
    model_trinity_geometries.getGeometryData();*/

    //Initialize CubeMap
    initializeCubeMap();

    //Lighting
    for (uint index = 0; index < animationSettings.lighting.maxLight; index++)
    {
        string name = "Lights[" + to_string(index) + "].isEnabled";
        animationSettings.lighting.lights[index].u_isLightEnabled = glGetUniformLocation(shader.shaderProgramObject, name.c_str());

        name = "Lights[" + to_string(index) + "].position";
        animationSettings.lighting.lights[index].u_position = glGetUniformLocation(shader.shaderProgramObject, name.c_str());

        name = "Lights[" + to_string(index) + "].ambient";
        animationSettings.lighting.lights[index].u_ambient = glGetUniformLocation(shader.shaderProgramObject, name.c_str());

        name = "Lights[" + to_string(index) + "].diffuse";
        animationSettings.lighting.lights[index].u_diffuse = glGetUniformLocation(shader.shaderProgramObject, name.c_str());

        name = "Lights[" + to_string(index) + "].specular";
        animationSettings.lighting.lights[index].u_specular = glGetUniformLocation(shader.shaderProgramObject, name.c_str());

        name = "Lights[" + to_string(index) + "].constant";
        animationSettings.lighting.lights[index].u_constant_attenuation = glGetUniformLocation(shader.shaderProgramObject, name.c_str());

        name = "Lights[" + to_string(index) + "].linear";
        animationSettings.lighting.lights[index].u_linear_attenuation = glGetUniformLocation(shader.shaderProgramObject, name.c_str());

        name = "Lights[" + to_string(index) + "].quadratic";
        animationSettings.lighting.lights[index].u_quadratic_attenuation = glGetUniformLocation(shader.shaderProgramObject, name.c_str());
    }

    animationSettings.lighting.initialize_sequence1();

    model_matrix_uniform = glGetUniformLocation(shader.shaderProgramObject, "u_model_matrix");
    view_matrix_uniform = glGetUniformLocation(shader.shaderProgramObject, "u_view_matrix");
    projection_uniform = glGetUniformLocation(shader.shaderProgramObject, "u_projection_matrix");


	modelMatrix = glm::mat4();

}

void Animation::update()
{
	current_time = (GLfloat)GetTickCount();
	delta_time = (current_time - last_time);
	last_time = current_time;
    	
	viewMatrix = camera.getViewMatrix();

    if (animationSettings.modelIndex == 1)
        model_dressCode.update();
    else
        model_trinity_animation.update();

	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(xTrans, yTrans, zTrans));

	modelMatrix = glm::rotate(modelMatrix, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(zRot), glm::vec3(0.0f, 0.0f, 1.0f));

    //Custom correction for Animations
    switch (animationSettings.action.actionSequence)
    {
    case ACTION_MOONWALK:
        modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        break;

    case ACTION_AEROPLANE:
        modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        break;

    case ACTION_INDIAN_DANCE:
        modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        break;

    default:
        break;
    }

    camera.update();
}

void Animation::render()
{
   	glUseProgram(shader.shaderProgramObject);

    glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(perspectiveProjectionMatrix));

    
    if (animationSettings.modelIndex == 1)
        model_dressCode.applyModelBoneTransform(animationSettings);
    else
        model_trinity_animation.applyModelBoneTransform(animationSettings);

    //glUniformMatrix4fv(model_trinity_geometries.morphMatrixUniform, 1, GL_FALSE, glm::value_ptr(animationSettings.lipSync.getVisemeMatrix()));
    setLigting();
    if (camera.currentScene != 0)
    {
        if (animationSettings.modelIndex == 1)
            model_dressCode.display(shader.shaderProgramObject);
        else
            model_trinity_animation.display(shader.shaderProgramObject);
    }
    //model_trinity_geometries.Modeldisplay(shader.shaderProgramObject);
     
    glUseProgram(0);

    displayCubeMap(viewMatrix, perspectiveProjectionMatrix, animationSettings.environment.cubeMapIndex);

}

void Animation::playSound()
{
	
}

void Animation::setLigting()
{
    for (uint index = 0; index < animationSettings.lighting.maxLight; index++)
    {
        glUniform1i(animationSettings.lighting.lights[index].u_isLightEnabled, animationSettings.lighting.lights[index].isLightEnabled);
        glUniform3fv(animationSettings.lighting.lights[index].u_position, 1, animationSettings.lighting.lights[index].position);
        glUniform3fv(animationSettings.lighting.lights[index].u_ambient, 1, animationSettings.lighting.lights[index].ambient);
        glUniform3fv(animationSettings.lighting.lights[index].u_diffuse, 1, animationSettings.lighting.lights[index].diffuse);
        glUniform3fv(animationSettings.lighting.lights[index].u_specular, 1, animationSettings.lighting.lights[index].specular);

        glUniform1f(animationSettings.lighting.lights[index].u_constant_attenuation, animationSettings.lighting.lights[index].constant_attenuation);
        glUniform1f(animationSettings.lighting.lights[index].u_linear_attenuation, animationSettings.lighting.lights[index].linear_attenuation);
        glUniform1f(animationSettings.lighting.lights[index].u_quadratic_attenuation, animationSettings.lighting.lights[index].quadratic_attenuation);
    }
}

GLuint Animation::LoadGLTexturesPNG(GLuint *texture, const char *filePath) 
{
    int iStatus = FALSE;

    FIBITMAP* imagen = NULL;
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(filePath,0);
	imagen = FreeImage_Load(formato, filePath);

    /*
    size_t length = strlen(filePath) + 1;
    wchar_t wtext[200];
    mbstowcs(wtext, filePath, length);
    LPWSTR p_fileParh = wtext;
    hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), p_fileParh, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    */

    if (imagen)
    {
        FIBITMAP* temp = imagen;
        imagen = FreeImage_ConvertTo32Bits(imagen);
        FreeImage_Unload(temp);

        int w = FreeImage_GetWidth(imagen);
        int h = FreeImage_GetHeight(imagen);
        
        char* pixels = (char*)FreeImage_GetBits(imagen);
        
       // In case if Image is invered.
        /* for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                invPixels[((((h - 1) - i)*w) + j) * 4 + 0] = pixeles[((i*w) + j) * 4 + 0];
                invPixels[((((h - 1) - i)*w) + j) * 4 + 1] = pixeles[((i*w) + j) * 4 + 1];
                invPixels[((((h - 1) - i)*w) + j) * 4 + 2] = pixeles[((i*w) + j) * 4 + 2];
                invPixels[((((h - 1) - i)*w) + j) * 4 + 3] = pixeles[((i*w) + j) * 4 + 3];
            }
        }
        */
        iStatus = TRUE;
        glGenTextures(1, texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);//rgba
        glBindTexture(GL_TEXTURE_2D, *texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGBA,
            w,
            h,
            0,
            GL_BGRA,
            GL_UNSIGNED_BYTE,
            pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

    }
    else 
    {
        g_log_file << "failed to load texture" << std::endl;
    }
    return iStatus;
}