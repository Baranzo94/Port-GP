#version 150

in vec3 vertexPosition;
in vec3 vertexNormals;
in vec2 vertexTexCoords;

out vec3 vertexNormalOut;
out vec3 cameraDirectionOut;
out vec3 lightDirectionOut;
out vec2 texCoordsOut;

uniform mat4 MVP;
uniform mat4 Model;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;
void main()
{
	
	
	vec3 vertexNormalModel = normalize(Model*vec4(vertexNormals,0.0f)).xyz;
	vec3 worldPos = (Model*vec4(vertexPosition, 1.0)).xyz;
	vec3 cameraDir = normalize(cameraPosition - worldPos);

	lightDirectionOut = normalize(lightDirection);
	vertexNormalOut = normalize(vertexNormalModel);
	cameraDirectionOut = normalize(cameraDir);

	texCoordsOut = vertexTexCoords;
	gl_Position = MVP * vec4(vertexPosition, 1.0); 

	
}