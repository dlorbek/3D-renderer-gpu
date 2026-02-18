#version 330 core

layout (location = 0) out uint outID;

uniform uint colorID;

void main()
{
    outID = colorID;
}
