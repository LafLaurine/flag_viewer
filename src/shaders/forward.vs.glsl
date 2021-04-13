#version 330

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

out vec3 vViewSpacePosition;
out vec3 vViewSpaceNormal;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

void main()
{
    vViewSpacePosition = vec3(uModelViewMatrix * vec4(aPosition, 1));
	vViewSpaceNormal = transpose(inverse(mat3(uModelViewMatrix))) * normalize(vViewSpaceNormal);
    gl_Position =  uModelViewProjMatrix * vec4(aPosition, 1);
}