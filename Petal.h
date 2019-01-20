#pragma once
#ifndef PETAL_H
#define PETAL_H
#include "LinearR4.h"
#include "coordinate.h"

class Petal
{
private:
	double yVelocity = 0;
	double xBeforeVelocity = 0;
	double zBeforeVelocity = 0;
	double xAfterVelocity = 0;
	double zAfterVelocity = 0;
	bool isFalling = false;
	bool treeCollided = false;
	bool groundCollided = false;

	
	static unsigned int myVBO;        
	static unsigned int myEBO;
	double current1 = 0.0;
	double current2 = 0.0;
public:
	static unsigned int myVAO;
	void fall();
	static unsigned int myMatVBO;
	void Render(int, Coordinate, float* mat, int, LinearMapR4 map);
	static void InitializeAttribLocations();

};

#endif