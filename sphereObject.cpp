#include <cstdlib>
#include "sphereObject.h"
#include "logging.h"

/// Set up vertex data (and buffer(s)) and attribute pointers
static GLfloat vertices[] = {
     0.000f,  0.000f,  1.000f, 0.5f, 0.75f,
     0.894f,  0.000f,  0.447f, 0.75f, 0.65f,
     0.276f,  0.851f,  0.447f, 0.6f, 0.65f,
    -0.724f,  0.526f,  0.447f, 0.3f, 0.6f,

    -0.724f, -0.526f,  0.447f, 0.15f, 0.6f,
     0.276f, -0.851f,  0.447f, 0.6f, 0.55f,
     0.724f,  0.526f, -0.447f, 0.7f, 0.35f,
    -0.276f,  0.851f, -0.447f, 0.4f, 0.35f,

    -0.894f,  0.000f, -0.447f, 0.25f, 0.25f,
    -0.276f, -0.851f, -0.447f, 0.0f, 0.0f,
     0.724f, -0.526f, -0.447f, 1.0f, 0.0f,
     0.000f,  0.000f, -1.000f, 0.5f, 0.25f
};

/// element buffer
static GLuint indices[] = {
    2, 1, 0,
    3, 2, 0,
    4, 3, 0,
    5, 4, 0,
    1, 5, 0,

    11, 6, 7,
    11, 7, 8,
    11, 8, 9,
    11, 9, 10,
    11, 10, 6,

    1, 2, 6,
    2, 3, 7,
    3, 4, 8,
    4, 5, 9,
    5, 1, 10,

    2,  7, 6,
    3,  8, 7,
    4,  9, 8,
    5, 10, 9,
    1, 6, 10
};

namespace gl
{

static int gSeedNum = 0;

void SphereObject::GetUniformLocations()
{
    /// The location of uniform variables related with light calculation
    _lightPosLoc = glGetUniformLocation(_pShader->GetProgram(), "lightPos");
    _viewPosLoc = glGetUniformLocation(_pShader->GetProgram(), "viewPos");
    _lightColorLoc = glGetUniformLocation(_pShader->GetProgram(), "lightColor");
    _objectColorLoc = glGetUniformLocation(_pShader->GetProgram(), "objectColor");

    /// The location of uniform variables related with PVM calculation
    _PVLoc = glGetUniformLocation(_pShader->GetProgram(), "PV");
    _MLoc = glGetUniformLocation(_pShader->GetProgram(), "M");
    _normalMatLoc = glGetUniformLocation(_pShader->GetProgram(), "normalMat");
}

SphereObject::SphereObject(Shader* pShader, const char* texturePath) : TriangleObject(pShader, texturePath)
{
    _vertices = vertices; _sizeVertices = sizeof(vertices);
    _indices = indices; _sizeIndices = sizeof(indices);
    _objType = Object_Sphere;
}

SphereObject::SphereObject(Shader* pShader, glm::vec3 objectColor) : TriangleObject(pShader, objectColor)
{
    _vertices = vertices; _sizeVertices = sizeof(vertices);
    _indices = indices; _sizeIndices = sizeof(indices);
    _focusedTime = 0.f;
    _startTime = 0.f;
    _seedNum = gSeedNum++;
    _objType = Object_Sphere;
}

SphereObject::~SphereObject() {
}

bool SphereObject::DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv)
{
    if(SetTextureToShader() == false)
        return false;

    GetUniformLocations();

    if(_isFirstRendering == true) {
        Reset(studioEnv);
        _isFirstRendering = false;
    }

    if(_lightPosLoc > -1) glUniform3fv(_lightPosLoc, 1, &studioEnv.LightPos.x);
    if(_viewPosLoc > -1) glUniform3fv(_viewPosLoc, 1, &studioEnv.ViewPos.x);
    if(_lightColorLoc > -1) glUniform3fv(_lightColorLoc, 1, &studioEnv.LightSpecular.x);
    if(_objectColorLoc > -1) {
            if(_isFocused)
            {
                glUniform3fv(_objectColorLoc, 1, &_focusColor.x);
                if(_focusedTime == 0.f) _focusedTime = time;
            }
            else
                glUniform3fv(_objectColorLoc, 1, &_objectColor.x);
    }

    /// Point light
    for(int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        string str("pointLights[");
        str += (char)('0' + i);
        str += "].position";
        glUniform3fv(glGetUniformLocation(_pShader->GetProgram(), str.c_str()), 1, &studioEnv.PointLightPos[i].x);
    }

    glm::mat4 T(1.0f);
    _movement += _movingDistance;

    T[3][0] = _curPos.x = _orgPos.x + _movement.x;
    T[3][1] = _curPos.y = _orgPos.y + _movement.y;
    T[3][2] = _curPos.z = _orgPos.z + _movement.z;

    ///printf("time (%.3f), z (%.3f), rsin (%.3f) \n", time, T[3][2], rsin);
    glm::mat4 R(1.0f);
    _modelMat = T* R *_S;

    if(_PVLoc > -1) glUniformMatrix4fv(_PVLoc, 1, GL_FALSE,  &PV[0][0]);
    if(_MLoc > -1) glUniformMatrix4fv(_MLoc, 1, GL_FALSE,  &_modelMat[0][0]);
    if(_normalMatLoc > -1) glUniformMatrix3fv(_normalMatLoc, 1, GL_FALSE,  &_normalMat[0][0]);

    DrawContainer();

    glm::vec3 objDir = _curPos - studioEnv.ViewPos;
    float dotDir = glm::dot(objDir, studioEnv.Front);
    ///printf("dorDir %f \n", dotDir);
    /// Reset to start position (z = 0)
    if((dotDir < 0.f) || (_focusedTime > 0.f && (time - _focusedTime > FOCUSEDTIME))
       || (time - _startTime > 10.f))
        Reset(studioEnv);

    return true;
}

void SphereObject::Reset(StudioEnv& studioEnv)
{
    _orgPos = studioEnv.PlayerPos;
    _movingDistance = decideMovingDistance(studioEnv);
    _movement = glm::vec3(0.f);
    _focusedTime = 0.f;
    _startTime = glfwGetTime();
    _isFocused = false;
    _objectColor = decideObjectColor(studioEnv);
    _seedNum = gSeedNum++;
}

glm::vec3 SphereObject::decideMovingDistance(StudioEnv& studioEnv)
{
    glm::vec3 movement = glm::normalize(studioEnv.ViewPos - _orgPos) * DEFAULT_MOVING_DISTANCE * float(studioEnv.GameStage);

    srand(_seedNum);
    float mov = (float)(rand() % 1000);
    /// choose the next position.
    movement *= (sin(glm::radians(mov)) + 1.50f);

    return movement;
}

glm::vec3 SphereObject::decideObjectColor(StudioEnv& studioEnv)
{
    glm::vec3   color(0.f);

    srand(_seedNum);
    float mov = (float)(rand() % 1000);

    /// choose the object color.
    color.x = glm::max(sin(glm::radians(mov)), 0.f);
    color.y = glm::max(cos(glm::radians(mov)), 0.f);
    color.z = glm::min(glm::max(tan(glm::radians(mov)), 0.f), 1.0f);

    return color;
}

void SphereObject::DrawContainer()
{
    /// Draw container
    glBindVertexArray(_vao);
    glDrawElements(GL_PATCHES, _sizeIndices / sizeof(GLuint), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if(_texturePath)
        glBindTexture(GL_TEXTURE_2D, 0);
}

bool SphereObject::IsIntersected(glm::vec3 org, glm::vec3 dir, float *distance)
{
    glm::vec3 center(_modelMat[3][0], _modelMat[3][1], _modelMat[3][2]);
    float radius = _S[0][0];

	/// work out components of quadratic
	glm::vec3 objDir = center - org;
	float objDist = glm::length(objDir);

	/// calculate estimated casted position with this object
    glm::vec3 castedPos = org + objDist * dir;

    float dist = glm::length(center - castedPos);

	*distance = objDist;

	printf("[%s] distance %f radius %f \n", __FUNCTION__, *distance, radius);
	if(dist <= radius)
        return true;

    return false;
}

}   /// namespace gl
