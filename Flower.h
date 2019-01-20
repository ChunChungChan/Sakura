#pragma once
#ifndef FLOWER_H
#define FLOWER_H
#include "Petal.h"
#include "coordinate.h"
class Flower
{
private:
	int numPetals =5;
	
	static unsigned int myVBO;
	static unsigned int myEBO;
public:
	Petal petals[5];
	static unsigned int myMatVBO;
	static unsigned int myVAO;
	bool isEmpty();
	void dropPetal();
	void Render(Coordinate, float* mat);
	static void InitializeAttribLocations();

};
#endif