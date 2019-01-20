#include "Rain.h"


rain::rain():rainDrop(4,4)
{
	pos = new float*[dropCount];
	for (int x = 0; x < dropCount; x++)
	{
		pos[x] = new float[3];
	}
}

rain::rain(int drop):rainDrop(4,4)
{
	this->dropCount = drop;
	pos = new float*[dropCount];
	for (int x = 0; x < dropCount; x++)
	{
		pos[x] = new float[3];
	}
}

rain::~rain()
{
	for (int x = 0; x < dropCount; x++)
	{
		delete[] pos[x];
	}
	delete[] pos;
}

void rain::InitializeAttribLocations()
{
	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y < dropCount/5; y++)
		{
			pos[y + x*dropCount/5][0] = (float)(static_cast <double> (rand()) / static_cast <double> (RAND_MAX / 2000.0f)) -1000.0f;
			pos[y + x*dropCount / 5][1] = (float)(static_cast <double> (rand()) / static_cast <double> (RAND_MAX / 2000.0f)) -1000.0f;
			pos[y + x*dropCount / 5][2] = 30.0f - 6*x;
		}
	}
	rainDrop.InitializeAttribLocations(vertPos_loc, vertNormal_loc, vertTexCoords_loc);

}

void rain::render()
{

	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y < dropCount/5; y++)
		{
			LinearMapR4 mat = viewMatrix;
			mat.Mult_glScale(0.02, 0.5, 0.02);
			mat.Mult_glTranslate(pos[y + x*dropCount / 5][0], pos[y + x*dropCount / 5][2]+30, pos[y + x*dropCount / 5][1]);
			mat.DumpByColumns(matEntries);
			glVertexAttrib4fv(8, matEntries);
			glVertexAttrib4fv(9, matEntries + 4);
			glVertexAttrib4fv(10, matEntries + 8);
			glVertexAttrib4fv(11, matEntries + 12);
			rainDrop.Render();
			if (pos[y + x*dropCount / 5][2] == 4)
			{
				pos[y + x*dropCount / 5][0] = (float)(static_cast <double> (rand()) / static_cast <double> (RAND_MAX / 2000.0f)) - 1000.0f;
				pos[y + x*dropCount / 5][1] = (float)(static_cast <double> (rand()) / static_cast <double> (RAND_MAX / 2000.0f)) - 1000.0f;
			}
			pos[y + x*dropCount / 5][2] = (float)((int)(pos[y + x*dropCount / 5][2] - 4.0f) % 41);
		}
	}

}
