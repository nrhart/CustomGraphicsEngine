/*
    This class is your specific game
*/

#ifndef EAE6320_CEXAMPLEGAME_H
#define EAE6320_CEXAMPLEGAME_H

// Includes
//=========

#include <Engine/Application/iApplication.h>
#include <Engine/Results/Results.h>
#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/GameObject/cCamera.h>
#include <Engine/GameObject/cGameObject.h>

#include <vector>

#include "cPipeManager.h"
#include <Engine/UserInterface/UIOverlay.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
#include "Resource Files/Resource.h"
#endif

// Class Declaration
//==================

namespace eae6320
{
    class cMyGame final : public Application::iApplication
    {
    private:

        // Window Config
        //--------------
#if defined( EAE6320_PLATFORM_WINDOWS )
        const char* GetMainWindowName() const final
        {
            return "Flappy Squirrel"
                " -- "
#if defined( EAE6320_PLATFORM_D3D )
                "Direct3D"
#elif defined( EAE6320_PLATFORM_GL )
                "OpenGL"
#endif
#ifdef _DEBUG
                " -- Debug"
#endif
                ;
        }
        const char* GetMainWindowClassName() const final { return "Noah's EAE6320 Main Window Class"; }
        const WORD* GetLargeIconId() const final { static constexpr WORD id = IDI_EAEGAMEPAD; return &id; }
        const WORD* GetSmallIconId() const final { static constexpr WORD id = IDI_EAEALIEN; return &id; }
        void GameOver();
        void ResetGame();
#endif

        // Run
        //----
        void UpdateBasedOnInput() final;
        void UpdateSimulationBasedOnInput() final;
        void UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) final;

        // Renderer
        void SubmitDataToBeRendered(
            const float i_elapsedSecondCount_systemTime,
            const float i_elapsedSecondCount_sinceLastSimulationUpdate) final;

        // Initialize / Clean Up
        cResult Initialize() final;
        cResult CleanUp() final;

        // Data
        //=====

        // Meshes
        Graphics::cMesh* m_squirrelMesh = nullptr;
        Graphics::cMesh* m_squareMesh = nullptr;
        Graphics::cMesh* m_rectangleMesh = nullptr;
        Graphics::cMesh* m_treeMesh = nullptr;

        // Effects
        Graphics::cEffect* m_animatedEffect = nullptr;
        Graphics::cEffect* m_standardEffect = nullptr;

        // GameObjects
        cCamera m_camera;
        cGameObject m_gameObject;   
        cGameObject m_gameObject2;  

        cPipeManager m_pipeManager;         
        Graphics::cMesh* m_pipeMesh = nullptr;  

        bool m_canJump = true;
		int m_score = 0;
        int m_scoreTextId = -1;
        bool m_isGameOver = false;

        enum class GameState
        {
            WaitingToStart, 
            Playing,
            Hit,        // hit pipe and play hit sound
            Die,        // waiting to play die sound
            Exit        // ready to exit
        };

        GameState m_gameState = GameState::WaitingToStart;
        float m_gameOverTimer = 0.0f;
    };
}

// Results
//========

namespace eae6320
{
    namespace Results
    {
        namespace Application
        {
            constexpr cResult ExampleResult(IsFailure, eSystem::Application, __LINE__, Severity::Default);
        }
    }
}

#endif  // EAE6320_CEXAMPLEGAME_H
