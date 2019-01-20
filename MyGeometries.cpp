//
//  MyGeometries.cpp
//
//   Sets up and renders 
//     - the ground plane, and
//     - the surface of rotation
//   for the Math 155A project #4.
//


// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR3.h"		// Adjust path as needed.
#include "LinearR4.h"		// Adjust path as needed.
#include "MathMisc.h"       // Adjust path as needed

#include "ShaderBuild.h"

#include "MyGeometries.h"
#include "CubeMapSample.h"
#include "PhongData.h"
#include "RgbImage.h"
#include "GlGeomCylinder.h"
#include "GlGeomSphere.h"
#include "Flower.h"
#include "coordinate.h"
#include "Petal.h"
#include "ctime"
#include "MyBatchSurface.h"
#include "Rain.h"
// **********************************
// Material to underlie a texture map.
// YOU MAY DEFINE A SECOND ONE OF THESE IF YOU WISH
// **********************************
phMaterial materialUnderTexture;
phMaterial materialUnderTextureDark;
phMaterial materialUnderTextureVeryDark;
phMaterial materialUnderTextureTransparent;

// **************************
// Information for loading textures
// **************************
const int NumTextures = 7;
unsigned int TextureNames[NumTextures];     // Texture names generated by OpenGL
const char* TextureFiles[NumTextures] = {
    "Fuji.bmp",
    "earth_clouds.bmp",
    "RoughWood.bmp",
    "Stem.bmp",
	"grass.bmp",
	"631pxGreenStar_1.bmp",
	"Flowers.bmp"
};
const int NumCubeMaps = 1;
unsigned int CubeMapNames[NumCubeMaps];     // Texture names generated by OpenGL
const char* CubeMapFiles[NumCubeMaps * 6] = {
	//  Texture from: Emil Persson, aka Humus. http://www.humus.name
	"Fjaderholmarna_posx.bmp",
	"Fjaderholmarna_negx.bmp",
	"Fjaderholmarna_posy.bmp",
	"Fjaderholmarna_negy.bmp",
	
	"Fjaderholmarna_posz.bmp",
	"Fjaderholmarna_negz.bmp",
};

// *******************************
// Two spheres and a cylinder
// *******************************
GlGeomSphere texSphere(4, 4);
GlGeomCylinder texCylinder(4, 4, 4);
GlGeomCylinder tree(10, 10, 10);
const int numFlower = 20000;
Flower Flowers[numFlower];
Coordinate co[numFlower];
MyBatchSurface vase(10);
const int treeCount = 7;
std::vector<MyBatchSurface> BiezierTree(treeCount, 10);
rain raining(1000);

//float PetalMatrice[numFlower * 5 * 16];

//double x;
//double y;
//double z;
//double rotation;
//int axis;

// ************************
// General data helping with setting up VAO (Vertex Array Objects)
//    and Vertex Buffer Objects.
// ***********************
const int NumObjects = 10;
const int iFloor = 0;
const int iCircularSurf = 1;
const int iWall = 2;            // RESERVED FOR USE BY 155A PROJECT

unsigned int myVBO[NumObjects];  // a Vertex Buffer Object holds an array of data
unsigned int myVAO[NumObjects];  // a Vertex Array Object - holds info about an array of vertex data;
unsigned int myEBO[NumObjects];  // a Element Array Buffer Object - holds an array of elements (vertex indices)

// ********************************************
// This sets up for texture maps. It is called only once
// ********************************************
void SetupForTextures()
{
	check_for_opengl_errors();
	// This material goes under the textures.
	// IY YOU WISH, YOU MAY DEFINE MORE THAN ONE OF THESE TO DIFFERENT GEOMETRIES
	materialUnderTexture.SpecularColor.Set(0.9, 0.9, 0.9,0.0f);
	materialUnderTexture.AmbientColor.Set(0.3, 0.3, 0.3,1.0f);
	materialUnderTexture.DiffuseColor.Set(0.7, 0.7, 0.7,1.0f);       // Increase or decrease to adjust brightness
	materialUnderTexture.SpecularExponent = 20.0;

	materialUnderTextureDark.SpecularColor.Set(0.9, 0.9, 0.9, 0.0f);
	materialUnderTextureDark.AmbientColor.Set(0.1, 0.1, 0.1, 1.0f);
	materialUnderTextureDark.DiffuseColor.Set(0.4, 0.4, 0.4,1.0f);       // Increase or decrease to adjust brightness
	materialUnderTextureDark.SpecularExponent = 70.0;

	materialUnderTextureVeryDark.SpecularColor.Set(0.6, 0.6, 0.6,0.0f);
	materialUnderTextureVeryDark.AmbientColor.Set(0.0, 0.0, 0.0,1.0f);
	materialUnderTextureVeryDark.DiffuseColor.Set(0.05, 0.05, 0.05,1.0f);       // Increase or decrease to adjust brightness
	materialUnderTextureVeryDark.SpecularExponent = 160.0;

	materialUnderTextureTransparent.SpecularColor.Set(0.9, 0.9, 0.9, 0.0f);
	materialUnderTextureTransparent.AmbientColor.Set(0.3, 0.3, 0.3, 0.1);
	materialUnderTextureTransparent.DiffuseColor.Set(0.7, 0.7, 0.7, 0.1);
	materialUnderTextureTransparent.SpecularExponent = 70.0;

	// Load texture maps
	RgbImage texMap;

	check_for_opengl_errors();
	glActiveTexture(GL_TEXTURE0);
	check_for_opengl_errors();
	glGenTextures(NumTextures, TextureNames);
	for (int i = 0; i < NumTextures; i++) {
		texMap.LoadBmpFile(TextureFiles[i]);      // Read i-th texture from the i-th file
		glBindTexture(GL_TEXTURE_2D, TextureNames[i]);  // Bind (select) the i-th OpenGL texture

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  // Requires that mipmaps be generated (see below)

																						 // Store the texture into the OpenGL texture named TextureNames[i]
		int textureWidth = texMap.GetNumCols();
		int textureHeight = texMap.GetNumRows();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texMap.ImageData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	check_for_opengl_errors();
	// Load cube maps
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(NumCubeMaps, CubeMapNames);
	for (int i = 0; i < NumCubeMaps; i++) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapNames[i]);  // Bind (select) the OpenGL cube map texture
		check_for_opengl_errors();
		for (int j = 0; j<6; j++) {
			texMap.LoadBmpFile(CubeMapFiles[6 * i + j], true);     // Read j-th face from the j-th file - flip upside down!
			int textureWidth = texMap.GetNumCols();
			int textureHeight = texMap.GetNumRows();
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + j,
				0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texMap.ImageData());
			check_for_opengl_errors();
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  // Requires that mipmaps be generated (see below)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		check_for_opengl_errors();
	}

	// Make sure that the shaders use the GL_TEXTURE_0 and GL_TEXTURE_1 textures.
	// This should really go in EduPhong.cpp, not here.
	glUseProgram(phShaderPhongPhong);
	glUniform1i(glGetUniformLocation(phShaderPhongPhong, "theTextureMap"), 0);
	glUniform1i(glGetUniformLocation(phShaderPhongPhong, "theCubeMap"), 1);
	glUseProgram(phShaderPhongGouraud);
	glUniform1i(glGetUniformLocation(phShaderPhongGouraud, "theTextureMap"), 0);
	glUniform1i(glGetUniformLocation(phShaderPhongGouraud, "theCubeMap"), 1);

}

// **********************
// This sets up geometries needed for 
//   (a) the floor (ground plane)
//   (b) the back wall
//   (c) the circular mesh
//   (d) two spheres
//   (e) one cylinder
//  It is called only once.
//  YOU NEED TO CHANGE THIS ONCE YOU ADD THE TEXTURE COORDINATES TO THE CIRCULAR SURFACE.
// **********************
void MySetupSurfaces() {

   /* texSphere.InitializeAttribLocations(vertPos_loc, vertNormal_loc, vertTexCoords_loc);*/
    //texCylinder.InitializeAttribLocations(vertPos_loc, vertNormal_loc, vertTexCoords_loc);
	Flower::InitializeAttribLocations();
	tree.InitializeAttribLocations(vertPos_loc, vertNormal_loc, vertTexCoords_loc);
	vase.InitializeAttribLocations();
	raining.InitializeAttribLocations();
	vase.ChangeRadius(0, 3);
	vase.ChangeRadius(1, 6);
	vase.ChangeRadius(2, 3);
	vase.ChangeRadius(3, 2);
	for (int x = 0; x < treeCount; x++)
	{
		BiezierTree[x].InitializeAttribLocations();
	}
	BiezierTree[0].ChangeRadius(0, 0.7);
	BiezierTree[0].ChangeRadius(1, 1);
	BiezierTree[0].ChangeRadius(2, 2);
	BiezierTree[0].ChangeRadius(3, 3);
	BiezierTree[0].Bend(3, -5);
	BiezierTree[1].ChangeRadius(0, 0.5);
	BiezierTree[1].ChangeRadius(1, 1);
	BiezierTree[1].ChangeRadius(2, 1);
	BiezierTree[1].ChangeRadius(3, 2);
	BiezierTree[1].Bend(2, -10);
	BiezierTree[2].ChangeRadius(0, 0.5);
	BiezierTree[2].ChangeRadius(1, 1);
	BiezierTree[2].ChangeRadius(2, 1);
	BiezierTree[2].ChangeRadius(3, 2);
	BiezierTree[2].Bend(2, 3);
	BiezierTree[3].ChangeRadius(0, 0.5);
	BiezierTree[3].ChangeRadius(1, 1);
	BiezierTree[3].ChangeRadius(2, 1);
	BiezierTree[3].ChangeRadius(3, 2);
	BiezierTree[3].Bend(2, 6);
	BiezierTree[4].ChangeRadius(0, 0.5);
	BiezierTree[4].ChangeRadius(1, 1);
	BiezierTree[4].ChangeRadius(2, 1);
	BiezierTree[4].ChangeRadius(3, 2);
	BiezierTree[4].Bend(1, -3);
	BiezierTree[5].ChangeRadius(0, 0.5);
	BiezierTree[5].ChangeRadius(1, 1);
	BiezierTree[5].ChangeRadius(2, 1);
	BiezierTree[5].ChangeRadius(3, 2);
	BiezierTree[5].Bend(2, -7);
	BiezierTree[6].ChangeRadius(0, 0.5);
	BiezierTree[6].ChangeRadius(1, 0.5);
	BiezierTree[6].ChangeRadius(2, 0.5);
	BiezierTree[6].ChangeRadius(3, 1.5);
	BiezierTree[5].Bend(1, 2);

    // Initialize the VAO's, VBO's and EBO's for the ground plane, the back wall
    // and the surface of rotation. Gives them the "vertPos" location,
    // and the "vertNormal"  and the "vertTexCoords" locations in the shader program.
    // No data is loaded into the VBO's or EBO's for the circular surface until the "Remesh"
    //   routines is called

    glGenVertexArrays(NumObjects, &myVAO[0]);
    glGenBuffers(NumObjects, &myVBO[0]);
    glGenBuffers(NumObjects, &myEBO[0]);
	/*Petal petal1;
	petal1.Render(TextureNames[1], materialUnderTexture);*/
    // For the Back Wall:
    // Allocate the needed Vertex Array Objects (VAO's),
    //      Vertex Buffer Objects (VBO's) and Element Array Buffer Objects (EBO's)
    // Since the floor has only four vertices.  Each vertex stores its
    //    position, its normal (0,1,0) and its (s,t)-coordinates.
    // YOU DO NOT NEED TO REMESH THE BACK WALL
	//float wallVerts[] = {
	//	// Position              // Normal                  // Texture coordinates
	//	-15.0f, 15.0f, -15.0f,      0.0f, 0.0f, 1.0f,          0.0f, 1.0f,         // Top left
	//	15.0f, 15.0f, -15.0f,      0.0f, 0.0f, 1.0f,          1.0f, 1.0f,         // Top right
	//	15.0f, -2.0f, -15.0f,      0.0f, 0.0f, 1.0f,          1.0f, 0.0f,         // Bottom right
	//	-15.0f, -2.0f, -15.0f,      0.0f, 0.0f, 1.0f,          0.0f, 0.0f,         // Bottom left
	//};
	//unsigned int wallElts[] = { 0, 3, 1, 2 };
	//glBindBuffer(GL_ARRAY_BUFFER, myVBO[iWall]);
	//glBindVertexArray(myVAO[iWall]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(wallVerts), wallVerts, GL_STATIC_DRAW);
	//glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
	//glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
	//glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	// Vertex normals in the VBO
	//glEnableVertexAttribArray(vertNormal_loc);									// Enable the stored vertices
	//glVertexAttribPointer(vertTexCoords_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
	//glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iWall]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallElts) * sizeof(unsigned int), wallElts, GL_STATIC_DRAW);

    // For the circular surface:
    // Allocate the needed VAO, VBO< EBO
    // The normal vectors is specified separately for each vertex. (It is not a generic attribute.)
    // YOU MUST MODIFY THIS TO (A) USE STRIDES OF 8 *sizeof(float), (B) COMMENT IN THE LINES FOR vertTexCoords 
 //   glBindVertexArray(myVAO[iCircularSurf]);
 //   glBindBuffer(GL_ARRAY_BUFFER, myVBO[iCircularSurf]);
 //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iCircularSurf]);
 //   glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	// Store vertices in the VBO
 //   glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
 //   glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Store normals in the VBO
 //   glEnableVertexAttribArray(vertNormal_loc);                                 // Enabled the stored normals (so they are not generic)
 //   // ONCE YOU HAVE TEXTURE COORDINATES WORKING FOR THE CIRCULAR SURFACE,
 //     //PUT THE NEXT TWO LINES BACK IN, AND CHANGE THE STRIDE TO "8" IN THE PREVIOUS CALLS TO glVertexAttribPointer.
 //   glVertexAttribPointer(vertTexCoords_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
 //   glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices

 //   // No data has been loaded into the VBO's for the circular surface yet.
 //   // This is done by the "SamsRemeshCircularSurf" routine.

 //   SamsRemeshCircularSurf();

 //   // FOR THE FLOOR: ADD YOUR OWN CODE!! 
 //   // IT WILL BE SIMILAR TO THE BACK WALL ABOVE.
 //   // YOU DO NOT NEED TO REMESH THE FLOOR - ONE RECTANGLE IS ENOUGH
	float floorVerts[] = {
		// Position              // Normal                  // Texture coordinates
		-40.0, -2.0,  40.0,        -1,-1,1,                     0,0,
		40.0, -2.0,  40.0,         1,-1,1,                    0,0,
		-40.0,  28.0,  40.0,         -1,1,1,                   0,0,
		40.0,  28.0,  40.0,          1,1,1,                  0,0,
		-40.0, -2.0, -40.0,           -1,-1,-1,                   0,0,
		40.0, -2.0, -40.0,            1,-1,-1,                  0,0,
		-40.0,  28.0, -40.0,            -1,1,-1,                 0,0,
		40.0,  28.0, -40.0,             1,1,-1,                0,0


	};
	unsigned int floorElts[] = { 0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1 };
	glBindBuffer(GL_ARRAY_BUFFER, myVBO[iFloor]);
	glBindVertexArray(myVAO[iFloor]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVerts), floorVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
	glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
	glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	// Vertex normals in the VBO
	glEnableVertexAttribArray(vertNormal_loc);									// Enable the stored vertices
	glVertexAttribPointer(vertTexCoords_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
	glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iFloor]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorElts) * sizeof(unsigned int), floorElts, GL_STATIC_DRAW);
    check_for_opengl_errors();      // Watch the console window for error messages!
}

void MyRemeshGeometries() 
{
// IT IS NOT NECESSARY TO REMESH EITHER THE FLOOR OR THE BACK WALL
// YOU DO NOT NEED TO CHANGE THIS FOR PROJECT #6.

   /* SamsRemeshCircularSurf();*/
    texSphere.Remesh(meshRes, meshRes);
    texCylinder.Remesh(meshRes, meshRes, meshRes);
	tree.Remesh(meshRes, meshRes, meshRes);
	vase.Remesh(meshRes);
	for (int x = 0; x < treeCount; x++)
	{
		BiezierTree[x].Remesh(meshRes);
	}

    check_for_opengl_errors();      // Watch the console window for error messages!
}

// **********************************************
// MODIFY THIS ROUTINE TO CALL YOUR OWN CODE IN
//    TO RENDER THE FLOOR AND THE SPHERES AND 
//    THE CYLINDER.
// **********************************************
extern bool cullBackFaces;

void MyRenderGeometries() {
	// ******
	// Render the Back Wall
	// ******
	glDepthRange(1, 1);
	glBindVertexArray(myVAO[iFloor]);
	materialUnderTexture.LoadIntoShaders();
	LinearMapR4 mat = viewMatrix;
	mat.m41 = 0;
	mat.m42 = 0;
	mat.m43 = 0;
	mat.DumpByColumns(matEntries);
	glVertexAttrib4fv(8, matEntries);
	glVertexAttrib4fv(9, matEntries + 4);
	glVertexAttrib4fv(10, matEntries + 8);
	glVertexAttrib4fv(11, matEntries + 12);
	// glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	glActiveTexture(GL_TEXTURE1);                   // Work with the cube map texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapNames[0]);     // Choose the first cube map texture
	//glFrontFace(GL_CW);
	glDisable(GL_CULL_FACE);
	glUniform1i(applyCubeMapLocation, 1);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, (void*)0);           // Enable applying the cube map as a box texture
	if (cullBackFaces) {
		glEnable(GL_CULL_FACE);
	}
	//glFrontFace(GL_CCW);
	glUniform1i(applyCubeMapLocation, 0);           // Turn off applying cube map  texture!
	glDepthRange(0, 1);

	currentTime += animateIncrement;

	materialUnderTexture.LoadIntoShaders();        // Use the bright underlying color
	//glBindVertexArray(myVAO[iWall]);                // Select the back wall VAO (Vertex Array Object)
	//materialUnderTexture.LoadIntoShaders();         // Use the bright underlying color
	//viewMatrix.DumpByColumns(matEntries);           // Apply the model view matrix
	//glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	//glBindTexture(GL_TEXTURE_2D, TextureNames[0]);     // Choose Brick wall texture
	//glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
	//												   // Draw the wall as a single triangle strip
	//glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
	//glUniform1i(applyTextureLocation, false);



	//{
	//	 x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 5));

	//	 y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 5));

	//	 z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 5));
	//	rotation = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	//	axis = (rand() % 3) + 1;
	// }
	//truck
	LinearMapR4 matDemo = viewMatrix;
	matDemo.Mult_glTranslate(0, -2, 0);
	matDemo.Mult_glScale(0.5, 5, 0.5);
	matDemo.DumpByColumns(matEntries);
	glVertexAttrib4fv(8, matEntries);
	glVertexAttrib4fv(9, matEntries + 4);
	glVertexAttrib4fv(10, matEntries + 8);
	glVertexAttrib4fv(11, matEntries + 12);
	// glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureNames[2]);
	glUniform1i(applyTextureLocation, true);
	BiezierTree[0].Render();
	//first branch
	matDemo = viewMatrix;
	matDemo.Mult_glTranslate(1.25, 3, -0.25);
	matDemo.Mult_glRotate(-PI / 3, 1, 0, 1);
	matDemo.Mult_glScale(0.2, 3, 0.2);
	matDemo.DumpByColumns(matEntries);
	glVertexAttrib4fv(8, matEntries);
	glVertexAttrib4fv(9, matEntries + 4);
	glVertexAttrib4fv(10, matEntries + 8);
	glVertexAttrib4fv(11, matEntries + 12);
	// glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	BiezierTree[1].Render();
	//sub branch
	matDemo = viewMatrix;

	matDemo.Mult_glTranslate(2.8, 3.75, -1.5);
	matDemo.Mult_glRotate(-PI / 3, 1, 0, 1);
	matDemo.Mult_glRotate(-PI / 3, 1, 0, 1);
	matDemo.Mult_glScale(0.1, 1.5, 0.1);
	matDemo.DumpByColumns(matEntries);
	glVertexAttrib4fv(8, matEntries);
	glVertexAttrib4fv(9, matEntries + 4);
	glVertexAttrib4fv(10, matEntries + 8);
	glVertexAttrib4fv(11, matEntries + 12);
	// glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	BiezierTree[2].Render();
	//second branch
	matDemo = viewMatrix;
	matDemo.Mult_glTranslate(0.6, 3, 0);
	matDemo.Mult_glRotate(2 * PI / 3, 0, 1, 0);
	matDemo.Mult_glRotate(-PI / 3, 1, 0, 1);
	matDemo.Mult_glScale(0.2, 3.25, 0.2);
	matDemo.DumpByColumns(matEntries);
	glVertexAttrib4fv(8, matEntries);
	glVertexAttrib4fv(9, matEntries + 4);
	glVertexAttrib4fv(10, matEntries + 8);
	glVertexAttrib4fv(11, matEntries + 12);
	// glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	BiezierTree[3].Render();
	//third branch
	matDemo = viewMatrix;
	matDemo.Mult_glTranslate(1.4, 4, 0.4);
	matDemo.Mult_glRotate(4 * PI / 3, 0, 1, 0);
	matDemo.Mult_glRotate(-PI / 3, 1, 0, 1);
	matDemo.Mult_glScale(0.2, 2, 0.2);
	matDemo.DumpByColumns(matEntries);
	glVertexAttrib4fv(8, matEntries);
	glVertexAttrib4fv(9, matEntries + 4);
	glVertexAttrib4fv(10, matEntries + 8);
	glVertexAttrib4fv(11, matEntries + 12);
	// glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	BiezierTree[4].Render();
	//fourth branch
	matDemo = viewMatrix;
	matDemo.Mult_glTranslate(1.5, 5.5, 0.0);
	matDemo.Mult_glRotate(PI / 2, 0, 1, 0);
	matDemo.Mult_glRotate(-PI / 6, 1, 0, 1);
	matDemo.Mult_glScale(0.2, 3, 0.2);
	matDemo.DumpByColumns(matEntries);
	glVertexAttrib4fv(8, matEntries);
	glVertexAttrib4fv(9, matEntries + 4);
	glVertexAttrib4fv(10, matEntries + 8);
	glVertexAttrib4fv(11, matEntries + 12);
	// glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	BiezierTree[5].Render();
	//second sub
	matDemo = viewMatrix;
	matDemo.Mult_glTranslate(1, 6.7, -0.65);
	matDemo.Mult_glRotate(-PI / 6, 1, 0, 1);
	matDemo.Mult_glScale(0.2, 1, 0.2);
	matDemo.DumpByColumns(matEntries);
	glVertexAttrib4fv(8, matEntries);
	glVertexAttrib4fv(9, matEntries + 4);
	glVertexAttrib4fv(10, matEntries + 8);
	glVertexAttrib4fv(11, matEntries + 12);
	// glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	BiezierTree[6].Render();
	glUniform1i(applyTextureLocation, false);


	glUniform1i(applyTextureLocation, true);
	glBindBuffer(GL_ARRAY_BUFFER, Flower::myMatVBO);
	float* FlowerMatrice = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindTexture(GL_TEXTURE_2D, TextureNames[3]);
	for (int x = 0; x < numFlower; x++) {
		Flowers[x].Render(co[x],FlowerMatrice+16*x);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindVertexArray(Flower::myVAO);
	glDrawElementsInstanced(GL_TRIANGLE_FAN, 8, GL_UNSIGNED_INT, (void*)0, numFlower);
	glBindBuffer(GL_ARRAY_BUFFER, Petal::myMatVBO);
	float* PetalMatrice = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindTexture(GL_TEXTURE_2D, TextureNames[6]);
	for (int x = 0; x < numFlower; x++) 
	{
		for (int j = 0; j < 5; j++)
		{
			Flowers[x].petals[j].Render(j, co[x], PetalMatrice + 16 * x * 5 + j * 16, x, mat.Inverse());
		}
	}
    glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindVertexArray(Petal::myVAO);
	glDrawElementsInstanced(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, (void*)0, numFlower * 5);
	glUniform1i(applyTextureLocation, false);
	if (isRaining)
	{

		materialUnderTextureTransparent.LoadIntoShaders();
		raining.render();
	}

	//matDemo = viewMatrix;
	//matDemo.Mult_glTranslate(-7, -2, 3);
	//matDemo.Mult_glScale(1,2,1);
	//matDemo.DumpByColumns(matEntries);
	//glVertexAttrib4fv(8, matEntries);
	//glVertexAttrib4fv(9, matEntries + 4);
	//glVertexAttrib4fv(10, matEntries + 8);
	//glVertexAttrib4fv(11, matEntries + 12);

	//glBindTexture(GL_TEXTURE_2D, TextureNames[1]);
	//vase.Render();
	//glUniform1i(applyTextureLocation, false);
	int chance = rand() % 100 + 1;
	if (chance <= 10)
	{
		int drop = rand() % numFlower;

		Flowers[drop].dropPetal();
	}

	// Render the sphere
    // Turn off applying texture!
 //   // **************
 //   // Render the circular surface
 //   SamsRenderCircularSurf();

 //   // ************ 
 //   // Render the floor
 //   //  YOU MUST WRITE THIS. IT WILL BE SIMILAR TO THE BACK WALL ABOVE

 //   // *************
 //   // Render two spheres and a cylinder
 //   // YOU MUST MODIFY THE CODE BELOW, AND ADD TEXTURES TO THE SPHERE AND THE CAPS OF THE CYLINDER.

 //   // Render a sphere with a earth image texture map
 //   //  YOU DO NOT NEED TO MODIFY THIS SPHERE OR ITS TEXTURE MAP
 //   LinearMapR4 matDemo = viewMatrix;
 //   matDemo.Mult_glTranslate(-3.5, 1.1, 0.4);
 //   matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
 //   glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
 //   glBindTexture(GL_TEXTURE_2D, TextureNames[3]);     // Choose Earth image texture
 //   glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
 //   texSphere.Render();                                 // Render the sphere
 //   glUniform1i(applyTextureLocation, false);           // Turn off applying texture!

 //   // Render a sphere with a earth image texture map
 //   // ADD A TEXTURE MAP TO THIS SPHERE - SIMILAR TO THE CODE ABOVE
 //   matDemo = viewMatrix;
 //   matDemo.Mult_glTranslate( 3.5, 1.1, 0.4);
 //   matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
 //   glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	//glBindTexture(GL_TEXTURE_2D, TextureNames[6]);
	//glUniform1i(applyTextureLocation, true);
 //   texSphere.Render();                                 // Render the sphere
	//glUniform1i(applyTextureLocation, false);


 //   // Render a cylinder 
 //   // YOU MAY LEAVE THE TEXTURE MAP ON THE SIDE OF THE CYLINDER
 //   // YOU MUST ADD TEXTURE MAPS TO THE TOP AND BOTTOM FACES OF THE CYLINDER
 //   matDemo = viewMatrix;
 //   matDemo.Mult_glTranslate(0.0, 1.1, 1.0);
	//matDemo.Mult_glRotate(PI / 2, 0, 0, 1.0);
 //   matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
 //   glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
 //   glBindTexture(GL_TEXTURE_2D, TextureNames[2]);     // Choose Earth image texture
 //   glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
 //   texCylinder.RenderSide();                             // Render the sphere side
	//glBindTexture(GL_TEXTURE_2D, TextureNames[5]);
 //   glUniform1i(applyTextureLocation, true);           // Turn off applying texture!
 //   texCylinder.RenderTop();                              // RENDER THIS WITH A TEXTURE MAP
	//glUniform1i(applyTextureLocation, false);
	//glBindTexture(GL_TEXTURE_2D, TextureNames[5]);
	//glUniform1i(applyTextureLocation, true);
 //   texCylinder.RenderBase();                             // RENDER THIS WITH A TEXTURE MAP
	//glUniform1i(applyTextureLocation, false);
        
    check_for_opengl_errors();      // Watch the console window for error messages!
}
//
//void SamsRenderCircularSurf()
//{
//    //glBindVertexArray(myVAO[iCircularSurf]);
//
//    //// Calculate the model view matrix for the circular surface
//    //LinearMapR4 matDemo = viewMatrix;
//    //matDemo.Mult_glTranslate(0.0, 0.75, -2.5);     // Center in the back & raise up
//    //matDemo.Mult_glScale(0.15, 2.0, 0.15);         // Adjust the circular diameter and height
//
//    //                                             // Set the uniform values (they are not stored with the VAO and thus must be set again everytime
//    //materialUnderTexture.LoadIntoShaders();                       // Use the bright underlying material
//    //matDemo.DumpByColumns(matEntries);
//    //glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries); // Select the modelview matrix to place it
//    //glBindTexture(GL_TEXTURE_2D, TextureNames[1]);     // Choose Mandala texture
//    //// ONCE YOU HAVE TEXTURE COORDINATES FOR THE CIRCULAR SURFACE, PUT THE NEXT LINE BACK IN.
//    //glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
//    //                                                               // Draw the the triangle strips
//    //for (int i = 0; i < meshRes; i++) {
//    //    glDrawElements(GL_TRIANGLE_STRIP, 1 + 2 * meshRes, GL_UNSIGNED_INT,
//    //        (void*)(i * (1 + 2 * meshRes) * sizeof(unsigned int)));
//    //}
//    //glUniform1i(applyTextureLocation, false);           // Disable applying the texture!
//
//}


// *******
// ADD TEXTURE COORDINATES TO EITHER YOUR OWN "MyRemeshCircularSurf" or
// TO THE PROVIDED "SamsRemeshCircularSurf"
// IF YOU USE YOUR OWN, ALSO REPLACE "SamsRenderCircularMesh" WITH YOUR
// OWN "MyRenderCircularMesh" IF NEEDED.
// TASKS FOR ADDING TEXTURE COORDINATES:
//    a. Change stride to 8*sizeof(float) instead 6*sizeof(float).
//    b. Be sure to allocate enough memory for circularVerts[].
//    c. Calculate the (s,t) texture coordinates, put them in the array.
//    d. Update the calls to glVertexAttribPointer and glEnableVertexAttribArray
//         in the routine MySetupSurfaces above.

//void SamsRemeshCircularSurf()
//{
//
// //   // Circular surface vertices.
// //   int numCircularVerts = 1 + meshRes * meshRes;
// //   float* circularVerts = new float[8 * numCircularVerts];
// //   // Circular surface elements (indices to vertices in a triangle strip)
// //   int numCircularElts = meshRes * (1 + 2 * meshRes);
// //   unsigned int* circularElements = new unsigned int[numCircularElts];
//
// //   float* toPtr = circularVerts;
// //   *(toPtr++) = 0.0f;      // Center point
// //   *(toPtr++) = 1.0f;
// //   *(toPtr++) = 0.0f;
// //   *(toPtr++) = 0.0f;      // Center normal
// //   *(toPtr++) = 1.0f;
// //   *(toPtr++) = 0.0f;
//	//*(toPtr++) = 0.5f;
//	//*(toPtr++) = 0.5f;
// //   for (int i = 0; i < meshRes; i++) {
// //       float theta = (float)PI2 * ((float)i) / (float)meshRes;
// //       float c = cosf(theta);
// //       float s = sinf(theta);
// //       for (int j = 1; j <= meshRes; j++) {
// //           float r = 5.0f * (float)PI * ((float)j) / (float)meshRes;
// //           float rprime = (r * cosf(r) - sinf(r)) / (r*r);
// //           float norm = sqrtf(1.0f + rprime * rprime);
// //           *(toPtr++) = c * r;                 // (i,j) point
// //           *(toPtr++) = sinf(r) / r;
// //           *(toPtr++) = s * r;
// //           *(toPtr++) = -c * rprime / norm;      // (i,j) normal
// //           *(toPtr++) = 1.0f / norm;
// //           *(toPtr++) = -s * rprime / norm;
//	//		*(toPtr++) = 0.5f+0.5*c*r/(5*PI);
//	//		*(toPtr++) = 0.5f+0.5*s*r/(5*PI);
// //       }
// //   }
//
// //   unsigned int* toElt = circularElements;
// //   for (int i = 0; i < meshRes; i++) {
// //       *(toElt++) = 0;         // Central vertex
// //       for (int j = 0; j < meshRes; j++) {
// //           *(toElt++) = ((i + 1) % meshRes) * meshRes + j + 1;
// //           *(toElt++) = i * meshRes + j + 1;
// //       }
// //   }
//
// //   glBindVertexArray(myVAO[iCircularSurf]);
// //   glBindBuffer(GL_ARRAY_BUFFER, myVBO[iCircularSurf]);
// //   glBufferData(GL_ARRAY_BUFFER, 8 * numCircularVerts * sizeof(float), circularVerts, GL_STATIC_DRAW);
// //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iCircularSurf]);
// //   glBufferData(GL_ELEMENT_ARRAY_BUFFER, numCircularElts * sizeof(unsigned int), circularElements, GL_STATIC_DRAW);
//
// //   delete[] circularVerts;
// //   delete[] circularElements;
//}
//
