#ifndef _mesh_h_
#define _mesh_h_

#include <vector>
#include <string>
#include <climits>
#include <filesystem>

#include <utils/bbox.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Mesh
{
public:
    Mesh() = default;
    Mesh( const std::filesystem::path &meshFile );

    struct Tria
    {
        unsigned int vidx[3] = { UINT_MAX,
                                 UINT_MAX,
                                 UINT_MAX }; // vertex indices
        unsigned int nidx[3] = { UINT_MAX,
                                 UINT_MAX,
                                 UINT_MAX }; // normal indices
        unsigned int tidx[3] = { UINT_MAX,
                                 UINT_MAX,
                                 UINT_MAX }; // tex coord indices
    };

    std::string        _name;
    std::vector<float> _vertices;
    std::vector<float> _normals;
    std::vector<float> _textureCoords;
    std::vector<Tria>  _trias;

    bool InitializeFromFile( const std::filesystem::path &meshFile );

    [[nodiscard]] const glm::vec3 &cog() noexcept
    {
        if ( !_cog )
            ComputeCog();
        return *_cog;
    }

    [[nodiscard]] const box3 &bbox() noexcept
    {
        if ( !_bbox )
            ComputeBBox();
        return *_bbox;
    }

    [[nodiscard]] bool IsSmoothShaded() const noexcept
    {
        return _smoothShaded;
    }

    void SetSmoothShaded( bool smooth )
    {
        _smoothShaded = smooth;
    }

    [[nodiscard]] bool HasNormals() const noexcept
    {
        return !_normals.empty();
    }

    [[nodiscard]] bool HasTextureCoords() const noexcept
    {
        return !_textureCoords.empty();
    }

    [[nodiscard]] bool IsEmpty() const noexcept
    {
        return _vertices.empty() && _trias.empty();
    }

private:
    void ComputeBBox();
    void ComputeCog();

    bool                       _smoothShaded = true;
    std::unique_ptr<box3>      _bbox;
    std::unique_ptr<glm::vec3> _cog;
};

#endif // _mesh_h_
