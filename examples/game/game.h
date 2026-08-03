#include <memory>
#include <filesystem>
#include <tinygraphics/app.h>
#include <tinygraphics/camera.h>
#include <tinygraphics/shader.h>
#include <tinygraphics/utils/meshgl.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Game : public tgrApplication
{
public:
    Game( const tgrWindowProperties   &wprops,
          const std::filesystem::path &exeDir );
    virtual ~Game() = default;

    virtual void Update() override;
    virtual bool OnEvent( tgrEvent &evt ) override;

private:
    void SetupCamera();
    void SetupGeometry();
    void SetupShader();

    tgrCamera                  _camera;
    std::unique_ptr<tgrShader> _shader;
    std::unique_ptr<tgrMeshGL>    _glMesh;
    std::filesystem::path      _exeDir;

    std::string _fragmentShaderSource;
};
