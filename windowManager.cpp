#include <stdio.h>
#include "windowManager.h"
#include "logging.h"

namespace gl
{

/**
 * Callbacks related with window framework event
 */
void errorCallback(int errCode, const char* msg)
{
	LogError("[GLFW ERROR] code : %d, msg %s \n", errCode, msg);
}

void windowClosedCallback(GLFWwindow* widow)
{
    /// nothing to do right now (do not call glfwDestroyWindow from here)
}

void windowSizeChangedCallback(GLFWwindow* window, int width, int height)
{
    /// nothing to do right now
}

void frameBufferSizeChangedCallback(GLFWwindow* window, int width, int height)
{
    /// set-up the window/screen coordinates
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    glfwMakeContextCurrent(NULL);
}

void keyInputCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch (key) {
        /** Close the window When a user presses the escape key */
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GL_TRUE);
                windowClosedCallback(window);
            }
            break;
    }
}

/**
 * OpenGL context management
 */
bool InitWindowManager()
{
    Log("Starting GLFW %s \n", glfwGetVersionString());

    glfwSetErrorCallback(errorCallback);
    if ( !glfwInit() ) {
        LogError("Could not start GLFW3 \n");
        return false;
    }

    return true;
}

void QuitWindowManager()
{
    glfwTerminate();
}

GLFWwindow * CreateGlWindow(int width, int height, const char *title,
        GLFWmonitor *monitor, GLFWwindow *share, int major, int minor)
{
    /// request an OpenGL context with specific features
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    /// the forward compatibility bit and core profile are only supported since
    /// OpenGL version 3.0 and 3.2 respectively; these hints are ignored for
    /// OpenGL_ES contexts
    if (major >= 3) {
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        if (major > 3 || minor >= 2) {
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
    }

    /// create the window
    GLFWwindow *window = glfwCreateWindow(width, height, title, monitor, share);
    if (window == NULL) {
        LogError("Could not open window with GLFW \n");
        return NULL;
    }

    /// Start GLEW extension handler
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if ( glewInit() != GLEW_OK ) {
        LogError("Could not open Glew \n");
        return NULL;
    }

	/// Enable depth test
	glEnable(GL_DEPTH_TEST);
	/// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

    /// set window callback functions
    glfwSetFramebufferSizeCallback(window, frameBufferSizeChangedCallback);
    glfwSetWindowCloseCallback(window, windowClosedCallback);
    glfwSetKeyCallback(window, keyInputCallback);
	glfwSetWindowSizeCallback( window, windowSizeChangedCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return window;
}

} /// namespace gl
