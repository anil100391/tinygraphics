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
Viewer::Viewer( const tgrWindowProperties &wprops )
    : tgrApplication( wprops, true )
{
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
    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );

    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    if ( !_shader )
    {
        std::filesystem::path vertexShader( exeDir.string() +
                                            "/assets/shaders/v3n3_vert.glsl" );
        std::filesystem::path fragmentShader(
            exeDir.string() + "/assets/shaders/v3n3_frag.glsl" );
        _shader = std::make_unique<tgrShader>( vertexShader, fragmentShader );
    }

    _glSubject->Vao()->Bind();
    _glSubject->Ibo()->Bind();
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

    static tgrRenderer r;
    // draw subject
    r.Draw( *_glSubject->Vao(), *_glSubject->Ibo(), *_shader );

    // draw ground
    auto groundMtx = glm::mat4( 1.0f );
    groundMtx      = glm::rotate( groundMtx,
                                  -std::numbers::pi_v<float> / 2,
                                  glm::vec3( 1.0f, 0.0f, 0.0f ) );
    groundMtx = glm::translate( groundMtx, glm::vec3( 0.0f, 0.0f, -1.0f ) );

    _shader->SetUniformMat4f( "u_M", groundMtx );
    _shader->SetUniform3f( "u_Color", glm::vec3( 0.412, 0.03f, 0.03f ) );
    r.Draw( *_glGround->Vao(), *_glGround->Ibo(), *_shader );

    r.DrawText( std::format( "FPS: {}", ImGui::GetIO().Framerate ), 32, 32 );

    // ImGui render
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::Begin( "Performance" );
    // ImGui::Text( "FPS: %.1f", ImGui::GetIO().Framerate );
    // ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    tgrApplication::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Viewer::OnEvent( tgrEvent &evt )
{
    _camera.OnEvent( evt );
    return tgrApplication::OnEvent( evt );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Viewer::GetGLBuffers( const tgrMesh                  &mesh,
                           tgrMeshBufferObjects::IOParams &mbosIO,
                           tgrVertexBufferLayout       &layout )
{
    if ( !tgrMeshBufferObjects::Get( mesh, mbosIO ) )
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
bool Viewer::CreateGLMesh( const tgrMesh &mesh, std::unique_ptr<tgrMeshGL> &glMesh )
{
    tgrMeshBufferObjects::IOParams mbosIO;
    tgrVertexBufferLayout       layout;
    if ( !GetGLBuffers( mesh, mbosIO, layout ) )
    {
        return false;
    }

    glMesh = std::make_unique<tgrMeshGL>(
        mbosIO.vertexAttribs, layout, mbosIO.connectivity );
    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Viewer::LoadSubject()
{
    auto asset =
        std::filesystem::path( exeDir.string() + "/assets/models/suzanne.obj" );

    tgrMesh mesh( asset );
    if ( !CreateGLMesh( mesh, _glSubject ) )
    {
        return false;
    }

    tgrBox3 box    = mesh.BBox();
    auto center = box.Center();
    _camera.SetLookAt( glm::vec3( center[0], center[1], center[2] ) );
    _camera.SetPosition( _camera.GetLookAt() +
                         glm::vec3( 0.0, 0.0, 1.0 ) * 1.25f * box.Radius() );
    _camera.SetUpVec( glm::vec3( 0.0f, 1.0f, 0.0f ) );
    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Viewer::LoadGround()
{
    auto asset =
        std::filesystem::path( exeDir.string() + "/assets/models/ground.obj" );

    tgrMesh mesh( asset );
    return CreateGLMesh( mesh, _glGround );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, const char *argv[] )
{
    exeDir = std::filesystem::path( argv[0] ).parent_path();

    tgrWindowProperties wprops;
    wprops._maximized = true;
    wprops._width     = 1920;
    wprops._height    = 1080;
    wprops._title     = "Tinygraphics Viewer";

    Viewer viewer( wprops );
    viewer.Run();

    return 0;
}
