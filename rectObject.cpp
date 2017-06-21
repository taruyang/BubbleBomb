#include <glm/gtc/matrix_transform.hpp>
#include "rectObject.h"
#include "logging.h"

/// Set up vertex data (and buffer(s)) and attribute pointers
static GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,  2.0f, 0.0f,
     1.0f,  1.0f, 0.0f,  2.0f, 2.0f,
    -1.0f,  1.0f, 0.0f,  0.0f, 2.0f
};

static GLuint indices[] = {
    0, 1, 2,        /// back
    0, 2, 3
};

namespace gl
{

RectObject::RectObject(Shader* pShader, const char* texturePath) : TriangleObject(pShader, texturePath)
{
    _vertices = vertices;
    _indices = indices;
    _sizeVertices = sizeof(vertices);
    _sizeIndices = sizeof(indices);
    _objType = Object_Rect;
}

RectObject::RectObject(Shader* pShader, glm::vec3 objectColor) : TriangleObject(pShader, objectColor)
{
    _vertices = vertices;
    _indices = indices;
    _sizeVertices = sizeof(vertices);
    _sizeIndices = sizeof(indices);
    _objType = Object_Rect;
}

RectObject::~RectObject() {
}

bool RectObject::DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv)
{
    if(SetTextureToShader() == false)
        return false;

    glm::mat4 proj = glm::ortho(0.0f, studioEnv.ScreenSize.x, 0.0f, studioEnv.ScreenSize.y);

    glm::mat4 R(1.0f);

    glm::mat4 modelMat = _T* R *_S;
    glm::mat4 PVM = proj * modelMat;

    glm::vec4 objColor(_objectColor, 0.3f);

    glUniformMatrix4fv(glGetUniformLocation (_pShader->GetProgram(), "PVM"), 1, GL_FALSE,  &PVM[0][0]);
    glUniform4fv(glGetUniformLocation (_pShader->GetProgram(), "objColor"), 1,  &objColor.x);

    DrawContainer();

    return true;
}

}   /// namespace gl
