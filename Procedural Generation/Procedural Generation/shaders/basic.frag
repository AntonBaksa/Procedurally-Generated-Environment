#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;

uniform vec3 lightDirection;
uniform vec3 viewPosition;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

float maxHeight = 4.0f;

uniform vec3 lowColor;
uniform vec3 highColor;

out vec4 FragColor; // The color produced for this fragment.

void main()
{
    // Interpolation can change a normal's length, so normalize per fragment.
    vec3 N = normalize(worldNormal);
    vec3 L = normalize(lightDirection);

    float diffuse = max(dot(N, L), 0.0);

    vec3 V = normalize(viewPosition - worldPosition);
    vec3 H = normalize(L + V);

    // Only a surface facing the light may receive a specular highlight.
    float specular = 0.0;
    if (diffuse > 0.0)
    {
        specular = pow(max(dot(N, H), 0.0), shininess);
    }

    float heightT = clamp(worldPosition.y / maxHeight, 0.0, 1.0);
    vec3 terrainColor = mix(lowColor, highColor, heightT);

    vec3 ambientColor = ambientStrength * terrainColor;
    vec3 diffuseColor = diffuse * terrainColor;
    vec3 specularColor = specularStrength * specular * lowColor;

    vec3 color = ambientColor + diffuseColor + specularColor;
    FragColor = vec4(color, 1.0);

}