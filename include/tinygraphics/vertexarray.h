#ifndef _vertexarray_h_
#define _vertexarray_h_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrVertexBuffer;
class tgrVertexBufferLayout;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrVertexArray
{
public:
    tgrVertexArray();
    ~tgrVertexArray();

    void AddBuffer( const tgrVertexBuffer       &vb,
                    const tgrVertexBufferLayout &layout,
                    bool                         instance = false );

    void Bind() const;
    void Unbind() const;

private:
    unsigned int _rendererID    = 0;
    unsigned int _numAttributes = 0;
};

#endif // _vertexarray_h_
