#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "IGraphicObject.h"
#include "shader.h"

namespace gl
{

using namespace std;

struct Vertex
{
    /// Position
    glm::vec3 Position;
    /// Normal
    glm::vec3 Normal;
    /// TexCoords
    glm::vec2 TexCoords;
};

typedef enum TextureType
{
    Texture_Diffuse,
    Texture_Specular
} TexType;

struct Texture
{
    GLuint  object;
    TexType type;
    string  path;
};

struct MeshData
{
    MeshData(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
    {
        Vertices = vertices;
        Indices = indices;
        Textures = textures;
    }

    vector<Vertex>  Vertices;
    vector<GLuint>  Indices;
    vector<Texture> Textures;
};

/**
 * @brief   Class to manage and draw a mesh object.
 *          This class is the base class of all mesh graphic object.
 */
class MeshObject : public IGraphicObject
{

public:
    /**
     * @brief   Constructor of Mesh object
     *
     * @param pShader       Shader Object to be used
     * @param vertices      All vertices for this mesh object
     * @param indices       All indices for this mesh object
     * @param textures      All texture information for this mesh object
     */
    MeshObject(Shader* pShader, vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);

    /**
     * @brief   Destructor of Mesh object
     */
    virtual ~MeshObject();

    /**
     * @brief   Initialize all processes before draw an object
     */
    virtual bool Initialize();

    /**
     * @brief   Draw a frame into a back buffer to display at next frame.
     *
     * @param time      Current time
     * @param PV        Project/View matrix
     * @return  result of method
     */
    virtual bool DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv);

    /**
     * @brief   For transformations of an object
     * @param scale     scaling vector
     * @param translate Translation vector
     * @return  result of method
     */
    virtual bool Transform(glm::vec3 scale, glm::vec3 translate);

    /**
     * @brief   Check this object is intersected with the ray
     * @param rayOrg    Ray origin in world space
     * @param rayDir    Ray direction in world space
     * @return  distance    distance from ray when the object is intersected
     * @return  result of method
     */
    virtual bool IsIntersected(glm::vec3 rayOrg, glm::vec3 rayDir, float *distance);

    /**
     * @brief   Update focus status of this object
     * @param isFocused    set current status if this is focused or not
     * @param focusColor   focus color;
     */
    virtual void Focus(bool isFocused, glm::vec3 focusColor = glm::vec3(0));

    /**
     * @brief   Change object's default color
     * @param objColor     color to set
     */
    virtual void ChangeColor(glm::vec3 objColor) { _objectColor = objColor;};

    /**
     * @brief   Reset Internal status
     * @param isFocused    set current status if this is focused or not
     * @param focusColor   focus color;
     */
    virtual void Reset(StudioEnv& studioEnv){};

    /**
     * @brief   Get current information
     * @return objInfo    Current information
     */
    void GetCurObjectInfo(struct ObjectInfo& objInfo);

protected:
    bool            SetTextureToShader();
    void            DrawContainer();

    /**  Mesh Data  */
    vector<Vertex>  _vertices;
    vector<GLuint>  _indices;
    vector<Texture> _textures;

    GLuint          _vbo;               /// vertex buffer object
    GLuint          _ebo;               /// element array buffer object
    GLuint          _vao;               /// vertex array object

    glm::mat4       _T;                 /// translation matrix
    glm::mat4       _S;                 /// scale matrix
    glm::mat3       _normalMat;         /// normal matrix
    glm::mat4       _modelMat;          /// model matrix

    Shader*         _pShader;           /// Shader object

    glm::vec3       _objectColor;       /// object color
    glm::vec3       _focusColor;        /// focused object color
    glm::vec3       _curPos;

    bool            _isFocused;
    double          _focusedTime;
    const float     FOCUSEDTIME = 0.7f;

};


}

#endif // MESH_H_INCLUDED
