#pragma once
//sVertex_types.h file

//This matches what's being used in the shader:
//  attribute vec3 vCol;
//  attribute vec3 vPos;

//struct sVertex_XYZ_RGB
//{
//    float x, y, z;  /*z*/
//    float r, g, b;
//};


//struct sVertex_XYZW_RGBA
//{
//    float x, y, z, w;   //Same as vec4 vPosition
//    float r, g, b, a;   //Same as vec4 vColour
//};

//in vec4 vColour;				// R,G,B,A ("alpha")
//in vec4 vPosition;				// X,Y,Z (W is ignored)
//in vec4 vNormal;				// Vertex normal X,Y,Z (W ignored)
//in vec4 vUVx2;					// 2 x Texture coords (vec4) UV0, UV1
//in vec4 vTangent;				// For bump mapping X,Y,Z (W ignored)
//in vec4 vBiNormal;				// For bump mapping X,Y,Z (W ignored)

struct sVertex_XYZW_RGBA_N_UV_T_B
{
    float x, y, z, w;       //Same as vec4 vPosition
    float r, g, b, a;       //Same as vec4 vColour
    float nx, ny, nz, nw;   //Same as vec4 vNormal
    float u0, v0, u1, v1;   //Same as vec4 vUVx2    (UV)
    float tx, ty, tz, tw;   //Same as vec4 vTangent (Tangent Vector)
    float bx, by, bz, bw;   //Same as vec4 vBiNormal
};