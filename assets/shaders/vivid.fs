precision mediump float;
varying vec2 position;
varying vec2 texcoord;
uniform sampler2D sampler;
void main() {
	vec2 coord = vec2(texcoord.x, texcoord.y);
	gl_FragColor = vec4(texture2D(sampler, coord).xyz * 2.0, 1.0);
}
