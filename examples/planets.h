#pragma once

#include <memory>
#include <tinygraphics/app.h>

#include <tinygraphics/shader.h>
#include <tinygraphics/camera.h>
#include <tinygraphics/texture.h>
#include <tinygraphics/utils/meshgl.h>

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

    void CreatePlanet();
    void CreateShader();

    std::unique_ptr<tgrMeshGL>     _glMesh;
    std::unique_ptr<tgrShader>  _shader;
    std::unique_ptr<tgrTexture> _texture;
    std::unique_ptr<tgrTexture> _normalMap;

    tgrCamera _camera;

    float _rotation = 0.0f;
    bool  _running  = true;
};
