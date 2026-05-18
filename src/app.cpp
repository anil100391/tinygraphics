#include <app.h>

#include <iostream>
#include <string>

#include <events/keyevent.h>
#include <events/mouseevent.h>
#include <events/windowevent.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void KeyCallback( GLFWwindow *window,
                  int         key,
                  int         scancode,
                  int         action,
                  int         mods )
{
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose( window, GLFW_TRUE );
    }

    static unsigned int repeatcnt = 0;

    if ( action == GLFW_PRESS )
    {
        repeatcnt = 0;
        KeyPressedEvent evt( key, 0 );
        app->OnEvent( evt );
    }
    else if ( action == GLFW_RELEASE )
    {
        KeyReleasedEvent evt( key );
        app->OnEvent( evt );
    }
    else if ( action == GLFW_REPEAT )
    {
        ++repeatcnt;
        KeyPressedEvent evt( key, repeatcnt );
        app->OnEvent( evt );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void MouseMoveCallback( GLFWwindow *window,
                        double      xpos,
                        double      ypos )
{
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );
    MouseMoveEvent evt( (int)xpos, (int)ypos );
    app->OnEvent( evt );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void MouseButtonCallback( GLFWwindow *window,
                          int         button,
                          int         action,
                          int         mods )
{
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );

    double xpos = 0.0;
    double ypos = 0.0;
    glfwGetCursorPos( window, &xpos, &ypos );

    if ( button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS )
    {
        MousePressedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::RIGHT );
        app->OnEvent( evt );
    }
    else if ( button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE )
    {
        MouseReleasedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::RIGHT );
        app->OnEvent( evt );
    }
    else if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
    {
        MousePressedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::LEFT );
        app->OnEvent( evt );
    }
    else if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE )
    {
        MouseReleasedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::LEFT );
        app->OnEvent( evt );
    }
    else if ( button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS )
    {
        MousePressedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::MIDDLE );
        app->OnEvent( evt );
    }
    else if ( button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE )
    {
        MouseReleasedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::MIDDLE );
        app->OnEvent( evt );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void MouseScrollCallback( GLFWwindow *window,
                          double xoffset,
                          double yoffset )
{
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );

    double xpos, ypos;
    glfwGetCursorPos( window, &xpos, &ypos );

    MouseScrollEvent evt( static_cast<int>(xpos), static_cast<int>(ypos),
                          xoffset, yoffset );
    app->OnEvent(evt);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void WindowResizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    WindowResizeEvent evt(width, height);
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );
    app->OnEvent(evt);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
WindowProperties::WindowProperties( unsigned int w,
                                    unsigned int h,
                                    const char *title,
                                    bool maximized )
    : _width(w),
      _height(h),
      _title(title),
      _maximized(maximized)
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Application::Application( const WindowProperties &wprops,
                          bool initDearImGui )
{
    if ( !glfwInit() )
        return;

    float scale = 1.0f;
    if ( initDearImGui )
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

        // Setup scaling
        scale = ImGui_ImplGlfw_GetContentScaleForMonitor( glfwGetPrimaryMonitor() );
        ImGuiStyle &style = ImGui::GetStyle();
        style.ScaleAllSizes( scale );   // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = scale;     // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
    }

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    if ( wprops._notitle )
    {
        glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );
    }

    _window = glfwCreateWindow( wprops._width * scale,
                                wprops._height * scale,
                                wprops._title.c_str(),
                                nullptr,
                                nullptr );

    if ( !_window )
    {
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer( _window, this );
    glfwSwapInterval( 1 );
    glfwMakeContextCurrent( _window );

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return;
    }

    // Callbacks
    glfwSetKeyCallback( _window, KeyCallback );
    glfwSetCursorPosCallback( _window, MouseMoveCallback );
    glfwSetMouseButtonCallback( _window, MouseButtonCallback );
    glfwSetScrollCallback( _window, MouseScrollCallback );
    glfwSetFramebufferSizeCallback( _window, WindowResizeCallback );

    if ( wprops._maximized )
    {
        glfwMaximizeWindow( _window );
    }

    if ( initDearImGui )
    {
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL( _window, true );          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Application::~Application()
{
    if ( _initDearImGui )
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    if ( _window )
        glfwDestroyWindow( _window );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::Update()
{
    glfwSwapBuffers( _window );
    glfwPollEvents();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::GetWindowSize(int &width, int &height) const noexcept
{
    glfwGetWindowSize(_window, &width, &height);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
float Application::GetCurrentTime() const noexcept
{
    return static_cast<float>(glfwGetTime());
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::GetCursorPosition( double &x, double &y ) const noexcept
{
    glfwGetCursorPos( _window, &x, &y );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Application::OnEvent( Event &evt )
{
    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::Run()
{
    while ( !glfwWindowShouldClose(_window) )
    {
        Update();
    }
}
