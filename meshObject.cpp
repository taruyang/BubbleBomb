#include <stddef.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "meshObject.h"

namespace gl
{

MeshObject::MeshObject(Shader* pShader, vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
{
    _vertices = vertices;
    _indices = indices;
    _textures = textures;

    _T = glm::mat4(1);
    _S = glm::mat4(1);

    _pShader = pShader;

    _objectColor = glm::vec3(0);
    _focusColor = glm::vec3(0);
    _isFocused = false;
}

MeshObject::~MeshObject()
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_ebo);
	glDeleteBuffers(1, &_vbo);
}

bool MeshObject::Initialize()
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), &_indices[0], GL_STATIC_DRAW);

    /// Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

    /// Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

    /// TexCoords attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);

    return true;
}

bool MeshObject::Transform(glm::vec3 s, glm::vec3 t)
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
    _modelMat = _T* R *_S;
    _normalMat = glm::transpose(glm::inverse(glm::mat3(_modelMat)));

    return true;
}

bool MeshObject::SetTextureToShader()
{
    /// Activate shader
    _pShader->Use();

    GLuint diffuseCnt = 1;
    GLuint specularCnt = 1;

    char uniformName[100];

    for(GLuint i = 0; i < _textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        GLuint cnt = (_textures[i].type == Texture_Diffuse) ? diffuseCnt++ : specularCnt++;
        const char* name = (_textures[i].type == Texture_Diffuse) ? "textureDiffuse" : "textureSpecular";

        sprintf(uniformName, "%s%d", name, cnt);

        glUniform1i(glGetUniformLocation(_pShader->GetProgram(), uniformName), i);

        glBindTexture(GL_TEXTURE_2D, _textures[i].object);
    }

    glUniform1f(glGetUniformLocation(_pShader->GetProgram(), "textureShininess"), 32.0f);

    return true;
}

/**
 * @brief   Get current information
 * @return objInfo    Current information
 */
void MeshObject::GetCurObjectInfo(struct ObjectInfo& objInfo)
{
    objInfo.CurPos = _curPos;
    objInfo.ObjColor = _objectColor;
    objInfo.ObjType = Object_Model;
    objInfo.ObjRadius = _S[0][0];
}

bool MeshObject::DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv)
{
    if(SetTextureToShader() == false)
        return false;

    const GLuint program = _pShader->GetProgram();

    /// lighting
    glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, &studioEnv.LightPos.x);
    glUniform3fv(glGetUniformLocation(program, "lightAmbient"), 1, &studioEnv.LightAmbient.x);

    if(_isFocused) {
        glUniform3fv(glGetUniformLocation(program, "lightDiffuse"), 1, &_focusColor.x);
        glUniform3fv(glGetUniformLocation(program, "lightSpecular"), 1, &_focusColor.x);
        if(time - _focusedTime > FOCUSEDTIME)
            _isFocused = false;
    }
    else {
        glUniform3fv(glGetUniformLocation(program, "lightDiffuse"), 1, &studioEnv.LightDiffuse.x);
        glUniform3fv(glGetUniformLocation(program, "lightSpecular"), 1, &studioEnv.LightSpecular.x);
    }

    /// Point light
    for(int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        string str("pointLights[");
        str += (char)('0' + i);
        str += "].position";
        glUniform3fv(glGetUniformLocation(program, str.c_str()), 1, &studioEnv.PointLightPos[i].x);
    }

    /// viewer
    glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, &studioEnv.ViewPos.x);

    /// matrix
    GLfloat rcos = cos(glm::radians(time * 50.));
    GLfloat rsin = sin(glm::radians(time * 50.));

    glm::mat4 R(rcos, 0.0f, -rsin, 0.0f,
                0.0f, 1.0f,  0.0f, 0.0f,
                rsin, 0.0f,  rcos, 0.0f,
                0.0f, 0.0f,  0.0f, 1.0f);

    /// The movement will be faster whenever the game stage increases.
    float speed = time * 30. * studioEnv.GameStage;
    float addtionalMovement = 10.f * sin(glm::radians(time));
    float rtan = glm::min(tan(glm::radians(speed)), 100.f);

    if(studioEnv.GameStage % 3 == 1) {
        _T[3][0] = _curPos.x = sin(glm::radians(speed)) * rtan * addtionalMovement + addtionalMovement;
        _T[3][1] = _curPos.y = cos(glm::radians(speed)) * rtan * addtionalMovement + addtionalMovement;
    }
    else if(studioEnv.GameStage % 3 == 2) {
        _T[3][0] = _curPos.x = cos(glm::radians(speed)) * rtan * addtionalMovement + addtionalMovement;
        _T[3][1] = _curPos.y = sin(glm::radians(speed)) * rtan * addtionalMovement + addtionalMovement;
    }
    else {
        _T[3][0] = _curPos.x = sin(glm::radians(speed)) * rtan * addtionalMovement + addtionalMovement;
        _T[3][1] = _curPos.y = sin(glm::radians(speed)) * rtan * addtionalMovement + addtionalMovement;
    }
    _curPos.z = _T[3][2];

    _modelMat = _T* R *_S;
    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(_modelMat)));

    glUniformMatrix4fv(glGetUniformLocation(program, "PV"), 1, GL_FALSE,  &PV[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program, "M"), 1, GL_FALSE,  &_modelMat[0][0]);
    glUniformMatrix3fv(glGetUniformLocation(program, "normalMat"), 1, GL_FALSE,  &normalMat[0][0]);

    DrawContainer();

    return true;
}

void MeshObject::DrawContainer()
{
    /// Draw mesh
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    for (GLuint i = 0; i < _textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void MeshObject::Focus(bool isFocused, glm::vec3 focusColor)
{
    _isFocused = isFocused;
    _focusColor = focusColor;
    _focusedTime = glfwGetTime();
}

bool MeshObject::IsIntersected(glm::vec3 org, glm::vec3 dir, float *distance)
{
    glm::vec3 center(_modelMat[3][0], _modelMat[3][1], _modelMat[3][2]);
    float radius = _S[0][0] * 2.f;

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

}
