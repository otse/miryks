layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 Uv;

out vec2 uv;

void main() 
{
	vec2 Calc = Position.xy + vec2(1, 1);
	uv = Uv;

	gl_Position = vec4(Position.x, Position.y, 0, 1.0);
}