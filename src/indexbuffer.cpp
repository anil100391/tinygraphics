#include <renderer.h>
#include <indexbuffer.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
tgrIndexBuffer::tgrIndexBuffer( const unsigned int *data, unsigned int count )
    : _count( count )
{
    glGenBuffers( 1, &_rendererID );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _rendererID );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                  count * sizeof( unsigned int ),
                  data,
                  GL_STATIC_DRAW );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
tgrIndexBuffer::~tgrIndexBuffer()
{
    glDeleteBuffers( 1, &_rendererID );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrIndexBuffer::Bind() const
{
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _rendererID );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrIndexBuffer::Unbind() const
{
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
unsigned int tgrIndexBuffer::GetCount() const
{
    return _count;
}
