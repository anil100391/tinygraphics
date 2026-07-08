#include <format>
#include <cstring>
#include <iostream>

#include <glad/glad.h>

#include <memory>
#include <renderer.h>
#include <textrenderer.h>
#include <utils/meshgl.h>

#include <glm/ext/matrix_transform.hpp>

#define WRITE_FONT_ATLAS 0

#if WRITE_FONT_ATLAS
#include <stb/stb_image_write.h>
#endif // WRITE_FONT_ATLAS

constexpr static int FIRST_CHAR = 32;
constexpr static int NUM_CHARS  = 96;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern unsigned char JetBrainsMonoNLNerdFontMono_Thin[];
extern unsigned int  JetBrainsMonoNLNerdFontMono_Thin_size;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TextRenderer::SetFont( const std::filesystem::path &fontFile )
{
    _font.fontFile = fontFile;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TextRenderer::SetFontSize( float size )
{
    _font.fontSize = size;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TextRenderer::Draw( const Renderer    &renderer,
                         const std::string &text,
                         unsigned int       px,
                         unsigned int       py,
                         const glm::vec3   &color )
{
    const auto &res = UpdateContext();

    bool depthTest = glIsEnabled( GL_DEPTH_TEST );
    if ( depthTest )
    {
        glDisable( GL_DEPTH_TEST );
    }

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // bind texture
    unsigned int texSlot = 0u;
    res.texture->Bind( 0u );
    res.shader->Bind();
    res.shader->SetUniform1i( "u_Texture", texSlot );

    float xmin = -0.5f;
    float xmax = 0.5f;
    float ymin = xmin;
    float ymax = xmax;
    // clang-format off
    std::vector<float> vertices{ xmin, ymin, 0.0f, 0.0f,
                                 xmax, ymin, 1.0f, 0.0f,
                                 xmax, ymax, 1.0f, 1.0f,
                                 xmin, ymax, 0.0f, 1.0f };
    // clang-format off
    VertexBufferLayout layout;
    layout.Push<float>( 2u ); // pos
    layout.Push<float>( 2u ); // tex coord

    // choose winding such that we don't loose our triangles to back face culling
    const std::vector<unsigned int> conn{ 0, 2, 1, 0, 3, 2 };
    static auto glMesh = std::make_unique<MeshGL>( vertices, layout, conn );

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    auto toNDC = [viewport](float px, float py, float &ndcX, float &ndcY)
    {
        ndcX = -1.0f + (2.0f * px) / viewport[2];
        ndcY = -1.0f + (2.0f * (viewport[3] - py)) / viewport[3];
    };

    // assume orthographic projection with units = screen pixels, origin at top
    // left
    float penx = px;
    float peny = py;
    for ( const auto &c : text )
    {
        if ( c < FIRST_CHAR )
        {
            continue;
        }

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            res.fontMetrics.data(),
            res.texture->GetWidth(),  // tex width
            res.texture->GetHeight(), // tex height
            c - FIRST_CHAR,           // char index
            &penx,
            &peny, // pointers to current position in screen pixel space
            &q,    // quad to draw
            1 );   // 1=opengl & d3d10+,0=d3d9

        toNDC(q.x0, q.y0, xmin, ymin);
        toNDC(q.x1, q.y1, xmax, ymax);
        // clang-format off
        std::vector<float> vertices{ xmin, ymin, q.s0, q.t0,
                                     xmax, ymin, q.s1, q.t0,
                                     xmax, ymax, q.s1, q.t1,
                                     xmin, ymax, q.s0, q.t1 };
        glMesh->vbo()->BufferData(vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof( float )), GL_DYNAMIC_DRAW);
        // clang-format on
        res.shader->SetUniformMat4f( "u_M", glm::mat4( 1.0f ) );
        res.shader->SetUniform3f( "u_Color", color );
        renderer.Draw( *glMesh->vao(), *glMesh->ibo(), *res.shader );
    }

    glBindTexture( GL_TEXTURE_2D, 0 );

    if ( depthTest )
    {
        glEnable( GL_DEPTH_TEST );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const TextRenderer::FontResource &TextRenderer::UpdateContext()
{
    auto [it, inserted] = _fontCache.emplace( _font, FontResource{} );
    if ( !inserted )
    {
        const auto &fontResource = it->second;
        assert( !fontResource.fontMetrics.empty() && fontResource.shader &&
                fontResource.texture );
        return it->second;
    }

    auto &fontResource = it->second;

    // Read the font
    std::vector<unsigned char> ttfBuffer( 1 << 20, 0 );
    std::vector<unsigned char> tempBitmap( 512 * 512, 0 );

    const auto &fontFile = _font.fontFile;
    if ( !fontFile.string().empty() )
    {
        auto file = fopen( fontFile.string().c_str(), "rb" );
        if ( !file )
        {
            std::cout << std::format(
                "{:8}: Failed to open {}\n", "Error", fontFile.string() );
        }

        std::fread( ttfBuffer.data(), 1, ttfBuffer.size(), file );
        std::fclose( file );
    }
    else
    {
        // default font
        std::memcpy(
            ttfBuffer.data(),
            &JetBrainsMonoNLNerdFontMono_Thin[0],
            std::min( JetBrainsMonoNLNerdFontMono_Thin_size,
                      static_cast<unsigned int>( ttfBuffer.size() ) ) );
    }

    stbtt_fontinfo info;
    int            ok = stbtt_InitFont( &info, ttfBuffer.data(), 0 );
    if ( !ok )
    {
        std::cout << std::format( "{:8}: Failed to open {}\n",
                                  "Error",
                                  "font initialization failed" );
    }

    int texWidth  = 512;
    int texHeight = 512;
    fontResource.fontMetrics.resize( 96u );

    stbtt_BakeFontBitmap( ttfBuffer.data(),
                          0, // font location (use offset=0 for plain .ttf)
                          _font.fontSize,    // height of font in pixels
                          tempBitmap.data(), // pixels
                          texWidth,          // width of bitmap to generate
                          texHeight,         // height of bitmap to generate
                          FIRST_CHAR,        // first char
                          NUM_CHARS,         // number of characters to bake
                          fontResource.fontMetrics.data() );

#if WRITE_FONT_ATLAS
    std::string output = "font.png";
    stbi_write_png( output.c_str(), 512, 512, 1, tempBitmap.data(), 512 );
#endif // WRITE_FONT_ATLAS

    fontResource.shader = GetOrCreateShader();
    fontResource.texture =
        std::make_unique<Texture>( tempBitmap.data(), texWidth, texHeight, 1 );

    return fontResource;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::shared_ptr<Shader> TextRenderer::GetOrCreateShader()
{
    for ( const auto &i : _fontCache )
    {
        if ( i.second.shader )
            return i.second.shader;
    }

    std::string vshaderSource =
        "#version 330 core\n"
        "layout(location = 0) in vec2 position;\n"
        "layout(location = 1) in vec2 texCoord;\n"
        "out vec2 fragTexCoord;\n"
        "uniform mat4 u_M;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = u_M * vec4(position, 0.0, 1.0);\n"
        "    fragTexCoord = texCoord;\n"
        "}\n";

    std::string fshaderSource =
        "#version 330 core\n"
        "in vec2 fragTexCoord;\n"
        "uniform sampler2D u_Texture;\n"
        "uniform vec3 u_Color;\n"
        "void main()\n"
        "{\n"
        "    float alpha = texture(u_Texture, fragTexCoord).r;\n"
        "    gl_FragColor = vec4(u_Color, alpha);\n"
        "}\n";

    return std::make_shared<Shader>( vshaderSource, fshaderSource );
}
