//Evan Sinasac - 1081418
//INFO6028 Graphics (Project 1)
//cMesh.h description:
//				Mesh/model information.  Given to us by Michael Feeney.  Removed the boolean for bIsWireframe as it is handled by a
//				single boolean in theMain.cpp that is dependent on the user pressing the space key to toggle.
//				Changes will continue to be made here depending on how we're doing lighting and things, not fully sure yet.
#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

struct cMesh
{
public:
	cMesh();
	std::string meshName;

	glm::vec3 positionXYZ;
	glm::vec3 orientationXYZ;		// "angle"
	float scale;

	//bool bIsWireframe;

	//Sets the overall colour of the object
	//by overwriting the 
	bool bOverrideColour;
	glm::vec3 vertexColourOverride;
};