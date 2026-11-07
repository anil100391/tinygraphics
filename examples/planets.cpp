#include <format>
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
static tgrRenderer r;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::vector<PlanetsDB::Info> PlanetsDB::s_database;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
constexpr float largestSolarDistance = 5.9064e12f; // Pluto
constexpr float largestRadius        = 6.9634e8f;  // Sun

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlanetsDB::Info PlanetsDB::GetInfo( CelestialBody body, bool normalized )
{
    if ( s_database.empty() )
    {
        PopulateDB();
    }

    auto info = s_database[static_cast<size_t>( body )];

    if ( normalized )
    {
        float rfac = body == CelestialBody::Sun ? 0.01f : 0.001f;
        info.radius /= (rfac * largestSolarDistance);
        info.solarDistance /= largestSolarDistance;
    }

    return info;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetsDB::PopulateDB()
{
    s_database = { { "Sun",
                     "Star",
                     6.9634e8f,
                     1.9891e30f,
                     0.0f,
                     0.0f,
                     25.0f,
                     0.0f,
                     274.0f },
                   { "Mercury",
                     "Planet",
                     2.4394e6f,
                     3.30103e23f,
                     5.7909e10f,
                     87.969f,
                     1407.6f,
                     0.034f,
                     3.70f },
                   { "Venus",
                     "Planet",
                     6.0518e6f,
                     4.86731e24f,
                     1.0821e11f,
                     224.701f,
                     5832.5f,
                     177.4f,
                     8.87f },
                   { "Earth",
                     "Planet",
                     6.3710e6f,
                     5.9722e24f,
                     1.4960e11f,
                     365.256f,
                     23.934f,
                     23.44f,
                     9.81f },
                   { "Mars",
                     "Planet",
                     3.3895e6f,
                     6.4171e23f,
                     2.2794e11f,
                     686.980f,
                     24.623f,
                     25.19f,
                     3.71f },
                   { "Jupiter",
                     "Planet",
                     6.9911e7f,
                     1.89813e27f,
                     7.7857e11f,
                     4332.59f,
                     9.925f,
                     3.13f,
                     23.12f },
                   { "Saturn",
                     "Planet",
                     5.8232e7f,
                     5.6834e26f,
                     1.4335e12f,
                     10759.2f,
                     10.656f,
                     26.73f,
                     8.96f },
                   { "Uranus",
                     "Planet",
                     2.5362e7f,
                     8.6810e25f,
                     2.8725e12f,
                     30688.5f,
                     17.24f,
                     97.77f,
                     8.69f },
                   { "Neptune",
                     "Planet",
                     2.4622e7f,
                     1.02409e26f,
                     4.4951e12f,
                     60182.0f,
                     16.11f,
                     28.32f,
                     11.15f },
                   { "Pluto",
                     "Dwarf Planet",
                     1.1883e6f,
                     1.303e22f,
                     5.9064e12f,
                     90530.0f,
                     15.15f,
                     122.54f,
                     0.66f } };

    s_database.resize( static_cast<size_t>( CelestialBody::Count ) );

    //{
    //    auto &info  = s_database[static_cast<size_t>( CelestialBody::Sun )];
    //    info.name   = "Sun";
    //    info.type   = "Star";
    //    info.radius = 0.0f;
    //    info.mass   = 0.0f;
    //    info.solarDistance = 0.0f;
    //    info.daysInAYear   = 0.0f;
    //    info.hoursInADay   = 0.0f;
    //}

    //{
    //    auto &info  = s_database[static_cast<size_t>( CelestialBody::Mercury
    //    )]; info.name   = "Mercury"; info.radius = 0.0f; info.mass   = 0.0f;
    //    info.solarDistance = 0.0f;
    //    info.daysInAYear   = 0.0f;
    //    info.hoursInADay   = 0.0f;
    //}

    //{
    //    auto &info  = s_database[static_cast<size_t>( CelestialBody::Venus )];
    //    info.name   = "Venus";
    //    info.radius = 0.0f;
    //    info.mass   = 0.0f;
    //    info.solarDistance = 0.0f;
    //    info.daysInAYear   = 0.0f;
    //    info.hoursInADay   = 0.0f;
    //}

    //{
    //    auto &info  = s_database[static_cast<size_t>( CelestialBody::Earth )];
    //    info.name   = "Earth";
    //    info.radius = 0.0f;
    //    info.mass   = 0.0f;
    //    info.solarDistance = 0.0f;
    //    info.daysInAYear   = 0.0f;
    //    info.hoursInADay   = 0.0f;
    //}

    //{
    //    auto &info  = s_database[static_cast<size_t>( CelestialBody::Mars )];
    //    info.name   = "Mars";
    //    info.radius = 6'794'000 / 2;
    //    info.mass   = 0.0f;
    //    info.solarDistance = 228'000'000'000;
    //    info.solarDistance /= 4'000'000'000;
    //    info.daysInAYear    = 687;
    //    info.hoursInADay    = 24.6f;
    //    info.axialTilt      = 25.19f;
    //    info.surfaceGravity = 3.71f;
    //}

    //{
    //    auto &info  = s_database[static_cast<size_t>( CelestialBody::Jupiter
    //    )]; info.name   = "Jupiter"; info.radius = 0.0f; info.mass   = 0.0f;
    //    info.solarDistance = 0.0f;
    //    info.daysInAYear   = 0.0f;
    //    info.hoursInADay   = 0.0f;
    //}

    //{
    //    auto &info  = s_database[static_cast<size_t>( CelestialBody::Saturn
    //    )]; info.name   = "Saturn"; info.radius = 0.0f; info.mass   = 0.0f;
    //    info.solarDistance = 0.0f;
    //    info.daysInAYear   = 0.0f;
    //    info.hoursInADay   = 0.0f;
    //}

    //{
    //    auto &info  = s_database[static_cast<size_t>( CelestialBody::Uranus
    //    )]; info.name   = "Uranus"; info.radius = 0.0f; info.mass   = 0.0f;
    //    info.solarDistance = 0.0f;
    //    info.daysInAYear   = 0.0f;
    //    info.hoursInADay   = 0.0f;
    //}

    //{
    //    auto &info  = s_database[static_cast<size_t>( CelestialBody::Neptune
    //    )]; info.name   = "Neptue"; info.radius = 0.0f; info.mass   = 0.0f;
    //    info.solarDistance = 0.0f;
    //    info.daysInAYear   = 0.0f;
    //    info.hoursInADay   = 0.0f;
    //}

    //{
    //    auto &info  = s_database[static_cast<size_t>( CelestialBody::Pluto )];
    //    info.name   = "Pluto";
    //    info.radius = 0.0f;
    //    info.mass   = 0.0f;
    //    info.solarDistance = 0.0f;
    //    info.daysInAYear   = 0.0f;
    //    info.hoursInADay   = 0.0f;
    //}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlanetExplorer::PlanetExplorer( const tgrWindowProperties &wprops )
    : tgrApplication( wprops, true )
{
    _camera.SetPosition( glm::vec3( -0.0f, 0.0f, 0.0f ) );

    const auto &info = PlanetsDB::GetInfo( CelestialBody::Mars, true );
    _camera.SetLookAt( glm::vec3( info.solarDistance, 0.0f, 0.0f ) );
    _camera.SetType( tgrCamera::PROJECTION::ORTHOGRAPHIC );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::Update()
{
    if ( _running )
    {
        _rotation = -0.5 * GetCurrentTime();
    }

    Render();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
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
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    tgrApplication::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool PlanetExplorer::OnEvent( tgrEvent &evt )
{
    if ( evt.GetEventType() == tgrEventType::KeyPressed &&
         static_cast<tgrKeyEvent &>( evt ).GetKeyCode() == 32 )
    {
        _running = !_running;
    }

    _camera.OnEvent( evt );
    return tgrApplication::OnEvent( evt );
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

    DrawCelestialBodies();

    ShowPlanetInfo();

    int width, height;
    GetWindowSize( width, height );
    auto fps =
        std::format( "FPS: {}", static_cast<int>( ImGui::GetIO().Framerate ) );
    r.DrawText( fps, width - 128, 32, glm::vec3( 0.5f, 0.5f, 0.5f ) );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::CreateSphereMesh()
{
    if ( _sphereGLMesh )
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

    tgrVertexBufferLayout layout;
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

    _sphereGLMesh = std::make_unique<tgrMeshGL>( vertices, layout, conn );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::CreateEllipseMesh( float a, float b, size_t numSegments )
{
    float e = std::sqrt( 1.0f - ( b * b ) / ( a * a ) );
    glm::vec3 focus1( -e * a, 0.0f, 0.0f );
    glm::vec3 focus2( e * a, 0.0f, 0.0f );

    std::vector<float> vertices;
    for ( size_t ii = 0; ii < numSegments; ++ii )
    {
        float theta =
            ( 1.0f * ii / ( numSegments - 1 ) ) * 2 * std::numbers::pi;
        float x = a * std::cos( theta );
        float y = b * std::sin( theta );
        float z = 0.0f;
        
        // position
        vertices.push_back( x );
        vertices.push_back( y );
        vertices.push_back( z );

        // texture coordinate
        vertices.push_back( 0.5f * ( x / a + 1.0f ) );
        vertices.push_back( 0.5f * ( y / b + 1.0f ) );
    }

    tgrVertexBufferLayout layout;
    layout.Push<float>( 3 ); // position
    layout.Push<float>( 2 ); // texture coordinate

    std::vector<unsigned int> conn;
    conn.reserve( 2 * numSegments );
    for ( size_t ii = 0; ii < numSegments; ++ii )
    {
        conn.push_back( ii );
        conn.push_back( ( ii + 1 ) % numSegments );
    }

    _ellipseGLMesh = std::make_unique<tgrMeshGL>( vertices, layout, conn );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::CreateShader()
{
    if ( _planetShader )
    {
        return;
    }

    std::filesystem::path vertexShader( exeDir.string() +
                                        "/shaders/v3n3t2t3bt3_vert.glsl" );
    std::filesystem::path fragmentShader( exeDir.string() +
                                          "/shaders/v3n3t2t3bt3_frag.glsl" );
    _planetShader = std::make_unique<tgrShader>( vertexShader, fragmentShader );

    std::filesystem::path sunFragShader(
        exeDir.string() + "/assets/shaders/sun_frag.glsl" );

    _sunShader = std::make_unique<tgrShader>( vertexShader, sunFragShader );

    // orbit shader
    {
        std::string orbitVShaderSrc = R"(

#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

out vec3 fragPos;
out vec2 fragTexCoord;

void main()
{
    gl_Position = u_P * u_V * u_M * vec4(position, 1.0);
    fragPos = vec3(u_M * vec4(position, 1.0));
    fragTexCoord = texCoord;
})";

        std::string orbitFShaderSrc = R"(
#version 330 core
in vec3 fragPos;
in vec2 fragTexCoord;
void main()
{
    gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
})";

        _orbitShader =
            std::make_unique<tgrShader>( orbitVShaderSrc, orbitFShaderSrc );
    }

    _texture =
        std::make_unique<tgrTexture>( exeDir.string() + "/assets/mars.jpg" );
    _normalMap = std::make_unique<tgrTexture>( exeDir.string() +
                                               "/assets/mars_normal.jpg" );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::unique_ptr<tgrShader>& PlanetExplorer::UpdateShader( CelestialBody body )
{
    if ( !_planetShader )
    {
        CreateShader();
    }

    auto &shaderToUse = body == CelestialBody::Sun ? _sunShader : _planetShader;

    shaderToUse->Bind();

    // ------------------------------------------------------------------------
    // Light and Camera
    // ------------------------------------------------------------------------
    shaderToUse->SetUniform3f( "u_LightPos", glm::vec3( 0.0f, 0.0f, 0.0f ) );
    shaderToUse->SetUniform3f( "u_CameraPos", _camera.GetPosition() );

    // ------------------------------------------------------------------------
    // MVP transforms
    // ------------------------------------------------------------------------
    const auto &info = PlanetsDB::GetInfo( body, true );
    glm::mat4 model =
        glm::translate( glm::mat4( 1.0f ),
                        glm::vec3( info.solarDistance, 0.0f, 0.0f ) ) *
        glm::scale( glm::mat4( 1.0f ),
                    glm::vec3( info.radius, info.radius, info.radius ) );

    shaderToUse->SetUniformMat4f( "u_M", model );
    shaderToUse->SetUniformMat4f( "u_V", _camera.GetViewMatrix() );

    int width, height;
    GetWindowSize( width, height );
    float     aspectRatio = ( 1.0f * width ) / height;
    glm::mat4 projection =
        _camera.GetProjectionMatrix( aspectRatio, 0.005f, 100.0f );
    shaderToUse->SetUniformMat4f( "u_P", projection );

    // ------------------------------------------------------------------------
    // celestial body texture
    // ------------------------------------------------------------------------
    _texture->Bind( 0 );
    shaderToUse->SetUniform1i( "u_Texture", 0 );

    _normalMap->Bind( 1 );
    shaderToUse->SetUniform1i( "u_NormalMap", 1 );
    return shaderToUse;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::unique_ptr<tgrShader> &
PlanetExplorer::UpdateOrbitShader( CelestialBody body )
{
    if ( !_orbitShader )
    {
        CreateShader();
    }

    auto &shaderToUse = _orbitShader;

    shaderToUse->Bind();

    // ------------------------------------------------------------------------
    // MVP transforms
    // ------------------------------------------------------------------------
    const auto &info  = PlanetsDB::GetInfo( body, true );
    glm::mat4   model = glm::scale( glm::mat4( 1.0f ),
                                    glm::vec3( info.solarDistance,
                                               info.solarDistance,
                                               info.solarDistance ) );

    shaderToUse->SetUniformMat4f( "u_M", model );
    shaderToUse->SetUniformMat4f( "u_V", _camera.GetViewMatrix() );

    int width, height;
    GetWindowSize( width, height );
    float     aspectRatio = ( 1.0f * width ) / height;
    glm::mat4 projection =
        _camera.GetProjectionMatrix( aspectRatio, 0.005f, 100.0f );
    shaderToUse->SetUniformMat4f( "u_P", projection );
    return shaderToUse;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::DrawCelestialBodies()
{
    CreateShader();

    BindSphereGeometry( true );
    for ( auto ii = 0u; ii < static_cast<size_t>( CelestialBody::Count ); ++ii )
    {
        auto& shader = UpdateShader( static_cast<CelestialBody>( ii ) );
        r.Draw( *_sphereGLMesh->Vao(), *_sphereGLMesh->Ibo(), *shader );
    }
    BindSphereGeometry( false );

    BindEllipseGeometry( true );
    for ( auto ii = 0u; ii < static_cast<size_t>( CelestialBody::Count ); ++ii )
    {
        UpdateOrbitShader( static_cast<CelestialBody>( ii ) );

        // Draw orbit
        r.Draw( *_ellipseGLMesh->Vao(),
                *_ellipseGLMesh->Ibo(),
                *_orbitShader,
                tgrRenderer::LINES );
    }
    BindEllipseGeometry( false );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::BindSphereGeometry( bool bind )
{
    if ( bind )
    {
        if ( !_sphereGLMesh )
        {
            CreateSphereMesh();
        }

        _sphereGLMesh->Vbo()->Bind();
        _sphereGLMesh->Vao()->Bind();
        _sphereGLMesh->Ibo()->Bind();
    }
    else if ( _sphereGLMesh )
    {
        _sphereGLMesh->Vbo()->Unbind();
        _sphereGLMesh->Vao()->Unbind();
        _sphereGLMesh->Ibo()->Unbind();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::BindEllipseGeometry( bool bind )
{
    if ( bind )
    {
        if ( !_ellipseGLMesh )
        {
            CreateEllipseMesh( 1.0f, 1.0f, 100 );
        }

        _ellipseGLMesh->Vbo()->Bind();
        _ellipseGLMesh->Vao()->Bind();
        _ellipseGLMesh->Ibo()->Bind();
    }
    else if (_ellipseGLMesh)
    {
        _ellipseGLMesh->Vbo()->Unbind();
        _ellipseGLMesh->Vao()->Unbind();
        _ellipseGLMesh->Ibo()->Unbind();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::ShowPlanetInfo()
{
    glm::vec3 titleColor = glm::vec3( 0.0f, 0.8f, 0.8f );
    glm::vec3 descColor  = glm::vec3( 0.8f, 0.8f, 0.0f );

    const auto &info = PlanetsDB::GetInfo( CelestialBody::Mars );

    unsigned int ypos = 0;
    r.SetFontSize( 32.0f );

    r.DrawText( std::format( "{} ({})", info.name, info.type ),
                32,
                ypos += 32,
                titleColor );

    r.SetFontSize( 24.0f );
    r.DrawText(
        std::format( "Diameter:          {:.0f} km", info.radius * 2 / 1000 ),
        32,
        ypos += 24,
        descColor );

    r.DrawText( std::format( "Distance from Sun: {:.0f} million km ({:.2f} AU)",
                             info.solarDistance / 1'000'000'000,
                             info.solarDistance * 6.6846E-12 ),
                32,
                ypos += 24,
                descColor );

    r.DrawText(
        std::format( "Year Length:       {} Earth Days", info.daysInAYear ),
        32,
        ypos += 24,
        descColor );

    r.DrawText( std::format( "Day Length:        {} Hours", info.hoursInADay ),
                32,
                ypos += 24,
                descColor );

    r.DrawText( std::format( "Axial Tilt:        {} degrees", info.axialTilt ),
                32,
                ypos += 24,
                descColor );

    r.DrawText(
        std::format( "Surface Gravity:   {} m/(s*s)", info.surfaceGravity ),
        32,
        ypos += 24,
        descColor );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, const char *argv[] )
{
    exeDir = std::filesystem::absolute( argv[0] ).parent_path();

    tgrWindowProperties wprops;
    wprops._maximized = true;
    wprops._notitle   = true;

    PlanetExplorer exp( wprops );
    exp.Run();
    return 0;
}
