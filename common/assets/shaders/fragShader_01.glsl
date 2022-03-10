//Fragment shader
#version 420

in vec4 fColour;	
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;

void main()
{
	//This is the pixel colour on the screen.
	//Just one pixel though
	
    vec4 lightLocation = vec4(-5.0f, 15.0f, 5.0f, 1.0f);
	float vertexToLightDistance = distance(fVertWorldLocation, lightLocation);
	gl_FragColor = (fColour / vertexToLightDistance) * 15.0f;
	
	gl_FragColor = fColour;	//Was: vec4(color, 1.0f)
	//gl_FragColor = fNormal;
};
