// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE.txt for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Attribute 0 is position
layout (location = 0) in vec2 inPosition;

// Attribute 1 is texture coordinate
layout(location = 1) in vec2 inTexCoord;

uniform mat4 uWorldTransform;
uniform mat4 uOrthoProj;
uniform vec3 uColor;
uniform vec2 uCameraPos;

// (u0, v0, u1, v1) for current sprite frame
uniform vec4 uTexRect;

// Any vertex outputs (other than position)
out vec2 fragTexCoord;

void main()
{
	// 1. Transform to world space
	vec4 worldPos = uWorldTransform * vec4(inPosition, 0.0, 1.0);

	// 2. Convert to view space (world - camera)
	vec2 viewPos = worldPos.xy - uCameraPos;

	// 3. Apply ortho projection to view space coordinates
	gl_Position = uOrthoProj * vec4(viewPos, 0.0, 1.0);

	// Map texture coordinates: scale by width/height and offset by x/y
	fragTexCoord = inTexCoord * uTexRect.zw + uTexRect.xy;
}