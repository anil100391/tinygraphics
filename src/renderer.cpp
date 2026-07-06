#include <iostream>

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
        std::cout << "[OpenGL Error] (" << error << "):" << function << " "
                  << file << ": " << line << "\n";
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Renderer::~Renderer()
{
    delete _fontRenderer;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Renderer::Clear() const
{
    glClear( GL_COLOR_BUFFER_BIT );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Renderer::Draw( const VertexArray &va,
                     const IndexBuffer &ib,
                     const Shader      &shader,
                     DRAW_MODE          dm ) const
{
    Draw( va, ib, shader, 0, dm );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Renderer::DrawInstanced( const VertexArray &va,
                              const IndexBuffer &ib,
                              const Shader      &shader,
                              unsigned int       count,
                              DRAW_MODE          dm ) const
{
    Draw( va, ib, shader, count, dm );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
GLenum Renderer::GetGLDrawMode( DRAW_MODE dm ) const
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
void Renderer::Draw( const VertexArray &va,
                     const IndexBuffer &ib,
                     const Shader      &shader,
                     unsigned int       instanceCount,
                     DRAW_MODE          dm ) const
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
void Renderer::Draw( const std::string &text, unsigned int px, unsigned int py )
{
    if ( text.empty() )
    {
        return;
    }

    if ( !_fontRenderer )
    {
        _fontRenderer = new TextRenderer();
    }

    _fontRenderer->Draw(*this, text, px, py );
}
