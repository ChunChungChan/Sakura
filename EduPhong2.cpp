
// *******************************
// EduPhong.cpp - Version 0.2 - February 17, 2018
//
// EduPhong.h and EduPhong.cpp code gives C++ classes
//     assisting in demostrating Phong lighting for 
//     educational purposes.
//
// Author: Sam Buss
//
// Software is "as-is" and carries no warranty. It may be used without
//  restriction, but if you modify it, please change the filenames to
//  prevent confusion between different versions.
// Bug reports: Sam Buss, sbuss@ucsd.edu
// *******************************

#include <stdio.h>

#include "ShaderBuild.h"
#include "EduPhong.h"

bool check_for_opengl_errors();

// *********************************
// Interface data for the shader programs.
unsigned int phShaderPhongPhong;
unsigned int phShaderPhongGouraud;
const unsigned int phVertPos_loc = 0;                  // Corresponds to "location = 0" in the vertex shader definition
const unsigned int phVertNormal_loc = 1;               // Corresponds to "location = 1" in the vertex shader definition
const unsigned int phEmissiveColor_loc = 3;            // Corresponds to "location = 3" in the vertex shader definition
const unsigned int phAmbientColor_loc = 4;             // Corresponds to "location = 4" in the vertex shader definition
const unsigned int phDiffuseColor_loc = 5;             // Corresponds to "location = 5" in the vertex shader definition
const unsigned int phSpecularColor_loc = 6;            // Corresponds to "location = 6" in the vertex shader definition
const unsigned int phSpecularExponent_loc = 7;         // Corresponds to "location = 7" in the vertex shader definition

unsigned int projMatLocationPG;				        // Location of the projectionMatrix in the Phong-Phong shader program.
unsigned int modelviewMatLocationPG;			    // Location of the modelviewMatrix in the Phong-Phong shader program.
unsigned int vMatInvLocationPG;				        // Location of the vvMatInv matrix in the Phong-Gouraud shader program.
unsigned int applyTextureLocationPG;				// Location of the applyTexture bool in the Phong-Gouraud shader program.
unsigned int applyCubeMapLocationPG;				// Location of the applyCubeMap bool in the Phong-Gouraud shader program.
unsigned int projMatLocationPP;				        // Location of the projectionMatrix in the Phong-Phong shader program.
//unsigned int modelviewMatLocationPP;			    // Location of the modelviewMatrix in the Phong-Phong shader program.
unsigned int vMatInvLocationPP;				        // Location of the vvMatInv matrix in the Phong-Phong shader program.
unsigned int applyTextureLocationPP;				// Location of the applyTexture bool in the Phong-Phong shader program.
unsigned int applyCubeMapLocationPP;				// Location of the applyCubeMap bool in the Phong-Phong shader program.

unsigned int globallightBlockIndexPG;               // Index of the global light block Phong-Gouraud
unsigned int lightsBlockIndexPG;                    // Index of the light array block Phong-Gouraud
unsigned int globallightBlockIndexPP;               // Index of the global light block Phong-Phong
unsigned int lightsBlockIndexPP;                    // Index of the light array block Phong-Phong

const char* projMatName = "projectionMatrix";		// Name of the uniform variable projectionMatrix
const char* modelviewMatName = "modelviewMatrix";	// Name of the uniform variable modelviewMatrix
const char* viewMatrixInvName = "viewMatrixInv";	    // Name of the uniform variable viewMatrixInv
const char* applyTextureName = "applyTexture";	    // Name of the uniform variable applyTexture
const char* applyCubeMapName = "applyCubeMap";	    // Name of the uniform variable applyCubeMap
const char* globallightBlockName= "phGlobal";       // Name of the global light uniform block
const char* lightsBlockName = "phLightArray";       // Name of the light array uniform block


// *********************************
// Source code for the shader programs

// The vertex shader for Phong lighting with Phong shading.
//   Mostly this copies material values, modelview position,
//   and modelview surface normal to the fragment shader.
const char vertexShader_PhongPhong[] =
"#version 330 core\n"
"layout (location = 0) in vec3 vertPos;	     // Position in attribute location 0\n"
"layout (location = 1) in vec3 vertNormal;	 // Surface normal in attribute location 1\n"
"layout (location = 2) in vec2 vertTexCoords;	 // Texture coordinates in attribute location 2\n"
"layout (location = 3) in vec4 EmissiveColor; // Surface material properties \n"
"layout (location = 4) in vec4 AmbientColor; \n"
"layout (location = 5) in vec4 DiffuseColor; \n"
"layout (location = 6) in vec4 SpecularColor; \n"
"layout (location = 7) in float SpecularExponent; \n"
"layout (location = 8) in mat4 modelviewMatrix; \n"  // Now a vertex attribute
""
"out vec3 mvPos;   // Vertex position in modelview coordinates\n"
"out vec3 origNormal; // Normal vector in local coordinates\n"
"out vec3 mvNormalFront; // Normal vector to vertex in modelview (world) coordinates\n"
"out vec4 matEmissive;\n"
"out vec4 matAmbient;\n"
"out vec4 matDiffuse;\n"
"out vec4 matSpecular;\n"
"out float matSpecExponent;\n"
"out vec2 theTexCoords;\n"
"out vec3 reflectDir; \n"
""
"uniform mat4 projectionMatrix;		// The projection matrix\n"
//"uniform mat4 modelviewMatrix;		// The modelview matrix\n"
"uniform mat4 viewMatrixInv;		// The inverse of the view matrix (use for cube map reflections)\n"
"uniform int applyCubeMap;   // 0 - not applied, 1 - as environment map, 2 - as reflection map \n"
""
"layout (std140) uniform phGlobal { \n"
"    vec4 GlobalAmbientColor;        // Global ambient light color \n"
"    int NumLights;                  // Number of lights \n"
"    bool LocalViewer;               // true for local viewer; false for directional viewer \n"
"    bool EnableEmissive;            // Control whether emissive colors are rendered \n"
"    bool EnableDiffuse;             // Control whether diffuse colors are rendered \n"
"    bool EnableAmbient;             // Control whether ambient colors are rendered \n"
"    bool EnableSpecular;            // Control whether specular colors are rendered \n"
"};\n"
""
"void main()\n"
"{\n"
"    vec4 mvPos4 = modelviewMatrix * vec4(vertPos.x, vertPos.y, vertPos.z, 1.0); \n"
"    gl_Position = projectionMatrix * mvPos4; \n"
"    mvPos = vec3(mvPos4.x,mvPos4.y,mvPos4.z)/mvPos4.w; \n"
"    origNormal = vertNormal; \n"
"    mvNormalFront = normalize(inverse(transpose(mat3(modelviewMatrix)))*vertNormal); // Unit normal from the surface \n"
"    matEmissive = EmissiveColor;\n"
"    matAmbient = AmbientColor;\n"
"    matDiffuse = DiffuseColor;\n"
"    matSpecular = SpecularColor;\n"
"    matSpecExponent = SpecularExponent;\n"
"    theTexCoords = vertTexCoords;\n"
"    if ( applyCubeMap==2 ) { \n"
"        vec3 vVectorNeg = LocalViewer ? mvPos : vec3(0.0, 0.0, -1.0); \n"
"        reflectDir = mat3(viewMatrixInv)*reflect(vVectorNeg,mvNormalFront); \n"
"    } \n"
"}\0";

// The base code for the fragment shader for Phong lighting with Phong shading.
//   This does all the hard work of the Phong lighting
const char fragmentShader_PhongPhongBase[] =
"#version 330 core\n"
""
"in vec3 mvPos;   // Vertex position in modelview coordinates\n"
"in vec3 origNormal; \n  // Normal vector in local coordinates \n"
"in vec3 mvNormalFront; // Normal vector to vertex (front facing) in modelview (world) coordinates\n"
"in vec4 matEmissive;\n"
"in vec4 matAmbient;\n"
"in vec4 matDiffuse;\n"
"in vec4 matSpecular;\n"
"in float matSpecExponent;\n"
""
"layout (std140) uniform phGlobal { \n"
"    vec4 GlobalAmbientColor;        // Global ambient light color \n"
"    int NumLights;                  // Number of lights \n"
"    bool LocalViewer;               // true for local viewer; false for directional viewer \n"
"    bool EnableEmissive;            // Control whether emissive colors are rendered \n"
"    bool EnableDiffuse;             // Control whether diffuse colors are rendered \n"
"    bool EnableAmbient;             // Control whether ambient colors are rendered \n"
"    bool EnableSpecular;            // Control whether specular colors are rendered \n"
"};\n"
""
"const int MaxLights = 8; \n"
"struct phLight { \n"
"    bool IsEnabled;             // True if light is turned on \n"
"    bool IsAttenuated;          // True if attenuation is active \n"
"    bool IsSpotLight;           // True if spotlight \n"
"    bool IsDirectional;         // True if directional \n"
"    vec3 Position; \n"
"    vec4 AmbientColor; \n"
"    vec4 DiffuseColor; \n"
"    vec4 SpecularColor; \n"
"    vec3 SpotDirection;     // Should be unit vector! \n"
"    float SpotCosCutoff;        // Cosine of cutoff angle \n"
"    float SpotExponent; \n"
"    float ConstantAttenuation; \n"
"    float LinearAttenuation; \n"
"    float QuadraticAttenuation; \n"
"};\n"
"layout (std140) uniform phLightArray { \n"
"    phLight Lights[MaxLights];\n"
"};\n"
""
"vec3 mvNormal; \n"
"in vec2 theTexCoords;	// Texture coordinates (interpolated from vertex shader) \n"
"in vec3 reflectDir;    // Reflection vector, for cube map computation if needed \n"
"uniform sampler2D theTextureMap;\n"
"uniform samplerCube theCubeMap;\n"
"uniform bool applyTexture;\n"
"uniform int applyCubeMap;   // 0 - not applied, 1 - as environment map, 2 - as reflection map \n"
""
"vec4 nonspecColor;  \n"
"vec4 specularColor;  \n"
"out vec4 fragmentColor;	// Color that will be used for the fragment\n"
"void CalculatePhongLighting();  // Calculate: nonspecColor and specularColor. \n"
""
"void main() { \n"
"    if ( gl_FrontFacing ) {\n"
"        mvNormal = mvNormalFront;\n"
"    }\n"
"    else {\n"
"        mvNormal = -mvNormalFront;\n"
"    }\n"
"    CalculatePhongLighting();  // Calculate: nonspecColor and specularColor. \n"
"    fragmentColor = vec4(nonspecColor);   // Add alpha value of 1.0.\n"
"    if ( applyTexture ) { \n"
"        fragmentColor *= texture(theTextureMap, theTexCoords);\n"
"    }\n"
"    if ( applyCubeMap==1 ) { \n"
"        fragmentColor *= texture(theCubeMap, origNormal); \n"
"    } \n"
"    else if ( applyCubeMap==2 ) { \n"
"        fragmentColor += texture(theCubeMap,reflectDir);"
"    } \n"
"    fragmentColor += vec4(specularColor);\n"
"}\0";

// The vertex shader for Phong lighting (with Gouraud shading).
//   This does all the hard work of the Phong lighting
const char vertexShader_PhongGouraudBase[] =
"#version 330 core\n"
""
"layout (location = 0) in vec3 vertPos;	     // Position in attribute location 0\n"
"layout (location = 1) in vec3 vertNormal;	 // Surface normal in attribute location 1\n"
"layout (location = 2) in vec2 vertTexCoords;	 // Texture coordinates in attribute location 2\n"
"layout (location = 3) in vec4 EmissiveColor; // Surface material properties \n"
"layout (location = 4) in vec4 AmbientColor; \n"
"layout (location = 5) in vec4 DiffuseColor; \n"
"layout (location = 6) in vec4 SpecularColor; \n"
"layout (location = 7) in float SpecularExponent; \n"
""
"out vec4 nonspecColor;  \n"
"out vec4 specularColor;  \n"
"out vec2 theTexCoords;\n"
"out vec3 origNormal; \n  // Normal vector in local coordinates \n"
"out vec3 reflectDir; \n  // Reflection vector in world coordinates \n"
""
"layout (std140) uniform phGlobal { \n"
"    vec4 GlobalAmbientColor;        // Global ambient light color \n"
"    int NumLights;                  // Number of lights \n"
"    bool LocalViewer;               // true for local viewer; false for directional viewer \n"
"    bool EnableEmissive;            // Control whether emissive colors are rendered \n"
"    bool EnableDiffuse;             // Control whether diffuse colors are rendered \n"
"    bool EnableAmbient;             // Control whether ambient colors are rendered \n"
"    bool EnableSpecular;            // Control whether specular colors are rendered \n"
"};\n"
""
"const int MaxLights = 8; \n"
"struct phLight { \n"
"    bool IsEnabled;             // True if light is turned on \n"
"    bool IsAttenuated;          // True if attenuation is active \n"
"    bool IsSpotLight;           // True if spotlight \n"
"    bool IsDirectional;         // True if directional \n"
"    vec3 Position; \n"
"    vec4 AmbientColor; \n"
"    vec4 DiffuseColor; \n"
"    vec4 SpecularColor; \n"
"    vec3 SpotDirection;     // Should be unit vector! \n"
"    float SpotCosCutoff;        // Cosine of cutoff angle \n"
"    float SpotExponent; \n"
"    float ConstantAttenuation; \n"
"    float LinearAttenuation; \n"
"    float QuadraticAttenuation; \n"
"};\n"
"layout (std140) uniform phLightArray { \n"
"    phLight Lights[MaxLights];\n"
"};\n"
""
"uniform mat4 projectionMatrix;		// The projection matrix\n"
"uniform mat4 modelviewMatrix;		// The modelview matrix\n"
"uniform mat4 viewMatrixInv;		// The inverse of the view matrix (use for cube map reflections)\n"
""
"vec3 mvPos;   // Vertex position in modelview coordinates\n"
"vec3 mvNormal; // Normal vector to vertex in modelview coordinates\n"
"vec4 matEmissive;\n"
"vec4 matAmbient;\n"
"vec4 matDiffuse;\n"
"vec4 matSpecular;\n"
"float matSpecExponent;\n"
"void CalculatePhongLighting();\n"
""
"void main()\n"
"{\n"
"    vec4 mvPos4 = modelviewMatrix * vec4(vertPos.x, vertPos.y, vertPos.z, 1.0); \n"
"    gl_Position = projectionMatrix * mvPos4; \n"
"    mvPos = vec3(mvPos4.x,mvPos4.y,mvPos4.z)/mvPos4.w; \n"
"    mvNormal = normalize(inverse(transpose(mat3(modelviewMatrix)))*vertNormal); // Unit normal from the surface \n"
"    matEmissive = EmissiveColor;\n"
"    matAmbient = AmbientColor;\n"
"    matDiffuse = DiffuseColor;\n"
"    matSpecular = SpecularColor;\n"
"    matSpecExponent = SpecularExponent;\n"
"    theTexCoords = vertTexCoords; \n"
"    origNormal = vertNormal; \n"
"    vec3 vVectorNeg = LocalViewer ? mvPos : vec3(0.0, 0.0, -1.0); \n"
"    reflectDir = mat3(viewMatrixInv)*reflect(vVectorNeg,mvNormal); \n"
"    CalculatePhongLighting();  // Calculate: nonspecColor and specularColor. \n"
"} \n"
"\0";


// Shared code for calculating Phong light!
const char shaderCalcPhong[] = 
""
"// This routine calculates the two vec3's nonspecColor and specularColor\n"
"void CalculatePhongLighting() { \n"
"    nonspecColor = vec4(0.0, 0.0, 0.0, 0.0);  \n"
"    specularColor = vec4(0.0, 0.0, 0.0, 0.0);  \n"
"    if ( EnableEmissive ) { \n"
"       nonspecColor = matEmissive; \n"
"    }\n"
"    if ( EnableAmbient ) { \n"
"         nonspecColor += matAmbient*GlobalAmbientColor; \n"
"    } \n"
"    vec3 vVector = LocalViewer ? -mvPos : vec3(0.0, 0.0, 1.0); // Unit vector towards non-local viewer \n"
"    vVector = normalize(vVector);\n"
"    for ( int i=0; i<NumLights; i++ ) {\n"
"        if ( Lights[i].IsEnabled ) { \n"
"            vec4 nonspecColorLt = vec4(0.0, 0.0, 0.0,0.0);\n"
"            vec4 specularColorLt = vec4(0.0, 0.0, 0.0,0.0);\n"
"            vec3 lVector = -Lights[i].Position;   // Direction to the light \n"
"            if ( !Lights[i].IsDirectional ) {\n "
"                lVector = -(lVector + mvPos);\n"
"            }\n"
"            lVector = normalize(lVector); // Unit vector to the light position.\n"
"            float dotEllNormal = dot(lVector, mvNormal); \n"
"            if (dotEllNormal > 0 ) { \n"
"                float spotCosine;\n"
"                if ( Lights[i].IsSpotLight ) {\n"
"                    spotCosine = -dot(lVector,Lights[i].SpotDirection);\n"
"                }\n"
"                if ( !Lights[i].IsSpotLight || spotCosine > Lights[i].SpotCosCutoff ) {\n"
"                    if ( EnableDiffuse ) { \n"
"                        nonspecColorLt += matDiffuse*Lights[i].DiffuseColor*dotEllNormal; \n"
"                    } \n"
"                    if ( EnableSpecular ) { \n"
"                        float rDotV = dot(vVector, 2.0*dotEllNormal*mvNormal - lVector); \n"
"                        if ( rDotV>0.0 ) {\n"
"                            float specFactor = pow( rDotV, matSpecExponent);\n"
"                            specularColorLt += specFactor*matSpecular*Lights[i].SpecularColor; \n"
"                        } \n"
"                    } \n"
"                    if ( Lights[i].IsSpotLight ) {"
"                        float spotAtten = pow(spotCosine,Lights[i].SpotExponent);"
"                        nonspecColorLt *= spotAtten; \n"
"                       specularColorLt *= spotAtten;\n"
"                    } \n"
"                }\n"
"            }\n"
"            if ( Lights[i].IsAttenuated ) { \n"
"                float dist = distance(mvPos,Lights[i].Position); \n"
"                float atten = 1.0/(Lights[i].ConstantAttenuation + (Lights[i].LinearAttenuation + Lights[i].QuadraticAttenuation*dist)*dist);\n"
"                nonspecColorLt *= atten; \n"
"                specularColorLt *= atten;\n"
"            } \n"
"            if ( EnableAmbient ) { \n"
"                nonspecColorLt += matAmbient*Lights[i].AmbientColor; \n"
"            } \n"
"            nonspecColor += nonspecColorLt;\n"
"            specularColor += specularColorLt;\n"
"        }\n "
"    }\n"
"}\0";

// The fragment shader for Gouraud shading (after Phong lighting)
// is very simple.  All it does it output the color as averaged (smoothed)
// the colors computed by the vertex shaders
const char fragmentShader_PhongGouraud[] =
"#version 330 core\n"
"in vec4 nonspecColor;   // Nonspecular color (smoothed) calculated at vertex \n"
"in vec4 specularColor;  // Specular color (smoothed) calculated at vertex \n"
"in vec2 theTexCoords;\n"
"in vec3 origNormal; \n  // Normal vector in local coordinates \n"
"in vec3 reflectDir; \n  // Reflection vector in world coordinates \n"
"out vec4 fragmentColor;	// Color that will be used for the fragment\n"
"uniform sampler2D theTextureMap;\n"
"uniform samplerCube theCubeMap;\n"
"uniform bool applyTexture;\n"
"uniform int applyCubeMap;   // 0 - not applied, 1 - as environment map, 2 - as reflection map \n"
""
"void main()\n"
"{\n"
"    fragmentColor = vec4(nonspecColor);   // Add alpha value of 1.0.\n"
"    if ( applyTexture ) { \n"
"        fragmentColor *= texture(theTextureMap, theTexCoords);\n"
"    }\n"
"    if ( applyCubeMap==1 ) { \n"
"        fragmentColor *= texture(theCubeMap, origNormal); \n"
"    } \n"
"    else if ( applyCubeMap==2 ) { \n"
"        fragmentColor += texture(theCubeMap,reflectDir);"
"    } \n"
"    fragmentColor += vec4(specularColor);\n"
"}\n\0";

/*
 * Build and compile two shader programs
 *  One is for Phong lighting with Phong shading
 *  The other is for Phong lighting with Gouraud shading
 */
unsigned int phongUBO;              // Uniform Buffer Object for Phong lighting information
const int numGlobal = 7;            // Number of entries in the phGlobal structure
const int numLightData = 14;        // Number of entries in the phLight structure
GLint offsetsGlobal[numGlobal];     // Offsets into the UBO data for phGlobal data items.
GLint offsetsLight[numLightData+1]; // Offsets into the UBO data for phLight data items.
int globallightBlockSize;           // Size of globallight buffer in bytes
int lightsBlockSize;                // Size of data for a single light
int lightsBlockOffset;              // Offset for the light blonk in the uniform buffer object
int lightStride;                    // Stride between light blocks in the shader.

void setup_phong_shaders() {
    char fragmentShader_PhongPhong[sizeof(fragmentShader_PhongPhongBase) + sizeof(shaderCalcPhong)];
    strcpy_s(fragmentShader_PhongPhong, sizeof(fragmentShader_PhongPhong), fragmentShader_PhongPhongBase);
    strcat_s(fragmentShader_PhongPhong, sizeof(fragmentShader_PhongPhong), shaderCalcPhong);
    phShaderPhongPhong = setup_shader_vertfrag(vertexShader_PhongPhong, fragmentShader_PhongPhong);
    char vertexShader_PhongGouraud[sizeof(vertexShader_PhongGouraudBase) + sizeof(shaderCalcPhong)];
    strcpy_s(vertexShader_PhongGouraud, sizeof(vertexShader_PhongGouraud), vertexShader_PhongGouraudBase);
    strcat_s(vertexShader_PhongGouraud, sizeof(vertexShader_PhongGouraud), shaderCalcPhong);
    phShaderPhongGouraud = setup_shader_vertfrag(vertexShader_PhongGouraud, fragmentShader_PhongGouraud);

    // Get the locations of the uniform variables in the shader programs.
    projMatLocationPG = glGetUniformLocation(phShaderPhongGouraud, projMatName);
    //modelviewMatLocationPG = glGetUniformLocation(phShaderPhongGouraud, modelviewMatName);
    vMatInvLocationPG = glGetUniformLocation(phShaderPhongGouraud, viewMatrixInvName);
    applyTextureLocationPG = glGetUniformLocation(phShaderPhongGouraud, applyTextureName);
    applyCubeMapLocationPG = glGetUniformLocation(phShaderPhongGouraud, applyCubeMapName);
    globallightBlockIndexPG = glGetUniformBlockIndex(phShaderPhongGouraud, globallightBlockName);
    lightsBlockIndexPG = glGetUniformBlockIndex(phShaderPhongGouraud, lightsBlockName);
    glUniformBlockBinding(phShaderPhongGouraud, globallightBlockIndexPG, 0);      // Buffer binding 0 for global lights
    glUniformBlockBinding(phShaderPhongGouraud, lightsBlockIndexPG, 1);      // Buffer binding 1 for lights

    projMatLocationPP = glGetUniformLocation(phShaderPhongPhong, projMatName);
   // modelviewMatLocationPP = glGetUniformLocation(phShaderPhongPhong, modelviewMatName);
    vMatInvLocationPP = glGetUniformLocation(phShaderPhongPhong, viewMatrixInvName);
    applyTextureLocationPP = glGetUniformLocation(phShaderPhongPhong, applyTextureName);
    applyCubeMapLocationPP = glGetUniformLocation(phShaderPhongPhong, applyCubeMapName);
    globallightBlockIndexPP = glGetUniformBlockIndex(phShaderPhongPhong, globallightBlockName);
    lightsBlockIndexPP = glGetUniformBlockIndex(phShaderPhongPhong, lightsBlockName);
    glUniformBlockBinding(phShaderPhongPhong, globallightBlockIndexPP, 0);      // Buffer binding 0 for global lights
    glUniformBlockBinding(phShaderPhongPhong, lightsBlockIndexPP, 1);      // Buffer binding 1 for lights

    glGetActiveUniformBlockiv(phShaderPhongGouraud, globallightBlockIndexPG, GL_UNIFORM_BLOCK_DATA_SIZE, &globallightBlockSize);
    glGetActiveUniformBlockiv(phShaderPhongGouraud, lightsBlockIndexPG, GL_UNIFORM_BLOCK_DATA_SIZE, &lightsBlockSize);
    glGenBuffers(1, &phongUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, phongUBO);
    int uboAlign;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uboAlign);
    lightsBlockOffset = uboAlign * (1 + (globallightBlockSize - 1) / uboAlign );
    int totalSize = lightsBlockOffset + lightsBlockSize;
    glBufferData(GL_UNIFORM_BUFFER, totalSize, 0, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, phongUBO, 0, globallightBlockSize);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, phongUBO, lightsBlockOffset, lightsBlockSize);

    // Query locations in the global lights block
    const char* globalNames[numGlobal] = {
        "GlobalAmbientColor", "NumLights", "LocalViewer",
        "EnableEmissive", "EnableDiffuse", "EnableAmbient", "EnableSpecular" 
    };
    GLuint indicesGlobal[numGlobal];
    // GLint sizesGlobal[numGlobal];
    // GLint typesGlobal[numGlobal];
    glGetUniformIndices(phShaderPhongGouraud, numGlobal, globalNames, indicesGlobal);
    glGetActiveUniformsiv(phShaderPhongGouraud, numGlobal, indicesGlobal, GL_UNIFORM_OFFSET, offsetsGlobal);
    // glGetActiveUniformsiv(phShaderPhongGouraud, numGlobal, indicesGlobal, GL_UNIFORM_SIZE, sizesGlobal);
    // glGetActiveUniformsiv(phShaderPhongGouraud, numGlobal, indicesGlobal, GL_UNIFORM_TYPE, typesGlobal);

    // Query locations in the individual lights block
    const char* lightNames[numLightData+1] = {
        "Lights[0].IsEnabled", "Lights[0].IsAttenuated", "Lights[0].IsSpotLight", "Lights[0].IsDirectional",
        "Lights[0].Position", "Lights[0].AmbientColor", "Lights[0].DiffuseColor", "Lights[0].SpecularColor", "Lights[0].SpotDirection",
        "Lights[0].SpotCosCutoff", "Lights[0].SpotExponent", "Lights[0].ConstantAttenuation", "Lights[0].LinearAttenuation", "Lights[0].QuadraticAttenuation",
        "Lights[1].IsEnabled"
    };
    GLuint indicesLight[numLightData+1];
    GLint sizesLight[numLightData];
    GLint typesLight[numLightData];
    glGetUniformIndices(phShaderPhongGouraud, numLightData+1, lightNames, indicesLight);
    glGetActiveUniformsiv(phShaderPhongGouraud, numLightData+1, indicesLight, GL_UNIFORM_OFFSET, offsetsLight);
    glGetActiveUniformsiv(phShaderPhongGouraud, numLightData, indicesLight, GL_UNIFORM_SIZE, sizesLight);
    glGetActiveUniformsiv(phShaderPhongGouraud, numLightData, indicesLight, GL_UNIFORM_TYPE, typesLight);
    lightStride = offsetsLight[numLightData] - offsetsLight[0];
    if (phMaxNumLights*lightStride != lightsBlockSize) {
        fprintf(stderr, "EduPhong: Likely error in layout with shaders.\n");
    }

    glUseProgram(phShaderPhongPhong);
    glUniform1i(applyTextureLocationPP, 0); // Default is to  not apply the texture
    glUseProgram(phShaderPhongGouraud);
    glUniform1i(applyTextureLocationPG, 0); // Default is to  not apply the texture
}

void phMaterial::LoadIntoShaders()
{
    float vecEntries[4];
    EmissiveColor.Dump(vecEntries);
    glVertexAttrib4fv(phEmissiveColor_loc, vecEntries);		// Emissive
    AmbientColor.Dump(vecEntries);
    glVertexAttrib4fv(phAmbientColor_loc, vecEntries);		// Ambient
    DiffuseColor.Dump(vecEntries);
    glVertexAttrib4fv(phDiffuseColor_loc, vecEntries);		// Diffuse
    SpecularColor.Dump(vecEntries);
    glVertexAttrib4fv(phSpecularColor_loc, vecEntries);		// Specular

    glVertexAttrib1f(phSpecularExponent_loc, SpecularExponent);	// Specular exponent
}

unsigned int trueGLbool = 0xffffffff, falseGLbool = 0;

void phGlobal::LoadIntoShaders()
{
    char* buffer = new char[globallightBlockSize];
    GlobalAmbientColor.Dump((float*)(buffer + offsetsGlobal[0]));
    memcpy(buffer + offsetsGlobal[1], &NumLights, sizeof(unsigned int));
    memcpy(buffer + offsetsGlobal[2], LocalViewer ? &trueGLbool : &falseGLbool, 4);      // Note the really obscure way of loading a bool as a 4 byte integer
    memcpy(buffer + offsetsGlobal[3], EnableEmissive ? &trueGLbool : &falseGLbool, 4);
    memcpy(buffer + offsetsGlobal[4], EnableDiffuse ? &trueGLbool : &falseGLbool, 4);
    memcpy(buffer + offsetsGlobal[5], EnableAmbient ? &trueGLbool : &falseGLbool, 4);
    memcpy(buffer + offsetsGlobal[6], EnableSpecular ? &trueGLbool : &falseGLbool, 4);
    glBindBuffer(GL_UNIFORM_BUFFER, phongUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, globallightBlockSize, buffer);

    delete[] buffer;
}

void phLight::LoadIntoShaders(int lightNumber) {
    assert(0<=lightNumber && lightNumber < phMaxNumLights);
    char* buffer = new char[lightStride];       // Allocate enough space to hold data for one light
    int d = globallightBlockSize;               // Subtract of size of the buffer used for global lighting data
    memcpy(buffer + offsetsLight[0], IsEnabled ? &trueGLbool : &falseGLbool, 4);      // Note: load a bool as a 4 byte integer
    memcpy(buffer + offsetsLight[1], IsAttenuated ? &trueGLbool : &falseGLbool, 4);
    memcpy(buffer + offsetsLight[2], IsSpotLight ? &trueGLbool : &falseGLbool, 4);
    memcpy(buffer + offsetsLight[3], IsDirectional ? &trueGLbool : &falseGLbool, 4);
    PosOrDir.Dump((float*)(buffer + offsetsLight[4]));
    AmbientColor.Dump((float*)(buffer + offsetsLight[5]));
    DiffuseColor.Dump((float*)(buffer + offsetsLight[6]));
    SpecularColor.Dump((float*)(buffer + offsetsLight[7]));
    SpotDirection.Dump((float*)(buffer + offsetsLight[8]));
    memcpy(buffer + offsetsLight[9], &SpotCosCutoff, sizeof(float));
    memcpy(buffer + offsetsLight[10], &SpotExponent, sizeof(float));
    memcpy(buffer + offsetsLight[11], &ConstantAttenuation, sizeof(float));
    memcpy(buffer + offsetsLight[12], &LinearAttenuation, sizeof(float));
    memcpy(buffer + offsetsLight[13], &QuadraticAttenuation, sizeof(float));
    glBindBuffer(GL_UNIFORM_BUFFER, phongUBO);
    int startLoc = lightsBlockOffset + lightNumber * lightStride;
    glBufferSubData(GL_UNIFORM_BUFFER, startLoc, lightStride, buffer);

}


bool phLight::CheckCorrectness()
{
    // If it is directional,  the position is instead the direction, and should be a unit vector.
    if (IsDirectional) {
        float sqNorm =
            (float)(PosOrDir[0] * PosOrDir[0]
                + PosOrDir[1] * PosOrDir[1]
                + PosOrDir[2] * PosOrDir[2]);
        if (sqNorm<0.000001) {
            fprintf(stdout, "phLight error: Directional light needs directon from light in 'Position'.\n");
            return false;
        }
    }
    // If it is a spot light, the position is instead the direction, and should be a unit vector.
    if (IsSpotLight) {
        float sqNorm =
            (float)(SpotDirection[0] * SpotDirection[0]
                + SpotDirection[1] * SpotDirection[1]
                + SpotDirection[2] * SpotDirection[2]);
        if (IsDirectional || sqNorm<0.999f || sqNorm>1.001f) {
            fprintf(stdout, "phLight error: Spot light misconfigured.\n");
            return false;
        }
    }
    return true;
}

bool phGlobal::CheckCorrectness() 
{
    if (NumLights > phMaxNumLights) {
        fprintf(stdout, "pgGlobal error: Too many lights.\n");
        return false;
    }
    return true;
}

