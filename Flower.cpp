#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR3.h"		
#include "LinearR4.h"		
#include "MathMisc.h"       
#include "ShaderBuild.h"
#include "Petal.h"
#include "TextureProj.h"
#include "PhongData.h"
#include "RgbImage.h"
#include "GlGeomCylinder.h"
#include "GlGeomSphere.h"
#include "Flower.h"
#include "MyGeometries.h"
 unsigned int Flower::myVAO = 0;
 unsigned int Flower::myVBO = 0;
 unsigned int Flower::myEBO = 0;
 unsigned int Flower::myMatVBO = 0;
bool Flower::isEmpty()
{
	return numPetals==0;
}

void Flower::dropPetal()
{
	if (!isEmpty())
	{
		petals[numPetals - 1].fall();
		numPetals--;
	}
}



void Flower::Render(Coordinate co, float* mat)
{

	//glBindTexture(GL_TEXTURE_2D, TextureNames);     
	////glUniform1i(applyTextureLocation, true);           
//}	
	//for (int x = 0; x < 5; x++)
	//{
	//	petals[x].Render(x,co, mat, index);                                 
		//glUniform1i(applyTextureLocation, false);          
	LinearMapR4 matDemo = viewMatrix;
	matDemo.Mult_glScale(0.1);
	matDemo.Mult_glTranslate(co.x, co.y, co.z);

	if (co.axis == 1)
	{
		matDemo.Mult_glRotate(PI2*co.rotation, 1, 0, 0);

	}
	else if (co.axis == 2)
	{
		matDemo.Mult_glRotate(PI2*co.rotation, 0, 1, 0);
	}
	else
	{
		matDemo.Mult_glRotate(PI2*co.rotation, 0, 0, 1);
	}
	
	/*glBindVertexArray(myVAO);*/	
	matDemo.DumpByColumns(mat); 
	//glVertexAttrib4fv(8, matEntries);
	//glVertexAttrib4fv(9, matEntries + 4);
	//glVertexAttrib4fv(10, matEntries + 8);
	//glVertexAttrib4fv(11, matEntries + 12);
	// glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	
	//glUniform1i(applyTextureLocation, true);
	/*glDrawElements(GL_TRIANGLE_FAN, 8, GL_UNSIGNED_INT, (void*)0)*/;
	//glUniform1i(applyTextureLocation, false);
	
	check_for_opengl_errors();
}

void Flower::InitializeAttribLocations()
{
	Petal::InitializeAttribLocations();
	if (myVAO == 0) {
		glGenVertexArrays(1, &myVAO);
		glGenBuffers(1, &myVBO);
		glGenBuffers(1, &myEBO);
		glGenBuffers(1, &myMatVBO);
	}
	float vert[] = {
		0.0f, 0.0f, 0.0f,									0.0f, 1.0f, 0.0f,0.5f,0.5f,
		0.866f, 0.0f, -0.5f,                                 0.0f, 1.0f, 0.0f,0.9f,0.25f,
		0.0f, 0.0f, -1.0f,                                 0.0f, 1.0f, 0.0f,0.067f,0.0f,
		-0.866f, 0.0f,-0.5f,                                 0.0f, 1.0f, 0.0f,0.067f,0.25f,
		-0.866f, 0.0f, 0.5f,                                 0.0f, 1.0f, 0.0f,0.5f,0.5f,
		0.0f,0.0f, 1.0f,                                  0.0f, 1.0f, 0.0f,0.5f,1.0f,
		0.866f, 0.0f, 0.5f,                                  0.0f, 1.0f, 0.0f,0.9f,0.75f,

	};
	unsigned int elt[] = {
		0, 1,2,3,4,5,6,1
	};
	glBindBuffer(GL_ARRAY_BUFFER, myVBO);
	glBindVertexArray(myVAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
	glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
	glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
	glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	// Vertex normals in the VBO
	glEnableVertexAttribArray(vertNormal_loc);
	glVertexAttribPointer(vertTexCoords_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
	glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elt) * sizeof(unsigned int), elt, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, myMatVBO);
	glBufferData(GL_ARRAY_BUFFER, numFlower * 16 * sizeof(float), (void*)0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(4 * sizeof(float)));	   // Vertex positions in the VBO
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));	   // Vertex positions in the VBO
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));	   // Vertex positions in the VBO
	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);
	glVertexAttribDivisor(10, 1);
	glVertexAttribDivisor(11, 1);
	glEnableVertexAttribArray(8);									// Enable the stored vertices
	glEnableVertexAttribArray(9);									// Enable the stored vertices
	glEnableVertexAttribArray(10);									// Enable the stored vertices
	glEnableVertexAttribArray(11);
	check_for_opengl_errors();
}
