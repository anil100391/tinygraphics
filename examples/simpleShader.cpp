#include <iostream>

#include <app.h>
#include <renderer.h>

#include <vertexarray.h>
#include <indexbuffer.h>
#include <vertexbuffer.h>
#include <vertexbufferlayout.h>

// #include <imgui.h>
// #include <imgui_impl_glfw.h>
// #include <imgui_impl_opengl3.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const char *glsl_version = "#version 130";

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestsApplication : public Application
{
public:
    TestsApplication();
    ~TestsApplication();

    virtual void Update() override;

    virtual bool OnEvent( Event &evt ) override
    {
        return true;
    }

private:

    std::unique_ptr<VertexArray>  _vao;
    std::unique_ptr<VertexBuffer> _vbo;
    std::unique_ptr<IndexBuffer>  _ibo;
    std::unique_ptr<Shader>       _shader;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestsApplication::TestsApplication() : Application( {1920, 1080, "tinygraphics", true} )
{
    /*
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL( _window, true );
    ImGui_ImplOpenGL3_Init( glsl_version );

    auto &io = ImGui::GetIO();
    auto font = io.Fonts->AddFontFromFileTTF( "res/fonts/Open_Sans/OpenSans-Regular.ttf", 16.0f );

    auto& style = ImGui::GetStyle();
    style.ChildRounding = 0.0f;
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    // style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.56f, 0.24, 1.0f);
    // style.Colors[ImGuiCol_ResizeGrip]    = ImVec4(0.08f, 0.56f, 0.24, 1.0f);
    // style.Colors[ImGuiCol_Button]        = ImVec4(0.08f, 0.56f, 0.24, 1.0f);
    */

    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec2 position;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "}"
    ;

    // "Plasma" by @XorDev
    // X Post: x.com/XorDev/status/1894123951401378051

    std::string fragmentShader =
        "#version 330 core\n"
        "uniform vec2 u_Size;\n"
        "uniform vec2 u_MousePos;\n"
        "uniform float u_Time;\n"

        "vec4 mainImage()\n"
        "{\n"
        "    // Centered, ratio corrected, coordinates\n"
        "    vec2 p = (2.0 * gl_FragCoord.xy - u_Size.xy) / min(u_Size.x, u_Size.y);\n"

        "    // Z depth\n"
        "    vec2 z = vec2(u_MousePos.xy / u_Size.xy);\n"

        "    // Fluid coordinates\n"
        "    vec2 f = p * (z += 4.0 - 4.0 * abs(0.7 - dot(p, p)));\n"

        "    // Clear frag color and loop 8 times\n"
        "    vec4 O;\n"
        "    vec2 i = vec2(0.0);\n"
        "    for ( O *= 0.0; i.y++ < 8.0;\n"
        "        // Set color waves and line brightness\n"
        "        O += (sin(f) + 1.0).xyyx * abs(f.x - f.y))\n"
        "        // Add fluid waves\n"
        "        f += cos(f.yx * i.y + i + u_Time) / i.y + 0.7;\n"

        "    // Tonemap, fade edges and color gradient\n"
        "    O = tanh(7.0 * exp(z.x - 4.0 - p.y * vec4(-1.0, 1.0, 2.0, 0.0)) / O);\n"
        "    return O;\n"
        "}\n"

        "void main()\n"
        "{\n"
        "    gl_FragColor = mainImage();\n"
        "}";

    float d = -1.0f;
    std::vector<float> positions = { -d, -d,
                                      d, -d,
                                      d,  d,
                                     -d,  d };

    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

    VertexBufferLayout layout;
    layout.Push<float>( 2 );
    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(positions.data(), static_cast<unsigned int>(positions.size() * sizeof(float)));
    _vao->AddBuffer( *_vbo, layout );
    _ibo = std::make_unique<IndexBuffer>(indices.data(), static_cast<unsigned int>(indices.size()));
    _shader = std::make_unique<Shader>(vertexShader, fragmentShader);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestsApplication::~TestsApplication()
{
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestsApplication::Update()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    renderer.Clear();

    /*
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow();

    if ( ImGui::CollapsingHeader( "Render Statistics" ) )
    {
        ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)",
                     1000.0f / ImGui::GetIO().Framerate,
                     ImGui::GetIO().Framerate );
    }

    ImGui::Separator();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
    */

    // uniforms
    _shader->Bind();
    int width, height;
    GetWindowSize(width, height);
    _shader->SetUniform2f( "u_Size", static_cast<float>(width), static_cast<float>(height) );
    double x, y;
    GetCursorPosition( x, y );
    _shader->SetUniform2f( "u_MousePos", static_cast<float>(x), static_cast<float>(y) );
    _shader->SetUniform1f( "u_Time", GetCurrentTime() );

    // draw
    renderer.Draw( *_vao, *_ibo, *_shader );
    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
    TestsApplication app;
    app.Run();
    return 0;
}
