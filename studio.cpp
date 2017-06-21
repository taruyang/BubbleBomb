#include <cstdlib>
#include <vector>
#include <unistd.h>
#include "windowManager.h"
#include "logging.h"
#include "shader.h"
#include "triangleObject.h"
#include "stageObject.h"
#include "sphereObject.h"
#include "model.h"
#include "meshObject.h"
#include "rectObject.h"
#include "studio.h"
#include "textRenderer.h"
#include "planetObject.h"

using namespace std;

namespace gl
{
/**
 * @brief   Check if there is a command from director ( keyboard input or mouse input )
 */
static bool    KeyCommands[GLFW_KEY_LAST];

static bool    IsFrameBufferChanged = false;

/**
 * @brief   check key commands. This function will update gCommand when there is changed request.
 */
static void keyCommandCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

static void frameBufferSizeChangedCallback(GLFWwindow* window, int width, int height);

Studio::Studio() : _window(nullptr)
{
    /// for lighting
    _studioEnv.LightAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
    _studioEnv.LightDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    _studioEnv.LightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
    _studioEnv.LightPos = glm::vec3(0);
    _studioEnv.GameStage = 1;
    /// Start logging
    RestartLog();
    InitWindowManager();
}

Studio::~Studio()
{
    while(_objs.size() != 0)
    {
        IGraphicObject* obj = _objs.back();
        delete obj;
        _objs.pop_back();
    }

    if(_pIndicator)
        delete _pIndicator;

    if(_pTextRenderer)
        delete _pTextRenderer;

    while(_shaders.size() != 0)
    {
        Shader* shader = _shaders.back();
        delete shader;
        _shaders.pop_back();
    }

    if(_window != nullptr)
    {
        glfwDestroyWindow(_window);      /// stop receiving events for the window and free resources
        QuitWindowManager();            /// clean-up rendering resources
    }

}

bool Studio::ScreenSetting(uint32_t w, uint32_t h)
{
    _w = w; _h = h;
    _window = CreateGlWindow(_w, _h, "OpenGL Window");
    if (_window == NULL) {
        QuitWindowManager();
        LogError("window or context creation failed. \n");
        return false;
    }

    glfwSetKeyCallback(_window, keyCommandCallback);
    glfwSetFramebufferSizeCallback(_window, frameBufferSizeChangedCallback);

    LogGlVersionInfo();

    /// Setting screen info
    _studioEnv.ScreenSize.x = w;
    _studioEnv.ScreenSize.y = h;

    return true;
}

bool Studio::CameraSetting(glm::vec3 camPos, glm::vec3 lookAt, float near, float far, float fov)
{
    float aspect = float(_w)/float(_h);

    _camera.LookAt(camPos, lookAt);

    _camera.SetViewingFrustrum(near, far, fov, aspect);

    /// Setting studio env
    _studioEnv.ViewPos = _camera.GetPosition();

    return true;
}

void Studio::displayGameStatus(const double time, glm::mat4 matrix, StudioEnv& studioEnv)
{
    if(_gameControl.IsGameEnded())
    {
        _pIndicator->ChangeColor(glm::vec3(1.f, 0.f, 0.f));
        _pIndicator->DrawNextFrame(time, matrix, studioEnv);
        _pTextRenderer->Print("Game Over !!!", glm::vec2(-150.f, 0.f), 2, glm::vec3(1.f, 0.f, 0.f), studioEnv);
    }
    else {
        char str[100];

        if(_gameControl.IsStageClearedStatus())
        {
            _pIndicator->ChangeColor(glm::vec3(0.f, 0.f, 0.7f));
            _pIndicator->DrawNextFrame(time, matrix, studioEnv);
            sprintf(str, "Stage %d", _gameControl.GetCurStage());
            _pTextRenderer->Print(str, glm::vec2(-100.f, 0.f), 2, glm::vec3(0.f, 1.f, 1.f), studioEnv);
            studioEnv.GameStage = _gameControl.GetCurStage();
        }
        else if(_gameControl.IsDamagedStatus()) {
            _pIndicator->ChangeColor(glm::vec3(1.f, 0.f, 0.f));
            _pIndicator->DrawNextFrame(time, matrix, studioEnv);
            _pTextRenderer->Print("Damaged !!!", glm::vec2(-100.f, 0.f), 2, glm::vec3(1.f, 0.f, 0.f), studioEnv);
        }
        else {
            _pIndicator->ChangeColor(glm::vec3(0.f));
            _pIndicator->DrawNextFrame(time, matrix, studioEnv);
        }

        if(_gameControl.IsShotStatus()) {
            _pTextRenderer->Print("+", glm::vec2(0.f, 0.f), 2, glm::vec3(1.f, 0.f, 0.f), studioEnv);
        }
        /// display Current Status
        sprintf(str, "Stage %d, Player %d, Enermy %d", _gameControl.GetCurStage(),
                _gameControl.GetPlayerEnergy(), _gameControl.GetEnermyEnergy());

        _pTextRenderer->Print(str, glm::vec2(100.f, 250.f), 1, glm::vec3(0.1f, 0.1f, 0.7f), studioEnv);
    }
}

void Studio::renderNextFrame(const double time, glm::mat4 matrix, StudioEnv& studioEnv)
{
    for(uint32_t i = 0; i < _objs.size(); i++)
    {
        IGraphicObject* pObj = _objs[i];
        pObj->DrawNextFrame(time, matrix, studioEnv);
    }

    displayGameStatus(time, matrix, studioEnv);
}

void Studio::checkObjectsOnStage(StudioEnv& studioEnv)
{
    IGraphicObject* pPlayer = nullptr;
    struct ObjectInfo objInfo;

    for(uint32_t i = 0; i < _objs.size(); i++) {
        pPlayer = _objs[i];
        pPlayer->GetCurObjectInfo(objInfo);
        if(objInfo.ObjType == Object_Model) {
            studioEnv.PlayerPos = objInfo.CurPos;
            break;
        }
    }

    IGraphicObject* pSphere = nullptr;
    for(uint32_t i = 0; i < _objs.size(); i++) {
        pSphere = _objs[i];
        pSphere->GetCurObjectInfo(objInfo);
        if(objInfo.ObjType == Object_Sphere
           || objInfo.ObjType == Object_Model)
        {
            if(glm::length(objInfo.CurPos - _studioEnv.ViewPos) <= objInfo.ObjRadius + 2.0f) {
                printf("ATTACKED!!! by %d \n", objInfo.ObjType);
                pSphere->Reset(_studioEnv);
                _gameControl.Damaged();
            }
        }
    }
}

void Studio::OnStage()
{
    /// camera rotation
    double time = glfwGetTime();

    _deltaTime = time - _lastRenderTime;
    _lastRenderTime = time;

    glm::mat4 viewMat = _camera.GetViewMatrix();
    glm::mat4 projMat = _camera.GetProjMatrix();

    checkObjectsOnStage(_studioEnv);
    renderNextFrame( time, projMat * viewMat, _studioEnv);
}

void Studio::ProcessKeyCommand()
{
    if(KeyCommands[GLFW_KEY_SPACE]) {
        Casting();
        KeyCommands[GLFW_KEY_SPACE] = false;
        _gameControl.Shoot();
    }
    else if(KeyCommands[GLFW_KEY_W]) {
        _camera.ProcessKeyboard(MOVE_FORWARD, _deltaTime);
    }
    else if(KeyCommands[GLFW_KEY_S]) {
        _camera.ProcessKeyboard(MOVE_BACKWARD, _deltaTime);
    }
    else
        return;

    _studioEnv.ViewPos = _camera.GetPosition();
    _studioEnv.Front = _camera.GetDirection();
}

void Studio::ProcessMouseCommand()
{
    double xPos, yPos;

    glfwGetCursorPos(_window, &xPos, &yPos);

    if(_firstMouse)
    {
        _prevX = xPos;
        _prevY = yPos;
        _firstMouse = false;
    }

    GLfloat xOffset = xPos - _prevX;
    GLfloat yOffset = _prevY - yPos;  /// Reversed since y-coordinates go from bottom to left

    _prevX = xPos;
    _prevY = yPos;

    _camera.ProcessMouseMovement(xOffset, yOffset);

    _studioEnv.ViewPos = _camera.GetPosition();
    _studioEnv.Front = _camera.GetDirection();
}

void Studio::ProcessFrameChangeCommand()
{
    int w, h;

    glfwGetFramebufferSize(_window, &w, &h);

    _w = w; _h = h;

    float aspect = float(_w)/float(_h);

    _camera.SetAspectRatio(aspect);

    /// Setting studio env
    _studioEnv.ViewPos = _camera.GetPosition();
    _studioEnv.ScreenSize.x = w;
    _studioEnv.ScreenSize.y = h;

    IsFrameBufferChanged = false;
}

void Studio::Casting()
{
    IGraphicObject* pObj;
    IGraphicObject* pSelectedObj = nullptr;

    float closest = 1000.0f;
    float distance = 0.0f;

    for (int i = 0; i < (int)_objs.size(); i++)
    {
        pObj = _objs[i];
        if (pObj->IsIntersected(_camera.GetPosition(), _camera.GetDirection(), &distance))
        {
            if (distance < closest)
            {
                if(pSelectedObj)
                    pSelectedObj->Focus(false);
                pSelectedObj = pObj;
                pObj->Focus(true, glm::vec3(1.f, 0.f, 0.f));

                struct ObjectInfo objInfo;
                pObj->GetCurObjectInfo(objInfo);
                if(objInfo.ObjType == Object_Model)
                    _gameControl.AttackEnermy();

                closest = distance;
                continue;
            }
        }
        pObj->Focus(false);
    }
}

void Studio::Shoot()
{
    /// the main render loop
    while (!glfwWindowShouldClose(_window)) {

		/** update other events like input handling */
		glfwPollEvents();

		ProcessKeyCommand();
		ProcessMouseCommand();
		ProcessFrameChangeCommand();

        if(_command.allowToRender)
        {
            /// make the context of the given window current on the calling thread
            glfwMakeContextCurrent(_window);
            /** wipe the drawing surface clear */
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            glViewport( 0, 0, _w, _h);

            OnStage();

            /// swap the back and front buffers
            glfwSwapBuffers(_window);
        }

        /// poll window events
        glfwPollEvents();
    }

    /// detach the context from the current thread
    glfwMakeContextCurrent(NULL);
}

bool Studio::Ready()
{
    Shader *pShaderBomb, *pShaderModel, *pShaderRect, *pShaderText, *pShaderPlanet;

    pShaderBomb = new Shader("./glsl/sphereVs.glsl", "./glsl/sphereFs.glsl", "./glsl/sphereTcs.glsl",
                          "./glsl/sphereTes.glsl", "./glsl/sphereGs.glsl"  );
    pShaderBomb->Initialize();
    _shaders.push_back(pShaderBomb);

    pShaderModel = new Shader("./glsl/modelVs.glsl", "./glsl/modelFs.glsl");
    pShaderModel->Initialize();
    _shaders.push_back(pShaderModel);

    pShaderPlanet = new Shader("./glsl/planetVs.glsl", "./glsl/planetFs.glsl");
    pShaderPlanet->Initialize();
    _shaders.push_back(pShaderPlanet);

    pShaderRect = new Shader("./glsl/textureVs.glsl", "./glsl/rectFs.glsl");
    pShaderRect->Initialize();
    _shaders.push_back(pShaderRect);

    pShaderText = new Shader("./glsl/textVs.glsl", "./glsl/textFs.glsl");
    pShaderText->Initialize();
    _shaders.push_back(pShaderText);

    IGraphicObject* pObj;

    for(int i = 0; i < 10; i++)
    {
        pObj = new SphereObject(pShaderBomb, glm::vec3(0.f));
        pObj->Initialize();
        pObj->Transform(glm::vec3(0.5f), glm::vec3(0.f, 0.f, -2.f));
        _objs.push_back(pObj);
    }

    Model model("./resource/Aircraft/Aircraft.obj");
    model.Parse();
    vector<MeshData> meshData = model.GetMeshData();

    for(int i = 0; i < (int)meshData.size(); i++)
    {
        pObj = new MeshObject(pShaderModel, meshData[i].Vertices, meshData[i].Indices, meshData[i].Textures);
        pObj->Initialize();
        pObj->Transform(glm::vec3(2.f), glm::vec3(0.0f));
        _objs.push_back(pObj);
    }

    Model planet("./resource/Rock/planet.obj");
    ///Model planet("./resource/Rock/rock.obj");
    planet.Parse();
    vector<MeshData> planetData = planet.GetMeshData();

    for(int i = 0; i < (int)planetData.size(); i++)
    {
        pObj = new PlanetObject(pShaderPlanet, planetData[i].Vertices, planetData[i].Indices, planetData[i].Textures);
        pObj->Initialize();
        pObj->Transform(glm::vec3(2.f), glm::vec3(0.f, 0.f, -30.0f));
        _objs.push_back(pObj);
    }

    _pIndicator = new RectObject(pShaderRect, "./resource/Cockpit.png");
    _pIndicator->Initialize();
    _pIndicator->Transform(glm::vec3(_studioEnv.ScreenSize.x, _studioEnv.ScreenSize.y, 0.f), glm::vec3(0.0f));

    _pTextRenderer = new TextRenderer(pShaderText, "font/arial.ttf");
    _pTextRenderer->Initialize();

    return true;
}

void keyCommandCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    static int  prevAction = GLFW_RELEASE;

    if(prevAction == action)
        return;

    if (action == GLFW_PRESS)
        KeyCommands[key] = true;
    else if(action == GLFW_RELEASE)
        KeyCommands[key] = false;

    prevAction = action;
}

void frameBufferSizeChangedCallback(GLFWwindow* window, int width, int height)
{
    /// set-up the window/screen coordinates
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    glfwMakeContextCurrent(NULL);
    IsFrameBufferChanged = true;
}

}
