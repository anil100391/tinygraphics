#include <memory>
#include <fstream>
#include <filesystem>

#include <string>
#include <tinygraphics/app.h>
#include <tinygraphics/renderer.h>

#include <tinygraphics/camera.h>
#include <tinygraphics/vertexarray.h>
#include <tinygraphics/indexbuffer.h>
#include <tinygraphics/vertexbuffer.h>
#include <tinygraphics/vertexbufferlayout.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static std::filesystem::path exeDir;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestsApplication : public Application
{
public:
    TestsApplication();
    ~TestsApplication() = default;

    virtual void Update() override;

private:

    std::unique_ptr<VertexArray>  _vao;
    std::unique_ptr<VertexBuffer> _vbo;
    std::unique_ptr<IndexBuffer>  _ibo;
    std::unique_ptr<Shader>       _shader;

    Camera                        _camera;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestsApplication::TestsApplication() : Application( {1920, 1080, "tinygraphics", true} )
{
    _camera.SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
    _camera.SetLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    _camera.SetType(Camera::PROJECTION::ORTHOGRAPHIC);

    std::string vertexShader;
    std::ifstream file(exeDir.string() + "/shaders/v3n3_vert.glsl");
    std::string line;
    while (std::getline(file, line))
    {
        vertexShader += line;
        vertexShader += "\n";
    }

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

    float xmin = -1.0f;
    float xmax = 1.0f;
    float zmin = -1.0f;
    float zmax = 1.0f;
    std::vector<float> positions = { xmin, 0.0f, zmin, 0.0f, -1.0f, 0.0f,
                                     xmax, 0.0f, zmin, 0.0f, -1.0f, 0.0f,
                                     xmax, 0.0f, zmax, 0.0f, -1.0f, 0.0f,
                                     xmin, 0.0f, zmax, 0.0f, -1.0f, 0.0f};

    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

    VertexBufferLayout layout;
    layout.Push<float>( 3 );
    layout.Push<float>( 3 );
    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(positions.data(), static_cast<unsigned int>(positions.size() * sizeof(float)));
    _vao->AddBuffer( *_vbo, layout );
    _ibo = std::make_unique<IndexBuffer>(indices.data(), static_cast<unsigned int>(indices.size()));
    _shader = std::make_unique<Shader>(vertexShader, fragmentShader);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestsApplication::Update()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    renderer.Clear();

    // uniforms
    _shader->Bind();
    int width, height;
    GetWindowSize(width, height);
    float aspectRatio = (1.0f * width) / height;
    _shader->SetUniform2f( "u_Size", static_cast<float>(width), static_cast<float>(height) );
    double x, y;
    GetCursorPosition( x, y );
    _shader->SetUniform2f( "u_MousePos", static_cast<float>(x), static_cast<float>(y) );
    _shader->SetUniform1f( "u_Time", GetCurrentTime() );
    _shader->SetUniformMat4f("u_M", glm::identity<glm::mat4>());
    _shader->SetUniformMat4f("u_V", _camera.GetViewMatrix());
    _shader->SetUniformMat4f("u_P", _camera.GetProjectionMatrix(aspectRatio, 0.1f, 100.0f));

    // draw
    renderer.Draw( *_vao, *_ibo, *_shader );
    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
    exeDir = std::filesystem::absolute( argv[0] ).parent_path();

    TestsApplication app;
    app.Run();
    return 0;
}
