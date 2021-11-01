#version 430 core

const int MaxLights = 10;

in vec2 outTexture;
in vec3 transformed_normals;
in vec3 light_direction[MaxLights];
in vec3 viewer_vector;
in vec3 frag_pos;

out vec4 fragColor;

struct LightProperties 
{
	int isEnabled;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular; 
    float constant;
    float linear;
    float quadratic;      
};

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	vec3 material_specular;
	float shininess;
	float transparency;
};

uniform LightProperties Lights[MaxLights];
uniform Material material;

void main()
{
    vec3 calc_color = vec3(0.0, 0.0, 0.0);
	vec3 normalized_transformed_normals = normalize(transformed_normals);
    vec3 normalized_viewer_vector = normalize(viewer_vector);
    
	for (int lightIndex = 0; lightIndex < MaxLights; ++lightIndex) 
	{
		if (Lights[lightIndex].isEnabled == 1)
		{
			//Ambient Light component
			vec3 normalized_light_direction = normalize(light_direction[lightIndex]);
            vec3 ambient = Lights[lightIndex].ambient * vec3(texture(material.texture_diffuse1, outTexture));


			//Diffuse Light component
            float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);
            vec3 diffuse = Lights[lightIndex].diffuse * tn_dot_ld * vec3(texture(material.texture_diffuse1, outTexture));
            
			//Specular Light component
			vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);
            float spec = pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), material.shininess);
			vec3 specular = Lights[lightIndex].specular * spec * material.material_specular * vec3(texture(material.texture_specular1, outTexture));
                    
		    //Attenuation
			float distance = length(Lights[lightIndex].position - frag_pos);
			float attenuation = 1.0f / (Lights[lightIndex].constant + Lights[lightIndex].linear * distance + Lights[lightIndex].quadratic * (distance * distance));
		    calc_color *= attenuation;
			
			calc_color = calc_color + ambient + diffuse + specular;
		}	
	}

	vec4 trans = texture(material.texture_diffuse1, outTexture);	
	fragColor = vec4(calc_color, trans.a);
}