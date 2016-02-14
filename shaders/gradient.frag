uniform bool timeWarp;
uniform float screenHeight;
uniform float faderTime;
uniform float faderDuration;

void main()
{
	gl_FragColor = gl_Color;
	if (timeWarp)
	{
		if (faderTime >= faderDuration)
		{
			gl_FragColor.a = .000009*((gl_FragCoord.y-(screenHeight/2))*(gl_FragCoord.y-(screenHeight/2)))-1;
		}
		else
		{
			gl_FragColor.a = faderTime*(1/faderDuration)*.000009*((gl_FragCoord.y-(screenHeight/2))*(gl_FragCoord.y-(screenHeight/2)))-1;
		}
	}
	else
	{
		if (faderTime >= faderDuration)
		{
			gl_FragColor.a = 0;
		}
		else
		{
			gl_FragColor.a = (faderDuration-faderTime)*(1/faderDuration)*.000009*((gl_FragCoord.y-(screenHeight/2))*(gl_FragCoord.y-(screenHeight/2)))-1;
		}
	}
	
}