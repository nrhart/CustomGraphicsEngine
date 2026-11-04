#pragma once

#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Math/Functions.h>

namespace eae6320
{
	class cCamera
	{
	public:
		Physics::sRigidBodyState m_rigidBody;

		float m_fieldOfViewDegrees = 15.0f;
		float m_aspectRatio = 1.0f;   
		float m_zNear = 0.1f;    
		float m_zFar = 100.0f;  

		Math::cMatrix_transformation PredictFutureTransform(
			const float i_elapsedSecondCount_sinceLastSimulationUpdate) const;

		Math::cMatrix_transformation GetWorldToCamera(
			const float i_elapsedSecondCount_sinceLastSimulationUpdate) const;

		Math::cMatrix_transformation GetCameraToProjected() const;
	};
}
