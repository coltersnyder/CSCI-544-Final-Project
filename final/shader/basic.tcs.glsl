#version 410 core

layout (vertices=4) out;

// varying input from vertex shader
in vec2 texCoord[];
// varying output to evaluation shader;
out vec2 TextureCoord[];

void main(){
    // Pass through attributes
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = texCoord[gl_InvocationID];

    // Invocation 0 controls levels for all patches
    if(gl_InvocationID == 0){
        gl_TessLevelOuter[0] = 16;
        gl_TessLevelOuter[1] = 16;
        gl_TessLevelOuter[2] = 16;
        gl_TessLevelOuter[3] = 16;

        gl_TessLevelInner[0] = 16;
        gl_TessLevelInner[1] = 16;
    }
}