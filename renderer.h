#ifndef _renderer_h_
#define _renderer_h_

#include <GL/glew.h>

#include <vertexarray.h>
#include <indexbuffer.h>
#include <shader.h>

#ifdef WIN
    #define ASSERT(x) if (!(x)) __debugBreak();
    #define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#endif

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Renderer
{
public:

    enum DRAW_MODE
    {
        LINES,
        TRIANGLES,
        QUADS
    };

    void Clear() const;

    void Draw( const VertexArray& va,
               const IndexBuffer &ib,
               const Shader &shader,
               DRAW_MODE dm = TRIANGLES ) const;

    void DrawInstanced( const VertexArray& va,
                        const IndexBuffer &ib,
                        const Shader &shader,
                        unsigned int count,
                        DRAW_MODE dm = TRIANGLES ) const;

private:
  
    GLenum GetGLDrawMode( DRAW_MODE dm ) const;
    void Draw( const VertexArray& va,
               const IndexBuffer &ib,
               const Shader &shader,
               unsigned int instanceCount,
               DRAW_MODE dm ) const;
};

#endif // _renderer_h_
