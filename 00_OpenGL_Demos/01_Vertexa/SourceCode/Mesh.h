#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <assimp\Importer.hpp>

using namespace std;
typedef unsigned int uint;
#define NUM_BONES_PER_VEREX 8

enum 
{
    ATTRIBUTE_VERTEX = 0,
    ATTRIBUTE_NORMAL,
    ATTRIBUTE_TEXTURE,
    ATTRIBUTE_BONE_ID_1,
    ATTRIBUTE_BONE_WEIGHT_ID_1,
    ATTRIBUTE_BONE_ID_2,
    ATTRIBUTE_BONE_WEIGHT_ID_2
};

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 text_coords;
};

struct Texture
{
	GLuint id;
	string type;
	aiString path;
};

struct BoneMatrix
{
	aiMatrix4x4 offset_matrix;
	aiMatrix4x4 final_world_transform;

};

struct VertexBoneData
{
	uint ids[NUM_BONES_PER_VEREX];   
	float weights[NUM_BONES_PER_VEREX];

	VertexBoneData()
	{
		memset(ids, 0, sizeof(ids));    
		memset(weights, 0, sizeof(weights));
	}

	void addBoneData(uint bone_id, float weight);
};

struct MaterialProperty
{
    aiColor3D materialDiffuse;
    aiColor3D materialSpecular;
    float shininess;

    MaterialProperty()
    {
        materialDiffuse = aiColor3D(1.0f, 1.0f, 1.0f);
        materialSpecular = aiColor3D(1.0f, 1.0f, 1.0f);
        shininess = 0.0;
    }
};

class Mesh
{
public:
	Mesh(vector<Vertex> vertic, vector<GLuint> ind, vector<Texture> textur, vector<VertexBoneData> bone_id_weights);
    Mesh::Mesh(vector<Vertex> vertic, vector<GLuint> ind, vector<Texture> textur, vector<VertexBoneData> bone_id_weights, MaterialProperty materialProp);
	Mesh() {};
	~Mesh();

	// Render mesh
	void Draw(GLuint shaders_program);

private:
	//Mesh data
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	vector<VertexBoneData> bones_id_weights_for_each_vertex;
    MaterialProperty materialProperty;

	//buffers
	GLuint vao;
	GLuint vbo_vertices;
	GLuint vbo_bones;
	GLuint ebo_elements;
    
	//inititalize buffer data
	void SetupMesh();
    void ReleaseMesh();
};
