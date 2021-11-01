#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexture0;
layout(location = 3) in ivec4 vBoneIds1;  
layout(location = 4) in vec4 vWeights1;
layout(location = 5) in ivec4 vBoneIds2;  
layout(location = 6) in vec4 vWeights2;

layout(location = 7) in vec3 vPosition_Morph_0;
layout(location = 8) in vec3 vPosition_Morph_1;
layout(location = 9) in vec3 vPosition_Morph_2;
layout(location = 10) in vec3 vPosition_Morph_3;
layout(location = 11) in vec3 vPosition_Morph_4;
layout(location = 12) in vec3 vPosition_Morph_5;
layout(location = 13) in vec3 vPosition_Morph_6;
layout(location = 14) in vec3 vPosition_Morph_7;
layout(location = 15) in vec3 vPosition_Morph_8;
layout(location = 16) in vec3 vPosition_Morph_9;

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

uniform mat4 u_model_matrix;     
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;
uniform mat4 u_morph_weight;

const int MAX_BONES = 140;
uniform mat4 bones[MAX_BONES];

const int MaxLights = 10;
uniform LightProperties Lights[MaxLights];

out vec3 light_direction[MaxLights];
out vec2 outTexture;
out vec3 transformed_normals;
out vec3 viewer_vector;
out vec3 frag_pos;

void main()
{
	mat4 bone_transform = bones[vBoneIds1[0]] * vWeights1[0];
		bone_transform += bones[vBoneIds1[1]] * vWeights1[1];
		bone_transform += bones[vBoneIds1[2]] * vWeights1[2];
		bone_transform += bones[vBoneIds1[3]] * vWeights1[3];
		bone_transform += bones[vBoneIds2[0]] * vWeights2[0];
		bone_transform += bones[vBoneIds2[1]] * vWeights2[1];
		bone_transform += bones[vBoneIds2[2]] * vWeights2[2];
		bone_transform += bones[vBoneIds2[3]] * vWeights2[3];

	vec3 morphed_Position = vPosition;		
	morphed_Position += u_morph_weight[0][0] * vPosition_Morph_0;
	morphed_Position += u_morph_weight[1][0] * vPosition_Morph_1;
	morphed_Position += u_morph_weight[2][0] * vPosition_Morph_2;
	morphed_Position += u_morph_weight[3][0] * vPosition_Morph_3;
	morphed_Position += u_morph_weight[0][1] * vPosition_Morph_4;
	morphed_Position += u_morph_weight[1][1] * vPosition_Morph_5;
	morphed_Position += u_morph_weight[2][1] * vPosition_Morph_6;
	morphed_Position += u_morph_weight[3][1] * vPosition_Morph_7;
	morphed_Position += u_morph_weight[0][2] * vPosition_Morph_8;
	//morphed_Position += u_morph_weight[1][2] * vPosition_Morph_9;

	vec4 boned_position = bone_transform * vec4(morphed_Position, 1.0); 
	//vec4 boned_position = vec4(morphed_Position, 1.0); 
	
	vec3 boned_normal = mat3(bone_transform) * vNormal; 

	vec4 eye_coordinates = u_view_matrix * u_model_matrix * boned_position;
    transformed_normals = mat3(u_view_matrix * u_model_matrix) * boned_normal;
    viewer_vector = -eye_coordinates.xyz;

	for (int lightIndex = 0; lightIndex < MaxLights; ++lightIndex) 
	{
		if (Lights[lightIndex].isEnabled == 1)
		{
		     light_direction[lightIndex] = vec3(Lights[lightIndex].position) - eye_coordinates.xyz;
        }
	}

	frag_pos = vec3(u_model_matrix * boned_position);
	outTexture = vTexture0;
	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * boned_position;
}