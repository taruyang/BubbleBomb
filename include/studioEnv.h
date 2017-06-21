#ifndef STUDIO_ENV_H_INCLUDED
#define STUDIO_ENV_H_INCLUDED

#include <glm/glm.hpp>

#define NUM_POINT_LIGHTS    6

struct StudioEnv
{
    glm::vec3   LightPos;
    glm::vec3   LightAmbient;
    glm::vec3   LightDiffuse;
    glm::vec3   LightSpecular;
    glm::vec3   ViewPos;
    glm::vec3   PlayerPos;
    glm::vec2   ScreenSize;
    glm::vec3   Front;
    glm::vec3   PointLightPos[NUM_POINT_LIGHTS] = {
        glm::vec3( 20.f,  0.f,  0.f),
        glm::vec3(-20.f,  0.f,  0.f),
        glm::vec3(  0.f, 20.f,  0.f),
        glm::vec3(  0.f,-20.f,  0.0f),
        glm::vec3(  0.f,  0.f, 20.0f),
        glm::vec3(  0.f,  0.f,-20.0f),
    };

    int         GameStage;
};

#endif // STUDIO_ENV_H_INCLUDED
