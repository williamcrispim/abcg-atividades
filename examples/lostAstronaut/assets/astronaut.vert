#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

out vec3 fragNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
    // Calcula a posição do vértice no espaço de recorte
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(inPosition, 1.0);

    // Calcula a normal do fragmento no espaço da câmera
    // mat3(transpose(inverse(modelMatrix))) é usado para transformar a normal
    // do espaço do modelo para o espaço da câmera, mantendo a orientação correta
    fragNormal = mat3(transpose(inverse(modelMatrix))) * inNormal;
}
