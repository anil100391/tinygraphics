#include <memory>
#include <fstream>
#include <filesystem>

#include <string>
#include <tinygraphics/app.h>
#include <tinygraphics/renderer.h>

#include <tinygraphics/camera.h>
#include <tinygraphics/framebuffer.h>
#include <tinygraphics/vertexarray.h>
#include <tinygraphics/indexbuffer.h>
#include <tinygraphics/vertexbuffer.h>
#include <tinygraphics/vertexbufferlayout.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static std::filesystem::path exeDir;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class DepthBufferApp : public Application
{
public:
    DepthBufferApp();
    ~DepthBufferApp() = default;

    virtual void Update() override;

private:
    std::unique_ptr<VertexArray>  _vao;
    std::unique_ptr<VertexBuffer> _vbo;
    std::unique_ptr<IndexBuffer>  _ibo;
    std::unique_ptr<VertexArray>  _fvao;
    std::unique_ptr<VertexBuffer> _fvbo;
    std::unique_ptr<Shader>       _fboShader;
    std::unique_ptr<Shader>       _shader;
    std::unique_ptr<FrameBuffer>  _frameBuffer;
    unsigned int                  _fboWidth  = 128;
    unsigned int                  _fboHeight = 128;

    Camera _camera;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
DepthBufferApp::DepthBufferApp()
    : Application( { 1920, 1080, "tinygraphics", true } )
{
    _camera.SetPosition( glm::vec3( 0.0f, -1.0f, 0.0f ) );
    _camera.SetLookAt( glm::vec3( 0.0f, 0.0f, 0.0f ) );
    _camera.SetType( Camera::PROJECTION::ORTHOGRAPHIC );

    float              xmin      = -1.0f;
    float              xmax      = 1.0f;
    float              zmin      = -1.0f;
    float              zmax      = 1.0f;
    std::vector<float> positions = { xmin, 0.0f, zmin, 0.0f, -1.0f, 0.0f,
                                     xmax, 0.0f, zmin, 0.0f, -1.0f, 0.0f,
                                     xmax, 0.0f, zmax, 0.0f, -1.0f, 0.0f,
                                     xmin, 0.0f, zmax, 0.0f, -1.0f, 0.0f };

    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

    _ibo = std::make_unique<IndexBuffer>(
        indices.data(), static_cast<unsigned int>( indices.size() ) );
    {
        VertexBufferLayout layout;
        layout.Push<float>( 3 );
        layout.Push<float>( 3 );
        _fvao = std::make_unique<VertexArray>();
        _fvbo = std::make_unique<VertexBuffer>(
            positions.data(),
            static_cast<unsigned int>( positions.size() * sizeof( float ) ) );
        _fvao->AddBuffer( *_fvbo, layout );
    }

    {

        // clang-format off
        positions = { xmin, 0.0f, zmin, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                      xmax, 0.0f, zmin, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                      xmax, 0.0f, zmax, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                      xmin, 0.0f, zmax, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f };

        // clang-format on
        VertexBufferLayout layout;
        layout.Push<float>( 3 );
        layout.Push<float>( 3 );
        layout.Push<float>( 2 );
        _vao = std::make_unique<VertexArray>();
        _vbo = std::make_unique<VertexBuffer>(
            positions.data(),
            static_cast<unsigned int>( positions.size() * sizeof( float ) ) );
        _vao->AddBuffer( *_vbo, layout );
    }

    _frameBuffer = std::make_unique<FrameBuffer>(
        _fboWidth, _fboHeight, FrameBuffer::DEPTH );

    {
        std::filesystem::path vShaderFile = exeDir / "shaders/v3n3_vert.glsl";
        std::filesystem::path fShaderFile = exeDir / "shaders/v3n3_frag.glsl";
        _fboShader = std::make_unique<Shader>( vShaderFile, fShaderFile );
    }

    {
        std::filesystem::path vShaderFile = exeDir / "shaders/v3n3t2_vert.glsl";
        std::filesystem::path fShaderFile = exeDir / "shaders/v3n3t2_frag.glsl";
        _shader = std::make_unique<Shader>( vShaderFile, fShaderFile );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void DepthBufferApp::Update()
{
    Renderer renderer;

    // ---- render to fbo ----
    _frameBuffer->Bind();      // bind FBO FIRST
    glEnable( GL_DEPTH_TEST ); // required for depth to be written
    glViewport( 0, 0, _fboWidth, _fboHeight );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_DEPTH_BUFFER_BIT ); // clears the FBO's depth attachment

    _fboShader->Bind();
    auto one = glm::identity<glm::mat4>();
    _fboShader->SetUniformMat4f(
        "u_M",
        glm::rotate( one, GetCurrentTime(), glm::vec3( 0.0f, 0.0f, 1.0f ) ) );
    _fboShader->SetUniformMat4f( "u_V", _camera.GetViewMatrix() );
    _fboShader->SetUniformMat4f(
        "u_P", _camera.GetProjectionMatrix( 1.0f, 0.1f, 2.0f ) );
    _fboShader->SetUniform3f( "u_LightPos", _camera.GetPosition() );
    _fboShader->SetUniform3f( "u_CameraPos", _camera.GetPosition() );
    _fboShader->SetUniform3f( "u_Color", glm::vec3( 0.0f, 0.0f, 1.0f ) );
    renderer.Draw( *_fvao, *_ibo, *_fboShader );
    _frameBuffer->Unbind();

    // ---- draw to main framebuffer ----
    int width, height;
    GetWindowSize( width, height );
    glViewport( 0, 0, width, height );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    _shader->Bind();
    float aspectRatio = ( 1.0f * width ) / height;
    _shader->SetUniformMat4f( "u_M", glm::identity<glm::mat4>() );
    _shader->SetUniformMat4f( "u_V", _camera.GetViewMatrix() );
    _shader->SetUniformMat4f(
        "u_P", _camera.GetProjectionMatrix( aspectRatio, 0.1f, 100.0f ) );
    _shader->SetUniform3f( "u_LightPos", _camera.GetPosition() );
    _shader->SetUniform3f( "u_CameraPos", _camera.GetPosition() );
    _frameBuffer->BindTexture();
    _shader->SetUniform1i( "u_Texture", 0 ); // integer for sampler

    renderer.Draw( *_vao, *_ibo, *_shader );

    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
    exeDir = std::filesystem::absolute( argv[0] ).parent_path();

    DepthBufferApp app;
    app.Run();
    return 0;
}
