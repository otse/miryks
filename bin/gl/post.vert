#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 Position;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat3 normalMatrix;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main(){
	gl_Position = osg_ProjectionMatrix * osg_ModelViewMatrix * vec4( position, 1.0 );
}
