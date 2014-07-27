precision mediump float;
varying vec2 position;
varying vec2 texcoord;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform float time;
void main() {
	vec2 coord = vec2(texcoord.x, texcoord.y);
	vec4 c1 = texture2D(sampler1, coord);
	vec4 c2 = texture2D(sampler2, coord);
	gl_FragColor = mix(c1, c2, 0.5) * 2.0;
}
