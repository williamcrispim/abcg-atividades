#version 300 es

layout(location = 0) in vec3 inPosition;

uniform vec4 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec4 fragColor;

void main() {
  // Transforma a posição do vértice para o espaço da câmera (view space)
  vec4 posEyeSpace = viewMatrix * modelMatrix * vec4(inPosition, 1);

  // Calcula a intensidade da cor com base na profundidade do vértice
  float i = 1.0 - (-posEyeSpace.z / 100.0);

  // Combina a intensidade calculada com a cor recebida como atributo
  fragColor = vec4(i, i, i, 1) * color;

  // Calcula a posição final do vértice no espaço de projeção
  gl_Position = projMatrix * posEyeSpace;
}
