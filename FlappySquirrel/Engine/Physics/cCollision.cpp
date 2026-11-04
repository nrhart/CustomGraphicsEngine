#include "cCollision.h"
#include <cmath>

bool eae6320::Physics::AABB::Intersects(const AABB& other) const
{
	return (
		std::fabs(center.x - other.center.x) <= (halfSize.x + other.halfSize.x) &&
		std::fabs(center.y - other.center.y) <= (halfSize.y + other.halfSize.y)
		);
}

bool eae6320::Physics::ResolveOverlap(
	Math::sVector& io_position,
	Math::sVector& io_velocity,
	const AABB& self,
	const AABB& other)
{
	Math::sVector delta = self.center - other.center;

	Math::sVector overlap;
	overlap.x = (self.halfSize.x + other.halfSize.x) - std::fabs(delta.x);
	overlap.y = (self.halfSize.y + other.halfSize.y) - std::fabs(delta.y);

	bool isGrounded = false;

	if (overlap.x > 0.0f && overlap.y > 0.0f)
	{
		// Choose smaller penetration axis
		if (overlap.x < overlap.y)
		{
			// Horizontal resolution
			float pushDirX = (delta.x > 0.0f) ? 1.0f : -1.0f;
			io_position.x += pushDirX * overlap.x;
			io_velocity.x = 0.0f;
		}
		else
		{
			// Vertical resolution
			float pushDirY = (delta.y > 0.0f) ? 1.0f : -1.0f;
			io_position.y += pushDirY * overlap.y;
			io_velocity.y = 0.0f;

			// Ground check — only mark as grounded if we were falling onto it
			if (pushDirY > 0.0f && io_velocity.y <= 0.0f)
			{
				isGrounded = true;
				io_position.y += 0.001f; 
			}
		}
	}

	return isGrounded;
}
