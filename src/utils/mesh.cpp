#include <memory>
#include <cassert>
#include <cstring>

#include <utils/mesh.h>
#include <utils/objio.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
tgrMesh::tgrMesh( const std::filesystem::path &meshFile )
{
    InitializeFromFile( meshFile );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool tgrMesh::InitializeFromFile( const std::filesystem::path &meshFile )
{
    _name = meshFile.filename().string();

    tgrObjIO io( meshFile, *this );
    return io.Populate();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrMesh::ComputeCog()
{
    _cog               = std::make_unique<glm::vec3>( 0.0f, 0.0f, 0.0f );
    float  sum[3]      = { 0.0f, 0.0f, 0.0f };
    size_t numVertices = _vertices.size() / 3;
    for ( size_t ii = 0; ii < numVertices; ++ii )
    {
        const float *v = &_vertices[3 * ii];
        sum[0] += v[0];
        sum[1] += v[1];
        sum[2] += v[2];
    }

    *_cog = glm::vec3( sum[0], sum[1], sum[2] );
    *_cog /= numVertices;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void tgrMesh::ComputeBBox()
{
    _bbox              = std::make_unique<tgrBox3>();
    size_t numVertices = _vertices.size() / 3;
    for ( size_t ii = 0; ii < numVertices; ++ii )
    {
        const float *v = &_vertices[3 * ii];
        _bbox->Expand( v );
    }
}
