#version 400

in vec3 vertex_position;
uniform mat4 view, proj;

void main() {
	gl_Position = proj * view * vec4( vertex_position, 1.0 );
}
