#include <vector>
#include <format>
#include <fstream>
#include <iostream>
#include <filesystem>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static bool ConvertToCpp( const std::filesystem::path &assetFile,
                          const std::filesystem::path &cppFile,
                          const std::string           &variable )
{
    if ( !std::filesystem::exists( assetFile ) )
    {
        return false;
    }

    std::ifstream src( assetFile, std::ios::binary );
    std::ofstream dst( cppFile );
    if ( !src.is_open() || !dst.is_open() )
    {
        return false;
    }

    std::vector<std::uint8_t> data( ( std::istreambuf_iterator<char>( src ) ),
                                    std::istreambuf_iterator<char>() );

    dst << "unsigned char " << variable << "[] = {\n";

    for ( std::size_t i = 0; i < data.size(); ++i )
    {
        if ( i % 12 == 0 )
            dst << "    ";

        dst << "0x" << std::uppercase << std::hex << std::setw( 2 )
            << std::setfill( '0' ) << static_cast<unsigned>( data[i] );

        if ( i + 1 != data.size() )
            dst << ", ";

        if ( ( i + 1 ) % 12 == 0 )
            dst << '\n';
    }

    dst << "};\n\nunsigned int " << variable << "_size = sizeof(" << variable
        << ");\n";

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, const char *argv[] )
{
    if ( argc < 3 )
    {
        std::cout << std::format(
            "Usage: asset2cpp <assetFile> <cppFile> [variable]" );
        return 1;
    }

    auto assetFile = std::filesystem::path( argv[1] );
    auto cppFile   = std::filesystem::path( argv[2] );
    auto variable  = assetFile.stem().string();
    if ( argc == 4 )
    {
        variable = argv[3];
    }

    return ConvertToCpp( assetFile, cppFile, variable ) ? 0 : 1;
}
