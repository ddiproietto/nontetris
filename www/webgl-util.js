function _compatRequestAnimationFrame(callback)
{
	//Ripped from https://developer.mozilla.org/en-US/docs/Web/API/window.requestAnimationFrame
	var requestAnimationFrame = window.requestAnimationFrame || window.mozRequestAnimationFrame ||
		window.webkitRequestAnimationFrame || window.msRequestAnimationFrame;
	requestAnimationFrame(callback);
}
function __Z16myteximage2DnullPN6client21WebGLRenderingContextEjiiiiijj(gl,p1,p2,p3,p4,p5,p6,p7,p8)
{
	gl.texImage2D(p1,p2,p3,p4,p5,p6,p7,p8,null);
}
