#pragma once

#include <filesystem>
#include <tinygraphics/app.h>
#include <tinygraphics/camera.h>
#include <tinygraphics/shader.h>
#include <tinygraphics/utils/meshgl.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Viewer : public Application
{
public:
    Viewer( const WindowProperties &wprops );
    virtual ~Viewer();

    virtual void Update() override;

    virtual bool OnEvent( Event &evt ) override;

private:
    bool LoadMesh( const std::filesystem::path &filePath );

    Camera                  _camera;
    std::unique_ptr<MeshGL> _glMesh;
    std::unique_ptr<Shader> _shader;
};
