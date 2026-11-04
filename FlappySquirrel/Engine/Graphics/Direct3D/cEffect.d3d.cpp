#include <Engine/Graphics/cEffect.h>

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/VertexFormats.h>
#include "Includes.h"

namespace eae6320
{
	namespace Graphics
	{
        void cEffect::Bind() const
        {
            auto* ctx = sContext::g_context.direct3dImmediateContext;
            EAE6320_ASSERT(ctx);

            constexpr ID3D11ClassInstance* const* kNoInterfaces = nullptr;
            constexpr unsigned kInterfaceCount = 0;

            // Vertex shader
            EAE6320_ASSERT(m_vertexShader && m_vertexShader->m_shaderObject.vertex);
            ctx->VSSetShader(m_vertexShader->m_shaderObject.vertex, kNoInterfaces, kInterfaceCount);

            // Pixel (fragment) shader
            EAE6320_ASSERT(m_fragmentShader && m_fragmentShader->m_shaderObject.fragment);
            ctx->PSSetShader(m_fragmentShader->m_shaderObject.fragment, kNoInterfaces, kInterfaceCount);

            // Render state (rasterizer/depth/blend as configured in Initialize)
            m_renderState.Bind();
        }
	}
}
