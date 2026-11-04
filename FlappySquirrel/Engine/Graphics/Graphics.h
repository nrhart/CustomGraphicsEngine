/*
    This file declares the external interface for the graphics system
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Includes
//=========

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>
#include <Engine/Math/cMatrix_transformation.h>
#endif

// Forward declarations
//=====================
namespace eae6320
{
    namespace Graphics
    {
        class cMesh;
        class cEffect;
    }
}

// Interface
//==========

namespace eae6320
{
    namespace Graphics
    {
        // Submission
        //-----------

        // These functions should be called from the application (on the application loop thread)

        void SubmitElapsedTime(const float i_elapsedSecondCount_systemTime,
            const float i_elapsedSecondCount_simulationTime);

        // Submit the clear color (alpha defaults to 1.0f)
        void SubmitClearColor(float r, float g, float b, float a = 1.0f);

        // Submit a mesh/effect pair to be drawn this frame
        void SubmitDrawCall(cMesh* i_mesh, cEffect* i_effect, const Math::cMatrix_transformation& i_localToWorld);

        void SubmitCamera(
            const Math::cMatrix_transformation& i_worldToCamera,
            const Math::cMatrix_transformation& i_cameraToProjected);

        // Frame boundaries
        cResult WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds);
        cResult SignalThatAllDataForAFrameHasBeenSubmitted();

        // Render
        //-------
        void RenderFrame();

        // Initialize / Clean Up
        //----------------------
        struct sInitializationParameters
        {
#if defined( EAE6320_PLATFORM_WINDOWS )
            HWND mainWindow = NULL;
#if defined( EAE6320_PLATFORM_D3D )
            uint16_t resolutionWidth = 0, resolutionHeight = 0;
#elif defined( EAE6320_PLATFORM_GL )
            HINSTANCE thisInstanceOfTheApplication = NULL;
#endif
#endif
        };

        cResult Initialize(const sInitializationParameters& i_initializationParameters);
        cResult CleanUp();
    }
}

#endif  // EAE6320_GRAPHICS_H
