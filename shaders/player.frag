#version 130

uniform sampler2D texture;
uniform bool timeWarp;

void main()
{
	if (timeWarp)
	{
		vec4 pixel = texture2D(texture, gl_TexCoord[0].xy) * 3.0;
		gl_FragColor = gl_Color * pixel * vec4(74.0/255.0,210.0/255.0,255/255,.75);
	}
	else
	{
		vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
		gl_FragColor = gl_Color * pixel;
	}
}