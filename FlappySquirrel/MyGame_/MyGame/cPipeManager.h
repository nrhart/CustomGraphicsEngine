#pragma once
#include <vector>
#include <cstdlib> // rand()
#include <ctime>   // time()
#include <Engine/GameObject/cGameObject.h>

namespace eae6320
{
    class cPipeManager
    {
    public:
        // Number of pipe pairs to spawn
        int pipeCount = 5;

        // Horizontal spacing
        float pipeSpacing = 6.0f;

        // Speed (negative moves left)
        float pipeSpeed = -4.0f;

        // Vertical gap between top/bottom pipes
        float gapSize = 3.0f;

        // Random offset range
        float minY = -2.0f;
        float maxY = 2.0f;

        // Mesh / Effect for pipes
        Graphics::cMesh* pipeMesh = nullptr;
        Graphics::cEffect* pipeEffect = nullptr;

        // Pipe objects (top AND bottom)
        std::vector<cGameObject> pipesTop;
        std::vector<cGameObject> pipesBottom;

        std::vector<bool> pipeScored;


    public:

        void Initialize(
            Graphics::cMesh* mesh,
            Graphics::cEffect* effect,
            float scrollSpeed,
            float startX,
            float gap,
            int count
        );

        void Update(float dt);
        void Render(float dt);
        bool CheckScore(const Math::sVector& playerPos);

    private:

        float RandomFloat(float minValue, float maxValue)
        {
            float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            return minValue + t * (maxValue - minValue);
        }

        void ResetPipePair(int index, float startX);
    };
}
