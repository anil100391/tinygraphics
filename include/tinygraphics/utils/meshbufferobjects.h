#ifndef _mesh_buffer_objects_h_
#define _mesh_buffer_objects_h_

#include <vector>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Mesh;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class MeshBufferObjects
{
public:
    struct IOParams
    {
        // input
        bool needNormals       = true;
        bool needTextureCoords = true;
        bool flatShading       = true;

        // output
        bool hasNormals       = true;
        bool hasTextureCoords = true;
        bool isFlatShaded     = true;

        std::vector<float>        vertexAttribs; // pos, normal, texcoordinates
        std::vector<unsigned int> connectivity;  // tria definition
    };

    static bool Get( const Mesh &m, IOParams &io );
};

#endif // _mesh_buffer_objects_h_
