#include <iostream>

#include <app.h>
#include <renderer.h>

#include <camera.h>
#include <vertexarray.h>
#include <indexbuffer.h>
#include <vertexbuffer.h>
#include <vertexbufferlayout.h>

#include <utils/mesh.h>
#include <utils/meshbufferobjects.h>

// #include <imgui.h>
// #include <imgui_impl_glfw.h>
// #include <imgui_impl_opengl3.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const char *glsl_version = "#version 130";

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestsApplication : public Application
{
public:
    TestsApplication();
    ~TestsApplication();

    virtual void Update() override;

    virtual bool OnEvent( Event &evt ) override
    {
        return true;
    }

private:

    void MakeSphereMesh( unsigned int segments, unsigned int rings, float radius );

    std::vector<float>            _vertices; // 3 pos, 3 normals per vertex
    std::vector<unsigned int>     _indices;

    std::unique_ptr<VertexArray>  _vao;
    std::unique_ptr<VertexBuffer> _vbo;
    std::unique_ptr<IndexBuffer>  _ibo;
    std::unique_ptr<Shader>       _shader;

    Camera                        _camera;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestsApplication::TestsApplication() : Application( {1920, 1080, "tinygraphics", true} )
{
    /*
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL( _window, true );
    ImGui_ImplOpenGL3_Init( glsl_version );

    auto &io = ImGui::GetIO();
    auto font = io.Fonts->AddFontFromFileTTF( "res/fonts/Open_Sans/OpenSans-Regular.ttf", 16.0f );

    auto& style = ImGui::GetStyle();
    style.ChildRounding = 0.0f;
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    // style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.56f, 0.24, 1.0f);
    // style.Colors[ImGuiCol_ResizeGrip]    = ImVec4(0.08f, 0.56f, 0.24, 1.0f);
    // style.Colors[ImGuiCol_Button]        = ImVec4(0.08f, 0.56f, 0.24, 1.0f);
    */

    glEnable( GL_DEPTH_TEST );
    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec3 normal;\n"
        "uniform mat4 u_M;\n"
        "uniform mat4 u_V;\n"
        "uniform mat4 u_P;\n"
        "out vec3 fragNormal;\n"
        "out vec3 fragPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = u_P * u_V * u_M * vec4(position, 1.0);\n"
        "    fragPos = vec3(u_M * vec4(position, 1.0));\n"
        "    fragNormal = normal;\n"
        "}"
    ;

    std::string fragmentShader =
        "#version 330 core\n"
        "in vec3 fragNormal;\n"
        "in vec3 fragPos;\n"
        "uniform vec3 u_Color;\n"
        "uniform vec3 u_LightPos;\n"
        "uniform vec3 u_CameraPos;\n"
        "void main()\n"
        "{\n"
        "    vec3 lightColor = vec3(1.0);\n"
        "    // AMBIENT\n"
        "    float ambientFactor = 0.05;\n"
        "    vec3 ambient = ambientFactor * lightColor;\n"

        "    // DIFFUSE\n"
        "    vec3 norm = normalize(fragNormal);\n"
        "    vec3 lightDir = normalize(u_LightPos - fragPos);\n"
        "    float diffuseFactor = max(dot(norm, lightDir), 0.0);\n"
        "    vec3 diffuse = diffuseFactor * lightColor;\n"

        "    // SPECULAR\n"
        "    float specularStrength = 0.5;\n"
        "    vec3 viewDir = normalize(u_CameraPos - fragPos);\n"
        "    vec3 reflectDir = reflect(lightDir, norm);\n"
        "    vec3 specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32) * lightColor;\n"

        "    vec3 color = (ambient +  diffuse /*+ specular*/) * u_Color;\n"
        "    gl_FragColor = vec4(color, 1.0);\n"
        "}"
    ;

    MakeSphereMesh( 64, 32, 0.5f );
    VertexBufferLayout layout;
    layout.Push<float>( 3 );
    layout.Push<float>( 3 );
    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(_vertices.data(), static_cast<unsigned int>(_vertices.size() * sizeof(float)));
    _vao->AddBuffer( *_vbo, layout );
    _ibo = std::make_unique<IndexBuffer>(_indices.data(), static_cast<unsigned int>(_indices.size()));
    _shader = std::make_unique<Shader>(vertexShader, fragmentShader);

    _camera.SetPosition( glm::vec3( -1.0f, -1.0f, 7.0f ) );
    _camera.SetLookAt( glm::vec3( 0.0f, 0.0f, 0.0f ) );
    _camera.SetUpVec( glm::vec3( 0.0f, 1.0f, 0.0f ) );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestsApplication::~TestsApplication()
{
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestsApplication::Update()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /*
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow();

    if ( ImGui::CollapsingHeader( "Render Statistics" ) )
    {
        ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)",
                     1000.0f / ImGui::GetIO().Framerate,
                     ImGui::GetIO().Framerate );
    }

    ImGui::Separator();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
    */

    // uniforms
    _shader->Bind();
    int width, height;
    GetWindowSize(width, height);
    _shader->SetUniform2f( "u_Size", static_cast<float>(width), static_cast<float>(height) );
    double x, y;
    GetCursorPosition( x, y );
    _shader->SetUniform2f( "u_MousePos", static_cast<float>(x), static_cast<float>(y) );
    _shader->SetUniform3f( "u_Color", 1.0f, 1.0f, 1.0f );
    _shader->SetUniform3f( "u_LightPos", -2.0f, -2.0f, 3.0f );

    glm::mat4 model = glm::mat4( 1.0f );
    glm::mat4 view = _camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective( glm::radians( 45.0f ),
                                         static_cast<float>( width ) / static_cast<float>( height ),
                                         0.1f,
                                         100.0f );
    _shader->SetUniformMat4f( "u_M", model );
    _shader->SetUniformMat4f( "u_V", view );
    _shader->SetUniformMat4f( "u_P", projection );

    // draw
    renderer.Draw( *_vao, *_ibo, *_shader );
    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestsApplication::MakeSphereMesh( unsigned int segments,
                                       unsigned int rings,
                                       float radius )
{
   _vertices.clear();
   _indices.clear();

   for (unsigned int y = 0; y <= rings; ++y)
   {
       for (unsigned int x = 0; x <= segments; ++x)
       {
           float u = static_cast<float>(x) / segments;
           float v = static_cast<float>(y) / rings;

           // Spherical coordinates
           float theta = u * 2.0f * M_PI;
           float phi = v * M_PI;

           // Convert to Cartesian coordinates
           float sinPhi = sin(phi);
           float cosPhi = cos(phi);
           float sinTheta = sin(theta);
           float cosTheta = cos(theta);

           _vertices.push_back(radius * sinPhi * cosTheta);   // x
           _vertices.push_back(radius * cosPhi);              // y
           _vertices.push_back(radius * sinPhi * sinTheta);   // z

           // Normals (same as positions for a sphere)
           _vertices.push_back(sinPhi * cosTheta);
           _vertices.push_back(cosPhi);
           _vertices.push_back(sinPhi * sinTheta);
       }
   }

   // Generate indices
   for (unsigned int y = 0; y < rings; ++y)
   {
       for (unsigned int x = 0; x < segments; ++x)
       {
           unsigned int i0 = y * segments + x;
           unsigned int i1 = y * segments + x + 1;
           unsigned int i2 = (y + 1) * segments + x + 1;
           unsigned int i3 = (y + 1) * segments + x;

           // Two triangles per quad
           _indices.push_back( i0 );
           _indices.push_back( i1 );
           _indices.push_back( i2 );
           _indices.push_back( i2 );
           _indices.push_back( i3 );
           _indices.push_back( i0 );
       }
   }

   // write obj file for debugging
   std::ofstream out("sphere.obj");
   for (size_t i = 0; i < _vertices.size(); i += 6)
   {
       out << "v " << _vertices[i] << " " << _vertices[i + 1] << " " << _vertices[i + 2] << "\n";
       out << "vn " << _vertices[i + 3] << " " << _vertices[i + 4] << " " << _vertices[i + 5] << "\n";
   }
   for (size_t i = 0; i < _indices.size(); i += 3)
   {
       out << "f " << _indices[i] + 1 << " " << _indices[i + 1] + 1 << " " << _indices[i + 2] + 1 << "\n";
   }
   out.close();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
    TestsApplication app;
    app.Run();
    return 0;
}
