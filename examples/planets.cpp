#include <numbers>
#include <filesystem>

#include "planets.h"

#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <tinygraphics/vertexbufferlayout.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static std::filesystem::path exeDir;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlanetExplorer::PlanetExplorer( const WindowProperties &wprops )
    : Application( wprops, true )
{
    _camera.SetPosition( glm::vec3( 1.5f, 0.0f, 0.0f ) );
    _camera.SetLookAt( glm::vec3( 0.0f, 0.0f, 0.0f ) );
    _camera.SetType( Camera::PROJECTION::ORTHOGRAPHIC );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::Update()
{
    if ( _running )
    {
        _rotation -= 0.0005f;
    }

    Render();

    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    // ImGui::Begin( "Debug" );
    // auto pos = _camera.GetPosition();
    // ImGui::Text( "Camera: (%f, %f, %f)", pos.x, pos.y, pos.z );
    // ImGui::End();
    //
    // ImGui::Begin( "Performance" );
    //
    // ImGuiIO &io = ImGui::GetIO();
    // ImGui::Text( "FPS: %.1f", io.Framerate );
    // ImGui::Text( "Frame time: %.3f ms", 1000.0f / io.Framerate );
    //
    // ImGui::End();
    //
    // ImGui::Render();
    // ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool PlanetExplorer::OnEvent( Event &evt )
{
    if ( evt.GetEventType() == EventType::KeyPressed &&
         static_cast<KeyEvent &>( evt ).GetKeyCode() == 32 )
    {
        _running = !_running;
    }

    return Application::OnEvent( evt );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::Render()
{
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_CULL_FACE );

    // Clear screen to draw
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Create planet if need be
    if ( !_glMesh )
    {
        CreatePlanet();
    }

    // Draw planet
    _glMesh->vbo()->Bind();
    _glMesh->vao()->Bind();
    _glMesh->ibo()->Bind();

    // uniforms
    if ( !_shader )
    {
        CreateShader();
    }

    _shader->Bind();
    int width, height;
    GetWindowSize( width, height );
    float aspectRatio = ( 1.0f * width ) / height;

    glm::vec3 lightPos = _camera.GetPosition();
    _shader->SetUniform3f( "u_LightPos", lightPos );

    glm::mat4 model = glm::rotate(
        glm::mat4( 1.0f ), _rotation, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    glm::mat4 view = _camera.GetViewMatrix();
    glm::mat4 projection =
        _camera.GetProjectionMatrix( aspectRatio, 0.1f, 100.0f );

    _shader->SetUniformMat4f( "u_M", model );
    _shader->SetUniformMat4f( "u_V", view );
    _shader->SetUniformMat4f( "u_P", projection );
    _shader->SetUniform1i( "u_Emission", 0 );
    _texture->Bind( 0 );
    _shader->SetUniform1i( "u_Texture", 0 );

    _normalMap->Bind( 1 );
    _shader->SetUniform1i( "u_NormalMap", 1 );

    static Renderer r;
    r.Draw( *_glMesh->vao(), *_glMesh->ibo(), *_shader );

    r.DrawText( "MARS (Planet)", 32, 32 );
    r.DrawText( "Diameter:          6794 km", 32, 64 );
    r.DrawText( "Distance from Sun: 228 million km (1.52 AU)", 32, 96 );
    r.DrawText( "Year Length:       687 Earth Days", 32, 128 );
    r.DrawText( "Day Length:        24.6 Hours", 32, 160 );
    r.DrawText( "Axial Tilt:        25.19 degrees", 32, 192 );
    r.DrawText( "Surface Gravity:   3.71 m/(s*s)", 32, 224 );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::CreatePlanet()
{
    if ( _glMesh )
    {
        // already created
        return;
    }

    // Create a sphere mesh
    // vertices
    std::vector<float> vertices;
    const float        radius                = 0.5f;
    const size_t       numHorizontalSegments = 64;
    const size_t       numVerticalSegments   = numHorizontalSegments * 2;
    for ( size_t ii = 0; ii < numHorizontalSegments; ++ii )
    {
        float v     = ( 1.0f * ii / ( numHorizontalSegments - 1 ) );
        float theta = v * std::numbers::pi; // angle between z axis and point

        for ( size_t jj = 0; jj < numVerticalSegments; ++jj )
        {
            float u = ( 1.0f * jj / ( numVerticalSegments - 1 ) ); // [0, 1]
            float phi =
                u * 2 * std::numbers::pi; // angle between x axis and projection
                                          // of point on x-y plane
                                          //
            float z      = radius * std::cos( theta );
            float xyproj = radius * std::sin( theta );
            float y      = xyproj * std::sin( phi );
            float x      = xyproj * std::cos( phi );

            // position
            vertices.push_back( x );
            vertices.push_back( y );
            vertices.push_back( z );

            // normal
            vertices.push_back( x / radius );
            vertices.push_back( y / radius );
            vertices.push_back( z / radius );

            // tex coordinates
            vertices.push_back( u );
            vertices.push_back( 1.0f - v );

            // tangent
            vertices.push_back(
                -y / xyproj ); // i * (cosphi + i*sinphi) = -sinphi + icosphi
            vertices.push_back( x / xyproj );
            vertices.push_back( 0.0f );

            // bitangent = normal cross tangent = [x/r,       y/r,      z/r]
            //                                  * [-y/xyproj, x/xyproj, 0]
            vertices.push_back( -x * z / ( radius * xyproj ) );
            vertices.push_back( -y * z / ( radius * xyproj ) );
            vertices.push_back( ( x * x + y * y ) / ( radius * xyproj ) );
        }
    }

    VertexBufferLayout layout;
    layout.Push<float>( 3 ); // position
    layout.Push<float>( 3 ); // normal
    layout.Push<float>( 2 ); // texture coordiante
    layout.Push<float>( 3 ); // tangent
    layout.Push<float>( 3 ); // bitangent

    // triangles
    std::vector<unsigned int> conn;
    for ( size_t ii = 0; ii < numHorizontalSegments - 1; ++ii )
    {
        for ( size_t jj = 0; jj < numVerticalSegments - 1; ++jj )
        {
            conn.push_back( ( ii * numVerticalSegments ) + jj );
            conn.push_back( ( ( ii + 1 ) * numVerticalSegments ) + jj + 1u );
            conn.push_back( ( ii * numVerticalSegments ) + jj + 1u );

            conn.push_back( ( ii * numVerticalSegments ) + jj );
            conn.push_back( ( ( ii + 1 ) * numVerticalSegments ) + jj );
            conn.push_back( ( ( ii + 1 ) * numVerticalSegments ) + jj + 1u );
        }
    }

    _glMesh = std::make_unique<MeshGL>( vertices, layout, conn );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::CreateShader()
{
    if ( _shader )
    {
        return;
    }

    std::filesystem::path vertexShader( exeDir.string() +
                                        "/shaders/v3n3t2t3bt3_vert.glsl" );
    std::filesystem::path fragmentShader( exeDir.string() +
                                          "/shaders/v3n3t2t3bt3_frag.glsl" );
    _shader = std::make_unique<Shader>( vertexShader, fragmentShader );
    _texture =
        std::make_unique<Texture>( exeDir.string() + "/assets/mars.jpg" );
    _normalMap = std::make_unique<Texture>( exeDir.string() +
                                            "/assets/mars_normal.jpg" );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, const char *argv[] )
{
    exeDir = std::filesystem::absolute( argv[0] ).parent_path();

    WindowProperties wprops;
    wprops._maximized = true;
    wprops._notitle   = true;

    PlanetExplorer exp( wprops );
    exp.Run();
    return 0;
}
