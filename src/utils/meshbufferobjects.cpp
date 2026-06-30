#include <numeric>

#include <utils/mesh.h>
#include <utils/meshbufferobjects.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MeshBufferObjects::Get( const Mesh &m, IOParams &io )
{
    if ( m.IsEmpty() )
    {
        return false;
    }

    io.isFlatShaded     = !m.IsSmoothShaded();
    io.hasNormals       = io.needNormals && m.HasNormals();
    io.hasTextureCoords = io.needTextureCoords && m.HasTextureCoords();

    size_t numFloatsPerVertex = 3; // position
    if ( io.hasNormals )
    {
        numFloatsPerVertex += 3; // normal
    }

    if ( io.hasTextureCoords )
    {
        numFloatsPerVertex += 2; // tex coord
    }

    io.vertexAttribs.reserve( numFloatsPerVertex * 3 * m._trias.size() );
    for ( const auto &tria : m._trias )
    {
        for ( auto nidx = 0u; nidx < 3u; ++nidx )
        {
            auto v = &m._vertices[3 * tria.vidx[nidx]];
            io.vertexAttribs.push_back( v[0] );
            io.vertexAttribs.push_back( v[1] );
            io.vertexAttribs.push_back( v[2] );

            if ( io.hasNormals )
            {
                auto n = &m._normals[3 * tria.nidx[nidx]];
                io.vertexAttribs.push_back( n[0] );
                io.vertexAttribs.push_back( n[1] );
                io.vertexAttribs.push_back( n[2] );
            }

            if ( io.hasTextureCoords )
            {
                auto t = &m._textureCoords[2 * tria.tidx[nidx]];
                io.vertexAttribs.push_back( t[0] );
                io.vertexAttribs.push_back( t[1] );
            }
        }
    }

    io.connectivity.resize( 3 * m._trias.size() );
    std::iota( io.connectivity.begin(), io.connectivity.end(), 0u );

    return true;
}
