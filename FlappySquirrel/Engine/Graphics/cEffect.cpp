#include "cEffect.h"
#include <Engine/Asserts/Asserts.h>

namespace eae6320
{
	namespace Graphics
	{
		// Factory
		cResult cEffect::Load(
			cEffect*& o_effect,
			const std::string& i_vertexShaderPath,
			const std::string& i_fragmentShaderPath)
		{
            auto result = Results::Success;
            cEffect* newEffect = new (std::nothrow) cEffect();
            if (!newEffect)
            {
                result = Results::OutOfMemory;
                o_effect = nullptr;
                return result;
            }

            if (!(result = newEffect->Initialize(i_vertexShaderPath, i_fragmentShaderPath)))
            {
                newEffect->DecrementReferenceCount();
                o_effect = nullptr;
                return result;
            }

            o_effect = newEffect;
            return result;
        }

        cResult cEffect::Initialize(
            const std::string& vertexShaderPath,
            const std::string& fragmentShaderPath)
        {
            auto result = Results::Success;

            if (!(result = cShader::Load(vertexShaderPath, m_vertexShader, eShaderType::Vertex)))
            {
                EAE6320_ASSERTF(false, "Can't init effect without vertex shader");
                return result;
            }
            if (!(result = cShader::Load(fragmentShaderPath, m_fragmentShader, eShaderType::Fragment)))
            {
                EAE6320_ASSERTF(false, "Can't init effect without fragment shader");
                return result;
            }

            // Render state (same as before)
            constexpr auto renderStateBits = []
                {
                    uint8_t renderStateBits = 0;
                    //RenderStates::DisableAlphaTransparency(renderStateBits);
                    //RenderStates::DisableDepthTesting(renderStateBits);
                    //RenderStates::DisableDepthWriting(renderStateBits);
                    //RenderStates::DisableDrawingBothTriangleSides(renderStateBits);
                    RenderStates::EnableAlphaTransparency(renderStateBits);
                    RenderStates::EnableDepthTesting(renderStateBits);
                    RenderStates::EnableDepthWriting(renderStateBits);
                    RenderStates::EnableDrawingBothTriangleSides(renderStateBits);
                    return renderStateBits;
                }();
            if (!(result = m_renderState.Initialize(renderStateBits)))
            {
                EAE6320_ASSERTF(false, "Can't init effect without render state");
                return result;
            }

#if defined(EAE6320_PLATFORM_GL)
            if (!(result = InitializeGL(result)))
            {
                return result;
            }
#endif
            return result;
        }

		cEffect::~cEffect()
		{
			EAE6320_ASSERT(m_referenceCount == 0);
			CleanUp();
		}

		void cEffect::CleanUp()
		{
#if defined(EAE6320_PLATFORM_GL)
			CleanUpGL();
#endif
			if (m_vertexShader)
			{
				m_vertexShader->DecrementReferenceCount();
				m_vertexShader = nullptr;
			}
			if (m_fragmentShader)
			{
				m_fragmentShader->DecrementReferenceCount();
				m_fragmentShader = nullptr;
			}
		}
	}
}
