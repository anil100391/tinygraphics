#ifndef _text_renderer_h_
#define _text_renderer_h_

#include <map>
#include <string>
#include <memory>
#include <filesystem>

#include <shader.h>
#include <texture.h>

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
    void SetFontSize( float fontSize );
    void Draw( const Renderer    &renderer,
               const std::string &text,
               unsigned int       px,
               unsigned int       py,
               const glm::vec3   &color = { 0.7f, 0.7f, 0.7f } );

private:
    struct FontParams
    {
        bool operator<( const FontParams &other ) const
        {
            if ( fontFile == other.fontFile )
            {
                return fontSize < other.fontSize;
            }

            return ( fontFile < other.fontFile );
        }

        std::filesystem::path fontFile{};
        float                 fontSize = 16.0;
    };

    struct FontResource
    {
        std::vector<stbtt_bakedchar> fontMetrics; // ASCII 32..126 is 95 glyphs
        std::unique_ptr<Texture>     texture;
        std::shared_ptr<Shader>      shader;
    };

    const FontResource&     UpdateContext();
    std::shared_ptr<Shader> GetOrCreateShader();

    FontParams                         _font;
    std::map<FontParams, FontResource> _fontCache;
};

#endif // _text_renderer_h_
