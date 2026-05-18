#pragma once

#include <memory>
#include <tinygraphics/app.h>

#include <shader.h>
#include <camera.h>
#include <texture.h>
#include <utils/meshgl.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class PlanetExplorer : public Application
{
public:

    PlanetExplorer( const WindowProperties &wprops );
    virtual ~PlanetExplorer();

    virtual void Update() override;
    virtual bool OnEvent( Event &evt ) override;

private:

    void Render();

    void CreatePlanet();
    void CreateShader();

    std::unique_ptr<MeshGL>  _glMesh;
    std::unique_ptr<Shader>  _shader;
    std::unique_ptr<Texture> _texture;
    std::unique_ptr<Texture> _normalMap;

    Camera _camera;
    bool   _animateCamera = true;
};
