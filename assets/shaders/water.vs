precision mediump float;
attribute vec3 vPosition;
attribute vec3 vNormal;
varying vec2 texcoord;
varying vec2 position;
varying vec3 normal;
varying vec4 color;
uniform mat4 matW;
uniform mat4 matV;
uniform mat4 matP;

const float airToWater = 1.0 / 1.33333;
const vec3 camDir = vec3(0, 0, -1);
const float waterDepth = 0.2;

void main() {
	mat4 matWV = matV * matW;
	gl_Position = matWV * vec4(vPosition.xyz, 1) * matP;
	normal = normalize(vNormal) * mat3(matW);

	vec3 rayDir = refract(camDir, normal, airToWater);

	vec3 bottom = rayDir * waterDepth / rayDir.z;

	position = vPosition.xz;
	texcoord = (vPosition.xz + bottom.xy) * vec2(0.5, -0.5) + vec2(0.5, 0.5);

	float mask = dot(normal, vec3(0, 0, 1));
	color = vec4(1, 1, 1, 1.0 - mask);
}
