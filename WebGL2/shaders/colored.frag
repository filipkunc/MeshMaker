#version 300 es
precision highp float;

// Fragment shader for colored points and lines
// Simply outputs the interpolated vertex color

in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
