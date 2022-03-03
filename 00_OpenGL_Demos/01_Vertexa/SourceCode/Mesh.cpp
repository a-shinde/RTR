#include "Mesh.h"


Mesh::Mesh(vector<Vertex> vertic, vector<GLuint> ind, vector<Texture> textur, vector<VertexBoneData> bone_id_weights)
{
	vertices = vertic;
	indices = ind;
	textures = textur;
	bones_id_weights_for_each_vertex = bone_id_weights;

	SetupMesh();
}

Mesh::Mesh(vector<Vertex> vertic, vector<GLuint> ind, vector<Texture> textur, vector<VertexBoneData> bone_id_weights, MaterialProperty materialProp)
{
    vertices = vertic;
    indices = ind;
    textures = textur;
    bones_id_weights_for_each_vertex = bone_id_weights;
    materialProperty = materialProp;

    SetupMesh();
}

Mesh::~Mesh()
{
}

void Mesh::ReleaseMesh()
{
  if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    // destroy vbo
    if (vbo_vertices)
    {
        glDeleteBuffers(1, &vbo_vertices);
        vbo_vertices = 0;
    }

    if (vbo_bones)
    {
        glDeleteBuffers(1, &vbo_bones);
        vbo_bones = 0;
    }

    if (ebo_elements)
    {
        glDeleteBuffers(1, &ebo_elements);
        ebo_elements = 0;
    }
}

void VertexBoneData::addBoneData(uint bone_id, float weight)
{
	for (uint i = 0; i < NUM_BONES_PER_VEREX; i++) 
	{
		if (weights[i] == 0.0) 
		{
			ids[i] = bone_id;
			weights[i] = weight;
			return;
		}
	}
}
 

void Mesh::Draw(GLuint shaders_program)
{

    glUniform1f(glGetUniformLocation(shaders_program, "material.shininess"), materialProperty.shininess);
    glUniform3f(glGetUniformLocation(shaders_program, "material.material_specular"), materialProperty.materialSpecular.r, materialProperty.materialSpecular.g, materialProperty.materialSpecular.b);
    glUniform1f(glGetUniformLocation(shaders_program, "material.transparency"), 1.0f);

	int diffuse_nr = 1;
	int specular_nr = 1;

	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		
		string number;
		string name = textures[i].type;
		if (name == "texture_diffuse")
		{
			number = to_string(diffuse_nr++);
		}
		else if (name == "texture_specular")
		{
			number = to_string(specular_nr++);
		}

		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		glUniform1i(glGetUniformLocation(shaders_program, ("material." + name + number).c_str()), i);
	}
	 
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);

	//vertices data
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(ATTRIBUTE_VERTEX);
    glVertexAttribPointer(ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(ATTRIBUTE_NORMAL);
    glVertexAttribPointer(ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(ATTRIBUTE_TEXTURE);
    glVertexAttribPointer(ATTRIBUTE_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, text_coords));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bones data
	glGenBuffers(1, &vbo_bones);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bones);
	glBufferData(GL_ARRAY_BUFFER, bones_id_weights_for_each_vertex.size() * sizeof(bones_id_weights_for_each_vertex[0]), &bones_id_weights_for_each_vertex[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(ATTRIBUTE_BONE_ID_1);
    glVertexAttribIPointer(ATTRIBUTE_BONE_ID_1, 4, GL_INT, sizeof(VertexBoneData), (GLvoid*)0); // for INT Ipointer
    glEnableVertexAttribArray(ATTRIBUTE_BONE_WEIGHT_ID_1);
    glVertexAttribPointer(ATTRIBUTE_BONE_WEIGHT_ID_1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (GLvoid*)offsetof(VertexBoneData, weights));
    glEnableVertexAttribArray(ATTRIBUTE_BONE_ID_2);
    glVertexAttribIPointer(ATTRIBUTE_BONE_ID_2, 4, GL_INT, sizeof(VertexBoneData), (GLvoid*)(4* sizeof(GL_INT))); // for INT Ipointer
    glEnableVertexAttribArray(ATTRIBUTE_BONE_WEIGHT_ID_2);
    glVertexAttribPointer(ATTRIBUTE_BONE_WEIGHT_ID_2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), ((GLvoid*)((offsetof(VertexBoneData, weights) + (4 * sizeof(GL_FLOAT))))));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //elements data
	glGenBuffers(1, &ebo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

}