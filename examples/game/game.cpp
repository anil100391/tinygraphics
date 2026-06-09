#include "renderer.h"
#include <filesystem>
#include <game.h>
#include <memory>
#include <fstream>
#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Game::Game(const WindowProperties &wprops, const std::filesystem::path &exeDir)
    : Application(wprops, true),
      _exeDir(exeDir)
{
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    SetupCamera();
    SetupGeometry();
    SetupShader();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Game::Update()
{
    glClearColor(0.5, 0.5, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _glMesh->vao()->Bind();
    _glMesh->ibo()->Bind();

    _shader->Bind();
    _shader->SetUniformMat4f("u_M", glm::mat4(1.0f));
    _shader->SetUniformMat4f("u_V", _camera.GetViewMatrix());
    _shader->SetUniformMat4f("u_P", _camera.GetProjectionMatrix(1.0f, 0.01f, 100.0f));
    _shader->SetUniform1f("u_Time", GetCurrentTime());

    Renderer r;
    r.Draw(*_glMesh->vao(), *_glMesh->ibo(), *_shader);

    // GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    SetupShader();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Game::OnEvent(Event &evt)
{
    // _camera.OnEvent(evt);
    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Game::SetupCamera()
{
    _camera.SetUpVec(glm::vec3(0.0f, 0.0f, 1.0f));
    _camera.SetLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    _camera.SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Game::SetupGeometry()
{
    const std::vector<float> vertices {-1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                                        1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                                        1.0f, 0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                                       -1.0f, 0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f};

    VertexBufferLayout vblayout;
    vblayout.Push<float>(3); // vertex
    vblayout.Push<float>(3); // normal
    vblayout.Push<float>(2); // texture coordinate
                             //
    const std::vector<unsigned int> conn{0, 1, 2, 0, 2, 3};

    _glMesh = std::make_unique<MeshGL>(vertices, vblayout, conn);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static bool ReadFileAsString(const std::filesystem::path &filePath, std::string &output)
{
    std::ifstream file(filePath);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            output += line;
            output += "\n";
        }
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Game::SetupShader()
{
    static std::filesystem::path vertShaderFile = _exeDir / "shaders/v3n3t2_vert.glsl";
    static std::filesystem::path fragShaderFile = _exeDir / "shaders/fragment_custom.glsl";

    // create fragment shader file if missing
    std::string fragmentSource;
    if (!std::filesystem::exists(fragShaderFile))
    {
        fragmentSource =
        "#version 330 core\n"

        "in vec3 fragNormal;\n"
        "in vec3 fragPos;\n"
        "in vec2 fragTexCoord;\n"

        "uniform float u_Time;\n"
        "uniform vec3 u_LightPos;\n"
        "uniform vec3 u_CameraPos;\n"
        "uniform sampler2D u_Texture;\n"

        "out vec4 fragColor;\n"
        "void main()\n"
        "{\n"
        "    vec2 pct = fragTexCoord;\n"
        "    fragColor = vec4(pct.x, pct.y, 0.5, 1.0);\n"
        "}\n";

        std::ofstream file(fragShaderFile);
        file.write(fragmentSource.c_str(), fragmentSource.size());
    }

    static auto vertLastModified = std::filesystem::last_write_time(vertShaderFile);
    static auto fragLastModified = std::filesystem::last_write_time(fragShaderFile);

    if ( _shader                                                              &&
         std::filesystem::last_write_time(vertShaderFile) == vertLastModified &&
         std::filesystem::last_write_time(fragShaderFile) == fragLastModified )
    {
        return;
    }

    std::string vertexSource;
    ReadFileAsString(vertShaderFile, vertexSource);
    ReadFileAsString(fragShaderFile, fragmentSource);

    vertLastModified = std::filesystem::last_write_time(vertShaderFile);
    fragLastModified = std::filesystem::last_write_time(fragShaderFile);

    _shader = std::make_unique<Shader>(vertexSource, fragmentSource);
}
