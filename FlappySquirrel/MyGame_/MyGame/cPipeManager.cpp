#include "cPipeManager.h"
#include <Engine/Graphics/Graphics.h>

namespace eae6320
{
    void cPipeManager::Initialize(
        Graphics::cMesh* mesh,
        Graphics::cEffect* effect,
        float scrollSpeed,
        float startX,
        float gap,
        int count
    )
    {
        srand((unsigned int)time(nullptr));

        pipeMesh = mesh;
        pipeEffect = effect;
        pipeSpeed = scrollSpeed;
        gapSize = gap;
        pipeCount = count;

        pipesTop.resize(pipeCount);
        pipesBottom.resize(pipeCount);
        pipeScored.resize(pipeCount, false);


        for (int i = 0; i < pipeCount; i++)
        {
            float x = startX + i * pipeSpacing;

            pipesTop[i].m_mesh = mesh;
            pipesTop[i].m_effect = effect;
            pipesTop[i].m_scale = Math::sVector(1.0f, 1.0f, 1.0f);
            pipesTop[i].m_colliderHalfSize = Math::sVector(0.5f, 5.0f, 0.0f);

            pipesBottom[i].m_mesh = mesh;
            pipesBottom[i].m_effect = effect;
            pipesBottom[i].m_scale = Math::sVector(1.0f, 1.0f, 1.0f);
            pipesBottom[i].m_colliderHalfSize = Math::sVector(0.5f, 5.0f, 0.0f);


            ResetPipePair(i, x);
        }
    }


    // Reset ONE pair of pipes
    void cPipeManager::ResetPipePair(int index, float startX)
    {
        float offsetY = RandomFloat(minY, maxY);

        const float halfPipeHeight = 5.0f;   // because mesh is now -5 to +5

        pipeScored[index] = false;

        // ----- TOP PIPE -----
        pipesTop[index].m_rigidBody.position =
            Math::sVector(startX,
                offsetY + (gapSize * 0.5f) + halfPipeHeight,
                -10.0f);

        // ----- BOTTOM PIPE -----
        pipesBottom[index].m_rigidBody.position =
            Math::sVector(startX,
                offsetY - (gapSize * 0.5f) - halfPipeHeight,
                -10.0f);
    }

    void cPipeManager::Update(float dt)
    {
        for (int i = 0; i < pipeCount; i++)
        {
            // Move left
            pipesTop[i].m_rigidBody.position.x += pipeSpeed * dt;
            pipesBottom[i].m_rigidBody.position.x += pipeSpeed * dt;

            // If off the screen, reset to right side
            if (pipesTop[i].m_rigidBody.position.x < -8.0f)
            {
                float startX = pipesTop[i].m_rigidBody.position.x + pipeCount * pipeSpacing;
                ResetPipePair(i, startX);
            }
        }
    }

    void cPipeManager::Render(float dt)
    {
        for (int i = 0; i < pipeCount; i++)
        {
            pipesTop[i].SubmitForRendering(dt);
            pipesBottom[i].SubmitForRendering(dt);
        }
    }

    bool cPipeManager::CheckScore(const Math::sVector& playerPos)
    {
        for (int i = 0; i < pipeCount; i++)
        {
            // If pipe already counted, skip it
            if (pipeScored[i])
                continue;

            // If player has passed the pipe's X
            if (playerPos.x > pipesTop[i].m_rigidBody.position.x)
            {
                pipeScored[i] = true;  // mark as scored
                return true;           // signal that a point happened
            }
        }
        return false;
    }

}
