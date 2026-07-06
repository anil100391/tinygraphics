#ifndef _text_renderer_h_
#define _text_renderer_h_

#include <string>
#include <memory>
#include <filesystem>

#include <stb/stb_truetype.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Renderer;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TextRenderer
{
public:
    void SetFont( const std::filesystem::path &fontFile );
    void SetfontSize( float fontSize );
    void Draw( const Renderer    &renderer,
               const std::string &text,
               unsigned int       px,
               unsigned int       py );

private:
    struct Context
    {
        std::filesystem::path fontFile;
        float                 fontSize = 32.0f;
        char                  first    = 32;
        stbtt_bakedchar       fontMetrics[96]; // ASCII 32..126 is 95 glyphs
        unsigned int          textureID = 0;
        bool                  dirty     = true;
    };

    void UpdateContext();

    std::unique_ptr<Context> _context;
};

#endif // _text_renderer_h_
