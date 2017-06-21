#ifndef IGRAPHIC_OBJECT_H
#define IGRAPHIC_OBJECT_H
#include <glm/glm.hpp>
#include "studioEnv.h"

using namespace std;

namespace gl
{

typedef enum {
    Object_Triangle,
    Object_Cube,
    Object_Stage,
    Object_Rect,
    Object_Sphere,
    Object_Model,
    Object_Planet
} ObjectType;

struct ObjectInfo{
    glm::vec3   CurPos;
    glm::vec3   ObjColor;
    ObjectType  ObjType;
    float       ObjRadius;
};

/**
 * @brief Interface class for an object.
 *        All objects should inherit and implement pure virtual base class for IGraphicObject.
 *
 */
class IGraphicObject  {

public:
    /**
     * @brief   Destructor of IGraphicObject
     */
    virtual ~IGraphicObject(){}

    /**
     * @brief   Initialize all processes before draw an object
     */
    virtual bool Initialize() = 0;

    /**
     * @brief   Draw a frame into a back buffer to display at next frame.
     *
     * @param time      Current time
     * @param PV        Project/View matrix
     * @param studioEnv Current environment information of a studio object
     * @return  result of method
     */
    virtual bool DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv) = 0;

    /**
     * @brief   For transformations of an object
     * @param scale     scaling vector
     * @param translate Translation vector
     * @return  result of method
     */
    virtual bool Transform(glm::vec3 scale, glm::vec3 translate) = 0;

    /**
     * @brief   Check this object is intersected with the ray
     * @param rayOrg    Ray origin in world space
     * @param rayDir    Ray direction in world space
     * @return  distance    distance from ray when the object is intersected
     * @return  result of method
     */
    virtual bool IsIntersected(glm::vec3 rayOrg, glm::vec3 rayDir, float *distance) = 0;

    /**
     * @brief   Update focus status of this object
     * @param isFocused    set current status if this is focused or not
     * @param focusColor   focus color;
     */
    virtual void Focus(bool isFocused, glm::vec3 focusColor = glm::vec3(0)) = 0;

    /**
     * @brief   Change object's default color
     * @param objColor     color to set
     */
    virtual void ChangeColor(glm::vec3 objColor) = 0;

    /**
     * @brief   Reset Internal status
     * @param isFocused    set current status if this is focused or not
     * @param focusColor   focus color;
     */
    virtual void Reset(StudioEnv& studioEnv) = 0;

    /**
     * @brief   Get current information
     * @return objInfo    Current information
     */
    virtual void GetCurObjectInfo(struct ObjectInfo& objInfo) = 0;
};
}   // gl
#endif // IGRAPHIC_OBJECT_H
