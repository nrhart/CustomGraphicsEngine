#include "cGameObject.h"

namespace eae6320
{
	void cGameObject::UpdateSimulation(const float i_elapsedSecondCount_sinceLastUpdate)
	{
		m_rigidBody.Update(i_elapsedSecondCount_sinceLastUpdate);
	}

	Math::cMatrix_transformation cGameObject::GetLocalToWorldTransform(
		const float i_elapsedSecondCount_sinceLastSimulationUpdate) const
	{
		return m_rigidBody.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
	}

	void cGameObject::SubmitForRendering(float i_elapsedSecondCount_sinceLastSimulationUpdate) const
	{
		if (m_mesh && m_effect)
		{
			auto localToWorld = GetLocalToWorldTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);

			const auto scaleMatrix = Math::cMatrix_transformation::CreateScale(m_scale);
			const auto scaledTransform = scaleMatrix * localToWorld;

			Graphics::SubmitDrawCall(m_mesh, m_effect, scaledTransform);
		}
	}
}
