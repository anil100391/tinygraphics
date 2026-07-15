#include <cassert>

#include <texture.h>

#include <glad/glad.h>

#include <stb/stb_image.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Texture::Texture( const std::filesystem::path &filePath )
{
    stbi_set_flip_vertically_on_load( 1 );
    auto channelsPerPixel = -1;
    auto localBuffer =
        stbi_load( filePath.string().c_str(), &_width, &_height, &channelsPerPixel, 4 );

    Create( localBuffer, _width, _height, GL_RGBA );

    if ( localBuffer )
    {
        stbi_image_free( localBuffer );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Texture::Texture( const unsigned char *pixels,
                  int                  width,
                  int                  height,
                  unsigned int         format )
    : _width( width ), _height( height )
{
    Create( pixels, _width, _height, format );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Texture::Create( const unsigned char *pixels,
                      int                  width,
                      int                  height,
                      unsigned int         format )
{
    glGenTextures( 1, &_rendererID );
    glBindTexture( GL_TEXTURE_2D, _rendererID );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    GLint  internalFormat = format;
    GLenum type           = GL_UNSIGNED_BYTE;

    // Depth textures need a floating point type, not unsigned byte.
    if ( format == GL_DEPTH_COMPONENT )
    {
        type = GL_FLOAT;
    }

    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  internalFormat,
                  width,
                  height,
                  0,
                  format,
                  type,
                  pixels );

    glBindTexture( GL_TEXTURE_2D, 0 );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Texture::~Texture()
{
    glDeleteTextures( 1, &_rendererID );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Texture::Bind( unsigned int slot ) const
{
    glActiveTexture( GL_TEXTURE0 + slot );
    glBindTexture( GL_TEXTURE_2D, _rendererID );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Texture::Unbind() const
{
    glBindTexture( GL_TEXTURE_2D, 0 );
}
