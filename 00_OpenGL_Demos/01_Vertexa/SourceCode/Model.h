#pragma once

#include <vector>
#include <string>
#include <map>
#include <glm\gtc\quaternion.hpp>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <GL\glew.h>
#include "Mesh.h"
#include "AnimationHelper.h"

using namespace std;

struct ANIM_SCENE
{
    Assimp::Importer importer;
    const aiScene* scene;    
};

class Model
{
public:
	Model();
	~Model();
	static const uint MAX_BONES = 140;


	void initShaders(GLuint shaderProgramObject);
	void loadModel(const string& path);
    void loadVisemes(const string& path);
    void loadAnimations(vector<string> animationNames);
    void update();
	void display(GLuint shaders_program);
    void applyModelBoneTransform(ANIMATION_SETTINGS animationSettings);
	void showNodeName(aiNode* node);
    map<string, unsigned int> getBoneMapping();

	glm::mat4 aiToGlm(aiMatrix4x4 matr);
	aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend); // super super n lerp =)

private:
	Assimp::Importer importer;
	const aiScene* scene;
    ANIM_SCENE* importer_secondModel;
    ANIM_SCENE* importedVisemes;
    vector<ANIM_SCENE*> importedAnimationScenes;
	vector<Mesh> meshes; // one mesh in one object
	string directory;

	map<string, uint> m_bone_mapping; // maps a bone name and their index
	uint m_num_bones = 0;
	vector<BoneMatrix> m_bone_matrices;
	aiMatrix4x4 m_global_inverse_transform;

	GLuint m_bone_location[MAX_BONES];
	float ticks_per_second = 0.0f;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> LoadMaterialTexture(aiMaterial* mat, aiTextureType type, string type_name);

	
	uint findPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
	uint findRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
	uint findScaling(float p_animation_time, const aiNodeAnim* p_node_anim);
	const aiNodeAnim* findNodeAnim(const aiAnimation* p_animation, const string p_node_name);
	// calculate transform matrix
	aiVector3D calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
	aiQuaternion calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
	aiVector3D calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim);

	void readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform, ANIMATION_SETTINGS animationSettings);
	void boneTransform(vector<aiMatrix4x4>& transforms, ANIMATION_SETTINGS animationSettings);
    bool isNodeMoved(aiMatrix4x4 matrix1, aiMatrix4x4 matrix2);

	// rotate Head
	glm::quat rotate_head_xz = glm::quat(cos(glm::radians(0.0f)), sin(glm::radians(0.0f)) * glm::vec3(1.0f, 0.0f, 0.0f));

};

