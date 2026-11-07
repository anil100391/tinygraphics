#pragma once

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
    Uranus,
    Neptune,
    Pluto,
    Count
};

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
    virtual ~PlanetExplorer() = default;

    virtual void Update() override;
    virtual bool OnEvent( tgrEvent &evt ) override;

private:

    void Render();

    void CreatePlanet( CelestialBody body );
    void CreateShader();

    void UpdateShaderCommon();
    void UpdateShader( CelestialBody body );

    void DrawPlanet();
    void ShowPlanetInfo();

    void CreateSphereMesh();

    std::unique_ptr<tgrMeshGL>  _glMesh;
    std::unique_ptr<tgrShader>  _shader;
    std::unique_ptr<tgrTexture> _texture;
    std::unique_ptr<tgrTexture> _normalMap;

    tgrCamera _camera;

    float _rotation = 0.0f;
    bool  _running  = true;
};
