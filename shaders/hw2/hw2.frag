#version 330

out vec4 fragColor;

uniform float inputTime;

void main() 
{
    vec4 finalColor = vec4(1);

    // Insert your code for "Slightly-More Advanced Shaders" here.
    float rotation = inputTime * 0.001 * 3.14;
    float phase = 0.67 * 3.14;

    finalColor.x = sin(rotation) * 0.5 + 0.5;
    finalColor.y = sin(rotation + phase) * 0.5 + 0.5;
    finalColor.z = sin(rotation + phase * 2) * 0.5 + 0.5;

    fragColor = finalColor;
}
