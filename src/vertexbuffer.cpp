#include <renderer.h>
#include <vertexbuffer.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
tgrVertexBuffer::tgrVertexBuffer( const void  *data,
                                  unsigned int size,
                                  int          usage )
{
    glGenBuffers( 1, &_rendererID );
    Bind();
    BufferData( data, size, usage );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrVertexBuffer::BufferData( const void  *data,
                                  unsigned int size,
                                  int          usage ) const
{
    Bind();
    glBufferData( GL_ARRAY_BUFFER, size, data, usage );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
tgrVertexBuffer::~tgrVertexBuffer()
{
    glDeleteBuffers( 1, &_rendererID );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrVertexBuffer::Bind() const
{
    glBindBuffer( GL_ARRAY_BUFFER, _rendererID );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrVertexBuffer::Unbind() const
{
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
