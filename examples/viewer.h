#pragma once

#include <filesystem>
#include <tinygraphics/app.h>
#include <tinygraphics/camera.h>
#include <tinygraphics/shader.h>
#include <tinygraphics/utils/meshgl.h>
#include <tinygraphics/utils/meshbufferobjects.h>

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
    bool LoadMesh( const std::filesystem::path &filePath, Mesh &mesh ) const;
    bool GetGLBuffers( const Mesh                  &mesh,
                       MeshBufferObjects::IOParams &mbosIO,
                       VertexBufferLayout          &layout );
    bool CreateGLMesh( const Mesh &mesh, std::unique_ptr<MeshGL> &glMesh );
    bool LoadSubject();
    bool LoadGround();

    Camera                  _camera;
    std::unique_ptr<MeshGL> _glSubject;
    std::unique_ptr<MeshGL> _glGround;
    std::unique_ptr<Shader> _shader;
};
