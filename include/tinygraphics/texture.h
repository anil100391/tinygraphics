#ifndef _texture_h_
#define _texture_h_

#include <filesystem>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Texture
{
public:
    // Create Texture from image file
    Texture( const std::filesystem::path &path );

    // Create texture from passed buffer
    Texture( const unsigned char *pixels,
             int                  width,
             int                  height,
             int                  channelsPerPixel );

    ~Texture();

    void Bind( unsigned int slot ) const;
    void Unbind() const;

    inline int GetWidth() const
    {
        return _width;
    }

    inline int GetHeight() const
    {
        return _height;
    }

    unsigned int GetID() const
    {
        return _rendererID;
    }

private:
    void Create( const unsigned char *pixels,
                 int                  width,
                 int                  height,
                 int                  channelsPerPixel );

    unsigned int _rendererID = 0;
    int          _width      = -1;
    int          _height     = -1;
};

#endif // _texture_h_
