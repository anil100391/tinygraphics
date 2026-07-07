#include <format>
#include <cstring>
#include <iostream>

#include <glad/glad.h>

#include <shader.h>
#include <renderer.h>
#include <textrenderer.h>
#include <utils/meshgl.h>

#include <glm/ext/matrix_transform.hpp>

#define WRITE_FONT_ATLAS 0

#if WRITE_FONT_ATLAS
#include <stb/stb_image_write.h>
#endif // WRITE_FONT_ATLAS

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern unsigned char JetBrainsMonoNLNerdFontMono_Thin[];
extern unsigned int  JetBrainsMonoNLNerdFontMono_Thin_size;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TextRenderer::SetFont( const std::filesystem::path &fontFile )
{
    if ( _context->fontFile != fontFile )
    {
        _context->fontFile = fontFile;
        _context->dirty    = true;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TextRenderer::SetfontSize( float size )
{
    if ( _context->fontSize != size )
    {
        _context->fontSize = size;
        _context->dirty    = true;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TextRenderer::Draw( const Renderer    &renderer,
                         const std::string &text,
                         unsigned int       px,
                         unsigned int       py )
{
    UpdateContext();

    bool depthTest = glIsEnabled( GL_DEPTH_TEST );
    if ( depthTest )
    {
        glDisable( GL_DEPTH_TEST );
    }

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // bind texture
    unsigned int texSlot = 0u;
    glActiveTexture( GL_TEXTURE0 + texSlot );
    glBindTexture( GL_TEXTURE_2D, _context->textureID );

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
    static auto shader = std::make_unique<Shader>(
        std::filesystem::path{ "/home/nebula/code/tinygraphics/shaders/v2t2_vert.glsl" },
        std::filesystem::path{ "/home/nebula/code/tinygraphics/shaders/v2t2_frag.glsl" } );

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    auto toNDC = [viewport](float px, float py, float &ndcX, float &ndcY)
    {
        ndcX = -1.0f + (2.0f * px) / viewport[2];
        ndcY = -1.0f + (2.0f * (viewport[3] - py)) / viewport[3];
    };

    shader->Bind();
    shader->SetUniform1i( "u_Texture", texSlot );
    // assume orthographic projection with units = screen pixels, origin at top
    // left
    float penx = px;
    float peny = py;
    glm::vec3 color((1.0f * rand())/RAND_MAX, (1.0f * rand())/RAND_MAX, (1.0f * rand()) / RAND_MAX);
    color = glm::vec3(0.7f, 0.7f, 0.0f);
    for ( const auto &c : text )
    {
        if ( c < 32 )
        {
            continue;
        }

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            _context->fontMetrics,
            512,    // tex width
            512,    // tex height
            c - 32, // char index
            &penx,
            &peny, // pointers to current position in screen pixel space
            &q,    // quad to draw
            1 );   // 1=opengl & d3d10+,0=d3d9
                   //

        toNDC(q.x0, q.y0, xmin, ymin);
        toNDC(q.x1, q.y1, xmax, ymax);
        // clang-format off
        std::vector<float> vertices{ xmin, ymin, q.s0, q.t0,
                                     xmax, ymin, q.s1, q.t0,
                                     xmax, ymax, q.s1, q.t1,
                                     xmin, ymax, q.s0, q.t1 };
        glMesh->vbo()->BufferData(vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof( float )), GL_DYNAMIC_DRAW);
        // clang-format on
        shader->SetUniformMat4f( "u_M", glm::mat4( 1.0f ) );
        shader->SetUniform3f( "u_Color", color );
        renderer.Draw( *glMesh->vao(), *glMesh->ibo(), *shader );
    }

    glBindTexture( GL_TEXTURE_2D, 0 );

    if ( depthTest )
    {
        glEnable( GL_DEPTH_TEST );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TextRenderer::UpdateContext()
{
    if ( !_context )
    {
        _context           = std::make_unique<TextRenderer::Context>();
        _context->fontSize = 32.0;
        _context->dirty    = true;
    }

    if ( !_context->dirty && _context->textureID != 0 )
    {
        return;
    }

    std::vector<unsigned char> ttfBuffer( 1 << 20, 0 );
    std::vector<unsigned char> tempBitmap( 512 * 512, 0 );

    const auto &fontFile = _context->fontFile;

    if ( !fontFile.string().empty() )
    {
        auto file = fopen( fontFile.c_str(), "rb" );
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

    stbtt_BakeFontBitmap( ttfBuffer.data(),
                          0,
                          32.0,
                          tempBitmap.data(),
                          512,
                          512,
                          32,
                          96,
                          _context->fontMetrics );

#if WRITE_FONT_ATLAS
    std::string output = "/home/nebula/font.png";
    stbi_write_png( output.c_str(), 512, 512, 1, tempBitmap.data(), 512 );
#endif // WRITE_FONT_ATLAS

    glGenTextures( 1, &_context->textureID );
    glBindTexture( GL_TEXTURE_2D, _context->textureID );
    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  GL_RED,
                  512,
                  512,
                  0,
                  GL_RED,
                  GL_UNSIGNED_BYTE,
                  tempBitmap.data() );

    // can free temp_bitmap at this point
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    _context->dirty = false;
}
