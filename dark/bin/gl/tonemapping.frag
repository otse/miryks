//#version 330 core

#define ONE_SKYRIM_UNIT_IN_CM 1.428

in vec2 uv;
out vec4 FragColor;

float exposure = 1.0;
uniform sampler2D texture;
uniform float time;

// https://learnopengl.com/Advanced-Lighting/HDR

void main()
{
	const float gamma = 1.0;
	vec3 hdrColor = texture2D(texture, uv).rgb;

	// Reinhard tone mapping
	//vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

	// exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	// Gamma correction 
	mapped = pow(mapped, vec3(1.0 / gamma));

	vec4 tex = texture2D(texture, uv);

	//vec4 diffuseColor = vec4(color, opacity);

	FragColor = tex;
	FragColor = vec4(mapped, 1.0);
}