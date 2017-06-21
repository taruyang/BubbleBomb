#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#define GLEW_NO_GLU
#include <GL/glew.h>	    /** Include GLEW and new version of GL on Windows */
#include <GLFW/glfw3.h>

#define DEFAULT_WINDOW_WIDTH 960    /** Default Window size */
#define DEFAULT_WINDOW_HEIGHT 540   /** Default Window Height */

/**
 * @brief   OpenGL window context management
 *          WindowManager includes APIs to manager GLFW,
 *          such as initialization, termination and looping the window manager.
 *          Also, includes the callbacks implementation to handle the messages from GLFW
 *          and the creation of an window.
 *
 */
namespace gl
{
/**
 * @brief     Initialize GLFW.
 * @return    true if succeed, or false in the other case.
 */
bool InitWindowManager();

/**
 * @brief   Terminate GLFW and clean-up resources.
 */
void QuitWindowManager();

/**
 * @brief   Create a new OpenGL Window.
 *          Create a new window and associated OpenGL context. A forward compatible
 *          (API>=3.0) core context (API>=3.2) is created if supported by the requested
 *          client API version.
 *
 * @param width     The window width in pixels, or the horizontal resolution if
 *                  a monitor is specified.
 * @param height    The window height in pixels, or the vertical resolution if
 *                  a monitor is specified.
 * @param title     The window title
 * @param monitor   NULL for windowed mode, or the monitor object for full
 *                  screen mode.
 * @param share     Share the context of the given window.
 * @param major     The returned OpenGL context must have at least this major
 *                  version number.
 * @param minor     The returned OpenGL context must have at least this minor
 *                  version number.
 * @return          The new window, or NULL if window creation failed.
 */
GLFWwindow * CreateGlWindow(int width, int height, const char *title,
        GLFWmonitor *monitor = NULL, GLFWwindow *share = NULL, int major = 3,
        int minor = 3);

} /// namespace gl
#endif  /// WINDOW_MANAGER_H
