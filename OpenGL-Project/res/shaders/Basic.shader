#shader vertex
#version 330 core

// refers to the lyaout defined in our vertx attribute
layout(location = 0) in vec4 position;

void main()
{
	//https://docs.gl/sl4/gl_Position
	gl_Position = position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
	color = u_Color;
};