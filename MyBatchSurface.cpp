#include "MyBatchSurface.h"
#include <iomanip>
MyBatchSurface::MyBatchSurface(int res)
{
	meshResolution = res;
}

MyBatchSurface::~MyBatchSurface()
{
}

void MyBatchSurface::InitializeAttribLocations()
{
	if (myVAO == 0) {
		glGenVertexArrays(1, &myVAO);
		glGenBuffers(1, &myVBO);
		glGenBuffers(1, &myEBO);
	}
	glBindBuffer(GL_ARRAY_BUFFER, myVBO);
	glBindVertexArray(myVAO);
	glBufferData(GL_ARRAY_BUFFER, (meshResolution)*2*(meshResolution + 1) *8, 0, GL_STATIC_DRAW);
	glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
	glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
	glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	// Vertex normals in the VBO
	glEnableVertexAttribArray(vertNormal_loc);
	glVertexAttribPointer(vertTexCoords_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
	glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshResolution * 2 * (meshResolution + 1) * sizeof(unsigned int), 0, GL_STATIC_DRAW);

	LoadBufferData();
}

void MyBatchSurface::Remesh(int res)
{
	meshResolution = res;
	LoadBufferData();
}

void MyBatchSurface::Render()
{
	glBindVertexArray(myVAO);
	for (int x = 0; x < meshResolution; x++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, meshResolution*4+2, GL_UNSIGNED_INT, (void*)(x*(meshResolution * 4 + 2 )* sizeof(unsigned int)));                              // Draw first triangle strip (back strip)
	}
}

void MyBatchSurface::ChangeRadius(int index, float radius)
{
	controlPoints[index * 12] = -radius;
	backControlPoints[index * 12] = -radius;
	controlPoints[index * 12+6] = radius;
	backControlPoints[index * 12+6] = -radius;
	controlPoints[index * 12+8] = radius;
	backControlPoints[index * 12+8] = radius;
	LoadBufferData();
}

void MyBatchSurface::Bend(int weight, double degree)
{
	int row = 0;
	for (int x = 0; x < weight * 3; x++)
	{
		if (x % 3 == 0)
		{
			row++;
		}
		if (controlPoints[x * 4 + 3] != 0)
		{
			controlPoints[x * 4] = controlPoints[x * 4] - (float)(degree / ((double)row));
			backControlPoints[x * 4] = backControlPoints[x * 4] - (float)(degree / ((double)row));
		}
	}
	LoadBufferData();
}

MyBatchSurface::MyBatchSurface(const MyBatchSurface& old)
{
	meshResolution = old.meshResolution;
	for (int x = 0; x < 48; x++)
	{
		controlPoints[x] = old.controlPoints[x];
		backControlPoints[x] = old.backControlPoints[x];
	}
}

void MyBatchSurface::LoadBufferData()
{
	biezier = new float[meshResolution * 2 * (meshResolution + 1) * 8];
	float deltaU = 1 / (float)(meshResolution );
	float deltaV = 1 / (float)(meshResolution );
	float v = 0;
	for (int i = 0; i < meshResolution +1; i++)
	{
		float u = 0;
		for (int j = 0; j < meshResolution; j++)
		{
			float tempX = 0;
			float tempY = 0;
			float tempZ = 0;
			float tempW = 0;
			float Bv[] = { 1,3,3,1 };
			float Bu[] = { 1,2,1 };
			for (int n = 0; n < 4; n++)
			{
				for (int m = 0; m < 3; m++)
				{
					tempX += (pow(u, m)*pow(1 - u, 2 - m)*controlPoints[4 * m + n * 12] * Bu[m])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				}
				for (int m = 0; m < 3; m++)
				{
					tempY += (pow(u, m)*pow(1 - u, 2 - m)*controlPoints[4 * m + n * 12 + 1] * Bu[m])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				}
				for (int m = 0; m < 3; m++)
				{
					tempZ += (pow(u, m)*pow(1 - u, 2 - m)*controlPoints[4 * m + n * 12 + 2] * Bu[m])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				}
				for (int m = 0; m < 3; m++)
				{
					tempW += (pow(u, m)*pow(1 - u, 2 - m)*controlPoints[4 * m + n * 12 + 3] * Bu[m])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				}
			}
			biezier[8 * j + 8 * (meshResolution)*2*i] = (tempX / tempW);
			biezier[8 * j + 1 + 8 * (meshResolution*2)*i] = (tempY / tempW);
			biezier[8 * j + 2 + 8 * (meshResolution*2)*i] = (tempZ / tempW);
			float NtempX = 0.0f;
			float NtempY = 0.0f;
			float NtempZ = 0.0f;
			float NtempW = 0.0f;
			for (int n = 0; n < 4; n++)
			{
				NtempX += (-2 * (1 - u)*controlPoints[0 + n * 12] + 2 * (1 - 2 * u) * controlPoints[4 + 12 * n] + 2 * u*controlPoints[8 + 12 * n])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				NtempY += (-2 * (1 - u)*controlPoints[1 + n * 12] + 2 * (1 - 2 * u) * controlPoints[5 + 12 * n] + 2 * u*controlPoints[9 + 12 * n])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				NtempZ += (-2 * (1 - u)*controlPoints[2 + n * 12] + 2 * (1 - 2 * u) * controlPoints[6 + 12 * n] + 2 * u*controlPoints[10 + 12 * n])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				NtempW += (-2 * (1 - u)*controlPoints[3 + n * 12] + 2 * (1 - 2 * u) * controlPoints[7 + 12 * n] + 2 * u*controlPoints[11 + 12 * n])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);

			}
			float dUX = (NtempX*tempW - tempX*NtempW) / (tempW*tempW);
			float dUY = (NtempY*tempW - tempY*NtempW) / (tempW*tempW);
			float dUZ = (NtempZ*tempW - tempZ*NtempW) / (tempW*tempW);
			NtempX = 0.0f;
			NtempY = 0.0f;
			NtempZ = 0.0f;
			NtempW = 0.0f;
			float *dVconst = new float[4];
			dVconst[0] = -3 * pow(1 - v, 2);
			dVconst[1] = 3 * pow(1 - v, 2) - 3 * v * 2 * (1 - v);
			dVconst[2] = 6 * v*(1 - v) - 3 * v*v;
			dVconst[3] = 3 * v*v;
			for (int n = 0; n < 4; n++)
			{
				for (int m = 0; m < 3; m++)
				{
					NtempX += (pow(u, m)*pow(1 - u, 2 - m)*controlPoints[4 * m + n * 12] * Bu[m])*dVconst[n];
				}
				for (int m = 0; m < 3; m++)
				{
					NtempY += (pow(u, m)*pow(1 - u, 2 - m)*controlPoints[4 * m + n * 12 + 1] * Bu[m])*dVconst[n];
				}
				for (int m = 0; m < 3; m++)
				{
					NtempZ += (pow(u, m)*pow(1 - u, 2 - m)*controlPoints[4 * m + n * 12 + 2] * Bu[m])*dVconst[n];
				}
				for (int m = 0; m < 3; m++)
				{
					NtempW += (pow(u, m)*pow(1 - u, 2 - m)*controlPoints[4 * m + n * 12 + 3] * Bu[m])*dVconst[n];
				}
			}
			float dVX = (NtempX*tempW - tempX*NtempW) / (tempW*tempW);
			float dVY = (NtempY*tempW - tempY*NtempW) / (tempW*tempW);
			float dVZ = (NtempZ*tempW - tempZ*NtempW) / (tempW*tempW);
			biezier[8 * j + 3 + 8 * (meshResolution*2)*i] = -(dUY * dVZ - dUZ * dVY);
			biezier[8 * j + 4 + 8 * (meshResolution*2)*i] = (dUX * dVZ - dUZ * dVX);
			biezier[8 * j + 5 + 8 * (meshResolution*2)*i] = -(dUX * dVY - dUY * dVX);
			biezier[8 * j + 6 + 8 * (meshResolution*2)*i] = u/2;
			biezier[8 * j + 7 + 8 * (meshResolution*2)*i] = 1-v;
			u += deltaU;
			delete[] dVconst;

		}
		for (int j = 0; j < meshResolution; j++)
		{
			float tempX = 0;
			float tempY = 0;
			float tempZ = 0;
			float tempW = 0;
			float Bv[] = { 1,3,3,1 };
			float Bu[] = { 1,2,1 };
			for (int n = 0; n < 4; n++)
			{
				for (int m = 0; m < 3; m++)
				{
					tempX += (pow(u, m)*pow(1 - u, 2 - m)*backControlPoints[4 * m + n * 12] * Bu[m])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				}
				for (int m = 0; m < 3; m++)
				{
					tempY += (pow(u, m)*pow(1 - u, 2 - m)*backControlPoints[4 * m + n * 12 + 1] * Bu[m])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				}
				for (int m = 0; m < 3; m++)
				{
					tempZ += (pow(u, m)*pow(1 - u, 2 - m)*backControlPoints[4 * m + n * 12 + 2] * Bu[m])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				}
				for (int m = 0; m < 3; m++)
				{
					tempW += (pow(u, m)*pow(1 - u, 2 - m)*backControlPoints[4 * m + n * 12 + 3] * Bu[m])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				}
			}
			biezier[8 * (meshResolution+j) + 8 * (meshResolution*2)*i] = (tempX / tempW);
			biezier[8 * (j+meshResolution) + 1 + 8 * (meshResolution*2)*i] = (tempY / tempW);
			biezier[8 * (j+meshResolution) + 2 + 8 * (meshResolution*2)*i] = (tempZ / tempW);
			float NtempX = 0.0f;
			float NtempY = 0.0f;
			float NtempZ = 0.0f;
			float NtempW = 0.0f;
			for (int n = 0; n < 4; n++)
			{
				NtempX += (-2 * (1 - u)*backControlPoints[0 + n * 12] + 2 * (1 - 2 * u) * backControlPoints[4 + 12 * n] + 2 * u*backControlPoints[8 + 12 * n])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				NtempY += (-2 * (1 - u)*backControlPoints[1 + n * 12] + 2 * (1 - 2 * u) * backControlPoints[5 + 12 * n] + 2 * u*backControlPoints[9 + 12 * n])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				NtempZ += (-2 * (1 - u)*backControlPoints[2 + n * 12] + 2 * (1 - 2 * u) * backControlPoints[6 + 12 * n] + 2 * u*backControlPoints[10 + 12 * n])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);
				NtempW += (-2 * (1 - u)*backControlPoints[3 + n * 12] + 2 * (1 - 2 * u) * backControlPoints[7 + 12 * n] + 2 * u*backControlPoints[11 + 12 * n])*(pow(v, n)*pow(1 - v, 3 - n)*Bv[n]);

			}
			float dUX = (NtempX*tempW - tempX*NtempW) / (tempW*tempW);
			float dUY = (NtempY*tempW - tempY*NtempW) / (tempW*tempW);
			float dUZ = (NtempZ*tempW - tempZ*NtempW) / (tempW*tempW);
			NtempX = 0.0f;
			NtempY = 0.0f;
			NtempZ = 0.0f;
			NtempW = 0.0f;
			float *dVconst = new float[4];
			dVconst[0] = -3 * pow(1 - v, 2);
			dVconst[1] = 3 * pow(1 - v, 2) - 3 * v * 2 * (1 - v);
			dVconst[2] = 6 * v*(1 - v) - 3 * v*v;
			dVconst[3] = 3 * v*v;
			for (int n = 0; n < 4; n++)
			{
				for (int m = 0; m < 3; m++)
				{
					NtempX += (pow(u, m)*pow(1 - u, 2 - m)*backControlPoints[4 * m + n * 12] * Bu[m])*dVconst[n];
				}
				for (int m = 0; m < 3; m++)
				{
					NtempY += (pow(u, m)*pow(1 - u, 2 - m)*backControlPoints[4 * m + n * 12 + 1] * Bu[m])*dVconst[n];
				}
				for (int m = 0; m < 3; m++)
				{
					NtempZ += (pow(u, m)*pow(1 - u, 2 - m)*backControlPoints[4 * m + n * 12 + 2] * Bu[m])*dVconst[n];
				}
				for (int m = 0; m < 3; m++)
				{
					NtempW += (pow(u, m)*pow(1 - u, 2 - m)*backControlPoints[4 * m + n * 12 + 3] * Bu[m])*dVconst[n];
				}
			}
			float dVX = (NtempX*tempW - tempX*NtempW) / (tempW*tempW);
			float dVY = (NtempY*tempW - tempY*NtempW) / (tempW*tempW);
			float dVZ = (NtempZ*tempW - tempZ*NtempW) / (tempW*tempW);
			biezier[8 * (j+meshResolution) + 3 + 8 * (meshResolution*2)*i] = (dUY * dVZ - dUZ * dVY);
			biezier[8 * (j+meshResolution) + 4 + 8 * (meshResolution*2)*i] = -(dUX * dVZ - dUZ * dVX);
			biezier[8 * (j+meshResolution) + 5 + 8 * (meshResolution*2)*i] = (dUX * dVY - dUY * dVX);
			biezier[8 * (j+meshResolution) + 6 + 8 * (meshResolution*2)*i] = 1-(u/2);
			biezier[8 * (j+meshResolution) + 7 + 8 * (meshResolution*2)*i] = 1-v;
			u -= deltaU;
			delete[] dVconst;

		}
		v += deltaV;
	}
	glBindVertexArray(myVAO);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO);
	glBufferData(GL_ARRAY_BUFFER, (meshResolution*2)*(meshResolution+1) * 8 * sizeof(float), biezier, GL_STATIC_DRAW);

	int current1 = 0;
	int *tempEle = new int[ meshResolution*(meshResolution*4+2)];
	unsigned int ele = 0;
	unsigned int shift = 0;
	for (int x = 0; x <  meshResolution*(meshResolution * 4 + 2); x++) {

		if (x==0||(x % 2 == 0&& !((x + 1 - shift) % (meshResolution * 4)==0)))
		{
			tempEle[current1++] = ele;
			ele += meshResolution*2;
		}
		else if ((x + 1 - shift) % (meshResolution * 4)==0)
		{
			tempEle[current1++] = ele;
			tempEle[current1++] = ele - (meshResolution * 4 - 1);
			x++;
			x++;
			shift++;
			shift++;
			ele -= (meshResolution * 2 - 1);
			tempEle[current1++] = ele;
		}
		else
		{
			tempEle[current1++] = ele;
			ele -= (meshResolution*2-1);
		}

	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshResolution*(meshResolution * 4 + 2) * sizeof(unsigned int), tempEle, GL_STATIC_DRAW);
	delete[] biezier;
	delete[] tempEle;
	glBindVertexArray(0);
}
