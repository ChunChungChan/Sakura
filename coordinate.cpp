

#include "coordinate.h"
#include "chrono"
#include "ctime"
#include "MathMisc.h"
Coordinate::Coordinate()
{

	pi = (float)(static_cast <double> (rand()) / static_cast <double> (RAND_MAX))*PI2;
	phi = (float)(static_cast <double> (rand()) / static_cast <double> (RAND_MAX))*PI / 2.0;
    radius = (float)(static_cast <double> (rand()) / static_cast <double> (RAND_MAX/30.0f))+60.0f;
	x = (float)(radius*cos(pi)*sin(phi)+10.0f);
	y = (float)(radius*cos(phi)+35.0f);
	z = (float)(radius*sin(pi)*sin(phi));

		rotation = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
		axis = (rand() % 3) + 1;
}
