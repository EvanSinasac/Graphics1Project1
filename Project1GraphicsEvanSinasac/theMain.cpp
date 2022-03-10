//Evan Sinasac - 1081418
//INFO6028 Graphics (Project 1)
//theMain.cpp description:
//			        The main purpose of project 1 is to display a scene (it is a graphics course) of models that are loaded from an external file.
//                  Models are located at SOLUTION_DIR/common/assets/models, where there is also a text file (modelsToLoad.txt) 
//                  with a list of the models to load into the scene.
//                  The world(scene) is made from the text file located at SOLUTION_DIR/common/assets in worldFile.txt, which describes the camera
//                  starting position, what model to load, where to load it, its orientation, scale and colour [in RGB[0-255]).
//                  Can switch between wire-frame and solid shading by pressing the space key

#include "GLCommon.h"	//common includes for GLAD and GLFW
#include "GLMCommon.h"	//common includes for GLM (mainly vector and matrix stuff

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>		//smart array
#include <fstream>		//C++ file I-O library

#include "shaderManager/cVAOManager.h"
#include "shaderManager/cMesh.h"
#include "shaderManager/cShaderManager.h"

//2 stages: load file into RAM, then copy RAM into GPU format

//Globals;
glm::vec3 cameraEye = glm::vec3(0.0f, 0.0f, 3.0f);     //default camera start position, just in case something goes wrong with loading the worldFile
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 1.0f);   //default camera Target position
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);       //default upVector

float lastX = 600.f;
float lastY = 320.f;
bool firstMouse = true;
float yaw = 90.f;
float pitch = 0.f;

float deltaTime = 0.f;
float lastFrame = 0.f;

cVAOManager		gVAOManager;
cShaderManager	gShaderManager;

std::vector<cMesh> g_vecMeshes;

bool bWireFrame = false;

std::vector<std::string> modelLocations;


//callbacks
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//Project 1 methods
bool loadModelsFromFile(GLuint& program);
bool loadWorldFile();


int main(int argc, char** argv)
{
    GLFWwindow* window;

    GLuint program = 0;     //0 means "no shader program"

    GLint mvp_location = -1;
    GLint vpos_location = -1;
    GLint vcol_location = -1;


    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1200, 640, "Project 1", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    //OpenGL error checks omitted for brevity (I should probably add them back)
    std::stringstream ss;

    cShaderManager::cShader vertShader;
    ss << SOLUTION_DIR << "common/assets/shaders/vertShader_01.glsl";
    vertShader.fileName = ss.str().c_str();
    ss.str("");

    cShaderManager::cShader fragShader;
    ss << SOLUTION_DIR << "common/assets/shaders/fragShader_01.glsl";
    fragShader.fileName = ss.str().c_str();
    ss.str("");

    if (gShaderManager.createProgramFromFile("Shader#1", vertShader, fragShader))
    {
        std::cout << "Shader compiled OK" << std::endl;
        //Set the "program" variable to the one the Shader Manager used...
        program = gShaderManager.getIDFromFriendlyName("Shader#1");
    }
    else
    {
        std::cout << "Error making shader program: " << std::endl;
        std::cout << gShaderManager.getLastError() << std::endl;
    }

    //Getting the locations from the shaders
    mvp_location = glGetUniformLocation(program, "MVP");

    GLint matModel_Location = glGetUniformLocation(program, "matModel");
    GLint matView_Location = glGetUniformLocation(program, "matView");
    GLint matProjection_Location = glGetUniformLocation(program, "matProjection");

    GLint matModelInverseTranspose_Location = glGetUniformLocation(program, "matModelInversetranspose");

    //Will remove colour from models, will be assigned by World file
    GLint bUseVertexColour_Location = glGetUniformLocation(program, "bUseVertexColour");
    GLint vertexColourOverride_Location = glGetUniformLocation(program, "vertexColourOverride");

    

    if (loadModelsFromFile(program))
    {
        std::cout << "loadModelsFromFile finished ok" << std::endl;
    }
    else
    {
        std::cout << "loadModelsFromFile did not finish ok, aborting" << std::endl;
        return -1;
    }

    if (loadWorldFile())
    {
        std::cout << "loadWorldFile finished OK" << std::endl;
    } 
    else
    {
        std::cout << "loadWorldFile did not finish OK, aborting" << std::endl;
        return -1;
    }

    //cameraTarget is (0,0,1) by default.
    //Going to be completely honest here, I tried to use SOHCAHTOA to determine the yaw angle depending on which quadrant the camera is in to face it towards the origin...
    //and it did not work at all lol
    //So I moved the values around and printed out values to the screen and kinda figured these out through brute force lol.
    //It's because I did all my math using the top right quadrant as the positive quadrant, but the top left quadrant is positive 
    //(which we know since we initially place the camera along the x-axis in the negative z, and moving the camera visually left puts us in the positive x)
    //also, tried putting this in the mouse_callback for the inital, but it doesn't run until the mouse moves, so we get a weird jump when we first move the mouse
    if (cameraEye.x > 0 && cameraEye.z > 0)
    {
        yaw = 180.f + (atan(cameraEye.z / cameraEye.x) * 180.f / 3.1459f);
    }
    else if (cameraEye.x > 0 && cameraEye.z < 0)
    {
        yaw = 90.f - (atan(cameraEye.z / cameraEye.x) * 180.f / 3.1459f);
    }
    else if (cameraEye.x < 0 && cameraEye.z > 0)
    {
        yaw = (atan(cameraEye.z / cameraEye.x) * 180.f / 3.1459f);
    }
    else if (cameraEye.x < 0 && cameraEye.z < 0)
    {
        yaw = (atan(cameraEye.z / cameraEye.x) * 180.f / 3.1459);
    }
    else if (cameraEye.x == 0.f)
    {
        if (cameraEye.z >= 0.f)
        {
            yaw = 270.f;
        }
        else
        {
            yaw = 90.f;
        }
    }
    else if (cameraEye.z == 0.f)
    {
        if (cameraEye.x <= 0)
        {
            yaw = 0.f;
        }
        else
        {
            yaw = 180.f;
        }
    }
    //anyways, after figuring out the yaw, we set the target at the negative of the xz of the camera position and y=0 (this faces the camera towards the origin)
    cameraTarget = glm::vec3(-1.f * cameraEye.x, 0, -1.f * cameraEye.z);
    glm::normalize(cameraTarget);                                           //and normalize it so when we initially add the target to the position it doesn't shift

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glm::mat4 matModel;
        glm::mat4 p;
        glm::mat4 v;
        glm::mat4 mvp;

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        //Turn on the depth buffer
        glEnable(GL_DEPTH);         //Turns on the depth buffer
        glEnable(GL_DEPTH_TEST);    //Check if the pixel is already closer

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //*********************************************************************
        //Screen is cleared and we're ready to draw
        //*********************************************************************
        for (unsigned int index = 0; index != g_vecMeshes.size(); index++)
        {
            cMesh curMesh = g_vecMeshes[index];

            //         mat4x4_identity(m);
            matModel = glm::mat4(1.0f);     //"Identity" ("do nothing", like x1)
            //*********************************************************************
            //Translate or "move the object
            //*********************************************************************
            glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
                                                    curMesh.positionXYZ);

            //matModel = matModel * matTranslate;
            //*********************************************************************
            //
            //*********************************************************************
            //Rotation around the Z-axis
            //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
            glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
                                            curMesh.orientationXYZ.z,   //(float)glfwGetTime()
                                            glm::vec3(0.0f, 0.0, 1.0f));

            //matModel = matModel * rotateZ;
            //*********************************************************************
            //
            //*********************************************************************
            //Rotation around the Y-axis
            glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
                                            curMesh.orientationXYZ.y,   //(float)glfwGetTime()
                                            glm::vec3(0.0f, 1.0, 0.0f));

            //matModel = matModel * rotateY;
            //*********************************************************************
            //
            //*********************************************************************
            //Rotation around the X-axis
            glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
                                            curMesh.orientationXYZ.x,   //(float)glfwGetTime()
                                            glm::vec3(1.0f, 0.0, 0.0f));

            //matModel = matModel * rotateX;
            //*********************************************************************
            //Scale
            //*********************************************************************
            glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
            glm::vec3(  curMesh.scale,  // Scale in X
                        curMesh.scale,  // Scale in Y
                        curMesh.scale));// Scale in Z

            //matModel = matModel * matScale;
            //*********************************************************************
            // 
            //*********************************************************************
            //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
            matModel = matModel * matTranslate;
            matModel = matModel * rotateZ;
            matModel = matModel * rotateY;
            matModel = matModel * rotateX;
            matModel = matModel * matScale;


            p = glm::perspective(0.6f,
                               ratio,
                                0.1f,
                                1000.0f);

            v = glm::mat4(1.0f);

            /*glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -4.0f);*/
            /*glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);*/
            /*glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);*/
            
            v = glm::lookAt(cameraEye,          // "eye"
                            cameraEye + cameraTarget,       // "at" //used to be    cameraTarget
                            upVector);

            //mat4x4_mul(mvp, p, m);
            mvp = p * v * matModel;
            glUseProgram(program);


            //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

            //Don't need this anymore since it's being done inside the shader
            //mvp = p * v * matModel;
            glUniformMatrix4fv(matModel_Location, 1, GL_FALSE, glm::value_ptr(matModel));
            glUniformMatrix4fv(matView_Location, 1, GL_FALSE, glm::value_ptr(v));
            glUniformMatrix4fv(matProjection_Location, 1, GL_FALSE, glm::value_ptr(p));

            //Inverse transpose of the mdoel matrix
            //(Used to calculate the normal location in vertex space
            glm::mat4 matInvTransposeModel = glm::inverse(glm::transpose(matModel));
            glUniformMatrix4fv(matModelInverseTranspose_Location, 1, GL_FALSE, glm::value_ptr(matInvTransposeModel));

            //Colour Override HACK (will be removed/changed later)
            if (curMesh.bOverrideColour)
            {
                glUniform1f(bUseVertexColour_Location, (float)GL_TRUE);
                glUniform3f(vertexColourOverride_Location,
                            curMesh.vertexColourOverride.r,
                            curMesh.vertexColourOverride.g,
                            curMesh.vertexColourOverride.b);
            }
            else
            {
                glUniform1f(bUseVertexColour_Location, (float)GL_FALSE);
            }

            //Wireframe
            if (bWireFrame)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                //GL_POINT, GL_LINE, and GL_FILL


            sModelDrawInfo modelInfo;
            if (gVAOManager.FindDrawInfoByModelName(g_vecMeshes[index].meshName, modelInfo))
            {
                glBindVertexArray(modelInfo.VAO_ID);
                glDrawElements( GL_TRIANGLES,
                                modelInfo.numberOfIndices,
                                GL_UNSIGNED_INT,
                                (void*)0);
                glBindVertexArray(0);
            }

        } //end of for
        //Scene is drawn

        //"Present" what we've drawn
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
} //end of main


bool loadModelsFromFile(GLuint& program)
{
    std::stringstream ss;
    std::stringstream sFile;
    sModelDrawInfo curModel;

    ss << SOLUTION_DIR << "common\\assets\\models\\modelsToLoad.txt";

    std::ifstream theFile(ss.str());

    if (!theFile.is_open())
    {
        fprintf(stderr, "Could not open modelsToLoad.txt");
        return false;
    }
    std::string nextToken;
    ss.str("");

    while (theFile >> nextToken)
    {
        if (nextToken == "end")
        {
            break;
        } //end if
        //Using a similar file reader that I made for the media fundamentals project one.
        //Essentially, going to read the file until we reach the keyword "end"^
        //To make sure we have a full file name, read until the word contains "ply", or whatever kind of model file we're loading
        //this more or less covers file names with spaces in them, but we're not going to have files with spaces in the name
        //realized the error check is essentially useless since we have to not have spaces in the model names for the worldFile to work correctly, leaving it in case something changes my mind and I add an error check there/for future coding model names with spaces are ok idk.  If you got this far in the comment, hello, hope you have a nice day :)
        if (nextToken.find("ply") != std::string::npos)
        {
            sFile << nextToken.c_str();
            ss << SOLUTION_DIR << "common\\assets\\models\\" << sFile.str().c_str();
            modelLocations.push_back(ss.str().c_str());
            if (gVAOManager.LoadModelIntoVAO(ss.str().c_str(), curModel, program))
            {
                std::cout << "Loaded the " << sFile.str().c_str() << " model OK" << std::endl;
            }
            else
            {
                std::cout << "Error: Didn't load the model OK" << std::endl;
            }

            ss.str("");
            sFile.str("");
        }
        else
        {
            sFile << nextToken.c_str() << " ";
        } //end else
    } //end while
    theFile.close();
    return true;
} //end of loadModelsFromFile

bool loadWorldFile()
{
    std::stringstream ss;
    std::stringstream sFile;
    cMesh curMesh;

    ss << SOLUTION_DIR << "common\\assets\\worldFile.txt";

    std::ifstream theFile(ss.str());

    if (!theFile.is_open())
    {
        fprintf(stderr, "Could not open modelsToLoad.txt");
        return false;
    }
    std::string nextToken;
    ss.str("");

    //Throwaway text describing the format of the file
    theFile >> nextToken;       //ModelFileName(extension)
    theFile >> nextToken;       //Position(x,y,z)
    theFile >> nextToken;       //Orientation(x,y,z)
    theFile >> nextToken;       //Scale
    theFile >> nextToken;       //Colour(r,g,b)

    theFile >> nextToken;       //Camera
    if (nextToken == "Camera")
    {
        //might as well check that we're in the right spot
        theFile >> nextToken;   //Camera x position
        cameraEye.x = std::stof(nextToken);
        theFile >> nextToken;   //Camera y position
        cameraEye.y = std::stof(nextToken);
        theFile >> nextToken;   //Camera z position
        cameraEye.z = std::stof(nextToken);
    }
    else
    {
        std::cout << "Uhm, we're in the wrong position in the worldFile, aborting!" << std::endl;
        return false;
    }

    while (theFile >> nextToken)    //this should always be the name of the model to load or end.  Potential error check, add a check for "ply" in the mdoel name
    {
        if (nextToken == "end")
        {
            break;
        }
        std::cout << nextToken << std::endl;        //Printing model names to console, just making sure we're loading ok.  Can be commented out whenever
        //First is the file name of model
        ss << SOLUTION_DIR << "common\\assets\\models\\" << nextToken;
        curMesh.meshName = ss.str().c_str();
        //Next 3 are the position of the model
        theFile >> nextToken;                                               //x position for the model
        curMesh.positionXYZ.x = std::stof(nextToken);
        theFile >> nextToken;                                               //y position for the model
        curMesh.positionXYZ.y = std::stof(nextToken);
        theFile >> nextToken;                                               //z position for the model
        curMesh.positionXYZ.z = std::stof(nextToken);
        //Next 3 are the orientation of the model
        theFile >> nextToken;                                               //x orientation value
        //curMesh.orientationXYZ.x = std::stof(nextToken);
        curMesh.orientationXYZ.x = glm::radians(std::stof(nextToken));
        theFile >> nextToken;                                               //y orientation value
        //curMesh.orientationXYZ.y = std::stof(nextToken);
        curMesh.orientationXYZ.y = glm::radians(std::stof(nextToken));
        theFile >> nextToken;                                               //z orientation value
        //curMesh.orientationXYZ.z = std::stof(nextToken);
        curMesh.orientationXYZ.z = glm::radians(std::stof(nextToken));
        //Next is the scale to multiply the model by
        theFile >> nextToken;                                               //scale multiplier
        curMesh.scale = std::stof(nextToken);
        //Next 3 are the r, g, b values for the model
        curMesh.bOverrideColour = true;                                     //always true for this
        theFile >> nextToken;                                               //RGB red value
        curMesh.vertexColourOverride.r = std::stof(nextToken) / 255.0f;     //convert to nice shader value (between 0 and 1)
        theFile >> nextToken;                                               //RGB green value
        curMesh.vertexColourOverride.g = std::stof(nextToken) / 255.0f;     //convert to nice shader value (between 0 and 1)
        theFile >> nextToken;                                               //RGB blue value
        curMesh.vertexColourOverride.b = std::stof(nextToken) / 255.0f;     //convert to nice shader value (between 0 and 1)

        g_vecMeshes.push_back(curMesh);     //push the model onto our vector of meshes
        ss.str("");                         //reset the stringstream
    } //end of while
    theFile.close();
    return true;
} //end of loadWorldFile


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    float cameraSpeed = 0.1f;       //multiply by deltaTime to make this system specific

    //Basic camera controls
    if (key == GLFW_KEY_A) {
        //Go Left
        cameraEye -= glm::normalize(glm::cross(cameraTarget, upVector)) * cameraSpeed;
    }
    if (key == GLFW_KEY_D)
    {   //Go right
        cameraEye += glm::normalize(glm::cross(cameraTarget, upVector)) * cameraSpeed;
    }
    if (key == GLFW_KEY_S)
    {
        //Go back
        cameraEye.x -= cameraTarget.x * cameraSpeed;    //These move the camera in the same direction as the camera target without changing the height
        cameraEye.z -= cameraTarget.z * cameraSpeed;
    }
    if (key == GLFW_KEY_W)
    {
        //Go forward
        cameraEye.x += cameraTarget.x * cameraSpeed;    //These move the camera in the same direction as the camera target without changing the height
        cameraEye.z += cameraTarget.z * cameraSpeed;
    }
    if (key == GLFW_KEY_Q)
    {    //Go "Down"
        cameraEye -= cameraSpeed * upVector;
    }
    if (key == GLFW_KEY_E)
    {     //Go "Up"
        cameraEye += cameraSpeed * upVector;
    }
    std::cout << "Camera: "
        << cameraEye.x << ", "
        << cameraEye.y << ", "
        << cameraEye.z << std::endl;

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        bWireFrame = !bWireFrame;
    }
} //end of key_callback

//Figured out the math for how to do this from https://learnopengl.com/Getting-started/Camera and http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
//Using the mouse position we calculate the direction that the camera will be facing
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //if it's the start of the program this smooths out a potentially glitchy jump
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    //find the offset of where the mouse positions have moved
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    //multiply by sensitivity so that it's not potentially crazy fast
    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    
    yaw += xOffset;         // The yaw is the rotation around the camera's y-axis (so we want to add the xOffset to it)
    pitch += yOffset;       // The pitch is the rotation around the camera's x-axis (so we want to add the yOffset to it)
    //This limits the pitch so that we can't just spin the camera under/over itself
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    //calculations for the new direction based on the mouse movements
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraTarget = glm::normalize(direction);
} //fly camera