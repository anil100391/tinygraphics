#pragma once

#include <cctype>
#include <charconv>
#include <filesystem>
#include <utils/mesh.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrObjIO
{
public:
    tgrObjIO( const std::filesystem::path &meshFile, tgrMesh &mesh );
    ~tgrObjIO() = default;

    bool Populate();

private:
    bool PopulateFromObj();
    bool ParseLine( const std::string &line );

    static const char *FindNonWhiteSpaceChar( const char *begin,
                                              const char *end );
    static const char *
    ParseFloat( const char *begin, const char *end, float &value );

    static const char *
    ParseIndex( const char *begin, const char *end, unsigned int &value );

    const std::filesystem::path _meshFile;
    tgrMesh                    &_mesh;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
inline const char *tgrObjIO::FindNonWhiteSpaceChar( const char *begin,
                                                    const char *end )
{
    auto p = begin;
    while ( p <= end && std::isspace( *p ) )
    {
        p++;
    }

    return ( p <= end ) ? p : nullptr;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
inline const char *
tgrObjIO::ParseFloat( const char *begin, const char *end, float &value )
{
    auto floatBegin = FindNonWhiteSpaceChar( begin, end );

    if ( !floatBegin )
    {
        return nullptr;
    }

    auto [ptr, ec] = std::from_chars( floatBegin, end, value );
    return ( ec == std::errc() ) ? ptr : nullptr;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
inline const char *
tgrObjIO::ParseIndex( const char *begin, const char *end, unsigned int &value )
{
    auto floatBegin = FindNonWhiteSpaceChar( begin, end );

    if ( !floatBegin )
    {
        return nullptr;
    }

    auto [ptr, ec] = std::from_chars( floatBegin, end, value );

    if ( *ptr == '/' )
    {
        ptr++;
    }

    if ( ec == std::errc() )
    {
        --value;
    }

    return ptr;
}
