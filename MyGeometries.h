#pragma once

// 
// MySurfaces.h   ---  Header file for MySurfaces.cpp.
// 
//   Sets up and renders 
//     - the ground plane, and
//     - the surface of rotation
//   for the Math 155A project #4.
//
//

//
// Function Prototypes
//
extern const int numFlower;
extern float PetalMatrice[];
void MySetupSurfaces();                // Called once, before rendering begins.
void SetupForTextures();               // Loads textures, sets Phong material
void MyRemeshGeometries();             // Called when mesh changes, must update resolutions.
void MyRenderGeometries();            // Called to render the two surfaces



