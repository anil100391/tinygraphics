#include <filesystem>
#include <numbers>
#include "planets.h"
#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <vertexbufferlayout.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static std::filesystem::path assetsDir;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlanetExplorer::PlanetExplorer( const WindowProperties &wprops )
    : Application( wprops, true )
{
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlanetExplorer::~PlanetExplorer()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::Update()
{
    if ( _animateCamera )
    {
        auto time = GetCurrentTime();
        time = 0.0f;
        auto dist = 1.5f;
        glm::vec3 cameraPos( dist * std::cos( time ), dist * std::sin( time ), 0.0f );
        _camera.SetPosition( cameraPos );
        _camera.SetLookAt( glm::vec3( 0.0f, 0.0f, 0.0f ) );
    }

    Render();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Debug");
    auto pos = _camera.GetPosition();
    ImGui::Text( "Camera: (%f, %f, %f)", pos.x, pos.y, pos.z );
    ImGui::End();
 
    ImGui::Begin( "Performance" );

    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text( "FPS: %.1f", io.Framerate );
    ImGui::Text( "Frame time: %.3f ms", 1000.0f / io.Framerate );

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool PlanetExplorer::OnEvent( Event &evt )
{
    if ( evt.GetEventType() == EventType::KeyPressed &&
         static_cast<KeyEvent &>(evt).GetKeyCode() == 32 )
    {
        _animateCamera = false;
    }

    return Application::OnEvent(evt);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::Render()
{
    // Clear screen to draw
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create planet if need be
    if ( !_glMesh )
    {
        CreatePlanet();
    }

    // Draw planet
    _glMesh->vbo()->Bind();
    _glMesh->vao()->Bind();
    _glMesh->ibo()->Bind();

    // uniforms
    if (!_shader )
    {
        CreateShader();
    }

    _shader->Bind();
    int width, height;
    GetWindowSize(width, height);
    _shader->SetUniform2f( "u_Size", static_cast<float>(width), static_cast<float>(height) );
    double x, y;
    GetCursorPosition( x, y );
    _shader->SetUniform2f( "u_MousePos", static_cast<float>(x), static_cast<float>(y) );
    _shader->SetUniform3f( "u_Color", 1.0f, 1.0f, 1.0f );

    glm::vec3 lightPos = _camera.GetPosition();
    _shader->SetUniform3f( "u_LightPos", lightPos );

    glm::mat4 model = glm::rotate( glm::mat4( 1.0f ), -0.25f * GetCurrentTime(), glm::vec3( 0.0f, 0.0f, 1.0f ) );
    glm::mat4 view = _camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective( glm::radians( 45.0f ),
                                         static_cast<float>( width ) / static_cast<float>( height ),
                                         0.1f,
                                         100.0f );
    _shader->SetUniformMat4f( "u_M", model );
    _shader->SetUniformMat4f( "u_V", view );
    _shader->SetUniformMat4f( "u_P", projection );
    _shader->SetUniform1i( "u_Emission", 0 );
    _texture->Bind( 0 );
    _shader->SetUniform1i( "u_Texture", 0 );

    _normalMap->Bind( 1 );
    _shader->SetUniform1i( "u_NormalMap", 1 );

    Renderer r;
    r.Draw(*_glMesh->vao(), *_glMesh->ibo(), *_shader);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::CreatePlanet()
{
    if ( _glMesh )
    {
        // already created
        return;
    }

    // Create a sphere mesh
    // vertices
    std::vector<float> vertices;
    const float radius                 = 0.5f;
    const size_t numHorizontalSegments = 64;
    const size_t numVerticalSegments   = numHorizontalSegments * 2;
    for ( size_t ii = 0; ii < numHorizontalSegments; ++ii )
    {
        float v     = (1.0f * ii / (numHorizontalSegments - 1));
        float theta = v * std::numbers::pi; // angle between z axis and point

        for ( size_t jj = 0; jj < numVerticalSegments; ++jj )
        {
            float u   = (1.0f * jj / (numVerticalSegments - 1));   // [0, 1]
            float phi = u * 2 * std::numbers::pi; // angle between x axis and projection of point on x-y plane
                                                  //
            float z      = radius * std::cos(theta);
            float xyproj = radius * std::sin(theta);
            float y      = xyproj * std::sin(phi);
            float x      = xyproj * std::cos(phi);

            // position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normal
            vertices.push_back(x / radius);
            vertices.push_back(y / radius);
            vertices.push_back(z / radius);

            // tex coordinates
            vertices.push_back(u);
            vertices.push_back(1.0f - v);

            // tangent
            vertices.push_back(-y / xyproj); // i * (cosphi + i*sinphi) = -sinphi + icosphi
            vertices.push_back( x / xyproj);
            vertices.push_back(0.0f );

            // bitangent = normal cross tangent = [x/r,       y/r,      z/r]
            //                                  * [-y/xyproj, x/xyproj, 0]
            vertices.push_back( -x * z / (radius * xyproj) );
            vertices.push_back( -y * z / (radius * xyproj) );
            vertices.push_back( (x * x + y * y) / (radius * xyproj) );
        }
    }

    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // texture coordiante
    layout.Push<float>(3); // tangent
    layout.Push<float>(3); // bitangent

    // triangles
    std::vector<unsigned int> conn;
    for ( size_t ii = 0; ii < numHorizontalSegments - 1; ++ii )
    {
        for ( size_t jj = 0; jj < numVerticalSegments - 1; ++jj )
        {
            conn.push_back((ii * numVerticalSegments) + jj);
            conn.push_back((ii * numVerticalSegments) + jj + 1u);
            conn.push_back(((ii + 1) * numVerticalSegments) + jj + 1u);

            conn.push_back((ii * numVerticalSegments) + jj);
            conn.push_back(((ii + 1) * numVerticalSegments) + jj + 1u);
            conn.push_back(((ii + 1) * numVerticalSegments) + jj);
        }
    }

    _glMesh = std::make_unique<MeshGL>( vertices, layout, conn );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlanetExplorer::CreateShader()
{
    if ( _shader )
    {
        return;
    }

    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec3 normal;\n"
        "layout(location = 2) in vec2 texCoord;\n"
        "layout(location = 3) in vec3 tangent;\n"
        "layout(location = 4) in vec3 bitangent;\n"
        "uniform mat4 u_M;\n"
        "uniform mat4 u_V;\n"
        "uniform mat4 u_P;\n"
        "out vec3 fragNormal;\n"
        "out vec3 fragPos;\n"
        "out vec2 fragTexCoord;\n"
        "out mat3 TBN;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = u_P * u_V * u_M * vec4(position, 1.0);\n"
        "    fragPos = vec3(u_M * vec4(position, 1.0));\n"
        "    fragNormal = mat3(transpose(inverse(u_M))) * normal;\n"
        "    fragTexCoord = texCoord;\n"
        "    vec3 T = normalize( vec3( u_M * vec4( tangent, 0.0 ) ) );\n"
        "    vec3 B = normalize( vec3( u_M * vec4( bitangent, 0.0 ) ) );\n"
        "    vec3 N = normalize( vec3( u_M * vec4( normal, 0.0 ) ) );\n"
        "    TBN = mat3( T, B, N );\n"
        "}"
    ;

    std::string fragmentShader =
        "#version 330 core\n"
        "in vec3 fragNormal;\n"
        "in vec3 fragPos;\n"
        "in vec2 fragTexCoord;\n"
        "in mat3 TBN;\n"
        "uniform vec3 u_Color;\n"
        "uniform vec3 u_LightPos;\n"
        "uniform vec3 u_CameraPos;\n"
        "uniform int  u_Emission;\n"
        "uniform sampler2D u_Texture;\n"
        "uniform sampler2D u_NormalMap;\n"
        "void main()\n"
        "{\n"
        "    if ( u_Emission != 0 )\n"
        "    {\n"
        "        gl_FragColor = vec4(u_Color, 1.0);\n"
        "        return;\n"
        "    }\n"

        "    vec3 lightColor = vec3(1.0);\n"
        "    // AMBIENT\n"
        "    float ambientFactor = 0.02;\n"
        "    vec3 ambient = ambientFactor * lightColor;\n"

        "    // DIFFUSE\n"
        "    // obtain normal from normal map in range [0,1]\n"
        "    vec3 norm = texture(u_NormalMap, fragTexCoord).rgb;\n"
        "    // transform normal vector to range [-1,1]\n"
        "    norm = norm * 2.0 - 1.0;\n"
        "    norm.xy *= 2.0;\n"
        "    // recompute z (instead of distorting it)"
        "    norm.z = sqrt( 1.0 - clamp( dot( norm.xy, norm.xy ), 0.0, 1.0 ) );\n"

        "    norm = normalize(TBN * norm);\n"

        "    // vec3 norm = normalize(fragNormal);\n"
        "    // norm = normalize(fragNormal);\n"

        "    vec3 lightDir = normalize(u_LightPos - fragPos);\n"
        "    float diffuseFactor = max(dot(norm, lightDir), 0.0);\n"
        "    vec3 diffuse = diffuseFactor * lightColor;\n"
        "    gl_FragColor = vec4(vec3(diffuse), 1.0);\n"
        "    // return;"

        "    // SPECULAR\n"
        "    float specularStrength = 0.5;\n"
        "    vec3 viewDir = normalize(u_CameraPos - fragPos);\n"
        "    vec3 reflectDir = reflect(lightDir, norm);\n"
        "    vec3 specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32) * lightColor;\n"

        "    vec3 color = (ambient +  diffuse + specular) * texture(u_Texture, fragTexCoord).rgb * 2;\n"
        "    // vec3 color = (ambient +  diffuse /*+ specular*/) * vec3(1.0);\n"
        "    // float gamma = 2.2;\n"
        "    // color = pow( color, vec3( 1.0 / gamma ) );\n"
        "    // color = pow( color, vec3(0.5) );\n"
        "    gl_FragColor = vec4(0.5 * color, 1.0);\n"
        "}"
    ;

    _shader = std::make_unique<Shader>( vertexShader, fragmentShader );

    _texture = std::make_unique<Texture>( assetsDir.string() + "/mars.jpg" );
    _normalMap = std::make_unique<Texture>( assetsDir.string() + "/mars_normal.jpg" );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main(int argc, const char* argv[])
{
    assetsDir = std::filesystem::absolute(argv[0]).parent_path() / "assets";

    WindowProperties wprops;
    wprops._maximized = true;
    wprops._notitle   = true;

    PlanetExplorer exp( wprops );
    exp.Run();
    return 0;
}
