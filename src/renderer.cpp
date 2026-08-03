#include <log.h>
#include <renderer.h>
#include <textrenderer.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void GLClearError()
{
    while ( glGetError() != GL_NO_ERROR )
    {
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool GLLogCall( const char *function, const char *file, int line )
{
    while ( GLenum error = glGetError() )
    {
        Log( LogLevel::Warning,
             "[OpenGL Error] ({}): {} {}: {}",
             error,
             function,
             file,
             line );
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
tgrRenderer::~tgrRenderer()
{
    delete _textRenderer;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrRenderer::Clear() const
{
    glClear( GL_COLOR_BUFFER_BIT );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrRenderer::Draw( const tgrVertexArray &va,
                        const tgrIndexBuffer &ib,
                        const tgrShader      &shader,
                        DRAW_MODE             dm ) const
{
    Draw( va, ib, shader, 0, dm );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrRenderer::DrawInstanced( const tgrVertexArray &va,
                                 const tgrIndexBuffer &ib,
                                 const tgrShader      &shader,
                                 unsigned int          count,
                                 DRAW_MODE             dm ) const
{
    Draw( va, ib, shader, count, dm );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
GLenum tgrRenderer::GetGLDrawMode( DRAW_MODE dm ) const
{
    switch ( dm )
    {
    case DRAW_MODE::TRIANGLES:
        return GL_TRIANGLES;
    case DRAW_MODE::LINES:
        return GL_LINES;
    default:
        return GL_INVALID_ENUM;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrRenderer::Draw( const tgrVertexArray &va,
                        const tgrIndexBuffer &ib,
                        const tgrShader      &shader,
                        unsigned int          instanceCount,
                        DRAW_MODE             dm ) const
{
    va.Bind();
    ib.Bind();
    shader.Bind();

    GLenum mode = GetGLDrawMode( dm );
    if ( instanceCount == 0 )
    {
        glDrawElements( mode, ib.GetCount(), GL_UNSIGNED_INT, nullptr );
    }
    else
    {
        glDrawElementsInstanced(
            mode, ib.GetCount(), GL_UNSIGNED_INT, nullptr, instanceCount );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrRenderer::SetFont( const std::filesystem::path &fontFile )
{
    GetOrCreateTextRenderer()->SetFont( fontFile );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrRenderer::SetFontSize( float fontSize )
{
    GetOrCreateTextRenderer()->SetFontSize( fontSize );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrRenderer::DrawText( const std::string &text,
                            unsigned int       px,
                            unsigned int       py,
                            const glm::vec3   &color )
{
    if ( text.empty() )
    {
        return;
    }

    GetOrCreateTextRenderer()->Draw( *this, text, px, py, color );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
tgrTextRenderer *tgrRenderer::GetOrCreateTextRenderer()
{
    if ( !_textRenderer )
    {
        _textRenderer = new tgrTextRenderer();
    }

    return _textRenderer;
}
