#ifndef RECTOBJECT_H_INCLUDED
#define RECTOBJECT_H_INCLUDED

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "IGraphicObject.h"
#include "triangleObject.h"
#include "shader.h"

namespace gl
{
using namespace std;

/**
 * @brief   Class to manage and draw a rectangle object.
 *          This class inherits the Triangle class because a flag is a composition of multiple triangles.
 *          A rectangle object consist of 4 vertices on xz plane which y is -1.
 */
class RectObject : public TriangleObject
{

public:
    /**
     * @brief   Constructor of rectangle object
     *
     * @param pShader       Shader Object to be used
     * @param objectColor   The color of this object
     */
    RectObject(Shader* pShader, glm::vec3 objectColor);

    /**
     * @brief   Constructor of rectangle object
     *
     * @param pShader       Shader Object to be used
     * @param texturePath   Texture file path to wrap this object
     */
    RectObject(Shader* pShader, const char* texturePath);

    /**
     * @brief   Destructor of rectangle object
     */
    virtual ~RectObject();

    /**
     * @brief   Draw a frame into a back buffer to display at next frame.
     *
     * @param time      Current time
     * @param PV        Project/View matrix
     * @param studioEnv Studio environment
     * @return  result of method
     */
    virtual bool DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv);

};
}  /// namespace gl

#endif // RECTOBJECT_H_INCLUDED
