precision mediump float;
attribute vec2 aVertexPosition;

uniform mat4 uPMatrix;
uniform vec4 uRTVec;

varying vec2 texture_coordinate;

void main(void)
{
	vec2 transl = vec2(uRTVec[2], uRTVec[3]);
	float sinrot = uRTVec[0];
	float cosrot = uRTVec[1];

	mat2 rotation = mat2(cosrot, sinrot, -sinrot, cosrot);
	gl_Position = uPMatrix * vec4(rotation*vec2(aVertexPosition[0],aVertexPosition[1])+transl, 0.0, 1.0);
	texture_coordinate = (aVertexPosition-vec2(2.0, 2.0))/4.0;
}
