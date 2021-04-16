#version 330 core

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;

uniform vec3 uLightIntensity;
uniform vec3 uColor;
uniform int wireframe;

out vec3 color;

void main()
{
	vec3 normal = normalize(vViewSpaceNormal);
	if(wireframe == 0)
		color = uColor * uLightIntensity;
	else if (wireframe == 1)
		color = vec3(0.0, 0.0, 1.0) * uLightIntensity;
};