#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP

#include <string>
#include <memory>
#include <r2tk/r2-singleton.hpp>
#include <GL/glew.h>
#include <GL/glfw.h>

struct InputState {
public:
    struct Keyboard {
        bool m_keys[GLFW_KEY_LAST + 1];

        Keyboard();
    } m_keyboard;

    struct Mouse {
        int m_x;
        int m_y;
        int m_wheelPos;
        bool m_buttons[GLFW_MOUSE_BUTTON_LAST];

        Mouse();
    } m_mouse;
};

class LabTemplate {
public:
    virtual ~LabTemplate() throw() {}

    virtual void onUpdate(float dt, const InputState& currentInput, const InputState& previousInput) = 0;
    virtual void onRender(float dt, float interpolation) = 0;
    virtual void onResize(int width, int height) = 0; 
    virtual void onKey(int key, int action)  {}
    virtual void onChar(int character, int action)  {}
    virtual void onMouseButton(int button, int action)  {}
    virtual void onMouseMotion(int x, int y)  {}
    virtual void onMouseWheel(int pos)  {}
};

class LabApplication : public r2::Singleton<LabApplication> {
public:
    struct ContextDescription {
        int m_windowWidth;
        int m_windowHeight;
        std::string m_windowTitle;

        int m_redBits;
        int m_greenBits;
        int m_blueBits;
        int m_depthBits;
        int m_stencilBits;

        int m_openglVersionMajor;
        int m_openglVersionMinor;
        bool m_fullscreen;

        ContextDescription();
    };

    /** Initialize GLFW and DevIL */
    LabApplication();

    /** Create a window and an OpenGL context.  */
    void createContext(const ContextDescription& description);

    /** Run the given lab */
    void start(std::unique_ptr<LabTemplate>& lab, int ticksPerSecond = 25, int maxTicksPerFrame = 5);
private:
    /** Handles initialization and termination of GLFW */
    class GLFWHandler {
    public:
        GLFWHandler();
        ~GLFWHandler() throw();
    };

    GLFWHandler m_glfwHandler;
    std::unique_ptr<LabTemplate> m_lab;
    std::string m_windowTitle;
    bool m_running;
    InputState m_currentInput;
    InputState m_previousInput;

    static void onResizeCallback(int width, int height);
    static int onCloseCallback();
    static void onKeyCallback(int key, int action);
    static void onCharCallback(int character, int action);
    static void onMouseButtonCallback(int button, int action);
    static void onMouseMotionCallback(int x, int y);
    static void onMouseWheelCallback(int pos);
    void run(int ticksPerSecond, int maxTicksPerFrame);
    void onUpdateWrapper(float dt);
    void onRenderWrapper(float dt, float interpolation);
    void onResizeWrapper(int width, int height);
    void onKeyWrapper(int key, int action);
    void onCharWrapper(int character, int action);
    void onMouseButtonWrapper(int button, int action);
    void onMouseMotionWrapper(int x, int y);
    void onMouseWheelWrapper(int pos);
};

#endif
