#include "stageObject.h"
#include "logging.h"

/// Set up vertex data (and buffer(s)) and attribute pointers
static GLfloat vertices[] = {
    -100.0f, -1.0f, -100.0f,  0.0f, 0.0f,
    -100.0f, -1.0f,  100.0f,  0.0f, 1.0f,
     100.0f, -1.0f,  100.0f,  1.0f, 1.0f,
     100.0f, -1.0f, -100.0f,  1.0f, 0.0f
};

static GLuint indices[] = {
    0, 1, 2,        /// back
    0, 2, 3
};

namespace gl
{
StageObject::StageObject(Shader* pShader, const char* texturePath) : TriangleObject(pShader, texturePath)
{
    _vertices = vertices;
    _indices = indices;
    _sizeVertices = sizeof(vertices);
    _sizeIndices = sizeof(indices);
    _objType = Object_Stage;

}

StageObject::~StageObject() {
}

bool StageObject::DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv)
{
    if(SetTextureToShader() == false)
        return false;

    glm::mat4 R(1.0f);

    glm::mat4 modelMat = _T* R *_S;
    glm::mat4 PVM = PV * modelMat;

    GLint PVMLoc = glGetUniformLocation (_pShader->GetProgram(), "PVM");
	if(PVMLoc <= -1)
    {
        LogError("Failed to get Uniform PVM \n");
        return false;
	}

    glUniformMatrix4fv(PVMLoc, 1, GL_FALSE,  &PVM[0][0]);

    DrawContainer();

    return true;
}

}   /// namespace gl
