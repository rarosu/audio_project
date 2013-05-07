#include "template.hpp"
#include "utility.hpp"
#include <cstring>
#include <sstream>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <IL/il.h>
#include <r2tk/r2-exception.hpp>

InputState::Keyboard::Keyboard() {
    memset(m_keys, false, sizeof(m_keys)); 
}

InputState::Mouse::Mouse() {
    m_x = 0;
    m_y = 0;
    m_wheelPos = 0;
    memset(m_buttons, false, sizeof(m_buttons));
}

LabApplication::ContextDescription::ContextDescription() {
    m_windowWidth = 640;
    m_windowHeight = 480;
    
    m_redBits = 8;
    m_greenBits = 8;
    m_blueBits = 8;
    m_depthBits = 24;
    m_stencilBits = 0;
    
    m_openglVersionMajor = 4;
    m_openglVersionMinor = 0;
    m_fullscreen = false;
}

LabApplication::GLFWHandler::GLFWHandler() {
    if (glfwInit() == GL_FALSE)
        throw r2ExceptionRuntimeM("Failed to initialize GLFW");
}

LabApplication::GLFWHandler::~GLFWHandler() throw() {
    glfwTerminate();
}


LabApplication::LabApplication()
    : m_running(true) {

    // Initialize DevIL
    ilInit();
}

void LabApplication::createContext(const ContextDescription& description) {
    m_windowTitle = description.m_windowTitle;

    // Initialize OpenGL context and window
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, description.m_openglVersionMajor);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, description.m_openglVersionMinor);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);

    int mode = description.m_fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW;
    int windowOpened = glfwOpenWindow(description.m_windowWidth,
            description.m_windowHeight,
            description.m_redBits,
            description.m_greenBits,
            description.m_blueBits,
            0,
            description.m_depthBits,
            description.m_stencilBits,
            mode);
    if (windowOpened != GL_TRUE)
        throw r2ExceptionRuntimeM("Failed to open window");

    glfwSetWindowTitle(description.m_windowTitle.c_str());
    glfwSetWindowSizeCallback(onResizeCallback);
    glfwSetWindowCloseCallback(onCloseCallback);
    glfwSetKeyCallback(onKeyCallback);
    glfwSetCharCallback(onCharCallback);
    glfwSetMouseButtonCallback(onMouseButtonCallback);
    glfwSetMousePosCallback(onMouseMotionCallback);
    glfwSetMouseWheelCallback(onMouseWheelCallback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = GLCheck(glewInit());
    if (glewError != GLEW_OK) {
        std::stringstream errorMessage;
        errorMessage << "Failed to initialize GLEW: ";
        errorMessage << glewGetErrorString(glewError);
        throw r2ExceptionRuntimeM(errorMessage.str());
    }

    // Setup default OpenGL state
    GLCheck(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

    std::cout << "INFO: OpenGL Version: " << glfwGetWindowParam(GLFW_OPENGL_VERSION_MAJOR) <<
                                      "." << glfwGetWindowParam(GLFW_OPENGL_VERSION_MINOR) << std::endl;
}

void LabApplication::start(std::unique_ptr<LabTemplate>& lab, int ticksPerSecond, int maxTicksPerFrame) {
    m_lab = std::move(lab);

    // Since the window and context has been created before this,
    // notify the lab class of the current dimensions.
    int width, height;
    glfwGetWindowSize(&width, &height);
    m_lab->onResize(width, height);

    // Run the main loop
    run(ticksPerSecond, maxTicksPerFrame);
}

void LabApplication::onResizeCallback(int width, int height) {
    LabApplication::Instance().onResizeWrapper(width, height);
}

int LabApplication::onCloseCallback() {
    LabApplication::Instance().m_lab.reset();
    LabApplication::Instance().m_running = false;

    return GL_TRUE;
}

void LabApplication::onKeyCallback(int key, int action) {
    LabApplication::Instance().onKeyWrapper(key, action);
}

void LabApplication::onCharCallback(int character, int action) {
    LabApplication::Instance().onCharWrapper(character, action);
}

void LabApplication::onMouseButtonCallback(int button, int action) {
    LabApplication::Instance().onMouseButtonWrapper(button, action);
}

void LabApplication::onMouseMotionCallback(int x, int y) {
    LabApplication::Instance().onMouseMotionWrapper(x, y);
}

void LabApplication::onMouseWheelCallback(int pos) {
    LabApplication::Instance().onMouseWheelWrapper(pos);
}

void LabApplication::run(int ticksPerSecond, int maxTicksPerFrame) {
    const float DT = 1.0f / ticksPerSecond;
    double nextTick = glfwGetTime() + DT;
    while (m_running) {

        int ticksThisFrame = 0;
        while (glfwGetTime() >= nextTick && ticksThisFrame < maxTicksPerFrame) {
            nextTick += DT;
            ++ticksThisFrame;

            onUpdateWrapper(DT);
            m_previousInput = m_currentInput;
        }

        float interpolation = 1.0f + (glfwGetTime() - nextTick) / DT;
        onRenderWrapper(DT, interpolation);
    }
}

void LabApplication::onUpdateWrapper(float dt) {
    if (m_lab != nullptr)
        m_lab->onUpdate(dt, m_currentInput, m_previousInput);
}

void LabApplication::onRenderWrapper(float dt, float interpolation) {
    if (m_lab != nullptr)
        m_lab->onRender(dt, interpolation);
}

void LabApplication::onResizeWrapper(int width, int height) {
    glViewport(0, 0, width, height);

    std::stringstream ss;
    ss << m_windowTitle << ": " << width << "x" << height;
    glfwSetWindowTitle(ss.str().c_str());

    if (m_lab != nullptr)
        m_lab->onResize(width, height);
}

void LabApplication::onKeyWrapper(int key, int action) {
    m_currentInput.m_keyboard.m_keys[key] = (action == GLFW_PRESS) ? true : false;
    if (m_lab != nullptr)
        m_lab->onKey(key, action);
}

void LabApplication::onCharWrapper(int character, int action) {
    if (m_lab != nullptr)
        m_lab->onChar(character, action);
}

void LabApplication::onMouseButtonWrapper(int button, int action) {
    m_currentInput.m_mouse.m_buttons[button] = (action == GLFW_PRESS) ? true : false;
    if (m_lab != nullptr)
        m_lab->onMouseButton(button, action);
}

void LabApplication::onMouseMotionWrapper(int x, int y) {
    m_currentInput.m_mouse.m_x = x;
    m_currentInput.m_mouse.m_y = y;
    if (m_lab != nullptr)
        m_lab->onMouseMotion(x, y);
}

void LabApplication::onMouseWheelWrapper(int pos) {
    m_currentInput.m_mouse.m_wheelPos = pos;
    if (m_lab != nullptr)
        m_lab->onMouseWheel(pos);
}

