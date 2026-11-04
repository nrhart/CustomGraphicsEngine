#include "Engine/Graphics/cMesh.h"
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/VertexFormats.h>
#include "Includes.h"

#include <vector>   // for std::vector (used to flip indices)

namespace eae6320::Graphics
{
    cResult cMesh::Initialize(
        const VertexFormats::sVertex_mesh* vertices,
        const uint16_t vertexCount,
        const uint16_t* indices,
        const uint16_t indexCount)
    {
        auto result = Results::Success;

        auto* const device = sContext::g_context.direct3dDevice;
        EAE6320_ASSERT(device);

        // Vertex format (position only)
        if (!(result = cVertexFormat::Load(
            eVertexType::Mesh, m_vertexFormat,
            "data/Shaders/Vertex/vertexInputLayout_mesh.shader")))
        {
            EAE6320_ASSERTF(false, "Can't initialize mesh without vertex format");
            return result;
        }

        // -------- Vertex Buffer --------
        {
            const auto byteCount =
                sizeof(VertexFormats::sVertex_mesh) * static_cast<unsigned int>(vertexCount);
            EAE6320_ASSERT(byteCount <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());

            D3D11_BUFFER_DESC desc{};
            desc.ByteWidth = static_cast<unsigned int>(byteCount);
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA init{};
            init.pSysMem = vertices;

            const auto hr = device->CreateBuffer(&desc, &init, &m_vertexBuffer);
            if (FAILED(hr))
            {
                EAE6320_ASSERTF(false, "VB creation failed (HR %#010x)", hr);
                Logging::OutputError("D3D failed to create VB (HR %#010x)", hr);
                return Results::Failure;
            }
        }

        // -------- Index Buffer (flip CCW -> CW for D3D) --------
        {
            m_indexCount = indexCount;

            // Copy input indices into a local vector
            std::vector<uint16_t> flippedIndices(indices, indices + indexCount);

            // Flip winding: swap the last two indices of each triangle
            for (size_t i = 0; i + 2 < flippedIndices.size(); i += 3)
            {
                std::swap(flippedIndices[i + 1], flippedIndices[i + 2]);
            }

            const auto byteCount =
                sizeof(uint16_t) * static_cast<unsigned int>(flippedIndices.size());
            EAE6320_ASSERT(byteCount <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());

            D3D11_BUFFER_DESC desc{};
            desc.ByteWidth = static_cast<unsigned int>(byteCount);
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

            D3D11_SUBRESOURCE_DATA init{};
            init.pSysMem = flippedIndices.data();

            const auto hr = device->CreateBuffer(&desc, &init, &m_indexBuffer);
            if (FAILED(hr))
            {
                EAE6320_ASSERTF(false, "IB creation failed (HR %#010x)", hr);
                Logging::OutputError("D3D failed to create IB (HR %#010x)", hr);
                return Results::Failure;
            }
        }

        return result;
    }

    void cMesh::Draw() const
    {
        auto* ctx = sContext::g_context.direct3dImmediateContext;
        EAE6320_ASSERT(ctx);
        EAE6320_ASSERT(m_vertexBuffer);
        EAE6320_ASSERT(m_indexBuffer);
        EAE6320_ASSERT(m_vertexFormat);

        // Bind VB
        {
            constexpr UINT slot = 0;
            constexpr UINT count = 1;
            const UINT stride = sizeof(VertexFormats::sVertex_mesh);
            const UINT offset = 0;
            ctx->IASetVertexBuffers(slot, count, &m_vertexBuffer, &stride, &offset);
        }

        // Bind IB
        {
            constexpr DXGI_FORMAT indexFmt = DXGI_FORMAT_R16_UINT;
            constexpr UINT offset = 0;
            ctx->IASetIndexBuffer(m_indexBuffer, indexFmt, offset);
        }

        // Layout + topology
        m_vertexFormat->Bind();
        ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // DrawIndexed
        ctx->DrawIndexed(static_cast<UINT>(m_indexCount), 0, 0);
    }

    void cMesh::CleanUp()
    {
        if (m_indexBuffer) { m_indexBuffer->Release(); m_indexBuffer = nullptr; }
        if (m_vertexBuffer) { m_vertexBuffer->Release(); m_vertexBuffer = nullptr; }
        if (m_vertexFormat) { m_vertexFormat->DecrementReferenceCount(); m_vertexFormat = nullptr; }
        m_indexCount = 0;
    }
}
