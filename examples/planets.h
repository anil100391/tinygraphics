#pragma once

#include <array>
#include <memory>
#include <tinygraphics/app.h>

#include <tinygraphics/camera.h>
#include <tinygraphics/shader.h>
#include <tinygraphics/texture.h>
#include <tinygraphics/utils/meshgl.h>

// -----------------------------------------------------------------------------
// TODO:
// -----------------------------------------------------------------------------
// * New Features
//   . Add moons
//   . Sit at specific coordinates on a planet observe sky
//   . Asteroid field
//   . Add a spaceship to navigate between celestial bodies
//   . Annotating bodies
// * Improvements
//   . Improve planet textures (normal maps etc)
//   . Improve sun shader
//   . Improve orbit shader
//   . Timing and speed enhancements
//   . Better camera controls
//   . Better on planet pov
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
enum class CelestialBody
{
    Sun,
    Mercury,
    Venus,
    Earth,
    Moon,
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
constexpr int NumBodies = static_cast<int>( CelestialBody::Count );

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class PlanetsDB
{
public:
    struct Info
    {
        std::string   name;
        std::string   type           = "Planet";
        float         radius         = 0.0f; // meters
        float         mass           = 0.0f; // kilograms
        CelestialBody parent         = CelestialBody::Sun;
        float         parentDistance = 0.0f; // meters
        float         daysInAYear    = 0.0f; // earth days
        float         hoursInADay    = 0.0f; // earth hours
        float         axialTilt      = 0.0f; // degrees
        float         surfaceGravity = 0.0f; // m/s^2
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

    virtual void Update( float deltaTime ) override;
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

    void ComputeCelestialBodyColors();
    void UpdateScreenSides();
    bool IsTooTinyToDrawAs3D( CelestialBody body ) const;
    void DrawCelestialBodies();
    void BindGLMesh( tgrMeshGL *meshGL, bool bind );
    void BindEllipseGeometry( bool bind );
    void ShowPlanetInfo();

    void CreateSphereMesh( float radius );
    void CreateRingsMesh( float innerRadius, float outerRadius );
    void CreateEllipseMesh( float a, float b, size_t numSegments );

    void PlaceCameraOnPlanet( CelestialBody body );

    bool IsPaused() const
    {
        return _userPause || _internalPause;
    }

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
    std::array<float, NumBodies>                       _screenSizes;
    std::array<glm::vec3, NumBodies>                   _bodyColors;

    tgrCamera _camera;

    int   _selectedBody  = 0;
    float _rotation      = 0.0f;
    bool  _userPause     = false;
    bool  _internalPause = false;

    class Simulator;
    Simulator *_simulator = nullptr;

    class CameraAnimator;
    CameraAnimator *_cameraAnimator = nullptr;
};
