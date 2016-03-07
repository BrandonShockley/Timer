uniform float screenWidth;
uniform float time;
uniform bool restarting;
uniform bool restartToggle;

void main()
{
	if (restarting)
	{
	gl_FragColor = gl_Color;
	if (time < 1.f)
		gl_FragColor.a = -((gl_FragCoord.x - screenWidth / 2) * (gl_FragCoord.x - screenWidth / 2))/(10000000*(time*time*time*time*
																																						time*time*time*time*
																																						time*time*time*time*
																																						time*time*time*time)) + 1;
	else
	{
		time = time - 1;
		gl_FragColor.a = ((gl_FragCoord.x - screenWidth / 2) * (gl_FragCoord.x - screenWidth / 2))/(100000*(time*time*time*time*
																																						time*time*time*time*
																																						time*time*time*time*
																																						time*time*time*time));
	}
	}
	else if (time > 1.f && restartToggle)
	{
	gl_FragColor = gl_Color;
	time = time - 1;
		gl_FragColor.a = ((gl_FragCoord.x - screenWidth / 2) * (gl_FragCoord.x - screenWidth / 2))/(1000000*(time*time*time*time*
																																						time*time*time*time*
																																						time*time*time*time*
																																						time*time*time*time));
	}
}