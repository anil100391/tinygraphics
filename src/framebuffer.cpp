#include <memory>
#include <cassert>

#include <log.h>
#include <glad/glad.h>

#include <framebuffer.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
FrameBuffer::FrameBuffer( unsigned int width,
                          unsigned int height,
                          Attachment   attachment )
{
    glGenFramebuffers( 1, &_frameBufferId );
    glBindFramebuffer( GL_FRAMEBUFFER, _frameBufferId );

    // create color attachment. choosing a 2d texture for it... could have been
    // a renderbuffer as well
    if ( attachment == COLOR )
    {
        _texture = std::make_unique<Texture>( nullptr, width, height, GL_RGB );

        // attach this texture to the framebuffer
        glFramebufferTexture2D( GL_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_2D,
                                _texture->GetID(),
                                0 );

        // create a depth and stencil attachement. choosing a single
        // renderbuffer for both of it
        glGenRenderbuffers( 1, &_renderBufferId );
        glBindRenderbuffer( GL_RENDERBUFFER, _renderBufferId );
        glRenderbufferStorage(
            GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height );

        // attach the render buffer to the framebuffer
        glFramebufferRenderbuffer( GL_FRAMEBUFFER,
                                   GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_RENDERBUFFER,
                                   _renderBufferId );
    }
    else
    {
        assert( attachment == DEPTH );
        _texture = std::make_unique<Texture>(
            nullptr, width, height, GL_DEPTH_COMPONENT );
        // attach this texture to the framebuffer
        glFramebufferTexture2D( GL_FRAMEBUFFER,
                                GL_DEPTH_ATTACHMENT,
                                GL_TEXTURE_2D,
                                _texture->GetID(),
                                0 );
        glDrawBuffer( GL_NONE );
        glReadBuffer( GL_NONE );
    }

    // Sanity check
    if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
    {
        Log( LogLevel::Error, "ERROR::INCOMPLE FRAMEBUFFER!" );
    }

    // unbind the framebuffer so that we don't accidently draw to it
    Unbind();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers( 1, &_frameBufferId );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FrameBuffer::Bind() const
{
    glBindFramebuffer( GL_FRAMEBUFFER, _frameBufferId );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FrameBuffer::Unbind() const
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FrameBuffer::BindTexture() const
{
    _texture->Bind( 0 );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FrameBuffer::UnbindTexture() const
{
    _texture->Unbind();
}
