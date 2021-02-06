//#version 330 core

#define ONE_SKYRIM_UNIT_IN_CM 1.428

in vec2 uv;
out vec4 FragColor;

uniform sampler2D texture;
uniform float time;

void main()
{
	vec4 tex = texture2D(texture, uv);

	//vec4 diffuseColor = vec4(color, opacity);

	FragColor = tex;
	FragColor.rgba += vec4(1, 0, 1, 1) * 0.2;
	FragColor.a = 0.5;

	//FragColor += tex * 0.5;
}