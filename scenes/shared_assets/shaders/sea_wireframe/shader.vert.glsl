#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;

out struct vertex_data
{
    vec4 position;
    vec4 normal;
} vertex;

// model transformation
uniform float time = 1.0;					     // user defined time

void main()
{
    vec4 position_transformed = position;
    float abs = (position_transformed[0] + position_transformed[1])/80 + 0.5;
    position_transformed[2] = 0.2f*cos(3.1415926 * (16*abs+time/2.0f)) + position_transformed[2];

    vertex.position = position_transformed;
    vertex.normal = vec4(0.2*3.1415926*sin(3.1415926 * (16*abs+time/2.0f)), 0.2f*3.1415926*sin(3.1415926 * (16*abs+time/2.0f)), 2.0, 1.0)/3 + normal;

    gl_Position = position_transformed;
}
