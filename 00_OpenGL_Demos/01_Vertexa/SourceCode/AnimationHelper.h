#pragma once
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Light.h"

using namespace std;

// Types of Human emotions that our Model can express
enum
{
    EXPRESSION_NEUTRAL,
    EXPRESSION_ANGREE,
    EXPRESSION_DISGUST,
    EXPRESSION_DOUBT,
    EXPRESSION_FEAR,
    EXPRESSION_SAD,
    EXPRESSION_SLEEPING,
    EXPRESSION_SMILE,
    EXPRESSION_SURPRISE,
};

struct EXPRESSION
{
    unsigned int emotion;
    float factor;

    EXPRESSION()
    {
        emotion = 0;
        factor = 0.0f;
    }
};

struct ENVIRONMENT
{
    unsigned int maxCubeMaps;
    unsigned int cubeMapIndex;

    ENVIRONMENT()
    {
        cubeMapIndex = 0;
        maxCubeMaps = 1;
    }
};


/* For Our Parser
enum
{
    VISEME_AH,
    VISEME_OO,
    VISEME_O,
    VISEME_EE,
    VISEME_TH,
    VISEME_G,
    VISEME_MBP,
    VISEME_S,
    VISEME_FV,
    VISEME_R
};*/

enum
{
    VISEME_P,
    VISEME_T,
    VISEME_S,
    VISEME_F,
    VISEME_R,
    VISEME_O,
    VISEME_U,
    VISEME_At,
    VISEME_A,
    VISEME_E,
    VISEME_K,
    VISEME_I,
};

struct VISEME
{
    float fadeInTime;
    float peakTime;
    float fadeOutTime;
    unsigned int value;
    bool isFirstViseme;
    bool isLastViseme;

    VISEME()
    {
        fadeInTime = 0.0f;
        fadeOutTime = 0.0f;
        peakTime = 0.0f;
        value = 0;
        isFirstViseme = false;
        isLastViseme = false;
    }

    float getVisemeWeight(float currentTime)
    {
        float factor = 0.0f;

        if (fadeInTime <= currentTime  && currentTime <= fadeOutTime)
        {
            if (currentTime >= peakTime)
            {
                float fadeOutDuration = fadeOutTime - peakTime;
                if (fadeOutDuration != 0)
                {
                    float lapsedDuration = fadeOutTime - currentTime;
                    factor = lapsedDuration / fadeOutDuration;
                }
            }
            else
            {
                float fadeInDuration = peakTime - fadeInTime;
                if (fadeInDuration != 0)
                {
                    float lapsedDuration = currentTime - fadeInTime;
                    factor = lapsedDuration / fadeInDuration;
                }
            }
        }
        
        return factor;
    }
};

struct LIP_SYNC_INPUT
{
    float visemeTime;
    string visemeValue;

    LIP_SYNC_INPUT()
    {
        visemeTime = 0.0f;
        visemeValue = "";
    }

    LIP_SYNC_INPUT(float time, string value)
    {
        visemeTime = time;
        visemeValue = value;
    }
};

struct LIP_SYNC
{
    float sentanceDuration;
    float sentanceStartTime;
    vector<VISEME> visemes;
    glm::mat4 visemeMatrix;
    bool isFinished;

    LIP_SYNC()
    {
        sentanceDuration = 0.0f;
        int currrent_viseme = 0;
        sentanceStartTime = 0.0f;
        isFinished = true;
    }

    bool setVisemes(vector<LIP_SYNC_INPUT> input)
    {
        if (!isCurrentPlaybackFinished())
        {
            return false;
        }

        visemes.clear();
        float currFadeInTime = 0.0;
        for (vector<LIP_SYNC_INPUT>::iterator itr = input.begin(); itr != input.end(); itr++)
        {
            VISEME currViseme;
            currViseme.peakTime = itr->visemeTime;
            
            //if (itr->visemeValue == "a" || itr->visemeValue == "@")
            //    currViseme.value = VISEME_AH;
            //if (itr->visemeValue == "O") currViseme.value = VISEME_OO;
            //if (itr->visemeValue == "u") currViseme.value = VISEME_O;
            //if (itr->visemeValue == "i" || itr->visemeValue == "e" || itr->visemeValue == "E")
            //    currViseme.value = VISEME_EE;
            //if (itr->visemeValue == "T" || itr->visemeValue == "t")
            //    currViseme.value = VISEME_TH;
            //if (itr->visemeValue == "k") currViseme.value = VISEME_G;
            //if (itr->visemeValue == "p") currViseme.value = VISEME_MBP;
            //if (itr->visemeValue == "S" || itr->visemeValue == "s")
            //    currViseme.value = VISEME_S;
            //if (itr->visemeValue == "f") currViseme.value = VISEME_FV;
            //if (itr->visemeValue == "r") currViseme.value = VISEME_R;

            if (itr->visemeValue == "p" || itr->visemeValue == "sil")
                    currViseme.value = VISEME_P;

            if (itr->visemeValue == "T" || itr->visemeValue == "t")
                currViseme.value = VISEME_T;

            if (itr->visemeValue == "S" || itr->visemeValue == "s")
                currViseme.value = VISEME_S;
            if (itr->visemeValue == "f") currViseme.value = VISEME_F;
            if (itr->visemeValue == "r") currViseme.value = VISEME_R;
            if (itr->visemeValue == "O") currViseme.value = VISEME_O;
            if (itr->visemeValue == "u") currViseme.value = VISEME_U;
            if (itr->visemeValue == "@") currViseme.value = VISEME_A;
            //if (itr->visemeValue == "@") currViseme.value = VISEME_At;
            if (itr->visemeValue == "a") currViseme.value = VISEME_A;
            if (itr->visemeValue == "e") currViseme.value = VISEME_E;
            if (itr->visemeValue == "k") currViseme.value = VISEME_K;
            if (itr->visemeValue == "i") currViseme.value = VISEME_I;

            currViseme.fadeInTime = currFadeInTime;
            currFadeInTime = itr->visemeTime;
            if ((itr + 1) != input.end())
                currViseme.fadeOutTime = (itr + 1)->visemeTime;

            visemes.push_back(currViseme);
        }

        if (!visemes.empty())
        {
            visemes[0].isFirstViseme = true;
            visemes[(visemes.size() - 1)].isLastViseme = true;
            visemes[(visemes.size() - 1)].fadeOutTime = visemes[(visemes.size() - 1)].peakTime + 100;
            sentanceDuration = visemes[(visemes.size() - 1)].peakTime;
        }
        return true;
    }

    //-----------------------------------------
    // Use this method when you deal with Morph Targets
    glm::mat4 getVisemeMatrix()
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                visemeMatrix[i][j] = 0.0f;

        for(vector<VISEME>::iterator itr = visemes.begin(); itr != visemes.end(); itr++)
        {
            visemeMatrix[(itr->value)/4][(itr->value)%4] += itr->getVisemeWeight((float)GetTickCount() - sentanceStartTime);
        }
        return visemeMatrix;
    }

    //-----------------------------------------
    // For Animating Visemes using Bones
    void getVisemeIndexAndValue( int &firstVisemeIndex, int &secondVisemeIndex, float &secondVisemeWeight)
    {
        GLfloat currentTime = (GLfloat)GetTickCount();
        firstVisemeIndex = 0;
        secondVisemeIndex = 0;
        bool isSecondVisemeIndexSet = false;
        for (int i = 0; i < visemes.size(); i++)
        {
            if (visemes[i].getVisemeWeight(currentTime - sentanceStartTime) != 0)
            {
                if (!isSecondVisemeIndexSet)
                {
                    secondVisemeIndex = visemes[i].value;
                    secondVisemeWeight = visemes[i].getVisemeWeight(currentTime - sentanceStartTime);
                    isSecondVisemeIndexSet = true;
                }
                else
                {
                    firstVisemeIndex = visemes[i].value;
                    break;
                }
            }
        }
    }

    bool isCurrentPlaybackFinished()
    {
        if ((GLfloat)GetTickCount() > (sentanceStartTime + sentanceDuration))
        {
            isFinished = true;
            return true;
        }
        else
            return false;
    }

    bool init(vector<LIP_SYNC_INPUT> input)
    {
        if (isCurrentPlaybackFinished())
        {
            setVisemes(input);
            sentanceStartTime = (GLfloat)GetTickCount();
            isFinished = false;
            return true;
        }
        else
            return false;
    }
};

// Types of actions that our Model can do
enum
{
    ACTION_OPENING_SCENE = 1,
    ACTION_IDEL_POSITION,
    ACTION_WALK,
    ACTION_AEROPLANE,
    ACTION_INDIAN_DANCE,
    ACTION_MOONWALK,
    ACTION_HELLO,
    ACTION_NONE,
};

struct ACTION
{
    unsigned int actionSequence;
    float actionStartTime;

    void startActionPlayback(float currentTime)
    {
        actionStartTime = currentTime;
    }

    ACTION()
    {
        actionSequence = ACTION_NONE;
    }
};


struct FACE_RANDOM_EXPRESSION
{
    float eyeLidCloseInterval;
    float eyeBrowUpInterval;

    FACE_RANDOM_EXPRESSION()
    {
        eyeLidCloseInterval = 4.0f;
        eyeBrowUpInterval = 7.0f;
    }
};

//-------------------------------------
struct ANIMATION_SETTINGS
{
    ACTION action;
    EXPRESSION expression;
    ENVIRONMENT environment;
    LIP_SYNC lipSync;
    FACE_RANDOM_EXPRESSION faceRandomExpressions;
    Lighting lighting;
    unsigned int modelIndex;

    bool startLipSyncPlayback(vector<LIP_SYNC_INPUT> input)
    {
        if (lipSync.isCurrentPlaybackFinished())
        {
            LIP_SYNC newLipSync;
            lipSync = newLipSync;
            lipSync.init(input);
            return true;
        }
        else
            return false;
    }

    ANIMATION_SETTINGS()
    {
        modelIndex = 0;
    }
};
