#ifndef _shader_h_
#define _shader_h_

#include <tuple>
#include <string>
#include <filesystem>
#include <unordered_map>

#include <glm/glm.hpp>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Shader
{
public:
    // @params
    // filePath: file with both vertex and fragment shader.
    //           we expect "#shader vertex" to be present before vertex shader
    //           and "#shader fragement" to be present before fragment shader
    //
    // For example
    //
    //          #shader vertex
    //          #version 330 core"
    //          layout(location = 0) in vec3 position;"
    //          layout(location = 1) in vec3 normal;"
    //          uniform mat4 u_MVP;"
    //          void main()
    //          {
    //              gl_Position = u_MVP * vec4(position, 1.0);
    //          }
    //
    //          #shader fragment
    //          #version 330 core
    //          void main()
    //          {
    //              gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //          }
    Shader( const std::filesystem::path &filepath );

    // @params
    // vertexShaderFilePath  : vertex shader source file
    // fragmentShaderFilePath: fragment shader source file
    Shader( const std::filesystem::path &vertexShaderFilePath,
            const std::filesystem::path &fragmentShaderFilePath );

    // @params
    // vertexSource  : vertex shader source as a string
    // fragmentSource: fragment shader source as a string
    Shader( const std::string &vertexSource,
            const std::string &fragmentSource );

    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Set uniforms
    void SetUniform1i( const std::string &name, int i0 );
    void SetUniform1f( const std::string &name, float f0 );
    void SetUniform2f( const std::string &name, float f0, float f1 );
    void SetUniform3f( const std::string &name, float f0, float f1, float f2 );
    void SetUniform4f( const std::string &name,
                       float              f0,
                       float              f1,
                       float              f2,
                       float              f3 );

    void SetUniform3f( const std::string &name, const glm::vec3 &v );
    void SetUniform4f( const std::string &name, const glm::vec4 &v );

    void SetUniformMat4f( const std::string &name, const glm::mat4 &mat );

private:
    std::tuple<std::string, std::string>
    ParseShader( const std::filesystem::path &filepath );
    std::tuple<std::string, std::string>
                 ParseShader( const std::filesystem::path &vertexShaderPath,
                              const std::filesystem::path &fragmentShaderPath );
    unsigned int CompileShader( unsigned int type, const std::string &source );
    unsigned int CreateShader( const std::string &vertexShader,
                               const std::string &fragmentShader );
    int          GetUniformLocation( const std::string &name );

    unsigned int                         _rendererID = 0;
    std::unordered_map<std::string, int> _uniformLocationCache;
};

#endif // _shader_h_
