#include <format>
#include <string>
#include <fstream>

#include <log.h>
#include <utils/objio.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ObjIO::ObjIO( const std::filesystem::path &meshFile, Mesh &mesh )
    : _mesh( mesh ), _meshFile( meshFile )
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool ObjIO::Populate()
{
    if ( ".obj" != _meshFile.extension() )
    {
        Log( LogLevel::Error, "Wrong parser" );
        return false;
    }

    return PopulateFromObj();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool ObjIO::PopulateFromObj()
{
    assert( ".obj" == _meshFile.extension() );

    std::ifstream f( _meshFile );
    if ( !f.is_open() )
    {
        Log( LogLevel::Error, "Unable to open {}", _meshFile.string() );
        return false;
    }

    Log( LogLevel::Info, "Parsing {}", _meshFile.string() );

    std::string line;
    while ( std::getline( f, line ) )
    {
        ParseLine( line );
    }

    Log( LogLevel::Info,
         "Done... {} vertices, {} triangles",
         _mesh._vertices.size() / 3,
         _mesh._trias.size() );

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool ObjIO::ParseLine( const std::string &line )
{
    if ( line.empty() )
    {
        return false;
    }

    auto end = line.data() + line.size();
    auto c   = FindNonWhiteSpaceChar( line.data(), end );

    if ( !c )
    {
        return false;
    }

    if ( *c == '#' )
    {
        // comment nothing to be done
        return true;
    }

    char buffer[64];

    float v[3] = { 0.0f, 0.0f, 0.0f };

    if ( *c == 'v' )
    {
        if ( c[1] == ' ' )
        {
            // vertex e.g.
            // "v 1.000000 1.000000 -1.000000"
            c += 2;
            c = ParseFloat( c, end, v[0] );
            c = ParseFloat( c, end, v[1] );
            c = ParseFloat( c, end, v[2] );
            _mesh._vertices.push_back( v[0] );
            _mesh._vertices.push_back( v[1] );
            _mesh._vertices.push_back( v[2] );
        }
        else if ( c[1] == 'n' )
        {
            // normal e.g.
            // "vn 1.000000 0.000000 0.000000"
            c += 3;
            c = ParseFloat( c, end, v[0] );
            c = ParseFloat( c, end, v[1] );
            c = ParseFloat( c, end, v[2] );
            _mesh._normals.push_back( v[0] );
            _mesh._normals.push_back( v[1] );
            _mesh._normals.push_back( v[2] );
        }
        else if ( c[1] == 't' )
        {
            // texture coordinates
            // "vt 1.000000 0.000000"
            c += 3;
            c = ParseFloat( c, end, v[0] );
            c = ParseFloat( c, end, v[1] );
            _mesh._textureCoords.push_back( v[0] );
            _mesh._textureCoords.push_back( v[1] );
        }
    }
    else if ( *c == 's' )
    {
        // Shading e.g.
        // "s 0" for flat and "s 1" for smooth
        c += 2;
        c = ParseFloat( c, end, v[0] );
        _mesh.SetSmoothShaded( v[0] != 0 );
    }
    else if ( *c == 'f' )
    {
        Mesh::Tria t0;
        // Faces e.g.
        // "f 1/1/1 5/2/1 7/3/1 3/4/1"
        c += 2;

        c = ParseIndex( c, end, t0.vidx[0] );
        c = ParseIndex( c, end, t0.tidx[0] );
        c = ParseIndex( c, end, t0.nidx[0] );

        c = ParseIndex( c, end, t0.vidx[1] );
        c = ParseIndex( c, end, t0.tidx[1] );
        c = ParseIndex( c, end, t0.nidx[1] );

        c = ParseIndex( c, end, t0.vidx[2] );
        c = ParseIndex( c, end, t0.tidx[2] );
        c = ParseIndex( c, end, t0.nidx[2] );

        // quad face
        if ( *c != '\0' )
        {
            Mesh::Tria t1;

            t1.vidx[0] = t0.vidx[0];
            t1.nidx[0] = t0.nidx[0];
            t1.tidx[0] = t0.tidx[0];

            t1.vidx[1] = t0.vidx[2];
            t1.nidx[1] = t0.nidx[2];
            t1.tidx[1] = t0.tidx[2];

            c = ParseIndex( c, end, t1.vidx[2] );
            c = ParseIndex( c, end, t1.tidx[2] );
            c = ParseIndex( c, end, t1.nidx[2] );

            _mesh._trias.push_back( std::move( t1 ) );
        }

        _mesh._trias.push_back( std::move( t0 ) );
    }

    return true;
}
