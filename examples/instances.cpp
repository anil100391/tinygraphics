#include <iostream>

#include <app.h>
#include <renderer.h>

#include <vertexarray.h>
#include <indexbuffer.h>
#include <vertexbuffer.h>
#include <vertexbufferlayout.h>
#include <glm/ext/matrix_transform.hpp>

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
    std::unique_ptr<VertexBuffer> _instanceVBO;
    std::unique_ptr<IndexBuffer>  _ibo;
    std::unique_ptr<Shader>       _shader;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestsApplication::TestsApplication() : Application( {1920, 1080, "tinygraphics", true} )
{
    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec2 position;\n"
        "layout(location = 1) in mat4 instanceMatrix;\n"
        "void main()\n"
        "{\n"
        "    // int x = gl_InstanceID / 25;\n"
        "    // int y = gl_InstanceID % 25;\n"
        "    // gl_Position = vec4(position + vec2(0.05, 0.0) * x + vec2(0.0, 0.05) * y, 0.0, 1.0);\n"
        "    gl_Position = instanceMatrix * vec4(position, 0.0, 1.0);\n"
        "}"
    ;

    std::string fragmentShader =
        "#version 330 core\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(1.0);\n"
        "}"
    ;

    float scale = 0.01f;
    float d = -1.0f * scale;
    std::vector<float> positions = { -d, -d,
                                      d, -d,
                                      d,  d,
                                     -d,  d };

    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

    VertexBufferLayout layout;
    layout.Push<float>( 2 );

    // instance array for updating positions
    std::vector<glm::mat4> instanceData( 625u );
    for ( unsigned int y = 0; y < 25; ++y )
    {
        for ( unsigned int x = 0; x < 25; ++x )
        {
            glm::mat4 translation = glm::translate( glm::mat4(1.0f),
                                                    glm::vec3( 0.05f * x, 0.05f * y, 0.0f ) - glm::vec3( 0.6f, 0.6f, 0.0f ) );
            instanceData[y * 25 + x] = translation;
        }
    }

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(positions.data(), static_cast<unsigned int>(positions.size() * sizeof(float)));
    _vao->AddBuffer( *_vbo, layout );

    // instance VBO
    _instanceVBO = std::make_unique<VertexBuffer>( instanceData.data(), static_cast<unsigned int>(instanceData.size() * sizeof( glm::mat4 )) );
    VertexBufferLayout instanceLayout;
    instanceLayout.Push<float>( 4 );
    instanceLayout.Push<float>( 4 );
    instanceLayout.Push<float>( 4 );
    instanceLayout.Push<float>( 4 );
    _vao->AddBuffer( *_instanceVBO, instanceLayout, true );

    _ibo = std::make_unique<IndexBuffer>(indices.data(), static_cast<unsigned int>(indices.size()));
    _shader = std::make_unique<Shader>(vertexShader, fragmentShader);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestsApplication::~TestsApplication()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestsApplication::Update()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    renderer.Clear();

    // draw
    renderer.DrawInstanced( *_vao, *_ibo, *_shader, 625u );
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
