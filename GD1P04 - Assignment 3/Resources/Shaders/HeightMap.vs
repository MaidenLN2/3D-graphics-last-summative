#version 460 core

// input from shader
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;

// inputs from uniforms
uniform mat4 PVM;
uniform sampler2D ImageTexture0;

// output
out vec2 FragTexCoords;

void main()
{
    vec3 heightPosition = Position;
    vec4 textureData = texture(ImageTexture0, TexCoords);
    heightPosition.y = heightPosition.y + (textureData.x * 25);

    gl_Position = PVM * vec4(heightPosition, 1.0f);
    
    FragTexCoords = TexCoords;
}