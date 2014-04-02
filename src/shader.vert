attribute vec2 aVertexPosition;
attribute vec2 aTextureCoord;

uniform vec4 uRTVec;
uniform mat4 uPMatrix;
varying vec2 texture_coordinate;

void main(void)
{
	vec2 transl = vec2(uRTVec[2], uRTVec[3]);
	float sinrot = uRTVec[0];
	float cosrot = uRTVec[1];

	mat2 rotation = mat2(cosrot, sinrot, -sinrot, cosrot);
	gl_Position = uPMatrix * vec4(rotation*aVertexPosition+transl, 0.0, 1.0);
	texture_coordinate = aTextureCoord;
}
