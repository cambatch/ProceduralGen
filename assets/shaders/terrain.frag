#version 460 core

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

struct Material {
    sampler2D diffuse;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material material;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lightDir = normalize(-dirLight.direction);

    // Directional lighting
    float diff = max(dot(norm, lightDir), 0.0);
    float ambientMultiplier = 1.2;
    float diffuseMultiplier = 1.6;

    float ao = 0.5 + 0.5 * dot(norm, vec3(0.0, 1.0, 0.0));

    vec3 ambient = dirLight.ambient * vec3(texture(material.diffuse, TexCoord)) * ao * ambientMultiplier;
    vec3 diffuse = dirLight.diffuse * diff * vec3(texture(material.diffuse, TexCoord)) * diffuseMultiplier;

    FragColor = vec4(ambient + diffuse, 1.0);
}
