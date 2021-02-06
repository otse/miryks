layout (location = 0) in vec3 Position;

out vec2 uv;

void main() 
{
	uv = vec2(Position.xy) * 2.0;

	gl_Position = vec4(Position, 1.0);
}