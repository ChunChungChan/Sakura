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
#include "MyGeometries.h"
#include "TestCollision.h"

 unsigned int Petal::myVAO = 0;
 unsigned int Petal::myVBO = 0;
 unsigned int Petal::myEBO = 0;
 unsigned int Petal::myMatVBO = 0;


	void Petal::InitializeAttribLocations() {
		if (myVAO == 0) {
			glGenVertexArrays(1, &myVAO);
			glGenBuffers(1, &myVBO);
			glGenBuffers(1, &myMatVBO);
			glGenBuffers(1, &myEBO);
		}
		float vert[] = {
			0.0f, 0.0f, 0.0f,									0.0f, 1.0f, 0.0f, 0.5f,0.5f,
			
			
			0.866f, 0.0f, -0.2f,                                0.0f, 1.0f,0.0f, 0.5f,0.5f,
			0.0f, 0.0f, -1.0f,                                 0.0f, 1.0f, 0.0f, 0.5f,0.5f,
			-1.0f, 0.0f, 0.0f,                                 0.0f, 1.0f, 0.0f,0.5f,0.5f,
			0.0f, 0.0f, 1.0f,                                  0.0f, 1.0f, 0.0f, 0.5f,0.5f,
			0.866f, 0.0f, 0.2f,                                  0.0f, 1.0f, 0.0f, 0.5f,0.5f,
		};
		unsigned int elt[] = {
			0, 1,2,3,4,5
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
		glBufferData(GL_ARRAY_BUFFER, numFlower*5*16*sizeof(float), (void*)0, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
		glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(4*sizeof(float)));	   // Vertex positions in the VBO
		glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));	   // Vertex positions in the VBO
		glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));	   // Vertex positions in the VBO
		glVertexAttribDivisor(8, 1);
		glVertexAttribDivisor(9, 1);
		glVertexAttribDivisor(10, 1);
		glVertexAttribDivisor(11, 1);
		glEnableVertexAttribArray(8);									// Enable the stored vertices
		glEnableVertexAttribArray(9);									// Enable the stored vertices
		glEnableVertexAttribArray(10);									// Enable the stored vertices
		glEnableVertexAttribArray(11);									// Enable the stored vertices


		check_for_opengl_errors();

	}
	void Petal::Render(int x,Coordinate co, float* mat, int index, LinearMapR4 map)
	{
		LinearMapR4 matDemo = viewMatrix;
		VectorR3 currentPos = { 0,0,0 };
		if (isFalling) {

			matDemo.Mult_glTranslate(xBeforeVelocity*current1, yVelocity*current1, zBeforeVelocity*current1);
			
			if (treeCollided)
			{
				matDemo.Mult_glTranslate(xAfterVelocity*current2, yVelocity*current2, zAfterVelocity*current2);
			}
		}
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
		matDemo.Mult_glRotate(1.25664*x, 0.0, 1.0, 0.0);
		matDemo.Mult_glTranslate(1.5, 0.0, 0.0);
		/*glBindVertexArray(myVAO)*/;
		if (isFalling) {
				
			matDemo.Mult_glRotate(PI*current1, 1, 0, 0);
			if (!treeCollided && !groundCollided)
			{
				current1 += 0.01;
			}
			else if (treeCollided && !groundCollided)
			{
				matDemo.Mult_glRotate(PI*current2, 1, 0, 0);
				current2 += 0.01;
			}

		}		

		matDemo.DumpByColumns(mat);
		matDemo.AffineTransformPosition(currentPos);
		if (isFalling && !groundCollided)
		{
			if (!treeCollided && !groundCollided)
			{
				treeCollided = isCollided(currentPos, 1, &xAfterVelocity, &zAfterVelocity, xBeforeVelocity, zBeforeVelocity, map);
			}
			if (!groundCollided)
			{
				groundCollided = isCollided(currentPos, 2, &xAfterVelocity, &zAfterVelocity, xBeforeVelocity, zBeforeVelocity, map);
			}
		}
		//glVertexAttrib4fv(8, mat + 16 * index * 5 + x * 16);
		//glVertexAttrib4fv(9, mat + 16 * index * 5 + x * 16 + 4);
		//glVertexAttrib4fv(10, mat + 16 * index * 5 + x * 16 + 8);
		//glVertexAttrib4fv(11, mat + 16 * index * 5 + x * 16 + 12);
		//// glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
		//glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, (void*)0);
		
		check_for_opengl_errors();
	}
	void Petal::fall() {
		if (!isFalling) {
			yVelocity = -1;
			xBeforeVelocity = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
			zBeforeVelocity = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
			if (abs(xBeforeVelocity) > abs(zBeforeVelocity))
			{
				
				zBeforeVelocity /= abs(xBeforeVelocity);
				xBeforeVelocity /= abs(xBeforeVelocity);
			}
			else
			{
				xBeforeVelocity /= abs(zBeforeVelocity);
				zBeforeVelocity /= abs(zBeforeVelocity);
			}
			
		}
		isFalling = true;
	}


