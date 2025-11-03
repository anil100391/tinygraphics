#ifndef _vertexarray_h_
#define _vertexarray_h_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class VertexBuffer;
class VertexBufferLayout;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class VertexArray
{
public:

    VertexArray();
    ~VertexArray();

    void AddBuffer( const VertexBuffer &vb,
                    const VertexBufferLayout &layout,
                    bool instance = false );

    void Bind() const;
    void Unbind() const;
private:

    unsigned int _rendererID = 0;
    unsigned int _numAttributes = 0;
};

#endif // _vertexarray_h_
