//Evan Sinasac - 1081418
//INFO6028 Graphics (Project 1)
//cVAOManager.cpp description:
//	                    Given to us, whenever we change the shaders we need to make the appropriate changes here to match

//#include "GLCommon.h"
#include "../GLCommon.h"

#include "cVAOManager.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

//#include "sVertex_types.h"
#include "../sVertex_types.h"


bool LoadPLYModelFromFile(std::string fileName, sModelDrawInfo& drawInfo);

sModelDrawInfo::sModelDrawInfo()
{
    this->VAO_ID = 0;

    this->VertexBufferID = 0;
    this->VertexBuffer_Start_Index = 0;
    this->numberOfVertices = 0;

    this->IndexBufferID = 0;
    this->IndexBuffer_Start_Index = 0;
    this->numberOfIndices = 0;
    this->numberOfTriangles = 0;

    // The "local" (i.e. "CPU side" temporary array)
    this->pVertices = 0;	// or NULL
    this->pIndices = 0;		// or NULL

    // You could store the max and min values of the 
    //  vertices here (determined when you load them):
    glm::vec3 maxValues;
    glm::vec3 minValues;

    //	scale = 5.0/maxExtent;		-> 5x5x5
    float maxExtent;

    return;
}


bool cVAOManager::LoadModelIntoVAO(
    std::string fileName,
    sModelDrawInfo& drawInfo,
    unsigned int shaderProgramID)

{
    // Load the model from file
    // (We do this here, since if we can't load it, there's 
    //	no point in doing anything else, right?)

    drawInfo.meshName = fileName;

    // TODO: Load the model from file

    LoadPLYModelFromFile(fileName, drawInfo);
    // 
    // Model is loaded and the vertices and indices are in the drawInfo struct
    // 

    // Create a VAO (Vertex Array Object), which will 
    //	keep track of all the 'state' needed to draw 
    //	from this buffer...

    // Ask OpenGL for a new buffer ID...
    glGenVertexArrays(1, &(drawInfo.VAO_ID));
    // "Bind" this buffer:
    // - aka "make this the 'current' VAO buffer
    glBindVertexArray(drawInfo.VAO_ID);

    // Now ANY state that is related to vertex or index buffer
    //	and vertex attribute layout, is stored in the 'state' 
    //	of the VAO... 


    // NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &(drawInfo.VertexBufferID));

    //	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
    // sVert vertices[3]
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(sVertex_XYZW_RGBA_N_UV_T_B) * drawInfo.numberOfVertices,	//sizeof(sVertex_XYZ_RGB) * drawInfo.numberOfVertices // ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
        (GLvoid*)drawInfo.pVertices,							        // pVertices,			//vertices, 
        GL_STATIC_DRAW);


    // Copy the index buffer into the video card, too
    // Create an index buffer.
    glGenBuffers(1, &(drawInfo.IndexBufferID));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
        sizeof(unsigned int) * drawInfo.numberOfIndices,
        (GLvoid*)drawInfo.pIndices,
        GL_STATIC_DRAW);

    //**************************************************************************
    // Set the vertex attributes.
    
    //Position
    GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPosition");	// program
    glEnableVertexAttribArray(vpos_location);	// vPosition
    glVertexAttribPointer(vpos_location, 4,		// vPosition
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex_XYZW_RGBA_N_UV_T_B),     // Stride // sizeof(float) * 6,      
        (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, x));

    //Colour
    GLint vcol_location = glGetAttribLocation(shaderProgramID, "vColour");	    // program;
    glEnableVertexAttribArray(vcol_location);	// vColour
    glVertexAttribPointer(vcol_location, 4,		// vColour
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex_XYZW_RGBA_N_UV_T_B),     // Stride // sizeof(float) * 6,      
        (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, r));

    //Normal
    GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");
    glEnableVertexAttribArray(vNormal_location);	// vNormal
    glVertexAttribPointer(vNormal_location, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex_XYZW_RGBA_N_UV_T_B),     // Stride // sizeof(float) * 6,      
        (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, nx));

    //UVx2
    GLint vUVx2_location = glGetAttribLocation(shaderProgramID, "vUVx2");
    glEnableVertexAttribArray(vUVx2_location);	// vNormal
    glVertexAttribPointer(vUVx2_location, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex_XYZW_RGBA_N_UV_T_B),     // Stride // sizeof(float) * 6,      
        (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, u0));

    //Tangent
    GLint vTangent_location = glGetAttribLocation(shaderProgramID, "vTangent");
    glEnableVertexAttribArray(vTangent_location);	// vNormal
    glVertexAttribPointer(vTangent_location, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex_XYZW_RGBA_N_UV_T_B),     // Stride // sizeof(float) * 6,      
        (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, tx));

    //BiNormal
    GLint vBiNormal_location = glGetAttribLocation(shaderProgramID, "vBiNormal");
    glEnableVertexAttribArray(vBiNormal_location);	// vNormal
    glVertexAttribPointer(vBiNormal_location, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex_XYZW_RGBA_N_UV_T_B),     // Stride // sizeof(float) * 6,      
        (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, bx));

    // Now that all the parts are set up, set the VAO to zero
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);


    // Store the draw information into the map
    this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;


    return true;
}


// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
    std::string filename,
    sModelDrawInfo& drawInfo)
{
    std::map< std::string /*model name*/,
        sModelDrawInfo /* info needed to draw*/ >::iterator
        itDrawInfo = this->m_map_ModelName_to_VAOID.find(filename);

    // Find it? 
    if (itDrawInfo == this->m_map_ModelName_to_VAOID.end())
    {
        // Nope
        std::cout << "Yo, didn't find it" << std::endl;     //error print if a model isn't found
        return false;
    }

    // Else we found the thing to draw
    // ...so 'return' that information
    drawInfo = itDrawInfo->second;
    return true;
}

bool LoadPLYModelFromFile(std::string fileName, sModelDrawInfo& drawInfo)
{
    // These structures match the PLY file format
    struct sVertex
    {
        float x, y, z, c, i;
        // Now with normals...
        float nx, ny, nz;
        //removed RGBA since I'm colouring the models from the worldFile
        // Now with RGBA...
        // (Note the file is in HTML style, so 0 to 255
        //  but the shader wants 0.0 to 1.0)
        //float red, green, blue, alpha;
    };
    struct sTriangle
    {
        unsigned int vertIndex[3];
    };

    std::ifstream theFile(fileName.c_str());

    //    std::ifstream theFile("GalacticaOriginal_ASCII_no_text.ply");
        //    std::ifstream theFile( fileName.c_str() );

            // Did it open?
    if (!theFile.is_open())          // theFile.is_open() == false
    {
        return false;
    }

    // The file is good to go.

    std::string nextToken;

    // Scan until we find the word "vertex"...
    while (theFile >> nextToken)
    {
        if (nextToken == "vertex")
        {
            break;  // Exits the while loop
        }
    }

    // Read the number of vertices
    theFile >> drawInfo.numberOfVertices;


    // Scan until we find the word "face"...
    while (theFile >> nextToken)
    {
        if (nextToken == "face")
        {
            break;  // Exits the while loop
        }
    }

    // Read the number of triangles (aka "faces")
    theFile >> drawInfo.numberOfTriangles;

    // Scan until we find the word "end_header"...
    while (theFile >> nextToken)
    {
        if (nextToken == "end_header")
        {
            break;  // Exits the while loop
        }
    }

    //    sVertex myVertexArray[500];
    std::vector<sVertex> vecVertexArray;    // aka "smart array"

    // Now we can read the vertices (in a for loop)
    for (unsigned int index = 0; index < drawInfo.numberOfVertices; index++)
    {
        sVertex tempVertex;

        theFile >> tempVertex.x;
        theFile >> tempVertex.y;
        theFile >> tempVertex.z;
        //        theFile >> tempVertex.c;
        // Now load normals:
        theFile >> tempVertex.nx;
        theFile >> tempVertex.ny;
        theFile >> tempVertex.nz;

        // Now load RGBA colours:
        //theFile >> tempVertex.red;
        //theFile >> tempVertex.green;
        //theFile >> tempVertex.blue;
        //theFile >> tempVertex.alpha;
        //       theFile >> tempVertex.i;

                //vecVertexArray[index] = tempVertex;
                // "Add to the end of the vector"
                // "push" == "add", "back" == "end"
        vecVertexArray.push_back(tempVertex);       // Add thing at end of smart array

    }

    std::vector<sTriangle> vecTriagleArray;    // aka "smart array"

    // Now we can read the triangles (in a for loop)
    for (unsigned int index = 0; index < drawInfo.numberOfTriangles; index++)
    {
        sTriangle tempTri;
        int discardThis;

        // 3 3087 3639 5315 
        theFile >> discardThis; // The "3" at the start
        theFile >> tempTri.vertIndex[0];
        theFile >> tempTri.vertIndex[1];
        theFile >> tempTri.vertIndex[2];

        vecTriagleArray.push_back(tempTri);
    }



    // Allocate the amount of space we need for the GPU side arrays
    drawInfo.pVertices = new sVertex_XYZW_RGBA_N_UV_T_B[drawInfo.numberOfVertices];
    //drawInfo.pVertices = new sVertex_XYZ_RGB[drawInfo.numberOfVertices];

    // There are 3 indices per triangle...
    drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];



    // Copy the vertices from the PLY format vector
    //  to the one we'll use to draw in the GPU
    for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
    {
        // Copy position...
        drawInfo.pVertices[index].x = vecVertexArray[index].x;
        drawInfo.pVertices[index].y = vecVertexArray[index].y;
        drawInfo.pVertices[index].z = vecVertexArray[index].z;
        // If unsure about the 4th value of a vec4, make it 1.0f
        drawInfo.pVertices[index].w = 1.0f;

        // Copy colour...
        // And convert from 0-255 into 0-1
        //drawInfo.pVertices[index].r = (vecVertexArray[index].red / 255.0f);
        //drawInfo.pVertices[index].g = (vecVertexArray[index].green / 255.0f);
        //drawInfo.pVertices[index].b = (vecVertexArray[index].blue / 255.0f);
        //drawInfo.pVertices[index].a = (vecVertexArray[index].alpha / 255.0f);

        // And copy the other things, too:
//        float nx, ny, nz, nw;   // in vec4 vNormal;	Vertex normal X,Y,Z (W ignored)
        drawInfo.pVertices[index].nx = vecVertexArray[index].nx;
        drawInfo.pVertices[index].ny = vecVertexArray[index].ny;
        drawInfo.pVertices[index].nz = vecVertexArray[index].nz;
        drawInfo.pVertices[index].nw = 1.0f;
    }

    // Copy the triangle ("index") values to the index (element) array
    unsigned int elementIndex = 0;
    for (unsigned int triIndex = 0; triIndex < drawInfo.numberOfTriangles;
        triIndex++, elementIndex += 3)
    {
        drawInfo.pIndices[elementIndex + 0] = vecTriagleArray[triIndex].vertIndex[0];
        drawInfo.pIndices[elementIndex + 1] = vecTriagleArray[triIndex].vertIndex[1];
        drawInfo.pIndices[elementIndex + 2] = vecTriagleArray[triIndex].vertIndex[2];
    }


    return true;
}