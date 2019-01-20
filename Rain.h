#pragma once
#ifndef RAIN_H
#define RAIN_H

#include "LinearR4.h"
#include "GlGeomSphere.h"
#include "ShaderBuild.h"
#include "CubeMapSample.h"
class rain {
private:
	int dropCount = 100;
	float ** pos;
	GlGeomSphere rainDrop;
public:
	rain();
	rain(int drop);
	~rain();
	void InitializeAttribLocations();
	void render();
};

#endif
