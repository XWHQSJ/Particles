#version 330 core
in vec4 vColor;
out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    // Point sprite: use gl_PointCoord as texture coordinate
    vec4 texColor = texture(uTexture, gl_PointCoord);
    // Additive blend with particle color
    FragColor = texColor * vColor;
}
