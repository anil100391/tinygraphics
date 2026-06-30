#include <memory>
#include <iostream>
#include <filesystem>

#include "viewer.h"

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
Viewer::Viewer( const WindowProperties &wprops ) : Application( wprops, true )
{
    glEnable( GL_BLEND );
    glEnable( GL_CULL_FACE );
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
    if ( !_glMesh )
    {
        LoadMesh( "/home/nebula/code/tinygraphics/examples/suzanne.obj" );
    }

    // Render
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    if ( !_shader )
    {
        std::string vertexShader;
        {
            std::ifstream file(
                "/home/nebula/code/tinygraphics/shaders/v3n3_vert.glsl" );
            std::string line;
            while ( std::getline( file, line ) )
            {
                vertexShader += line;
                vertexShader += "\n";
            }
        }

        std::string fragmentShader;
        {
            std::ifstream file(
                "/home/nebula/code/tinygraphics/shaders/v3n3_frag.glsl" );
            std::string line;
            while ( std::getline( file, line ) )
            {
                fragmentShader += line;
                fragmentShader += "\n";
            }
        }

        _shader = std::make_unique<Shader>( vertexShader, fragmentShader );
    }

    _glMesh->vao()->Bind();
    _glMesh->ibo()->Bind();
    _shader->Bind();

    _shader->SetUniformMat4f( "u_M", glm::mat4( 1.0f ) );
    _shader->SetUniformMat4f( "u_V", _camera.GetViewMatrix() );
    int width, height;
    GetWindowSize( width, height );
    float ar = ( 1.0f * width ) / height;
    _shader->SetUniformMat4f( "u_P",
                              _camera.GetProjectionMatrix( ar, 0.1f, 100.0f ) );
    _shader->SetUniform3f( "u_LightPos", _camera.GetPosition() );
    _shader->SetUniform3f( "u_CameraPos", _camera.GetPosition() );
    _shader->SetUniform3f( "u_Color", glm::vec3( 0.5f, 0.5f, 0.5f ) );

    Renderer r;
    r.Draw( *_glMesh->vao(), *_glMesh->ibo(), *_shader );

    // ImGui render
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin( "Performance" );
    ImGui::Text( "FPS: %.1f", ImGui::GetIO().Framerate );
    ImGui::End();

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
bool Viewer::LoadMesh( const std::filesystem::path &filePath )
{
    Mesh mesh( filePath );

    MeshBufferObjects::IOParams mbosIO;
    if ( !MeshBufferObjects::Get( mesh, mbosIO ) )
    {
        std::cout << "error bo get\n";
    }

    VertexBufferLayout layout;
    layout.Push<float>( 3u ); // vertex
    if ( mbosIO.hasNormals )
    {
        layout.Push<float>( 3u );
    }

    if ( mbosIO.hasTextureCoords )
    {
        layout.Push<float>( 2u );
    }

    _glMesh = std::make_unique<MeshGL>(
        mbosIO.vertexAttribs, layout, mbosIO.connectivity );

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
int main( int argc, const char *argv[] )
{
    WindowProperties wprops;
    wprops._maximized = true;
    wprops._width     = 1920;
    wprops._height    = 1080;
    wprops._title     = "Tinygraphics Viewer";

    Viewer viewer( wprops );
    viewer.Run();

    return 0;
}
