#include <memory>
#include <format>
#include <filesystem>

#include "viewer.h"
#include "glm/ext/matrix_transform.hpp"

#include <tinygraphics/utils/mesh.h>
#include <tinygraphics/utils/meshbufferobjects.h>

#include <tinygraphics/renderer.h>
#include <tinygraphics/vertexbufferlayout.h>

#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static std::filesystem::path exeDir;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Viewer::Viewer( const WindowProperties &wprops ) : Application( wprops, true )
{
    glEnable( GL_BLEND );
    // glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Viewer::~Viewer()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Viewer::Update()
{
    if ( !_glSubject )
    {
        LoadSubject();
    }

    if ( !_glGround )
    {
        LoadGround();
    }

    // Render
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    if ( !_shader )
    {
        std::filesystem::path vertexShader( exeDir.string() +
                                            "/assets/shaders/v3n3_vert.glsl" );
        std::filesystem::path fragmentShader(
            exeDir.string() + "/assets/shaders/v3n3_frag.glsl" );
        _shader = std::make_unique<Shader>( vertexShader, fragmentShader );
    }

    _glSubject->vao()->Bind();
    _glSubject->ibo()->Bind();
    _shader->Bind();

    static float lastFrameTime = GetCurrentTime();
    float        time          = GetCurrentTime();
    float        dt            = time - lastFrameTime;
    if ( time > lastFrameTime )
    {
        dt            = time - lastFrameTime;
        lastFrameTime = time;
    }
    _shader->SetUniformMat4f(
        "u_M",
        glm::rotate( glm::mat4( 1.0f ), time, glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
    _shader->SetUniformMat4f( "u_V", _camera.GetViewMatrix() );
    int width, height;
    GetWindowSize( width, height );
    float ar = ( 1.0f * width ) / height;
    _shader->SetUniformMat4f( "u_P",
                              _camera.GetProjectionMatrix( ar, 0.1f, 100.0f ) );
    _shader->SetUniform3f( "u_LightPos", _camera.GetPosition() );
    _shader->SetUniform3f( "u_CameraPos", _camera.GetPosition() );
    _shader->SetUniform3f( "u_Color", glm::vec3( 0.32f, 0.31f, 0.26f ) );

    static Renderer r;
    // draw subject
    r.Draw( *_glSubject->vao(), *_glSubject->ibo(), *_shader );

    // draw ground
    auto groundMtx = glm::mat4( 1.0f );
    groundMtx      = glm::rotate( groundMtx,
                                  -std::numbers::pi_v<float> / 2,
                                  glm::vec3( 1.0f, 0.0f, 0.0f ) );
    groundMtx = glm::translate( groundMtx, glm::vec3( 0.0f, 0.0f, -1.0f ) );

    _shader->SetUniformMat4f( "u_M", groundMtx );
    _shader->SetUniform3f( "u_Color", glm::vec3( 0.412, 0.03f, 0.03f ) );
    r.Draw( *_glGround->vao(), *_glGround->ibo(), *_shader );
    r.Draw( std::format( "FPS: {}", ImGui::GetIO().Framerate ), 32, 32 );
    r.Draw( "Did I hear a squeak!", 32, 64 );

    // ImGui render
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::Begin( "Performance" );
    // ImGui::Text( "FPS: %.1f", ImGui::GetIO().Framerate );
    // ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Viewer::OnEvent( Event &evt )
{
    _camera.OnEvent( evt );
    return Application::OnEvent( evt );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Viewer::GetGLBuffers( const Mesh                  &mesh,
                           MeshBufferObjects::IOParams &mbosIO,
                           VertexBufferLayout          &layout )
{
    if ( !MeshBufferObjects::Get( mesh, mbosIO ) )
    {
        return false;
    }

    layout.Push<float>( 3u ); // vertex
    if ( mbosIO.hasNormals )
    {
        layout.Push<float>( 3u );
    }

    if ( mbosIO.hasTextureCoords )
    {
        layout.Push<float>( 2u );
    }

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Viewer::CreateGLMesh( const Mesh &mesh, std::unique_ptr<MeshGL> &glMesh )
{
    MeshBufferObjects::IOParams mbosIO;
    VertexBufferLayout          layout;
    if ( !GetGLBuffers( mesh, mbosIO, layout ) )
    {
        return false;
    }

    glMesh = std::make_unique<MeshGL>(
        mbosIO.vertexAttribs, layout, mbosIO.connectivity );
    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Viewer::LoadSubject()
{
    auto asset =
        std::filesystem::path( exeDir.string() + "/assets/models/suzanne.obj" );

    Mesh mesh( asset );
    if ( !CreateGLMesh( mesh, _glSubject ) )
    {
        return false;
    }

    box3 box    = mesh.bbox();
    auto center = box.center();
    _camera.SetLookAt( glm::vec3( center[0], center[1], center[2] ) );
    _camera.SetPosition( _camera.GetLookAt() +
                         glm::vec3( 0.0, 0.0, 1.0 ) * 1.25f * box.radius() );
    _camera.SetUpVec( glm::vec3( 0.0f, 1.0f, 0.0f ) );
    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Viewer::LoadGround()
{
    auto asset =
        std::filesystem::path( exeDir.string() + "/assets/models/ground.obj" );

    Mesh mesh( asset );
    return CreateGLMesh( mesh, _glGround );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, const char *argv[] )
{
    exeDir = std::filesystem::path( argv[0] ).parent_path();

    WindowProperties wprops;
    wprops._maximized = true;
    wprops._width     = 1920;
    wprops._height    = 1080;
    wprops._title     = "Tinygraphics Viewer";

    Viewer viewer( wprops );
    viewer.Run();

    return 0;
}
