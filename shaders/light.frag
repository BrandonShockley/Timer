uniform vec2 worldLocation;
uniform vec2 viewLocation;
uniform float zoom;
uniform float windowHeight;
uniform float time;
uniform float radius;
uniform float tileHeight;

void main()
{
	if (((viewLocation.y + (windowHeight - gl_FragCoord.y) * zoom) - worldLocation.y) + tileHeight / 2 > 0)
		gl_FragColor = vec4(1.0, .1, .1, (-(sqrt((worldLocation.x - (viewLocation.x + gl_FragCoord.x * zoom))*
													(worldLocation.x - (viewLocation.x + gl_FragCoord.x * zoom))+
													((viewLocation.y + (windowHeight - gl_FragCoord.y) * zoom) - worldLocation.y)*
													((viewLocation.y + (windowHeight - gl_FragCoord.y) * zoom) - worldLocation.y)))*
													(sqrt((worldLocation.x - (viewLocation.x + gl_FragCoord.x * zoom))*
													(worldLocation.x - (viewLocation.x + gl_FragCoord.x * zoom))+
													((viewLocation.y + (windowHeight - gl_FragCoord.y) * zoom) - worldLocation.y)*
													((viewLocation.y + (windowHeight - gl_FragCoord.y) * zoom) - worldLocation.y))) * (2 + sin(time / 200))/radius + 230)/255);
}