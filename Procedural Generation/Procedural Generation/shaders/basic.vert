#version 330 core

// The vertex shader processes each vertex's local-space attributes.
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

// These matrices are created in C++ and uploaded to the GPU as uniforms.
// A uniform keeps the same value for every vertex processed by this draw call.
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec3 worldPosition;
out vec3 worldNormal;

void main()
{
    // Position path: local -> world -> view -> clip.
    vec4 world = model * vec4(aPosition, 1.0);
    worldPosition = world.xyz;
    gl_Position = projection * view * world;

    // Normal path: local direction -> world direction. Translation must not
    // affect a direction, so normals use a mat3 normal matrix rather than model.
    worldNormal = normalMatrix * aNormal;
}