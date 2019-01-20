#pragma once
#ifndef BATCH_SURFACE_H
#define BATCH_SURFACE_H

#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR3.h"		
#include "LinearR4.h"		
#include "MathMisc.h"       
#include "ShaderBuild.h"
#include "TextureProj.h"
#include "PhongData.h"
#include "RgbImage.h"
#include "MyGeometries.h"
#include "CubeMapSample.h"
class MyBatchSurface
{
public:
	MyBatchSurface() : MyBatchSurface(4) {}
	MyBatchSurface(int res);
	~MyBatchSurface();

	void InitializeAttribLocations();

	void Remesh(int res);

	void Render();
	void RenderTop();
	void RenderBase();
	void RenderSide();
	void ChangeRadius(int index, float radius);
	void Bend(int index, double degree);

	MyBatchSurface(const MyBatchSurface&);
	MyBatchSurface& operator=(const MyBatchSurface&) = delete;
	MyBatchSurface(MyBatchSurface&&) = delete;
	MyBatchSurface& operator=(MyBatchSurface&&) = delete;

private:
	void LoadBufferData();


private:
	unsigned int myVAO = 0;        // Vertex Array Object
	unsigned int myVBO = 0;        // Vertex Buffer Object
	unsigned int myEBO = 0;        // Element Buffer Object;


	int meshResolution;
	float controlPoints[48] = { -1, 2, 0, 1, 0, 0, 1, 0, 1, 2, 0, 1, -1.5, 1.5, 0, 1, 0, 0, 1.5, 0, 1.5, 1.5, 0, 1, -1.5,0.5,0,1,0,0,1.5,0,1.5,0.5,0,1,-1,0,0,1,0,0,1,0,1,0,0,1 };
	float backControlPoints[48] = { -1, 2, 0, 1, 0, 0, -1, 0, 1, 2, 0, 1, -1.5, 1.5, 0, 1, 0, 0, -1.5, 0, 1.5, 1.5, 0, 1, -1.5,0.5,0,1,0,0,-1.5,0,1.5,0.5,0,1,-1,0,0,1,0,0,-1,0,1,0,0,1 };
	float* biezier;
	float* top;
	float* bottom;
	int numControlPoints = 12;
	int numBiezier = meshResolution*meshResolution;
};

#endif
