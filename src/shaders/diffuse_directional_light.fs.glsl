#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

uniform vec3 uLightDirection;
uniform vec3 uLightIntensity;
uniform int isFloor;

out vec3 fColor;

void main()
{
  if(isFloor != 0){
    fColor = vec3(1.0f, 0.9f, 0.8f);
	}
  vec3 viewSpaceNormal=normalize(vViewSpaceNormal);
  fColor=vec3(1./3.14) * uLightIntensity * dot(viewSpaceNormal, uLightDirection);
}
