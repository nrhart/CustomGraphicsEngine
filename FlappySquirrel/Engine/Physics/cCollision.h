/*
	Simple AABB collision helpers for engine-level physics.
	Works with the existing Math::sVector and Physics namespace layout.
*/

#ifndef EAE6320_PHYSICS_COLLISION_H
#define EAE6320_PHYSICS_COLLISION_H

#include <Engine/Math/sVector.h>

namespace eae6320
{
	namespace Physics
	{
		// Axis-aligned bounding box in 2D/3D (Z is unused for 2D cases)
		struct AABB
		{
			Math::sVector center;    // world-space center
			Math::sVector halfSize;  // half-extents (x,y[,z])

			// Overlap test (X/Y only; Z ignored for 2D platformer use)
			bool Intersects(const AABB& other) const;
		};

		// Resolves overlap between two AABBs by moving "self" out of "other"
		// along the smallest penetration axis (X or Y). Also zeros the
		// corresponding velocity component. Returns true if any resolve happened.
		bool ResolveOverlap(
			Math::sVector& io_position,   // self position to adjust
			Math::sVector& io_velocity,   // self velocity to zero on the resolved axis
			const AABB& self,             // self collider (center should match io_position)
			const AABB& other             // static or dynamic collider to resolve against
		);
	}
}

#endif	// EAE6320_PHYSICS_COLLISION_H
