#ifndef TRIANGLE_OBJECT_H
#define TRIANGLE_OBJECT_H

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "IGraphicObject.h"
#include "shader.h"

namespace gl
{

using namespace std;

/**
 * @brief   Class to manage and draw a triangle object.
 *          This class is the base class of all graphic object. All objects should inherit this class.
 *          In this version, the internal implementation has upgraded to support multiple objects and
 *          Element Buffer Objects.
 */
class TriangleObject : public IGraphicObject
{

public:
    /**
     * @brief   Constructor of Mesh object
     *
     * @param pShader       Shader Object to be used
     * @param texturePath   Texture file path to wrap this object
     */
    TriangleObject(Shader* pShader, const char* texturePath);

    /**
     * @brief   Constructor of Triangle object
     *
     * @param pShader       Shader Object to be used
     * @param objectColor   The color of this object
     */
    TriangleObject(Shader* pShader, glm::vec3 objectColor);

    /**
     * @brief   Destructor of Triangle object
     */
    virtual ~TriangleObject();

    /**
     * @brief   Initialize all processes before draw an object
     */
    virtual bool Initialize();

    /**
     * @brief   Draw a frame into a back buffer to display at next frame.
     *
     * @param time      Current time
     * @param PV        Project/View matrix
     * @param studioEnv Studio environment
     * @return  result of method
     */
    virtual bool DrawNextFrame(const double time, glm::mat4 matrix, StudioEnv& studioEnv);

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
    bool IsIntersected(glm::vec3 rayOrg, glm::vec3 rayDir, float *distance) { return false; };

    /**
     * @brief   Update focus status of this object
     * @param isFocused    set current status if this is focused or not
     * @param focusColor   focus color;
     * @return  result of method
     */
    void Focus(bool isFocused, glm::vec3 focusColor = glm::vec3(0)){ _isFocused = isFocused; _focusColor = focusColor;};

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
    void Reset(StudioEnv& studioEnv) {};

    /**
     * @brief   Get current information
     * @return objInfo    Current information
     */
    void GetCurObjectInfo(struct ObjectInfo& objInfo);

protected:
    bool        CreateVAO();        /// Create vao object
    bool        CreateTexture();    /// Create texture object

    bool        SetTextureToShader();
    void        DrawContainer();

    GLfloat*    _vertices;
    GLuint*     _indices;
    size_t      _sizeVertices;
    size_t      _sizeIndices;

    GLuint      _vbo;               /// vertex buffer object
    GLuint      _ebo;               /// element array buffer object
    GLuint      _vao;               /// vertex array object
    GLuint      _tex;               /// texture object

    glm::mat4   _T;                 /// translation matrix
    glm::mat4   _S;                 /// scale matrix
    glm::mat3   _normalMat;         /// normal matrix

    glm::vec3   _objectColor;       /// object color
    glm::vec3   _focusColor;        /// focused object color
    glm::vec3   _orgPos;            /// Starting Position
    glm::vec3   _curPos;

    const char* _texturePath;       /// Texture file path

    Shader*     _pShader;           /// Shader object

    bool        _isFocused;

    ObjectType  _objType;

};
}  /// namespace gl

#endif /// TRIANGLE_OBJECT_H
