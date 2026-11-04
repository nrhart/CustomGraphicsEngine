#pragma once
#include <Engine/Results/Results.h>
#include <Engine/Graphics/cVertexFormat.h>
#include <Engine/Graphics/cShader.h>
#include <Engine/Graphics/sContext.h>
#include <Engine/Assets/ReferenceCountedAssets.h>

struct ID3D11Buffer;
struct ID3D11DeviceContext;

namespace eae6320::Graphics
{
    namespace VertexFormats { struct sVertex_mesh; }

    class cMesh
    {
    public:
        EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

        static cResult Load(
            cMesh*& o_mesh,
            const char* const i_pathToBuiltMesh
        );

        static cResult Load(
            cMesh*& o_mesh,
            const VertexFormats::sVertex_mesh* vertices,
            uint16_t vertexCount,
            const uint16_t* indices,
            uint16_t indexCount
        );

        void Draw() const;

    private:
        EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cMesh);
        EAE6320_ASSETS_DECLAREREFERENCECOUNT();

        cMesh() = default;
        ~cMesh();

        cResult Initialize(
            const VertexFormats::sVertex_mesh* vertices,
            uint16_t vertexCount,
            const uint16_t* indices,
            uint16_t indexCount
        );
        void CleanUp();

        cVertexFormat* m_vertexFormat = nullptr;
        uint16_t       m_indexCount = 0;

#if defined( EAE6320_PLATFORM_D3D )
        ID3D11Buffer* m_vertexBuffer = nullptr;
        ID3D11Buffer* m_indexBuffer = nullptr;
#endif

#if defined( EAE6320_PLATFORM_GL )
        unsigned int m_vertexArrayId = 0;
        unsigned int m_vertexBufferId = 0;
        unsigned int m_indexBufferId = 0;
#endif
    };
}
