// Graphics.cpp

#include "Graphics.h"
#include "sContext.h"

#include "cMesh.h"
#include "cEffect.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/UserOutput/UserOutput.h>
#include <Engine/Graphics/VertexFormats.h> 

#include <utility>
#include <cmath>

namespace
{
    // Constant buffers
    eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame);
    eae6320::Graphics::cConstantBuffer s_constantBuffer_drawCall(eae6320::Graphics::ConstantBufferTypes::DrawCall);

    // A hard limit on how many draw calls per frame
    constexpr size_t k_maxDrawCallsPerFrame = 64;

    struct sDrawCall
    {
        eae6320::Graphics::cMesh* mesh = nullptr;
        eae6320::Graphics::cEffect* effect = nullptr;
        eae6320::Math::cMatrix_transformation localToWorld;
    };

    struct sDataRequiredToRenderAFrame
    {
        eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;
        float clearR = 0.0f, clearG = 0.0f, clearB = 0.0f, clearA = 1.0f;

        // Fixed-size array of draw calls
        sDrawCall drawCalls[k_maxDrawCallsPerFrame];
        size_t drawCallCount = 0;
    };

    // Double-buffered frame data
    sDataRequiredToRenderAFrame s_frameData[2];
    auto* s_dataBeingSubmittedByApplicationThread = &s_frameData[0];
    auto* s_dataBeingRenderedByRenderThread = &s_frameData[1];

    eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
    eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;
}

// -------- Submission --------

void eae6320::Graphics::SubmitElapsedTime(const float sys, const float sim)
{
    auto& c = s_dataBeingSubmittedByApplicationThread->constantData_frame;
    c.g_elapsedSecondCount_systemTime = sys;
    c.g_elapsedSecondCount_simulationTime = sim;
}

void eae6320::Graphics::SubmitClearColor(float r, float g, float b, float a)
{
    s_dataBeingSubmittedByApplicationThread->clearR = r;
    s_dataBeingSubmittedByApplicationThread->clearG = g;
    s_dataBeingSubmittedByApplicationThread->clearB = b;
    s_dataBeingSubmittedByApplicationThread->clearA = a;
}

void eae6320::Graphics::SubmitCamera(
    const Math::cMatrix_transformation& i_worldToCamera,
    const Math::cMatrix_transformation& i_cameraToProjected)
{
    auto& c = s_dataBeingSubmittedByApplicationThread->constantData_frame;
    c.g_transform_worldToCamera = i_worldToCamera;
    c.g_transform_cameraToProjected = i_cameraToProjected;
}


void eae6320::Graphics::SubmitDrawCall(cMesh* i_mesh, cEffect* i_effect, const Math::cMatrix_transformation& i_localToWorld)
{
    EAE6320_ASSERT(i_mesh && i_effect);

    auto& frame = *s_dataBeingSubmittedByApplicationThread;

    if (frame.drawCallCount < k_maxDrawCallsPerFrame)
    {
        auto& drawCall = frame.drawCalls[frame.drawCallCount];
        drawCall.mesh = i_mesh;
        drawCall.effect = i_effect;
        drawCall.localToWorld = i_localToWorld;


        i_mesh->IncrementReferenceCount();
        i_effect->IncrementReferenceCount();

        ++frame.drawCallCount;
    }
#if defined(_DEBUG)
    else
    {
        EAE6320_ASSERTF(false, "Exceeded maximum draw calls (%zu) this frame!", k_maxDrawCallsPerFrame);
    }
#endif
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int ms)
{
    return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, ms);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
    return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

// -------- Render --------

void eae6320::Graphics::RenderFrame()
{
    // Wait until game has submitted all data
    Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread);
    std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
    s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal();

    auto& frame = *s_dataBeingRenderedByRenderThread;

    // Clear back buffer
    sContext::g_context.ClearBackBuffer(
        frame.clearR,
        frame.clearG,
        frame.clearB,
        frame.clearA
    );
    sContext::g_context.ClearDepth();

    // Update constant buffer
    s_constantBuffer_frame.Update(&frame.constantData_frame);

    // Draw all submitted calls
    for (size_t i = 0; i < frame.drawCallCount; ++i)
    {
        auto& drawCall = frame.drawCalls[i];

        if (drawCall.mesh && drawCall.effect)
        {
            // Fill the struct
            eae6320::Graphics::ConstantBufferFormats::sDrawCall cb;
            cb.g_transform_localToWorld = drawCall.localToWorld;

            // Send to GPU
            s_constantBuffer_drawCall.Update(&cb);

            // Draw
            drawCall.effect->Bind();
            drawCall.mesh->Draw();

            drawCall.effect->DecrementReferenceCount();
            drawCall.mesh->DecrementReferenceCount();
        }
    }

    // Reset draw call count for reuse
    frame.drawCallCount = 0;

    // Swap buffers
    sContext::g_context.SwapBuffers();
}

// -------- Init / CleanUp --------

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& params)
{
    auto result = Results::Success;

    if (!(result = sContext::g_context.Initialize(params)))
        return result;

    if (result = s_constantBuffer_frame.Initialize())
    {
        const auto stages = static_cast<uint_fast8_t>(eShaderType::Vertex) |
            static_cast<uint_fast8_t>(eShaderType::Fragment);
        s_constantBuffer_frame.Bind(stages);
    }

    if (result = s_constantBuffer_drawCall.Initialize())
    {
        const auto stages = static_cast<uint_fast8_t>(eShaderType::Vertex) |
            static_cast<uint_fast8_t>(eShaderType::Fragment);
        s_constantBuffer_drawCall.Bind(stages);
    }

    s_whenAllDataHasBeenSubmittedFromApplicationThread
        .Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled);
    s_whenDataForANewFrameCanBeSubmittedFromApplicationThread
        .Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
            Concurrency::EventState::Signaled);
    return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
    // Clean up both frame buffers in case of leftover references
    for (auto& frame : s_frameData)
    {
        for (size_t i = 0; i < frame.drawCallCount; ++i)
        {
            if (frame.drawCalls[i].effect) { frame.drawCalls[i].effect->DecrementReferenceCount(); frame.drawCalls[i].effect = nullptr; }
            if (frame.drawCalls[i].mesh) { frame.drawCalls[i].mesh->DecrementReferenceCount(); frame.drawCalls[i].mesh = nullptr; }
        }
        frame.drawCallCount = 0;
    }

    s_constantBuffer_frame.CleanUp();
    s_constantBuffer_drawCall.CleanUp();

    return sContext::g_context.CleanUp();
}