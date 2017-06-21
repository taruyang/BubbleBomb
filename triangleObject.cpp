#include <math.h>
#include <glm/glm.hpp>
#include "triangleObject.h"
#include "logging.h"
#include "SOIL.h"

/// Set up vertex data (and buffer(s)) and attribute pointers
static GLfloat vertices[] = {
    /// Positions         /// Texture Coords
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, /// Lower-left corner
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, /// Lower-right corner
     0.0f,  0.5f, 0.0f,   0.5f, 1.0f  /// Top-center corner
};

static GLuint indices[] = {
    0, 1, 2,
};

namespace gl
{

TriangleObject::TriangleObject(Shader* pShader, const char* texturePath)
{
    _vertices = vertices;
    _indices = indices;
    _sizeVertices = sizeof(vertices);
    _sizeIndices = sizeof(indices);
    _T = glm::mat4(1);
    _S = glm::mat4(1);

    _pShader = pShader;
    _texturePath = texturePath;

    _objectColor = glm::vec3(0);
    _isFocused = false;
    _focusColor = glm::vec3(0);
    _objType = Object_Triangle;
}

TriangleObject::TriangleObject(Shader* pShader, glm::vec3 objectColor) : TriangleObject(pShader, nullptr)
{
    _objectColor = objectColor;
}

TriangleObject::~TriangleObject()
{
    glDeleteVertexArrays(1, &_vao);
    if(_texturePath) glDeleteTextures(1, &_tex);
    glDeleteBuffers(1, &_ebo);
	glDeleteBuffers(1, &_vbo);
}

bool TriangleObject::Initialize()
{
    if(CreateVAO() == false)
    {
        LogError("CreateVAO is failed. \n");
        return false;
    }

    if(CreateTexture() == false)
    {
        LogError("CreateTexture is failed. \n");
        return false;
    }

    return true;
}

bool TriangleObject::CreateTexture()
{
    if(_texturePath == nullptr)
        return true;

    glGenTextures(1, &_tex);

    /// All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    glBindTexture(GL_TEXTURE_2D, _tex);

    /// Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	/// Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /// Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /// Load, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image(_texturePath, &width, &height, 0, SOIL_LOAD_RGBA);
    if(image == NULL)
    {
        LogError("SOIL_load_image fail \n");
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    /// Unbind texture when done, so we won't accidentally mess up our texture.
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

bool TriangleObject::CreateVAO()
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _sizeVertices, _vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _sizeIndices, _indices, GL_STATIC_DRAW);

    /// Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5* sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    /// TexCoord attribute
    if(_texturePath)
    {
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    /// Unbind VAO
    glBindVertexArray(0);

    return true;
}

void TriangleObject::GetCurObjectInfo(struct ObjectInfo& objInfo)
{
    objInfo.CurPos = _curPos;
    objInfo.ObjColor = _objectColor;
    objInfo.ObjType = _objType;
    objInfo.ObjRadius = _S[0][0];
}

bool TriangleObject::Transform(glm::vec3 s, glm::vec3 t)
{

    _T = glm::mat4 ( 1.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, 1.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 1.0f, 0.0f,
                 t.x,  t.y,  t.z,  1.0f);

    _S = glm::mat4 ( s.x,  0.0f, 0.0f, 0.0f,
                 0.0f, s.y,  0.0f, 0.0f,
                 0.0f, 0.0f, s.z,  0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 R(1.0f);
    glm::mat4 modelMat = _T* R *_S;
    _normalMat = glm::transpose(glm::inverse(glm::mat3(modelMat)));
    _orgPos = t;
    _curPos = t;

    return true;
}

bool TriangleObject::SetTextureToShader()
{
    /// Activate shader
    _pShader->Use();

    if(_texturePath == nullptr)
        return true;

    /// Bind Textures using texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tex);

    GLint imgLoc = glGetUniformLocation (_pShader->GetProgram(), "TexImg");
	if(imgLoc <= -1)
    {
        LogError("Failed to get Uniform TexImg \n");
        return false;
	}

    glUniform1i(imgLoc, 0);

    return true;
}

bool TriangleObject::DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv)
{
    if(SetTextureToShader() == false)
        return false;

    GLfloat rcos = cos(glm::radians(time * 50.));
    GLfloat rsin = sin(glm::radians(time * 50.));

    glm::mat4 R(rcos, 0.0f, -rsin, 0.0f,
                0.0f, 1.0f,  0.0f, 0.0f,
                rsin, 0.0f,  rcos, 0.0f,
                0.0f, 0.0f,  0.0f, 1.0f);

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

void TriangleObject::DrawContainer()
{
    /// Draw container
    glBindVertexArray(_vao);

    glDrawElements(GL_TRIANGLES, _sizeIndices / sizeof(GLuint), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    if(_texturePath)
        glBindTexture(GL_TEXTURE_2D, 0);
}


} /// namespace gl
