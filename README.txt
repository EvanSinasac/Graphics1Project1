Evan Sinasac - 1081418
INFO6028 Graphics (Project 1)

Program can be run in Visual Studio 2019 in Debug and Release modes for x64.  I have not done the CMake stuff for x86 for the GLFW stuff, so it will not run in x86.
The SOLUTION_DIR(solution directory) is the folder that this README is located in.

Program Instructions:
WASD	- Move Camera around the scene (as if it's a player)
Q	- Lower Camera
E	- Raise Camera
Mouse	- Move Camera (as if you are holding a camera and moving the lens)
Space	- Switch between Wire-Frame and Solid fill
Esc	- Close the program

PLY Models:
I have adjusted/reverted the shaders and VAOManager to a form that loads PLY models with vertices and normals, so in order to include any new models they need to be exported without colour or textures in the model, just vertices and normals.  

After doing so, the model can be placed in SOLUTION_DIR/common/assets/models, then add the name of the model (including the extension) in the modelsToLoad.txt
This will allow the program to load the models into the VAOManager

Then, at SOLUTION_DIR/common/assets there is a text file named worldFile.txt
In this file is the description of the scene.  To add something to the world, add the new object between the Camera line and the "end".  After Camera is the x,y,z position of the camera.
(The program stops reading the file at end).  This file describes everything about the object, so add the model name (including extension), then the x,y,z coordinates for its position, then the x,y,z values for rotation [in degrees(which is easier for humans to think in usually)], then the scale of the object, and finally the RGB values(0-255) to determine the colour of the object.

The models I used are from Runemark Studio's Dark Fantasy Kit (https://assetstore.unity.com/packages/3d/environments/fantasy/dark-fantasy-kit-123894), and Creepy Cat's 3D Scifi Kit Vol 3 (https://assetstore.unity.com/packages/3d/environments/sci-fi/3d-scifi-kit-vol-3-121447), as well as our bunny model. 
 
Camera:
I was having issues figuring out how to make a fly camera so, I went to Google.  The main resources I used to figure out how to make this camera were: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/ and https://learnopengl.com/Getting-started/Camera (mainly the learnopengl).  Essentially, after setting up the mouse_callback (which is how GLFW gets the X and Y information from mouse movement), we figure out the offset from where the mouse last was, use this information to change the yaw and pitch of where the camera is looking, and then use SOH CAH TOA to calculate the exact XYZ position of the camera target in world space(again, most of the math is from the learnopengl source).  There is some additional logic to change the inital yaw value from lines [164-211] so that the inital target is always facing the origin along the z-axis.

Video:
https://youtu.be/GyyuJQqbV-U