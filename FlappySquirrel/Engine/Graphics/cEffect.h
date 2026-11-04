#pragma once
#include <Engine/Results/Results.h> 
#include <Engine/Graphics/cShader.h>
#include <Engine/Graphics/cRenderState.h>
#include <Engine/Graphics/sContext.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include <string>

namespace eae6320
{
    namespace Graphics
    {
        class cEffect
        {
        public:
            EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

            static cResult Load(
                cEffect*& o_effect,
                const std::string& i_vertexShaderPath,
                const std::string& i_fragmentShaderPath
            );

            void Bind() const;

        private:
            EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEffect);

            EAE6320_ASSETS_DECLAREREFERENCECOUNT();

            cEffect() = default;
            ~cEffect();

            cResult Initialize(const std::string& i_vertexShaderPath, const std::string& i_fragmentShaderPath);
            void CleanUp();

#if defined( EAE6320_PLATFORM_GL )
            cResult InitializeGL(cResult result);
            void CleanUpGL();
#endif
            cShader* m_vertexShader = nullptr;
            cShader* m_fragmentShader = nullptr;
            cRenderState  m_renderState;
#if defined( EAE6320_PLATFORM_GL )
            GLuint        m_programId = 0;
#endif
        };
    }
}
