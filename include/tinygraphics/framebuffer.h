#ifndef _framebuffer_h_
#define _framebuffer_h_

#include <texture.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrFrameBuffer
{
public:
    enum Attachment
    {
        COLOR,
        DEPTH
    };

    tgrFrameBuffer( unsigned int width,
                    unsigned int height,
                    Attachment   attachment );

    ~tgrFrameBuffer();

    void Bind() const;
    void Unbind() const;

    void         BindTexture() const;
    void         UnbindTexture() const;
    unsigned int TextureID() const
    {
        return _texture->GetID();
    }

private:
    unsigned int                _frameBufferId  = 0;
    unsigned int                _renderBufferId = 0;
    std::unique_ptr<tgrTexture> _texture;
};

#endif // _framebuffer_h_
