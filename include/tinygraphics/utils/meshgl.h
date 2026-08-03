#ifndef _mesh_gl_h_
#define _mesh_gl_h_

#include <renderer.h>
#include <vertexarray.h>
#include <vertexbuffer.h>
#include <vertexbufferlayout.h>
#include <indexbuffer.h>

#include <vector>
#include <memory>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrMeshGL
{
public:
    tgrMeshGL() = default;

    tgrMeshGL( const std::vector<float>        &vertices,
               const tgrVertexBufferLayout     &layout,
               const std::vector<unsigned int> &conn,
               int                              bufferUsage = GL_STATIC_DRAW );

    ~tgrMeshGL() = default;

    tgrVertexArray *Vao() const
    {
        return _vao.get();
    }

    tgrVertexBuffer *Vbo() const
    {
        return _vbo.get();
    }

    tgrIndexBuffer *Ibo() const
    {
        return _ibo.get();
    }

protected:
    void PopulateBuffers( const std::vector<float>        &vertices,
                          const tgrVertexBufferLayout     &layout,
                          const std::vector<unsigned int> &conn,
                          int bufferUsage = GL_STATIC_DRAW );

    std::unique_ptr<tgrVertexArray>  _vao;
    std::unique_ptr<tgrVertexBuffer> _vbo;
    std::unique_ptr<tgrIndexBuffer>  _ibo;
};

#endif //_mesh_gl_h_
