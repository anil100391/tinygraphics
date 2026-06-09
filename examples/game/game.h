#include <memory>
#include <filesystem>
#include <tinygraphics/app.h>
#include <tinygraphics/camera.h>
#include <tinygraphics/shader.h>
#include <tinygraphics/utils/meshgl.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Game : public Application
{
public:

    Game(const WindowProperties &wprops, const std::filesystem::path &exeDir);
    virtual ~Game() = default;

    virtual void Update() override;
    virtual bool OnEvent(Event &evt) override;

private:

    void SetupCamera();
    void SetupGeometry();
    void SetupShader();

    Camera                  _camera;
    std::unique_ptr<Shader> _shader; 
    std::unique_ptr<MeshGL> _glMesh; 
    std::filesystem::path   _exeDir;

    std::string             _fragmentShaderSource;
};
