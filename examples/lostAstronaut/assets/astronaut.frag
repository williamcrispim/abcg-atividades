#version 300 es
precision highp float;

in vec3 fragColor;
out vec4 outColor;

void main() {
    vec3 brazilGreen = vec3(0.0, 0.5, 0.0);
    vec3 brazilYellow = vec3(1.0, 1.0, 0.0);
    vec3 brazilBlue = vec3(0.0, 0.0, 1.0);

    // Calcula a posição relativa no gradiente vertical com base na coordenada Y do fragmento
    float t = gl_FragCoord.y / 600.0;

    // Mistura as cores da bandeira do Brasil no gradiente
    vec3 gradientColor = mix(brazilGreen, brazilYellow, t);

    // Mistura a cor resultante com a cor azul
    gradientColor = mix(gradientColor, brazilBlue, t);

    // Define a cor final do fragmento
    outColor = vec4(gradientColor, 1.0);
}
