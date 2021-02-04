//#version 330 core

// attributes
// values are linearly interpolated between the vertices

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 Uv;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat3 normalMatrix;

uniform mat3 uvTransform;

out vec2 vUv;

out vec3 vViewPosition;
out mat4 modelView;

out float fogDepth;

void main()
{
	vUv = ( uvTransform * vec3( Uv, 1 ) ).xy;

	modelView = view * model;
	
	gl_Position = projection * modelView * vec4(Position, 1.0);

	vec4 mvPosition = modelView * vec4(vec3(Position), 1.0);

	vViewPosition = - mvPosition.xyz;

	fogDepth = -mvPosition.z;
}