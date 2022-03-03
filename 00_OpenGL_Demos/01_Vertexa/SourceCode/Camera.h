#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>


class CameraSequence
{
public:
    glm::vec3 initialPosition;
    glm::vec3 currentPosition;
    glm::vec3 finalPosition;
    glm::vec3 initialRotation;
    glm::vec3 currentRotation;
    glm::vec3 finalRotation;
    float PosIncrementer;
    float RotIncrementer;
    float prePosDiff;
    float PreRotDiff;
    bool isSequenceFinished;

    CameraSequence(glm::vec3 cPos, glm::vec3 fPos, glm::vec3 cRot, glm::vec3 fRot, float pIncr, float rIncer)
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

        prePosDiff = glm::length(finalPosition - initialPosition) + 0.00001;
        PreRotDiff = glm::length(finalRotation - initialRotation) + 0.00001;
    }

    void moveCamera()
    {
        if (!isSequenceFinished)
        {
            prePosDiff = (float)glm::length(finalPosition - currentPosition);
            currentPosition = currentPosition + (finalPosition - initialPosition)* PosIncrementer;
        }
    }

    void rotateCamera()
    {
        if (!isSequenceFinished)
        {
            PreRotDiff = (float)glm::length(finalRotation - currentRotation);
            currentRotation = currentRotation + (finalRotation - initialRotation)* RotIncrementer;
        }
    }


    bool isMotionFinished()
    {
        float posDiff = (glm::length(finalPosition - currentPosition));
        float rotDiff = (glm::length(finalRotation - currentRotation));

        if ((posDiff >= prePosDiff)
            && (rotDiff >= PreRotDiff))
        {
            isSequenceFinished = true;
            return true;
        }
        else
            return false;
    }

    void resetCameraSequence()
    {
        isSequenceFinished = false;
        prePosDiff = glm::length(finalPosition - initialPosition) + 0.0001;
        PreRotDiff = glm::length(finalRotation - initialRotation) + 0.0001;
        currentPosition = initialPosition;
        currentRotation = initialRotation;
    }
};

class CameraScene
{
public:
    std::vector<CameraSequence> cameraSequence;
    int currentSequence = 0;
    bool sceneFinished = true;
    bool cameraPasue = false;

    void update()
    {
        if (!sceneFinished)
        {
            if (!cameraSequence[currentSequence].isMotionFinished())
            {
                if (!cameraPasue)
                {
                    cameraSequence[currentSequence].moveCamera();
                    cameraSequence[currentSequence].rotateCamera();
                }
            }
            else
            {
                if (currentSequence < (cameraSequence.size()-1))
                {
                    currentSequence++;
                }
                else
                {
                    sceneFinished = true;
                }
            }
        }   
    }

    void startScene()
    {
        sceneFinished = false;
        currentSequence = 0;
        cameraPasue = false;

        for (std::vector<CameraSequence>::iterator itr = cameraSequence.begin(); itr != cameraSequence.end(); itr++)
        {
            itr->resetCameraSequence();
        }
    }

    glm::vec3 getCurrentPosition()
    {
        return cameraSequence[currentSequence].currentPosition;
    }

    glm::vec3 getCurrentRotation()
    {
        return cameraSequence[currentSequence].currentRotation;
    }
};

class Camera
{
public:
    std::vector<CameraScene> cameraScenes;
    int currentScene = 0;
    glm::mat4 modelMatrix;
    

    Camera()
    {

        //Insert Empty Scene  = 0
        CameraScene emptyScene;

        float cameraSpeed1 = 0.0018f;
        float cameraSpeed2 = 0.0018f;

        emptyScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(0.0f, 0.0f, 90.0f),
                glm::vec3(0.0f, 0.0f, 90.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                cameraSpeed1, cameraSpeed1));

        cameraScenes.push_back(emptyScene);

        //----------------------------------------
        //Insert Opening Scene  = 1
        CameraScene openingScene;

        cameraSpeed1 = 0.0005f;
        cameraSpeed2 = 0.0003f;

        openingScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(0.0f, 0.0f, 95.0f),
                glm::vec3(0.0f, 0.0f, 55.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                cameraSpeed1, cameraSpeed2));

        openingScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(0.0f, 0.0f, 55.0f),
                glm::vec3(0.0f, 5.0f, 50.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                cameraSpeed1, cameraSpeed2));

        openingScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(0.0f, 5.0f, 50.0f),
                glm::vec3(0.0f, 5.0f, 47.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                cameraSpeed1, cameraSpeed2));


        cameraScenes.push_back(openingScene);

        //----------------------------------------
        //Insert idle Scene  = 2
        CameraScene idleScene;

        cameraSpeed1 = 0.0018f;
        cameraSpeed2 = 0.0018f;


        idleScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(0.8f, 4.2f, 13.0f),
                glm::vec3(0.8f, 4.2f, 13.0f),
                glm::vec3(0.0f, -2.0f, 0.0f),
                glm::vec3(0.0f, -2.0f, 0.0f),
                cameraSpeed1, cameraSpeed2));

        cameraScenes.push_back(idleScene);

        //---------------------------------------------
        //Insert Walk Scene   = 3
        float cameraWalkPositionSpeed = 0.002f;
        float cameraWalkRotationSpeed = 0.004f;
        CameraScene walkScene;

        walkScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(0.0f, 3.5f, 9.0f),
                glm::vec3(0.0f, 5.0f, 70.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(-1.0f, 0.0f, 0.0f),
                cameraWalkPositionSpeed, cameraWalkRotationSpeed));


        cameraScenes.push_back(walkScene);
        //-----------------------------------------
        //Insert AeroPlane Scene  = 4
        CameraScene aeroPlaneScene;

        float cameraAeroplanePositionSpeed = 0.0025f;
        float cameraAeroplaneRotationSpeed = 0.01f;


        aeroPlaneScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(-5.0f, 0.0f, 40.0f),
                glm::vec3(-5.0f, 0.0f, 30.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                cameraAeroplanePositionSpeed, cameraAeroplaneRotationSpeed));

        aeroPlaneScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(-5.0f, 0.0f, 30.0f),
                glm::vec3(-2.0f, 0.0f, 40.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                cameraAeroplanePositionSpeed, cameraAeroplaneRotationSpeed));

        cameraScenes.push_back(aeroPlaneScene);

        //-----------------------------------------
        //Insert Indian Dance Scene   = 5

        CameraScene IndianDanceScene;

        float cameraIndianDancePositionSpeed = 0.0007f;
        float cameraIndianDanceRotationSpeed = 0.005f;

        IndianDanceScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(0.0f, 0.0f, 9.0f),
                glm::vec3(0.0f, 0.0f, 30.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 00.0f, 0.0f),
                cameraIndianDancePositionSpeed, cameraIndianDanceRotationSpeed));

        IndianDanceScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(0.0f, 0.0f, 30.0f),
                glm::vec3(-8.0f, 0.0f, 30.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                cameraIndianDancePositionSpeed, cameraIndianDanceRotationSpeed));

        IndianDanceScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(-8.0f, 0.0f, 30.0f),
                glm::vec3(-3.0f, 0.0f, 30.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                cameraIndianDancePositionSpeed, cameraIndianDanceRotationSpeed));

        cameraScenes.push_back(IndianDanceScene);

        //------------------------------------
        //Insert MOONWALK

        CameraScene moonwalkScene;

        float moonwalkPositionSpeed = 0.001f;
        float moonwalkRotationSpeed = 0.005f;

        moonwalkScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(-5.0f, 0.0f, 20.0f),
                glm::vec3(8.0f, 0.0f, 20.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 00.0f, 0.0f),
                moonwalkPositionSpeed, moonwalkPositionSpeed));

        cameraScenes.push_back(moonwalkScene);

        //------------------------------------
        //Insert Hello

        CameraScene helloScene;

        float helloPositionSpeed = 0.001f;
        float helloRotationSpeed = 0.005f;

        helloScene.cameraSequence.push_back(
            CameraSequence(glm::vec3(0.0f, 2.0f, 22.0f),
                glm::vec3(0.0f, 2.0f, 19.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                helloPositionSpeed, helloRotationSpeed));

        cameraScenes.push_back(helloScene);
    }

    glm::mat4 getViewMatrix()
    {
        //TBD
        //Calculate ...

        //For Time Being

        //Not Y and Z rotation axis are switched. Not updated yet

        glm::mat4 viewMatrix = glm::mat4();
        viewMatrix = glm::translate(viewMatrix, -cameraScenes[currentScene].getCurrentPosition()); 
        glm::mat4 rotateMatrix = glm::rotate(glm::mat4(), glm::radians(cameraScenes[currentScene].getCurrentRotation().x), glm::vec3(-1.0f, 0.0f, 0.0f));
        rotateMatrix = glm::rotate(rotateMatrix, glm::radians(cameraScenes[currentScene].getCurrentRotation().y), glm::vec3(0.0f, -1.0f, 0.0f));
        rotateMatrix = glm::rotate(rotateMatrix, glm::radians(cameraScenes[currentScene].getCurrentRotation().z), glm::vec3(0.0f, 0.0f, -1.0f));

        //	viewMatrix = glm::lookAt(cameraSequence[0].currentPosition, cameraSequence[0].currentRotation, glm::vec3(1.0f, 1.0f, 1.0f));
        // return viewMatrix;
        return (viewMatrix * rotateMatrix);

    }

    void update()
    {
        if (!cameraScenes[currentScene].sceneFinished)
        {
            cameraScenes[currentScene].update();
        }
    }

    void start()
    {
        cameraScenes[currentScene].startScene();
    }
};

