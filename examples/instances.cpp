#include <memory>
#include <iostream>

#include <tinygraphics/app.h>
#include <tinygraphics/renderer.h>

#include <tinygraphics/camera.h>
#include <tinygraphics/vertexarray.h>
#include <tinygraphics/indexbuffer.h>
#include <tinygraphics/vertexbuffer.h>
#include <tinygraphics/vertexbufferlayout.h>
#include <glm/ext/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class VectorField : public Application
{
public:
    VectorField();
    ~VectorField();

    virtual void Update() override;

    virtual bool OnEvent( Event &evt ) override
    {
        _camera.OnEvent( evt );
        return Application::OnEvent( evt );
    }

private:

    struct Charge
    {
        float amount;
        glm::vec2 pos;
    };

    Charge                        _q0;
    Charge                        _q1;

    glm::vec2 ElectricField( glm::vec2 pos ) const;

    std::unique_ptr<VertexArray>  _vao;
    std::unique_ptr<VertexBuffer> _vbo;
    std::unique_ptr<VertexBuffer> _instanceVBO;
    std::unique_ptr<VertexBuffer> _colorVBO;
    std::unique_ptr<IndexBuffer>  _ibo;
    std::unique_ptr<Shader>       _shader;

    int _numInstances = 100 * 100;
    Camera _camera;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
VectorField::VectorField() : Application( {1080, 1080, "tinygraphics", true} )
{
    _camera.SetPosition( glm::vec3( 0.0f, 0.0f, 1.4f ) );
    _camera.SetLookAt( glm::vec3( 0.0f, 0.0f, 0.0f ) );
    _camera.SetUpVec( glm::vec3( 0.0f, 1.0f, 0.0f ) );
    _camera.SetType( Camera::PROJECTION::ORTHOGRAPHIC );

    // glLineWidth(3.0f);
    _q0.amount = -1.0f;
    _q0.pos    = glm::vec2( -0.5f, 0.0f );

    _q1.amount = 4.0f;
    _q1.pos    = glm::vec2( 0.5f, 0.0f );

    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec2 position;\n"
        "layout(location = 1) in vec4 instanceData;\n"  // xy=pos, z=angle, w=scale
        "layout(location = 2) in float fac;\n"
        "uniform mat4 u_V;\n"
        "uniform mat4 u_P;\n"
        "out float fragFac;\n"
        "void main()\n"
        "{\n"
        "    float c = cos(instanceData.z);\n"
        "    float s = sin(instanceData.z);\n"
        "    vec2 scaled = position * instanceData.w;\n"
        "    vec2 rotated = vec2(scaled.x * c - scaled.y * s, scaled.x * s + scaled.y * c);\n"
        "    vec2 worldPos = rotated + instanceData.xy;\n"
        "    gl_Position = u_P * u_V * vec4(worldPos, 0.0, 1.0);\n"
        "    fragFac   = fac * 6.2831; // radians\n"
        "}\n"
    ;

    std::string fragmentShader =  
        "#version 330 core\n"
        "in float fragFac;\n"
        "void main()\n"
        "{\n"
        "    float hue = fragFac;\n"
        "    vec3 temperature1 = 0.5 + 0.5 * cos( hue + vec3( 0.0, 0.5, 1.0 ) );\n"
        "    vec3 temperature2 = 0.5 + 0.5 * cos(hue + vec3(6.0, 1.0, 2.0));\n"
        "    vec3 rainbow1 = 0.5 + 0.5 * cos(hue + vec3(0.0, 2.0, 4.0));\n"
        "    vec3 rainbow2 = 0.5 + 0.5 * cos(hue + vec3(0.0, 1.0, 3.0));\n"
        "    gl_FragColor = vec4(rainbow2, 1.0);\n"
        "}"
    ;

    float scale = 0.03f;
    std::vector<float> positions = { 0.0f, 0.0f,
                                     1.0f * scale,  0.0f,
                                     0.8f * scale,  0.1f * scale,
                                     0.8f * scale, -0.1f * scale };

    std::vector<unsigned int> indices = { 0, 1, 1, 2, 2, 3, 3, 1 };

    VertexBufferLayout layout;
    layout.Push<float>( 2 );

    // instance array for updating positions
    std::vector<glm::vec4> instanceData( _numInstances );
    std::vector<glm::vec3> colors( _numInstances );
    std::vector<float> scales( _numInstances );
    unsigned int N = std::sqrt( _numInstances );
    for ( unsigned int y = 0; y < N; ++y )
    {
        for ( unsigned int x = 0; x < N; ++x )
        {
            auto p = glm::vec2( -1.0f + (2.0f * x) / (N-1), -1.0f + (2.0f * y) / (N-1));
            auto ef = ElectricField( p );
            auto scale = std::min( glm::length( ef ), 50.0f );
            auto efnorm = glm::normalize( ef );
            auto angle = std::atan2( efnorm.y, efnorm.x );
            instanceData[y * N + x] = glm::vec4( p.x, p.y, angle, std::min( scale / 50.0f, 1.0f ) );
            scales[y * N + x] = scale;
        }
    }

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(positions.data(), static_cast<unsigned int>(positions.size() * sizeof(float)));
    _vao->AddBuffer( *_vbo, layout );

    // instance VBO
    _instanceVBO = std::make_unique<VertexBuffer>( instanceData.data(), static_cast<unsigned int>(instanceData.size() * sizeof( glm::vec4 )) );
    VertexBufferLayout instanceLayout;
    instanceLayout.Push<float>( 4 );
    _vao->AddBuffer( *_instanceVBO, instanceLayout, true );

    // instance VBO
    auto maxScale = *std::max_element( scales.begin(), scales.end() );
    for ( unsigned int y = 0; y < N; ++y )
    {
        for ( unsigned int x = 0; x < N; ++x )
        {
            scales[y * N + x] = std::sqrt( scales[y * N + x] / maxScale );
            auto fac = scales[y * N + x];
            colors[y * N + x] = glm::vec3( fac );
        }
    }

    _colorVBO = std::make_unique<VertexBuffer>( scales.data(), static_cast<unsigned int>( scales.size() * sizeof( float ) ) );
    VertexBufferLayout colorLayout;
    colorLayout.Push<float>( 1 );
    _vao->AddBuffer( *_colorVBO, colorLayout, true );

    _ibo = std::make_unique<IndexBuffer>(indices.data(), static_cast<unsigned int>(indices.size()));
    _shader = std::make_unique<Shader>(vertexShader, fragmentShader);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
VectorField::~VectorField()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void VectorField::Update()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    renderer.Clear();

    // draw
    _shader->Bind();
    _vbo->Bind();
    _vao->Bind();
    _ibo->Bind();
    _shader->Bind();
    int w, h;
    GetWindowSize( w, h );
    auto aspectRatio = static_cast<float>(w) / h;
    _shader->SetUniformMat4f( "u_V", _camera.GetViewMatrix() );
    _shader->SetUniformMat4f( "u_P", _camera.GetProjectionMatrix( aspectRatio, 0.1f, 100.0f ) );
    renderer.DrawInstanced( *_vao, *_ibo, *_shader, _numInstances, Renderer::LINES );

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin( "Performance" );

    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text( "FPS: %.1f", io.Framerate );
    ImGui::Text( "Frame time: %.3f ms", 1000.0f / io.Framerate );
    const auto &cpos = _camera.GetPosition();
    ImGui::Text( "Camera: (%.1f, %.1f, %.1f)", cpos.x, cpos.y, cpos.z );

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
glm::vec2 VectorField::ElectricField( glm::vec2 pos ) const
{
    auto rq0 = pos - _q0.pos;
    auto rq0Len = glm::length( rq0 );
    auto e0 = _q0.amount *rq0 / (rq0Len * rq0Len * rq0Len);

    auto rq1 = pos - _q1.pos;
    auto rq1Len = glm::length( rq1 );
    auto e1 = _q1.amount *rq1 / (rq1Len * rq1Len * rq1Len);

    return e0 + e1;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
    VectorField app;
    app.Run();
    return 0;
}
