#pragma once
#ifndef TEST_COLLISION
#define TEST_COLLISION
#include "Petal.h"

inline bool isCollided(VectorR3 pos, int ObjId, double * xVelocity, double * zVelocity, double xBefore, double zBefore, LinearMapR4 map)
{
    viewMatrix.Inverse().AffineTransformPosition(pos);
	if (ObjId == 1)
	{
		
		float x = pos.x;
		float z = pos.z;

		if (sqrt(x*x + z*z) < 5)
		{
			x += xBefore * 5;
			z += zBefore * 5;
			*xVelocity = xBefore + x;
			*zVelocity = zBefore + z;
			if (abs(*xVelocity) > abs(*zVelocity))
			{

				*zVelocity /= abs(*xVelocity);
				*xVelocity /= abs(*xVelocity);
			}
			else
			{
				*xVelocity /= abs(*zVelocity);
				*zVelocity /= abs(*zVelocity);
			}
			
			return true;
		}
	}
	if (ObjId == 2)
	{
		float y = pos.y;
		if (y <= -2)
			return true;
	}
	return false;
}
#endif // !1
