attribute vec2 aVertexPosition;
attribute vec2 aTextureCoord;

varying vec2 texture_coordinate;

void main(void)
{
	gl_Position = vec4(aVertexPosition[0],aVertexPosition[1], 0.0, 1.0);
	texture_coordinate = aTextureCoord;//(aVertexPosition+vec2(1.0,1.0))/2.0;
}
