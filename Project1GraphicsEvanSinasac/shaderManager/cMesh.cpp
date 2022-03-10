//Evan Sinasac - 1081418
//INFO6028 Graphics (Project 1)
//cMesh.cpp description:
//				Mesh/model information.  Given to us by Michael Feeney.  Removed the boolean for bIsWireframe as it is handled by a
//				single boolean in theMain.cpp that is dependent on the user pressing the space key to toggle.
//				Changes will continue to be made here depending on how we're doing lighting and things, not fully sure yet.

#include "cMesh.h"

cMesh::cMesh()
{
	this->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	this->orientationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	this->scale = 1.0f;
	
	//this->bIsWireframe = false;

	this->bOverrideColour = false;
	this->vertexColourOverride = glm::vec3(1.f, 1.f, 1.f);
	//OR
	//this->positionXYZ.x = 0.0f; //...
}