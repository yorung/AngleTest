attribute vec2 vPosition;
varying vec2 texcoord;
varying vec2 position;

void main() {
	gl_Position = vec4(vPosition.xy, 0, 1);
	texcoord.x = vPosition.x * 0.5 + 0.5;
	texcoord.y = vPosition.y * 0.5 + 0.5;
	position = vPosition;
}
