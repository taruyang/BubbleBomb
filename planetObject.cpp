#include <stddef.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "planetObject.h"

namespace gl
{

PlanetObject::PlanetObject(Shader* pShader, vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) :
    MeshObject(pShader, vertices, indices, textures)
{
    _modelMatrices = nullptr;
}

PlanetObject::~PlanetObject()
{
    if(_modelMatrices)
        delete[] _modelMatrices;

}

bool PlanetObject::generateModelMatrix()
{
    /// Generate a large list of semi-random model transformation matrices
    _modelMatrices = new glm::mat4[_amount];
    srand(glfwGetTime()); // initialize random seed
    GLfloat radius = 50.0f;
    GLfloat offset = 30.0f;
    GLuint halfAmount = _amount / 2;
    GLuint index = 0;
    for(GLuint i = 0; i < halfAmount; i++)
    {
        glm::mat4 model;
        /// 1. Translation: Randomly displace along circle with radius 'radius' in range [-offset, offset]
        GLfloat angle = (GLfloat)i / (GLfloat)halfAmount * 360.0f;
        GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat x = sin(angle) * radius + displacement;
        displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat y = -2.5f + displacement * 0.4f; // Keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        /// 2. Scale: Scale between 0.05 and 0.25f
        GLfloat scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        /// 3. Rotation: add random rotation around a (semi)randomly picked rotation axis vector
        GLfloat rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        /// 4. Now add to list of matrices
        _modelMatrices[index++] = model;
    }

    for(GLuint i = 0; i < halfAmount; i++)
    {
        glm::mat4 model;
        /// 1. Translation: Randomly displace along circle with radius 'radius' in range [-offset, offset]
        GLfloat angle = (GLfloat)i / (GLfloat)halfAmount * 360.0f;
        GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat y = sin(angle) * radius + displacement;
        displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat z = -2.5f + displacement * 0.4f; // Keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat x = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        /// 2. Scale: Scale between 0.05 and 0.25f
        GLfloat scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        /// 3. Rotation: add random rotation around a (semi)randomly picked rotation axis vector
        GLfloat rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        /// 4. Now add to list of matrices
        _modelMatrices[index++] = model;
    }

    return true;
}

bool PlanetObject::generateInstanceAttribute()
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, _amount * sizeof(glm::mat4), &_modelMatrices[0], GL_STATIC_DRAW);

    glBindVertexArray(_vao);
    /// Set attribute pointers for matrix (4 times vec4)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);

    return true;
}

bool PlanetObject::Initialize()
{
    if(MeshObject::Initialize())
    {
        generateModelMatrix();
        generateInstanceAttribute();
    }

    return true;
}

/**
 * @brief   Get current information
 * @return objInfo    Current information
 */
void PlanetObject::GetCurObjectInfo(struct ObjectInfo& objInfo)
{
    MeshObject::GetCurObjectInfo(objInfo);
    objInfo.ObjType = Object_Planet;
}

bool PlanetObject::DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv)
{
    if(SetTextureToShader() == false)
        return false;

    const GLuint program = _pShader->GetProgram();

    /// lighting
    glUniform1f(glGetUniformLocation(program, "textureShininess"), 32.0f);

    glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, &studioEnv.LightPos.x);
    glUniform3fv(glGetUniformLocation(program, "lightAmbient"), 1, &studioEnv.LightAmbient.x);
    glUniform3fv(glGetUniformLocation(program, "lightDiffuse"), 1, &studioEnv.LightDiffuse.x);
    glUniform3fv(glGetUniformLocation(program, "lightSpecular"), 1, &studioEnv.LightSpecular.x);

    /// Point light
    for(int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        string str("pointLights[");
        str += (char)('0' + i);
        str += "].position";
        glUniform3fv(glGetUniformLocation(program, str.c_str()), 1, &studioEnv.PointLightPos[i].x);
    }

    _curPos.y = _T[3][0]; _curPos.y = _T[3][1]; _curPos.z = _T[3][2];

    /// viewer
    glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, &studioEnv.ViewPos.x);

    glm::mat4 R(1.0f);
    _modelMat = _T* R *_S;
    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(_modelMat)));

    glUniformMatrix4fv(glGetUniformLocation(program, "PV"), 1, GL_FALSE,  &PV[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program, "M"), 1, GL_FALSE,  &_modelMat[0][0]);
    glUniformMatrix3fv(glGetUniformLocation(program, "normalMat"), 1, GL_FALSE,  &normalMat[0][0]);

    DrawContainer();

    return true;
}

void PlanetObject::DrawContainer()
{
    /// Draw mesh
    glBindVertexArray(_vao);
    glDrawElementsInstanced(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0, _amount);
    glBindVertexArray(0);

    for (GLuint i = 0; i < _textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

}
