#ifndef _renderer_h_
#define _renderer_h_

#include <glad/glad.h>

#include <shader.h>
#include <vertexarray.h>
#include <indexbuffer.h>

// clang-format off
#ifdef WIN
    #define ASSERT(x) if (!(x)) __debugBreak();
    #define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#endif
// clang-format on

void GLClearError();
bool GLLogCall( const char *function, const char *file, int line );

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TextRenderer;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    enum DRAW_MODE
    {
        LINES,
        TRIANGLES
    };

    void Clear() const;

    void Draw( const VertexArray &va,
               const IndexBuffer &ib,
               const Shader      &shader,
               DRAW_MODE          dm = TRIANGLES ) const;

    void DrawInstanced( const VertexArray &va,
                        const IndexBuffer &ib,
                        const Shader      &shader,
                        unsigned int       count,
                        DRAW_MODE          dm = TRIANGLES ) const;

    // text rendering
    void SetFont( const std::filesystem::path &fontFile );
    void SetFontSize( float fontSize );
    void DrawText( const std::string &text,
                   unsigned int       px,
                   unsigned int       py,
                   const glm::vec3   &color = { 0.7f, 0.7f, 0.0f } );

private:
    GLenum GetGLDrawMode( DRAW_MODE dm ) const;
    void   Draw( const VertexArray &va,
                 const IndexBuffer &ib,
                 const Shader      &shader,
                 unsigned int       instanceCount,
                 DRAW_MODE          dm ) const;

    TextRenderer *GetOrCreateTextRenderer();
    TextRenderer *_textRenderer = nullptr;
};

#endif // _renderer_h_
