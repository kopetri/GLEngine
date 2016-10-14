#version 330

struct LightObject {
    vec3 position;
    vec4 color;
};


uniform int lightCounter = 3;
uniform LightObject lightArray[3];
uniform samplerCube cubemap;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform float roughness;

out vec4 colorOutput;

in vec3 worldPos;
in vec2 TexCoords;
in vec3 normal;
in vec3 baseColor;

float PI  = 3.14159265359f;


vec3 FresnelSchlick(float NdotV, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);
}


vec3 FresnelSchlick(float NdotV, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - NdotV, 5.0);
}


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;

    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    return (alpha2) / (PI * (NdotH2 * (alpha2 - 1.0) + 1.0) * (NdotH2 * (alpha2 - 1.0) + 1.0));
}


float GeometryAttenuationGGXSmith(float NdotL, float NdotV, float roughness)
{
    float NdotL2 = NdotL * NdotL;
    float NdotV2 = NdotV * NdotV;
    float kRough2 = roughness * roughness;

    float ggxL = (2.0 * NdotL) / (NdotL + sqrt(NdotL2 + kRough2 * (1.0 - NdotL2)));
    float ggxV = (2.0 * NdotV) / (NdotV + sqrt(NdotV2 + kRough2 * (1.0 - NdotV2)));

    return ggxL * ggxV;
}


vec3 colorLinear(vec3 colorVector) {
  vec3 linearColor = pow(colorVector.rgb, vec3(2.2f));

  return vec3(linearColor);
}


vec3 colorSRGB(vec3 colorVector) {
  vec3 srgbColor = pow(colorVector.rgb, vec3(1.0f / 2.2f));

  return vec3(srgbColor);
}



void main()
{
    vec3 V = normalize(viewPos - worldPos);
    vec3 N = normalize(normal);
    vec3 R = normalize(-reflect(V, N));

    vec3 color = vec3(0.0f);
    vec3 albedo = vec3(1.0f);
    vec3 specular = vec3(0.0f);
    vec3 envMap = texture(cubemap, R).rgb;

    for (int i = 0; i < lightCounter; i++)
    {
        vec3 L = normalize(lightArray[i].position - worldPos);
        vec3 lightColor = colorLinear(lightArray[i].color.rgb);
        float distance = length(lightArray[i].position - worldPos);
        float attenuation = 1.0 / (distance * distance);


        // Lambert diffuse computation
        float lambertDiffuse = max(dot(N, L), 0.0f);
        vec3 diffuse = lambertDiffuse * albedo * lightColor;


        // Fresnel (Schlick) computation (F term)
        // F0 = 0.04 --> dielectric UE4
        // F0 = 0.658 --> Glass
        vec3 F0 = vec3(0.658f);
        vec3 F = FresnelSchlick(max(dot(N, V), 0.0), F0);
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;


        // Distribution (GGX) computation (D term)
        vec3 H = normalize(L + V);
        float D = DistributionGGX(N, H, roughness);


        // Geometry attenuation (GGX-Smith) computation (G term)
        float NdotL = abs(dot(N, L));
        float NdotV = abs(dot(N, V));
        float G = GeometryAttenuationGGXSmith(NdotL, NdotV, roughness);


        // Specular component computation
        specular = (F * G * D) / (4 * NdotL * NdotV);


        // Attenuation computation
        diffuse *= attenuation;
        specular *= attenuation;

//        color += albedo * lambertDiffuse * lightColor * kD + specular;
        color += lambertDiffuse * envMap * NdotL * (kD + specular * (1.0f - kD));
    }

    color = pow(color, vec3(1.0/2.2));
    colorOutput = vec4(color, 1.0);
}
