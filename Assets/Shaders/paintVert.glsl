#version 440 core

uniform mat4 paintSpaceMatrix;
uniform mat4 modelMatrix;
uniform vec3 paintBallDirection;

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 UV;
layout(location = 3) in vec3 normal;

out Vertex
{
	out vec2 interp_UV;
	out vec3 vNormal;
	out vec3 paintDir;
	out vec4 posPaintSpace;
} OUT;

void main()
{
    
    OUT.interp_UV = UV;
    OUT.vNormal = normalize(normal);
    OUT.paintDir = normalize(paintBallDirection);


    gl_Position = paintSpaceMatrix * modelMatrix * vec4(position, 1.0f);
    

    vec4 mPosition = modelMatrix * vec4( position, 1.0 );
    OUT.posPaintSpace = paintSpaceMatrix * mPosition;
}