#version 330

layout(location = 0) in vec4 vertexPosition;

uniform float inputTime;

void main()
{
    vec4 modifiedVertexPosition = vertexPosition;

    // Insert your code for "Slightly-More Advanced Shaders" here.
    float offset = sin(inputTime * 0.001 * 3.14);
    modifiedVertexPosition.y = modifiedVertexPosition.y + offset;

    gl_Position = modifiedVertexPosition;
}
