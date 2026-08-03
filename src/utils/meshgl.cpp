#include <utils/meshgl.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
tgrMeshGL::tgrMeshGL( const std::vector<float>        &vertices,
                      const tgrVertexBufferLayout     &layout,
                      const std::vector<unsigned int> &conn,
                      int                              bufferUsage )
{
    PopulateBuffers( vertices, layout, conn, bufferUsage );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrMeshGL::PopulateBuffers( const std::vector<float>        &vertices,
                                 const tgrVertexBufferLayout     &layout,
                                 const std::vector<unsigned int> &conn,
                                 int                              bufferUsage )
{
    _vao = std::make_unique<tgrVertexArray>();
    _vbo = std::make_unique<tgrVertexBuffer>(
        vertices.data(),
        static_cast<unsigned int>( vertices.size() * sizeof( float ) ),
        bufferUsage );

    _vao->AddBuffer( *_vbo, layout );

    _ibo = std::make_unique<tgrIndexBuffer>(
        conn.data(), static_cast<unsigned int>( conn.size() ) );
}
