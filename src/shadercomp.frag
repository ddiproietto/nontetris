uniform float uComp;

varying vec2 texture_coordinate;

void main(void)
{
	float intensity = (uComp*0.8) + 0.2*step(1.0, uComp);
	float onoff = step(texture_coordinate.x, uComp);
	float col = 1.0 - intensity*onoff;
	gl_FragColor = vec4(col, col, col, 1.0);
}

