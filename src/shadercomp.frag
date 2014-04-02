uniform float uComp;
uniform vec3 uColor;

varying vec2 texture_coordinate;

void main(void)
{
	vec3 invcolor = vec3(1.0, 1.0, 1.0) - uColor;
	float onoff = step(texture_coordinate.x, uComp);
	gl_FragColor = vec4(1.0 - onoff*invcolor.r, 1.0 - onoff*invcolor.g, 1.0-onoff*invcolor.b, 1.0);
}
