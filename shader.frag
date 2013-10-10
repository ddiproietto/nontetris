uniform sampler2D myTexture;

varying vec2 texture_coordinate;

void main(void)
{
	gl_FragColor = texture2D(myTexture, texture_coordinate);
}

