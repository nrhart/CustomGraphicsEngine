#include "cCamera.h"

namespace eae6320
{
	Math::cMatrix_transformation cCamera::PredictFutureTransform(
		const float i_elapsedSecondCount_sinceLastSimulationUpdate) const
	{
		return m_rigidBody.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
	}

	Math::cMatrix_transformation cCamera::GetWorldToCamera(
		const float i_elapsedSecondCount_sinceLastSimulationUpdate) const
	{
		return Math::cMatrix_transformation::CreateWorldToCameraTransform(
			PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
	}

	Math::cMatrix_transformation cCamera::GetCameraToProjected() const
	{
		return Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(
			Math::ConvertDegreesToRadians(m_fieldOfViewDegrees),
			m_aspectRatio,
			m_zNear,
			m_zFar
		);
	}
}
