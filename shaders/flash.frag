#version 130

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
		float tim = time - 1;
		gl_FragColor.a = ((gl_FragCoord.x - screenWidth / 2) * (gl_FragCoord.x - screenWidth / 2))/(100000*(tim*tim*tim*tim*
																																						tim*tim*tim*tim*
																																						tim*tim*tim*tim*
																																						tim*tim*tim*tim));
	}
	}
	else if (time > 1.f && restartToggle)
	{
	gl_FragColor = gl_Color;
	float tim = time - 1;
		gl_FragColor.a = ((gl_FragCoord.x - screenWidth / 2) * (gl_FragCoord.x - screenWidth / 2))/(1000000*(tim*tim*tim*tim*
																																						tim*tim*tim*tim*
																																						tim*tim*tim*tim*
																																						tim*tim*tim*tim));
	}
}