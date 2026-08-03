#include <vertexarray.h>
#include <vertexbuffer.h>
#include <vertexbufferlayout.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
tgrVertexArray::tgrVertexArray()
{
    glGenVertexArrays( 1, &_rendererID );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
tgrVertexArray::~tgrVertexArray()
{
    glDeleteVertexArrays( 1, &_rendererID );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrVertexArray::Bind() const
{
    glBindVertexArray( _rendererID );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrVertexArray::Unbind() const
{
    glBindVertexArray( 0 );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrVertexArray::AddBuffer( const tgrVertexBuffer       &vb,
                                const tgrVertexBufferLayout &layout,
                                bool                         instance )
{
    Bind();
    vb.Bind();
    const auto  &elements = layout.GetElements();
    unsigned int offset   = 0;
    for ( unsigned int ii = 0; ii < elements.size(); ++ii )
    {
        const auto &element = elements[ii];
        glEnableVertexAttribArray( _numAttributes );
        glVertexAttribPointer( _numAttributes,
                               element.count,
                               element.type,
                               element.normalized,
                               layout.GetStride(),
                               (const void *)(uint64_t)offset );
        offset += element.count *
                  tgrVertexBufferElement::GetSizeOfType( element.type );

        if ( instance )
        {
            glVertexAttribDivisor( _numAttributes, 1 );
        }

        ++_numAttributes;
    }
}
