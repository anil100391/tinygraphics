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
extern unsigned char JetBrainsMonoNLNerdFontMono_Thin[];
extern unsigned int  JetBrainsMonoNLNerdFontMono_Thin_size;

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
constexpr float near_plane           = 0.005f;     // near clipping plane
constexpr float far_plane            = 100.0f;     // far clipping plane

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct Settings
{
    float speed        = 3600.0f * 24.0f; // 1 s == 1 earth day
    float sunScale     = 100.0f;
    float planetsScale = 500.0f;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Settings settings;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlanetsDB::Info PlanetsDB::GetInfo( CelestialBody body, bool normalized )
{
    if ( s_database.empty() )
    {
        PopulateDB();
    }

    auto info = s_database[ToIndex( body )];

    if ( normalized )
    {
        float scale = body == CelestialBody::Sun ? settings.sunScale
                                                 : settings.planetsScale;
        info.radius = info.radius * scale / largestSolarDistance;
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
                   { "Saturn Rings",
                     "Planet Ring",
                     1.4022e8f,
                     0.0f,
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

    s_database.resize( NumBodies );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class PlanetExplorer::Simulator
{
public:

    Simulator();
    ~Simulator() = default;

    void SetSpeedFactor( float speedFactor )
    {
        _speedFac = speedFactor;
    }

    float GetSpeedFactor() const
    {
        return _speedFac;
    }

    void Update( float time );

    struct Data
    {
        glm::vec3 location;
        float     rotation;
    };

    const Data &GetData( CelestialBody body )
    {
        return _data[ToIndex( body )];
    }

private:

    float             _speedFac = 1.0f;
    float             _time     = 0.0f;
    std::vector<Data> _data;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlanetExplorer::Simulator::Simulator()
{
    _data.resize( NumBodies );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::Simulator::Update( float time )
{
    _time = _speedFac * time;

    for ( auto ii = 0; ii < NumBodies; ++ii )
    {
        auto        body = ToBody( ii );
        const auto &info = PlanetsDB::GetInfo( body, false );

        // body position
        float angularVelAroundSun =
            ( body == CelestialBody::Sun )
                ? 0.0f
                : 2.0f * std::numbers::pi /
                      ( info.daysInAYear * 24.0f * 3600.0f );
        float theta        = _time * angularVelAroundSun;
        _data[ii].location = glm::vec3( info.solarDistance * std::cos( theta ),
                                        info.solarDistance * std::sin( theta ),
                                        0.0f );

        // body rotation about it's own axis
        float angularVelAboutOwnAxis =
            2.0f * std::numbers::pi / ( info.hoursInADay * 3600.0f );
        float phi          = _time * angularVelAboutOwnAxis;
        _data[ii].rotation = phi;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class PlanetExplorer::CameraAnimator
{
public:
    CameraAnimator( tgrCamera &camera ) : _camera( camera )
    {
    }

    ~CameraAnimator() = default;

    void StartAnimation( const glm::vec3 &targetLookAt,
                         float            startTime,
                         float            duration )
    {
        _startLookAt  = _camera.GetLookAt();
        _targetLookAt = targetLookAt;
        _startTime    = startTime;
        _duration     = duration;

        _startPos = _camera.GetPosition();
        float intialDistanceFromTarget =
            glm::length( _startPos - _startLookAt );
        _targetPos =
            _targetLookAt + glm::normalize( _startPos - _targetLookAt ) *
                                intialDistanceFromTarget;
    }

    bool Update( float currentTime )
    {
        if ( currentTime >= _startTime + _duration )
        {
            return false;
        }

        float t = ( currentTime - _startTime ) / _duration;
        t       = std::clamp( t, 0.0f, 1.0f );

        glm::vec3 newLookAt = ( 1.0f - t ) * _startLookAt + t * _targetLookAt;
        _camera.SetLookAt( newLookAt );

        glm::vec3 newPos = ( 1.0f - t ) * _startPos + t * _targetPos;
        _camera.SetPosition( newPos );

        return true;
    }

private:
    tgrCamera &_camera;
    glm::vec3  _startLookAt;
    glm::vec3  _targetLookAt;
    float      _startTime;
    float      _duration;

    // internally computed
    glm::vec3 _startPos;
    glm::vec3 _targetPos;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlanetExplorer::PlanetExplorer( const tgrWindowProperties &wprops )
    : tgrApplication( wprops, true )
{
    ImFontConfig fontConfig{};
    fontConfig.FontDataOwnedByAtlas = false;

    ImFont *font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
        JetBrainsMonoNLNerdFontMono_Thin,
        JetBrainsMonoNLNerdFontMono_Thin_size,
        24.0f,
        &fontConfig );

    ImGui::GetIO().FontDefault = font;

    _camera.SetPosition( glm::vec3( -0.0014663, 0.0356589, 0.00140489 ) );
    _camera.SetPosition( glm::vec3( 0.0f, 0.0f, 0.0140489 ) );
    _camera.SetUpVec( glm::vec3( -0.151733, -0.0486303, 0.987225 ) );
    _camera.SetUpVec( glm::vec3( 0.0f, 1.0f, 0.0f ) );
    _camera.SetType( tgrCamera::PROJECTION::PERSPECTIVE );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlanetExplorer::~PlanetExplorer()
{
    delete _simulator;
    delete _cameraAnimator;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::Update()
{
    if ( _running )
    {
        _rotation = -0.5 * GetCurrentTime();
    }

    if ( !_simulator )
    {
        _simulator = new Simulator;
    }

    _simulator->SetSpeedFactor( settings.speed );
    if ( _running )
    {
        _simulator->Update( GetCurrentTime() );
    }

    // update camera
    if ( !_cameraAnimator || !_cameraAnimator->Update( GetCurrentTime() ) )
    {
        const auto &info = _simulator->GetData( ToBody( _selectedBody ) );
        _camera.SetLookAt( info.location / largestSolarDistance );
    }

    Render();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    const char *bodyNames[NumBodies] = { "Sun",
                                         "Mercury",
                                         "Venus",
                                         "Earth",
                                         "Mars",
                                         "Jupiter",
                                         "Saturn",
                                         "Saturn Rings",
                                         "Uranus",
                                         "Neptune",
                                         "Pluto" };

    ImGui::Begin( "Explore" );
    if ( ImGui::BeginListBox(
             "##bodies",
             ImVec2( -FLT_MIN, (NumBodies + 0.1) * ImGui::GetTextLineHeightWithSpacing() ) ) )
    {
        for ( int n = 0; n < IM_COUNTOF( bodyNames ); n++ )
        {
            bool                 isSelected = ( _selectedBody == n );
            ImGuiSelectableFlags flags       = 0;
            if ( ImGui::Selectable( bodyNames[n], isSelected, flags ) )
            {
                if (!_cameraAnimator)
                {
                    _cameraAnimator = new CameraAnimator( _camera );
                }

                const auto &info = _simulator->GetData( ToBody( n ) );
                _cameraAnimator->StartAnimation( info.location /
                                                     largestSolarDistance,
                                                 GetCurrentTime(),
                                                 2.0f );

                _selectedBody = n;
            }

            // Set the initial focus when opening the combo (scrolling +
            // keyboard navigation focus)
            if ( isSelected )
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    ImGui::Separator();

    ImGui::Text( "Scale Factor" );
    ImGui::SliderFloat( "Sun", &settings.sunScale, 1.0f, 150.0f );
    ImGui::SliderFloat( "Planets", &settings.planetsScale, 1.0f, 1000.0f );

    ImGui::Separator();

    ImGui::Text( "Simulation Speed" );
    static int speed = 3600 * 24;
    ImGui::SliderInt( "Speed", &speed, 1, 3600 * 24 * 10 );
    settings.speed = speed;

    ImGui::End();

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
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
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

// ------------------------------------------------------------------
// ------------------------------------------------------------------
void PlanetExplorer::CreateRingsMesh( float innerRadius, float outerRadius )
{
    if ( _ringsGLMesh )
    {
        // already created
        return;
    }

    // Create a ring mesh
    // vertices
    std::vector<float> vertices;
    const size_t       numRSegments     = 2;
    const size_t       numThetaSegments = 128;
    for ( size_t ii = 0; ii < numThetaSegments; ++ii )
    {
        float u     = ( 1.0f * ii / ( numThetaSegments - 1 ) );
        float cosTheta = std::cos( u * 2 * std::numbers::pi );
        float sinTheta = std::sin( u * 2 * std::numbers::pi );
        for ( size_t jj = 0; jj < numRSegments; ++jj )
        {
            float v = ( 1.0f * jj / ( numRSegments - 1 ) ); // [0, 1]
            float R = ( 1.0f - v ) * innerRadius + v * outerRadius;
            float x = R * cosTheta;
            float y = R * sinTheta;

            // position
            vertices.push_back( x );
            vertices.push_back( y );
            vertices.push_back( 0.0f );

            // normal
            vertices.push_back( 0.0f );
            vertices.push_back( 0.0f );
            vertices.push_back( 1.0f );

            // tex coordinates
            vertices.push_back( 0.0f );
            vertices.push_back( v );
        }
    }

    tgrVertexBufferLayout layout;
    layout.Push<float>( 3 ); // position
    layout.Push<float>( 3 ); // normal
    layout.Push<float>( 2 ); // texture coordiante

    // triangles
    std::vector<unsigned int> conn;
    for ( size_t ii = 0; ii < numThetaSegments - 1; ++ii )
    {
        for ( size_t jj = 0; jj < numRSegments - 1; ++jj )
        {
            conn.push_back( ( ii * numRSegments ) + jj );
            conn.push_back( ( ( ii + 1 ) * numRSegments ) + jj + 1u );
            conn.push_back( ( ii * numRSegments ) + jj + 1u );

            conn.push_back( ( ii * numRSegments ) + jj );
            conn.push_back( ( ( ii + 1 ) * numRSegments ) + jj );
            conn.push_back( ( ( ii + 1 ) * numRSegments ) + jj + 1u );
        }
    }

    _ringsGLMesh = std::make_unique<tgrMeshGL>( vertices, layout, conn );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::CreateEllipseMesh( float a, float b, size_t numSegments )
{
    float     e = std::sqrt( 1.0f - ( b * b ) / ( a * a ) );
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

    // Shader with surface colors only
    {
        std::filesystem::path vertexShader( exeDir.string() +
                                            "/shaders/v3n3t2_vert.glsl" );
        std::filesystem::path fragmentShader( exeDir.string() +
                                              "/shaders/v3n3t2_frag.glsl" );
        _planetShader =
            std::make_unique<tgrShader>( vertexShader, fragmentShader );
    }

    // Shader with surface color and depth map
    {
        std::filesystem::path vertexShader( exeDir.string() +
                                            "/shaders/v3n3t2t3bt3_vert.glsl" );
        std::filesystem::path fragmentShader(
            exeDir.string() + "/shaders/v3n3t2t3bt3_frag.glsl" );
        _planetShaderWithDepthMap =
            std::make_unique<tgrShader>( vertexShader, fragmentShader );
    }

    std::filesystem::path sunVertShader( exeDir.string() +
                                         "/assets/shaders/sun_vert.glsl" );
    std::filesystem::path sunFragShader( exeDir.string() +
                                         "/assets/shaders/sun_frag.glsl" );

    _sunShader = std::make_unique<tgrShader>( sunVertShader, sunFragShader );

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
    gl_FragColor = vec4(0.3, 0.3, 0.3, 0.5);
})";

        _orbitShader =
            std::make_unique<tgrShader>( orbitVShaderSrc, orbitFShaderSrc );
    }

    for ( auto ii = 0; ii < NumBodies; ++ii )
    {
        const auto           &info = PlanetsDB::GetInfo( ToBody( ii ) );
        std::filesystem::path colorTexFile =
            exeDir.string() + "/assets/" + ToLower( info.name ) + ".jpg";
        if ( std::filesystem::exists( colorTexFile ) )
        {
            _textures[ii] = std::make_unique<tgrTexture>( colorTexFile );
        }

        std::filesystem::path normalTexFile =
            exeDir.string() + "/assets/" + ToLower( info.name ) + "_normal.jpg";
        if ( std::filesystem::exists( normalTexFile ) )
        {
            _normalMaps[ii] = std::make_unique<tgrTexture>( normalTexFile );
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::unique_ptr<tgrShader> &
PlanetExplorer::UpdateCelestialBodyShader( CelestialBody body )
{
    if ( !_planetShader )
    {
        CreateShader();
    }

    auto &shaderToUse = GetShader( body );

    shaderToUse->Bind();

    // ------------------------------------------------------------------------
    // Time
    // ------------------------------------------------------------------------
    shaderToUse->SetUniform1f( "u_Time", GetCurrentTime() );

    // ------------------------------------------------------------------------
    // Light and Camera
    // ------------------------------------------------------------------------
    shaderToUse->SetUniform3f( "u_LightPos", glm::vec3( 0.0f, 0.0f, 0.0f ) );
    shaderToUse->SetUniform3f( "u_CameraPos", _camera.GetPosition() );

    // ------------------------------------------------------------------------
    // MVP transforms
    // ------------------------------------------------------------------------
    const auto &info           = PlanetsDB::GetInfo( body, true );
    const auto &simData        = _simulator->GetData( body );
    glm::vec    location = simData.location / largestSolarDistance;
    glm::mat4   model =
        glm::translate( glm::mat4( 1.0f ), location ) * // position
        glm::rotate( glm::mat4( 1.0f ),
                     ToRadians( info.axialTilt ),
                     glm::vec3( 0.0f, -1.0f, 0.0f ) ) * // axial tilt
        glm::rotate( glm::mat4( 1.0f ),
                     simData.rotation,
                     glm::vec3( 0.0f, 0.0f, 1.0f ) ) * // axial rotation
        glm::scale( glm::mat4( 1.0f ),
                    glm::vec3( info.radius, info.radius, info.radius ) ); // size

    shaderToUse->SetUniformMat4f( "u_M", model );
    shaderToUse->SetUniformMat4f( "u_V", _camera.GetViewMatrix() );

    int width, height;
    GetWindowSize( width, height );
    float     aspectRatio = ( 1.0f * width ) / height;
    glm::mat4 projection =
        _camera.GetProjectionMatrix( aspectRatio, near_plane, far_plane );
    shaderToUse->SetUniformMat4f( "u_P", projection );

    // ------------------------------------------------------------------------
    // celestial body texture
    // ------------------------------------------------------------------------
    auto &colorTex = _textures[ToIndex( body )];
    if ( colorTex )
    {
        colorTex->Bind( 0 );
        shaderToUse->SetUniform1i( "u_Texture", 0 );

        auto &normalMap = _normalMaps[ToIndex( body )];
        if ( normalMap )
        {
            normalMap->Bind( 1 );
            shaderToUse->SetUniform1i( "u_NormalMap", 1 );
        }
    }

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
        _camera.GetProjectionMatrix( aspectRatio, near_plane, far_plane );
    shaderToUse->SetUniformMat4f( "u_P", projection );
    return shaderToUse;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::unique_ptr<tgrMeshGL> &PlanetExplorer::GetGLMesh( CelestialBody body,
                                                       bool createIfNull )
{
    switch ( body )
    {
    case CelestialBody::SaturnRings:
    {
        if ( createIfNull && !_ringsGLMesh )
        {
            CreateRingsMesh( 0.5f, 1.0f );
        }

        return _ringsGLMesh;
    }
    default:
    {
        if ( createIfNull && !_sphereGLMesh )
        {
            CreateSphereMesh();
        }

        return _sphereGLMesh;
    }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::unique_ptr<tgrShader> &PlanetExplorer::GetShader( CelestialBody body )
{
    switch ( body )
    {
    case CelestialBody::Sun:  return _sunShader;
    case CelestialBody::Mars: return _planetShaderWithDepthMap;
    default:                  return _planetShader;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::DrawCelestialBodies()
{
    CreateShader();

    // Draw celestial bodies
    for ( auto ii = 0u; ii < NumBodies; ++ii )
    {
        auto  body   = ToBody( ii );

        // Since we want to see 2d rings mesh from both sides
        if ( body == CelestialBody::SaturnRings )
        {
            glDisable( GL_CULL_FACE );
        }

        auto &meshGL = GetGLMesh( body, true );
        BindGLMesh( meshGL, true );
        auto &shader = UpdateCelestialBodyShader( body );

        r.Draw( *meshGL->Vao(), *meshGL->Ibo(), *shader );

        BindGLMesh( meshGL, false );

        if ( body == CelestialBody::SaturnRings )
        {
            glEnable( GL_CULL_FACE );
        }
    }

    // Draw orbits
    BindEllipseGeometry( true );
    for ( auto ii = 0u; ii < NumBodies; ++ii )
    {
        UpdateOrbitShader( ToBody( ii ) );

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
void PlanetExplorer::BindGLMesh( std::unique_ptr<tgrMeshGL> &meshGL, bool bind )
{
    if ( meshGL )
    {
        if ( bind )
        {
            meshGL->Vbo()->Bind();
            meshGL->Vao()->Bind();
            meshGL->Ibo()->Bind();
        }
        else if ( meshGL )
        {
            meshGL->Vbo()->Unbind();
            meshGL->Vao()->Unbind();
            meshGL->Ibo()->Unbind();
        }
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
    else if ( _ellipseGLMesh )
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

    const auto &info = PlanetsDB::GetInfo( ToBody( _selectedBody ) );

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
