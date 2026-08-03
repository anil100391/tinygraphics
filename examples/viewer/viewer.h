#pragma once

#include <filesystem>
#include <tinygraphics/app.h>
#include <tinygraphics/camera.h>
#include <tinygraphics/shader.h>
#include <tinygraphics/utils/meshgl.h>
#include <tinygraphics/utils/meshbufferobjects.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Viewer : public tgrApplication
{
public:
    Viewer( const tgrWindowProperties &wprops );
    virtual ~Viewer();

    virtual void Update() override;

    virtual bool OnEvent( tgrEvent &evt ) override;

private:
    bool LoadMesh( const std::filesystem::path &filePath, tgrMesh &mesh ) const;
    bool GetGLBuffers( const tgrMesh                  &mesh,
                       tgrMeshBufferObjects::IOParams &mbosIO,
                       tgrVertexBufferLayout       &layout );
    bool CreateGLMesh( const tgrMesh &mesh, std::unique_ptr<tgrMeshGL> &glMesh );
    bool LoadSubject();
    bool LoadGround();

    tgrCamera                  _camera;
    std::unique_ptr<tgrMeshGL>    _glSubject;
    std::unique_ptr<tgrMeshGL>    _glGround;
    std::unique_ptr<tgrShader> _shader;
};
