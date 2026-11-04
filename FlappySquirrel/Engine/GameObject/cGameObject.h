#pragma once

#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/cMesh.h>

namespace eae6320
{
	class cGameObject
	{
	public:
		cGameObject() = default;
		~cGameObject() = default;

		Physics::sRigidBodyState m_rigidBody;

		Graphics::cMesh* m_mesh = nullptr;
		Graphics::cEffect* m_effect = nullptr;

		Math::sVector m_scale = Math::sVector(1.0f, 1.0f, 1.0f);
		Math::sVector m_colliderHalfSize = Math::sVector(0.5f, 0.5f, 0.0f);

		void UpdateSimulation(const float i_elapsedSecondCount_sinceLastUpdate);

		Math::cMatrix_transformation GetLocalToWorldTransform(
			const float i_elapsedSecondCount_sinceLastSimulationUpdate) const;

		void SubmitForRendering(float i_elapsedSecondCount_sinceLastSimulationUpdate) const;
	};
}
