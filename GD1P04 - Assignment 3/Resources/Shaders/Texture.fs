#version 460 core

in vec3 FragColour;
in vec2 FragTexCoords;

// uniform
uniform sampler2D ImageTexture0;

out vec4 FinalColour;

void main()
{
    FinalColour = texture(ImageTexture0, FragTexCoords);
}