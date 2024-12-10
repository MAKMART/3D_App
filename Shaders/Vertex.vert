//#version 330 core
//
//layout(location = 0) in vec3 aPos;
//layout(location = 1) in vec3 aNormal;
//layout(location = 2) in vec2 aTexCoords;
//
//out vec2 TexCoords;
//out vec3 FragPos;
//out vec3 Normal;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//void main() {
//    FragPos = vec3(model * vec4(aPos, 1.0));
//    Normal = mat3(transpose(inverse(model))) * aNormal;
//    TexCoords = aTexCoords;
//
//    gl_Position = projection * view * vec4(FragPos, 1.0);
//}
//

#version 330 core

// Vertex attributes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in ivec4 aBoneIDs; // Bone IDs affecting the vertex
layout(location = 4) in vec4 aWeights; // Weights of the corresponding bones

// Outputs to fragment shader
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 uBoneTransforms[100]; // Bone transformation matrices

void main() {
    // Skinning: Calculate transformed position based on bone weights
    vec4 totalPosition = vec4(0.0);
    for (int i = 0; i < 4; i++) {
        if (aWeights[i] > 0.0) {
            totalPosition += uBoneTransforms[aBoneIDs[i]] * vec4(aPos, 1.0) * aWeights[i];
        }
    }

    // Fallback for vertices not affected by bones (shouldn't happen in practice)
    if (length(totalPosition) == 0.0) {
        totalPosition = vec4(aPos, 1.0);
    }

    // Transform normal using the model matrix (ignoring bone influences for normals)
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Transform position and pass texture coordinates
    FragPos = vec3(model * totalPosition);
    TexCoords = aTexCoords;

    // Final vertex position in clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
