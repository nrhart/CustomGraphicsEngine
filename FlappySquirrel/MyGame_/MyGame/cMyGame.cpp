// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Application/iApplication.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Audio/cAudio.h>

#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Math/Functions.h>
#include <Engine/Graphics/ConstantBufferFormats.h>
#include <Engine/Physics/cCollision.h>

static eae6320::Audio::cAudio g_Audio;
UIOverlay::Overlay g_overlay;


//===============================================================================================
// UPDATE BASED ON INPUT
//===============================================================================================
void eae6320::cMyGame::UpdateBasedOnInput()
{
    // Quit with ESC
    if (UserInput::IsKeyPressed(UserInput::KeyCodes::Escape))
    {
        const auto result = Exit(EXIT_SUCCESS);
        EAE6320_ASSERT(result);
    }

    // Space = flap
    static bool wasSpace = false;
    bool isSpace = UserInput::IsKeyPressed(UserInput::KeyCodes::Space);

    if (m_gameState == GameState::WaitingToStart)
    {
        if (isSpace && !wasSpace)
        {
            m_gameState = GameState::Playing;
            return;
        }
        wasSpace = isSpace;
        return; // ignore movement before start
    }

    // Only allow flapping when actually playing
    if (m_gameState == GameState::Playing)
    {
        if (isSpace && !wasSpace && m_canJump)
        {
            m_gameObject.m_rigidBody.velocity.y = 6.0f;
            eae6320::Audio::cAudio::PlaySoundEffect("data/audio/flap.audio");
        }
    }

    wasSpace = isSpace;

    if (UserInput::IsKeyPressed(UserInput::KeyCodes::P))
    {
        // Pause
        SetSimulationRate(0.0f);
    }
    //else
    //{
    //    // Normal speed
    //    SetSimulationRate(1.0f);
    //}

}



//===============================================================================================
// SIMPLE INPUT SIMULATION (no horizontal movement in Flappy Bird)
//===============================================================================================
void eae6320::cMyGame::UpdateSimulationBasedOnInput()
{
    // No left/right movement
    m_gameObject.m_rigidBody.velocity.x = 0.0f;
}



//===============================================================================================
// UPDATE SIMULATION BASED ON TIME (gravity + camera + pipes)
//===============================================================================================
void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float dt)
{
    if (m_gameState == GameState::WaitingToStart)
    {
        m_gameObject.m_rigidBody.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
        m_gameObject.m_rigidBody.acceleration = Math::sVector(0.0f, 0.0f, 0.0f);
        return;
    }
    // =====================================================
    // 0. If in GAME OVER state, handle timing + exit
    // =====================================================
    if (m_gameState == GameState::Exit)
    {
        m_gameOverTimer += dt;
        if (m_gameOverTimer > 1.0f)
        {
            // Allow death sound to finish before quitting
            GameOver();
        }
        return;
    }
    else if (m_gameState == GameState::Hit)
    {
        m_gameOverTimer += dt;
        if (m_gameOverTimer > 0.35f) // small delay before "die" sound
        {
            m_gameState = GameState::Die;
            m_gameOverTimer = 0.0f;

            eae6320::Audio::cAudio::PlaySoundEffect("data/audio/die.audio");
        }
        return;
    }
    else if (m_gameState == GameState::Die)
    {
        m_gameOverTimer += dt;
        if (m_gameOverTimer > 1.0f) // allow die.wav to finish
        {
            GameOver();
        }
        return;
    }


    // =====================================================
    // 1. Gravity
    // =====================================================
    const float gravity = -26.0f;
    m_gameObject.m_rigidBody.acceleration = Math::sVector(0.0f, gravity, 0.0f);

    // =====================================================
    // 2. Physics integration
    // =====================================================
    m_gameObject.UpdateSimulation(dt);

    // =====================================================
    // 3. Clamp falling speed
    // =====================================================
    const float maxFallSpeed = -13.0f;
    if (m_gameObject.m_rigidBody.velocity.y < maxFallSpeed)
        m_gameObject.m_rigidBody.velocity.y = maxFallSpeed;

    // =====================================================
    // 4. Lock X-position
    // =====================================================
    m_gameObject.m_rigidBody.position.x = -3.0f;

    // =====================================================
    // 5. Static camera
    // =====================================================
    m_camera.m_rigidBody.position = Math::sVector(0.0f, 0.0f, 30.0f);

    // =====================================================
    // 6. Update pipes
    // =====================================================
    m_pipeManager.Update(dt);

    // =====================================================
    // 7. Build colliders
    // =====================================================
    Physics::AABB floorCollider;
    floorCollider.center = m_gameObject2.m_rigidBody.position;
    floorCollider.halfSize = m_gameObject2.m_colliderHalfSize * m_gameObject2.m_scale;

    Physics::AABB playerCollider;
    playerCollider.center = m_gameObject.m_rigidBody.position;
    playerCollider.halfSize = m_gameObject.m_colliderHalfSize * m_gameObject.m_scale;

    // =====================================================
    // 8. Floor collision = instant death
    // =====================================================
    if (Physics::ResolveOverlap(
        m_gameObject.m_rigidBody.position,
        m_gameObject.m_rigidBody.velocity,
        playerCollider,
        floorCollider))
    {
        if (m_gameState == GameState::Playing)
        {
            m_gameState = GameState::Die;
            m_gameOverTimer = 0.0f;

            m_canJump = false;
            m_gameObject.m_rigidBody.velocity.y = 0.0f;

            eae6320::Audio::cAudio::PlaySoundEffect("data/audio/die.audio");
        }
        return;
    }

    // =====================================================
    // 9. Pipe collision (hit sound - falling - die)
    // =====================================================
    for (int i = 0; i < m_pipeManager.pipeCount; i++)
    {
        Physics::AABB top, bottom;

        top.center = m_pipeManager.pipesTop[i].m_rigidBody.position;
        top.halfSize = m_pipeManager.pipesTop[i].m_colliderHalfSize;

        bottom.center = m_pipeManager.pipesBottom[i].m_rigidBody.position;
        bottom.halfSize = m_pipeManager.pipesBottom[i].m_colliderHalfSize;

        bool hitTop = playerCollider.Intersects(top);
        bool hitBottom = playerCollider.Intersects(bottom);

        if ((hitTop || hitBottom) && m_gameState == GameState::Playing)
        {
            // Enter HIT state
            m_gameState = GameState::Hit;
            m_gameOverTimer = 0.0f;

            // Turn off control
            m_canJump = false;
            m_gameObject.m_rigidBody.velocity.y = 0.0f;

            // Play hit sound once
            eae6320::Audio::cAudio::PlaySoundEffect("data/audio/hit.audio");

            return; // stop checking other pipes
        }
    }


    // =====================================================
    // 10. Score check
    // =====================================================
    if (m_pipeManager.CheckScore(m_gameObject.m_rigidBody.position))
    {
        eae6320::Audio::cAudio::PlaySoundEffect("data/audio/point.audio");
        m_score++;
        g_overlay.ClearText();
        std::wstring scoreText = L"Score: " + std::to_wstring(m_score);
        m_scoreTextId = g_overlay.AddText(
            scoreText,
            10, 10,
            L"Segoe UI",   // or whatever font works
            28,
            0xFFFFFFFF,    // white
            true
        );

        eae6320::Logging::OutputMessage(("Score: " + std::to_string(m_score)).c_str());
    }
}


//===============================================================================================  
// SUBMIT RENDER DATA
//===============================================================================================
void eae6320::cMyGame::SubmitDataToBeRendered(
    const float i_elapsedSecondCount_systemTime,
    const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
    // Submit background color
    Graphics::SubmitClearColor(0.4f, 0.6f, 0.9f, 1.0f);

    // Camera transforms
    auto worldToCamera = m_camera.GetWorldToCamera(i_elapsedSecondCount_sinceLastSimulationUpdate);
    auto cameraToProject = m_camera.GetCameraToProjected();

    Graphics::SubmitCamera(worldToCamera, cameraToProject);

    // Draw the bird
    m_gameObject.SubmitForRendering(i_elapsedSecondCount_sinceLastSimulationUpdate);

    // Draw the floor
    m_gameObject2.SubmitForRendering(i_elapsedSecondCount_sinceLastSimulationUpdate);

    // Draw pipes via Pipe Manager
    // NOTE: This assumes your cPipeManager has:
    //   void SubmitForRendering( float dt );
    m_pipeManager.Render(i_elapsedSecondCount_sinceLastSimulationUpdate);
}



//===============================================================================================
// INITIALIZE
//===============================================================================================
eae6320::cResult eae6320::cMyGame::Initialize()
{
    auto result = Results::Success;

    // ------------------------
    // Audio
    // ------------------------
    eae6320::Audio::cAudio::Initialize();
    eae6320::Audio::cAudio::PlayMusic("data/audio/bgmusic.audio", true, 0.5f);

    // Initialize UI
    if (!g_overlay.Initialize(GetModuleHandle(nullptr)))
    {
        EAE6320_ASSERTF(false, "UIOverlay: Initialize failed");
    }
    else
    {
        g_overlay.CreateForWindow(eae6320::Application::iApplication::GetMainWindow(), true);

        std::wstring scoreText = L"Score: " + std::to_wstring(m_score);
        m_scoreTextId = g_overlay.AddText(
            scoreText,
            10, 10,
            L"Segoe UI",   // or whatever font works
            28,
            0xFFFFFFFF,    // white
            true
        );

        g_overlay.Show(0, 0, 512, 512, 1.0f);
    }

    // ------------------------
    // Effects
    // ------------------------
    if (!(result = Graphics::cEffect::Load(
        m_standardEffect,
        "data/Shaders/Vertex/standard.shader",
        "data/Shaders/Fragment/standard.shader")))
        return result;

    if (!(result = Graphics::cEffect::Load(
        m_animatedEffect,
        "data/Shaders/Vertex/standard.shader",
        "data/Shaders/Fragment/animatedColor.shader")))
        return result;

    // ------------------------
    // Meshes
    // ------------------------

    // Bird
    if (!(result = Graphics::cMesh::Load(
        m_squirrelMesh,
        "data/Meshes/squirrelRight.mesh")))
        return result;

    // Tree / background object (you already had this)
    if (!(result = Graphics::cMesh::Load(
        m_pipeMesh,
        "data/Meshes/tree.mesh")))
        return result;

    // Pipe mesh (reuse rectangle or a dedicated pipe mesh if you have one)
    // If you already created a "pipe.mesh", use that instead.
    if (!(result = Graphics::cMesh::Load(
        m_rectangleMesh,
        "data/Meshes/rectangle.mesh")))
        return result;

    // ------------------------
    // Game Object setup
    // ------------------------

    // Bird
    m_gameObject.m_mesh = m_squirrelMesh;
    m_gameObject.m_effect = m_standardEffect;
    m_gameObject.m_colliderHalfSize = Math::sVector(0.3f, 0.3f, 0.0f);
    //m_gameObject.m_scale = Math::sVector(0.5f, 0.5f, 1.0f);

    // floor
    m_gameObject2.m_mesh = m_rectangleMesh;
    m_gameObject2.m_effect = m_animatedEffect;
    m_gameObject2.m_rigidBody.position = Math::sVector(0.0f, -3.5f, 0.0f);
    m_gameObject2.m_scale = Math::sVector(3.0f, 1.0f, 1.0f);
    m_gameObject2.m_colliderHalfSize = Math::sVector(1.5f, 0.5f, 0.0f);

    // ------------------------
    // Camera + Bird initial positions
    // ------------------------
    m_camera.m_rigidBody.position = Math::sVector(0.0f, 0.0f, 30.0f);
    m_gameObject.m_rigidBody.position = Math::sVector(-3.0f, 0.0f, 0.0f);

    // ------------------------
    // Pipe Manager initialization
    // ------------------------
    const float pipeScrollSpeed = -5.0f;   // move left
    const float pipeSpawnInterval = 1.5f;  // seconds between spawns
    const float pipeStartX = 6.0f;         // off-screen to the right
    const float pipeGapSize = 3.5f;        // vertical gap for bird
    const int   initialPipeCount = 5;

    m_pipeManager.Initialize(
        m_pipeMesh,         // mesh
        m_standardEffect,   // effect
        pipeScrollSpeed,    // scroll speed (negative = left)
        pipeStartX,         // start X position offscreen
        pipeGapSize,        // vertical gap between pipes
        initialPipeCount    // number of pipe pairs
    );


    return Results::Success;
}

//===============================================================================================
// CLEAN UP
//===============================================================================================
eae6320::cResult eae6320::cMyGame::CleanUp()
{
    if (m_squirrelMesh) { m_squirrelMesh->DecrementReferenceCount(); m_squirrelMesh = nullptr; }
    if (m_treeMesh) { m_treeMesh->DecrementReferenceCount(); m_treeMesh = nullptr; }
	if (m_pipeMesh) { m_pipeMesh->DecrementReferenceCount(); m_pipeMesh = nullptr; }
    if (m_standardEffect) { m_standardEffect->DecrementReferenceCount(); m_standardEffect = nullptr; }
    if (m_animatedEffect) { m_animatedEffect->DecrementReferenceCount(); m_animatedEffect = nullptr; }

    g_Audio.Shutdown();
    //g_overlay.Shutdown();
    return Results::Success;
}

void eae6320::cMyGame::GameOver()
{
    // End the game immediately
    const auto result = Exit(EXIT_SUCCESS);
    EAE6320_ASSERT(result);
}

void eae6320::cMyGame::ResetGame()
{
    // Reset bird
    m_gameObject.m_rigidBody.position = Math::sVector(-3.0f, 0.0f, 0.0f);
    m_gameObject.m_rigidBody.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
    m_gameObject.m_rigidBody.acceleration = Math::sVector(0.0f, 0.0f, 0.0f);

    // Game state flags
    m_isGameOver = false;
}
