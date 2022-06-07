#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model *position;
    TexCoord = aTexCoord;
};

#shader fragment
#version 330 core

out vec4 color;

in vec2 TexCoord;
uniform sampler2D ourTexture;

void main()
{
    color = texture(ourTexture, TexCoord);
};
