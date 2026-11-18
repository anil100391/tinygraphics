#pragma once

#include <array>
#include <memory>
#include <tinygraphics/app.h>

#include <tinygraphics/shader.h>
#include <tinygraphics/camera.h>
#include <tinygraphics/texture.h>
#include <tinygraphics/utils/meshgl.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
enum class CelestialBody
{
    Sun,
    Mercury,
    Venus,
    Earth,
    Mars,
    Jupiter,
    Saturn,
    SaturnRings,
    Uranus,
    Neptune,
    Pluto,
    Count
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
constexpr int ToIndex( CelestialBody body )
{
    return static_cast<int>( body );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
constexpr CelestialBody ToBody( int index )
{
    if ( index < 0 || index >= static_cast<int>( CelestialBody::Count ) )
    {
        throw std::out_of_range( "Invalid CelestialBody index" );
    }

    return static_cast<CelestialBody>( index );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::string ToLower( std::string s, const char whiteSpaceReplacer = '_' )
{
    std::transform( s.begin(),
                    s.end(),
                    s.begin(),
                    [whiteSpaceReplacer]( unsigned char c )
                    {
                        return std::tolower( c ) == ' ' ? whiteSpaceReplacer
                                                        : std::tolower( c );
                    } );
    return s;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
float ToRadians( float degrees )
{
    return degrees * std::numbers::pi / 180.0f;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
constexpr int NumBodies = ToIndex( CelestialBody::Count );

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class PlanetsDB
{
public:
    struct Info
    {
        std::string name;
        std::string type           = "Planet";
        float       radius         = 0.0f; // meters
        float       mass           = 0.0f; // kilograms
        float       solarDistance  = 0.0f; // meters
        float       daysInAYear    = 0.0f; // earth days
        float       hoursInADay    = 0.0f; // earth hours
        float       axialTilt      = 0.0f; // degrees
        float       surfaceGravity = 0.0f; // m/s^2
    };

    static Info GetInfo( CelestialBody body, bool normalized = false );

private:
    static void PopulateDB();

    static std::vector<Info> s_database;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class PlanetExplorer : public tgrApplication
{
public:
    PlanetExplorer( const tgrWindowProperties &wprops );
    virtual ~PlanetExplorer();

    virtual void Update() override;
    virtual bool OnEvent( tgrEvent &evt ) override;

private:
    void Render();

    void CreateShader();

    std::unique_ptr<tgrShader> &UpdateCelestialBodyShader( CelestialBody body );
    std::unique_ptr<tgrShader> &UpdateOrbitShader( CelestialBody body );
    std::unique_ptr<tgrShader> &UpdateCelestialBodiesPointShader();

    std::unique_ptr<tgrMeshGL>          &GetGLMesh( CelestialBody body,
                                                    bool          createIfNull );
    std::pair<tgrMeshGL *, unsigned int> GetGLPointsMesh();
    std::unique_ptr<tgrShader>          &GetShader( CelestialBody body );

    void DrawCelestialBodies();
    void BindGLMesh( tgrMeshGL *meshGL, bool bind );
    void BindEllipseGeometry( bool bind );
    void ShowPlanetInfo();

    void CreateSphereMesh( float radius );
    void CreateRingsMesh( float innerRadius, float outerRadius );
    void CreateEllipseMesh( float a, float b, size_t numSegments );

    void PlaceCameraOnPlanet( CelestialBody body );

    std::unique_ptr<tgrMeshGL> _sphereGLMesh;
    std::unique_ptr<tgrMeshGL> _ringsGLMesh;
    std::unique_ptr<tgrMeshGL> _ellipseGLMesh;
    std::unique_ptr<tgrShader> _sunShader;
    std::unique_ptr<tgrShader> _planetShaderWithDepthMap;
    std::unique_ptr<tgrShader> _planetShader;
    std::unique_ptr<tgrShader> _orbitShader;
    std::unique_ptr<tgrShader> _pointsShader;

    // for point drawing of bodies if they are smaller than a few pixel
    std::unique_ptr<tgrMeshGL>                         _bodyPointsGL;
    std::array<std::unique_ptr<tgrTexture>, NumBodies> _textures;
    std::array<std::unique_ptr<tgrTexture>, NumBodies> _normalMaps;

    tgrCamera _camera;

    int   _selectedBody = 0;
    float _rotation     = 0.0f;
    bool  _running      = true;

    class Simulator;
    Simulator *_simulator = nullptr;

    class CameraAnimator;
    CameraAnimator *_cameraAnimator = nullptr;
};
