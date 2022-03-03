#include<Windows.h>
#include <fstream>
#include "Model.h"
#include "Animation.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
extern std::ofstream g_log_file;
extern GLfloat zRot;

Model::Model()
{
	scene = nullptr;
}


Model::~Model()
{
	importer.FreeScene();
}

void Model::initShaders(GLuint shaderProgramObject)
{
	for (uint i = 0; i < MAX_BONES; i++) 
	{
		string name = "bones[" + to_string(i) + "]";
		m_bone_location[i] = glGetUniformLocation(shaderProgramObject, name.c_str());
	}
}

void Model::update()
{
    //  making new quaternions for rotate
    //	rotatation_matrix *= glm::quat(cos(glm::radians(1.0f / 2)), sin(glm::radians(1.0f / 2)) * glm::vec3(1.0f, 0.0f, 0.0f));
}

void Model::applyModelBoneTransform(ANIMATION_SETTINGS animationSettings)
{
    vector<aiMatrix4x4> transforms;
    boneTransform(transforms, animationSettings);

    for (uint i = 0; i < transforms.size(); i++)
    {
        glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, (const GLfloat*)&transforms[i]);
    }
}

void Model::display(GLuint shaders_program)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shaders_program);
	}
}

void Model::loadModel(const string& path)
{
    scene = importer.ReadFile(path, aiProcess_Triangulate);
	
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		g_log_file << "error at assimp Importing: " << importer.GetErrorString() << endl;
		g_log_file.flush();
		return;
	}
	m_global_inverse_transform = scene->mRootNode->mTransformation;
	m_global_inverse_transform.Inverse();

    directory = path.substr(0, path.find_last_of('/'));
    ticks_per_second = 1.0f;

	processNode(scene->mRootNode, scene);
}

void Model::loadAnimations(vector<string> animationNames)
{    
    for (vector<string> ::iterator animationPath = animationNames.begin();
        animationPath != animationNames.end(); animationPath++)
    {
        ANIM_SCENE *anim_Scene = new ANIM_SCENE();
        anim_Scene->scene = anim_Scene->importer.ReadFile(*animationPath, aiProcess_Triangulate);
      
        if (!anim_Scene->scene || !anim_Scene->scene->HasAnimations())
        {
            g_log_file << "error at assimp Importing Animations: " << importer.GetErrorString() << endl;
            g_log_file.flush();
            continue;
        }
        else
            importedAnimationScenes.push_back(anim_Scene);
    }
}


void Model::loadVisemes(const string& path)
{
    importedVisemes = new ANIM_SCENE();
    importedVisemes->scene = importedVisemes->importer.ReadFile(path, aiProcess_Triangulate);

    if (!importedVisemes->scene || !importedVisemes->scene->HasAnimations())
    {
        g_log_file << "error at assimp Importing Visemes: " << importer.GetErrorString() << endl;
        g_log_file.flush();
    }
}

void Model::showNodeName(aiNode* node)
{
	g_log_file << node->mName.data << endl;
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		showNodeName(node->mChildren[i]);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	Mesh mesh;
	for (uint i = 0; i < scene->mNumMeshes; i++)
    {
		aiMesh* current_mesh = scene->mMeshes[i];
		mesh = processMesh(current_mesh, scene);
		meshes.push_back(mesh); 
	}

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	g_log_file << "bones: " << mesh->mNumBones << " vertices: " << mesh->mNumVertices << std::endl;

	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	vector<VertexBoneData> bones_id_weights_for_each_vertex;
    MaterialProperty materialProperty;

	vertices.reserve(mesh->mNumVertices); 
	indices.reserve(mesh->mNumVertices);

	bones_id_weights_for_each_vertex.resize(mesh->mNumVertices);

	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		if (mesh->mNormals != NULL)
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}
		else
		{
			vertex.normal = glm::vec3();
		}
 
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.text_coords = vec;
		}
		else
		{
			vertex.text_coords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}


	for (uint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i]; 
		indices.push_back(face.mIndices[0]); 
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	
    if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuse_maps = LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
		bool exist = false;
		for (int i = 0; (i < textures.size()) && (diffuse_maps.size() != 0); i++)
		{
			if (textures[i].path ==  diffuse_maps[0].path)
			{
				exist = true;
			}
		}

		if(!exist && diffuse_maps.size() != 0) 
            textures.push_back(diffuse_maps[0]);
		

		vector<Texture> specular_maps = LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular");
		exist = false;
		for (int i = 0; (i < textures.size()) && (specular_maps.size() != 0); i++)
		{
			if (textures[i].path == specular_maps[0].path)
			{
				exist = true;
			}
		}
		if (!exist  && specular_maps.size() != 0) 
            textures.push_back(specular_maps[0]);

        material->Get(AI_MATKEY_COLOR_SPECULAR, materialProperty.materialSpecular);
        material->Get(AI_MATKEY_SHININESS, materialProperty.shininess);
	}

	// load bones
	for (uint i = 0; i < mesh->mNumBones; i++)
	{
		uint bone_index = 0;
		string bone_name(mesh->mBones[i]->mName.data);

		g_log_file << mesh->mBones[i]->mName.data << endl;

		if (m_bone_mapping.find(bone_name) == m_bone_mapping.end())
		{
			// Allocate an index for a new bone
			bone_index = m_num_bones;
			m_num_bones++;
			BoneMatrix bi;
			m_bone_matrices.push_back(bi);
			m_bone_matrices[bone_index].offset_matrix = mesh->mBones[i]->mOffsetMatrix;
			m_bone_mapping[bone_name] = bone_index;
		}
		else
		{
			bone_index = m_bone_mapping[bone_name];
		}

		for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			uint vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			bones_id_weights_for_each_vertex[vertex_id].addBoneData(bone_index, weight);
		}
	} 

	return Mesh(vertices, indices, textures, bones_id_weights_for_each_vertex, materialProperty);
}

vector<Texture> Model::LoadMaterialTexture(aiMaterial* mat, aiTextureType type, string type_name)
{
	vector<Texture> textures;
	for (uint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		string filename = string(str.C_Str());
		filename = directory + '/' + filename;        
		g_log_file << filename << endl;

		Texture texture;
		
        //Load texture
        Animation::LoadGLTexturesPNG(&texture.id, filename.c_str());
		texture.type = type_name;
		texture.path = str;
		textures.push_back(texture);
	}
	return textures;
}

uint Model::findPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	for (uint i = 0; i < p_node_anim->mNumPositionKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

uint Model::findRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	
	for (uint i = 0; i < p_node_anim->mNumRotationKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mRotationKeys[i + 1].mTime) 
		{
			return i; 
		}
	}

	assert(0);
	return 0;
}

uint Model::findScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	
	for (uint i = 0; i < p_node_anim->mNumScalingKeys - 1; i++)
	{
 		if (p_animation_time < (float)p_node_anim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

aiVector3D Model::calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	if (p_node_anim->mNumPositionKeys == 1) // Keys   
	{
		return p_node_anim->mPositionKeys[0].mValue;
	}

	uint position_index = findPosition(p_animation_time, p_node_anim);       
	uint next_position_index = position_index + 1; //    
	assert(next_position_index < p_node_anim->mNumPositionKeys);
	//   
	float delta_time = (float)(p_node_anim->mPositionKeys[next_position_index].mTime - p_node_anim->mPositionKeys[position_index].mTime);
	//  = (       ) /    
	float factor = (p_animation_time - (float)p_node_anim->mPositionKeys[position_index].mTime) / delta_time;
//	assert(factor >= 0.0f && factor <= 1.0f);
	aiVector3D start = p_node_anim->mPositionKeys[position_index].mValue;
	aiVector3D end = p_node_anim->mPositionKeys[next_position_index].mValue;
	aiVector3D delta = end - start;

	return start + factor * delta;
}

aiQuaternion Model::calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	if (p_node_anim->mNumRotationKeys == 1) // Keys   
	{
		return p_node_anim->mRotationKeys[0].mValue;
	}

	uint rotation_index = findRotation(p_animation_time, p_node_anim); //      
	uint next_rotation_index = rotation_index + 1; //    
	assert(next_rotation_index < p_node_anim->mNumRotationKeys);
	//   
	float delta_time = (float)(p_node_anim->mRotationKeys[next_rotation_index].mTime - p_node_anim->mRotationKeys[rotation_index].mTime);
	//  = (       ) /    
	float factor = (p_animation_time - (float)p_node_anim->mRotationKeys[rotation_index].mTime) / delta_time;
	
	//g_log_file << "p_node_anim->mRotationKeys[rotation_index].mTime: " << p_node_anim->mRotationKeys[rotation_index].mTime << endl;
	//g_log_file << "p_node_anim->mRotationKeys[next_rotaion_index].mTime: " << p_node_anim->mRotationKeys[next_rotation_index].mTime << endl;
	//g_log_file << "delta_time: " << delta_time << endl;
	//g_log_file << "animation_time: " << p_animation_time << endl;
	//g_log_file << "animation_time - mRotationKeys[rotation_index].mTime: " << (p_animation_time - (float)p_node_anim->mRotationKeys[rotation_index].mTime) << endl;
	//g_log_file << "factor: " << factor << endl << endl << endl;

	//assert(factor >= 0.0f && factor <= 1.0f);
	aiQuaternion start_quat = p_node_anim->mRotationKeys[rotation_index].mValue;
	aiQuaternion end_quat = p_node_anim->mRotationKeys[next_rotation_index].mValue;

	return nlerp(start_quat, end_quat, factor);
}

aiVector3D Model::calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	if (p_node_anim->mNumScalingKeys == 1) // Keys   
	{
		return p_node_anim->mScalingKeys[0].mValue;
	}

	uint scaling_index = findScaling(p_animation_time, p_node_anim); //      
	uint next_scaling_index = scaling_index + 1; //    
	assert(next_scaling_index < p_node_anim->mNumScalingKeys);
	//   
	float delta_time = (float)(p_node_anim->mScalingKeys[next_scaling_index].mTime - p_node_anim->mScalingKeys[scaling_index].mTime);
	
	float  factor = (p_animation_time - (float)p_node_anim->mScalingKeys[scaling_index].mTime) / delta_time;
	//assert(factor >= 0.0f && factor <= 1.0f);
	aiVector3D start = p_node_anim->mScalingKeys[scaling_index].mValue;
	aiVector3D end = p_node_anim->mScalingKeys[next_scaling_index].mValue;
	aiVector3D delta = end - start;

	return start + factor * delta;
}

const aiNodeAnim * Model::findNodeAnim(const aiAnimation * p_animation, const string p_node_name)
{

	for (uint i = 0; i < p_animation->mNumChannels; i++)
	{
		const aiNodeAnim* node_anim = p_animation->mChannels[i]; //    node
		if (string(node_anim->mNodeName.data) == p_node_name)
		{
			return node_anim;
		}
	}

	return nullptr;
}
						
void Model::readNodeHierarchy(float currentTime, const aiNode* p_node, const aiMatrix4x4 parent_transform, ANIMATION_SETTINGS animationSettings)
{

    bool isNodeProcessed = false;

	string node_name(p_node->mName.data);
    aiMatrix4x4 node_transform = p_node->mTransformation;

    const aiNodeAnim* node_anim_visemes = NULL;
    if (importedVisemes->scene && importedVisemes->scene->HasAnimations())
    {
        const aiAnimation* animation_visemes = importedVisemes->scene->mAnimations[0];
        node_anim_visemes = findNodeAnim(animation_visemes, node_name);
    }

    if (node_anim_visemes && !isNodeProcessed)
    {
        int firstVisemeIndex, secondVisemeIndex = 0;
        float secondVisemeWeight = 0.0f;

        animationSettings.lipSync.getVisemeIndexAndValue(firstVisemeIndex, secondVisemeIndex, secondVisemeWeight);

        aiQuaternion rotate_quat_first = node_anim_visemes->mRotationKeys[firstVisemeIndex].mValue;
        aiQuaternion rotate_quat_second = node_anim_visemes->mRotationKeys[secondVisemeIndex].mValue;
        aiQuaternion rotate_quat = nlerp(rotate_quat_first, rotate_quat_second, secondVisemeWeight);
        aiMatrix4x4 rotate_matr = aiMatrix4x4(rotate_quat.GetMatrix());

        aiVector3D translate_vector_first = node_anim_visemes->mPositionKeys[firstVisemeIndex].mValue;
        aiVector3D translate_vector_second = node_anim_visemes->mPositionKeys[secondVisemeIndex].mValue;
        aiVector3D delta = translate_vector_second - translate_vector_first;
        aiVector3D translate_vector = translate_vector_first + secondVisemeWeight * delta;
        aiMatrix4x4 translate_matr = aiMatrix4x4();
        aiMatrix4x4::Translation(translate_vector, translate_matr);

        aiMatrix4x4 node_transform_viseme = aiMatrix4x4();
        node_transform_viseme = translate_matr * rotate_matr;
        if (isNodeMoved(node_transform, node_transform_viseme))
        {
            isNodeProcessed = true;
            node_transform = node_transform_viseme;
        }
    }

	// -------------------------------------------------------
    // For Expressions
    const aiNodeAnim* node_anim_expression = NULL;
    if (!importedAnimationScenes.empty())
    {
        const aiAnimation* animation_expression = importedAnimationScenes[0]->scene->mAnimations[0];
        node_anim_expression = findNodeAnim(animation_expression, node_name);
    }

    if (node_anim_expression && !isNodeProcessed)
    {
        bool isExpression = false;
        bool isAction = false;
        aiMatrix4x4 node_transform_expression = aiMatrix4x4();
        //rotation
        aiQuaternion rotate_quat = node_anim_expression->mRotationKeys[animationSettings.expression.emotion].mValue;
        aiMatrix4x4 rotate_matr = aiMatrix4x4(rotate_quat.GetMatrix());

        if (string(node_name) == string("Model_orbicularis03_L") || string(node_name) == string("Model_orbicularis03_R"))
        {
            float eyeCloseTime = (float)fmod(currentTime/1000.0, animationSettings.faceRandomExpressions.eyeLidCloseInterval);
            if( eyeCloseTime > (animationSettings.faceRandomExpressions.eyeLidCloseInterval - 0.15f))
            {
                aiQuaternion eyeLidQuat = aiQuaternion(0.0f, 0.0f, 0.8f, -0.5f);
                rotate_matr = aiMatrix4x4(eyeLidQuat.GetMatrix());
            }
            if (eyeCloseTime == (animationSettings.faceRandomExpressions.eyeLidCloseInterval))
            {
                animationSettings.faceRandomExpressions.eyeLidCloseInterval =  2.0f + (rand()%1500 /1000.0f);
            }
        }

        //translation
        aiVector3D translate_vector = node_anim_expression->mPositionKeys[animationSettings.expression.emotion].mValue;
        aiMatrix4x4 translate_matr;
        aiMatrix4x4::Translation(translate_vector, translate_matr);

        node_transform_expression = translate_matr * rotate_matr;
        if (isNodeMoved(node_transform, node_transform_expression))
        {
            isNodeProcessed = true;
            node_transform = node_transform_expression;
        }
    }
    
    //---------------------------------------------------------
    // For Actions and other animations like Walk, Dance
    const aiNodeAnim* node_anim_action = NULL;
    float animationDuration = 0.0f;
    if (animationSettings.action.actionSequence != ACTION_NONE &&
        (importedAnimationScenes.size() > animationSettings.action.actionSequence))
    {
        const aiAnimation* animation_action = importedAnimationScenes[animationSettings.action.actionSequence]->scene->mAnimations[0];
        animationDuration = importedAnimationScenes[animationSettings.action.actionSequence]->scene->mAnimations[0]->mDuration;
        node_anim_action = findNodeAnim(animation_action, node_name);
    }
    
	if (node_anim_action && !isNodeProcessed)
    {

        float animation_time = (currentTime - animationSettings.action.actionStartTime)/1000.0;
        if (animation_time >= animationDuration)
            animation_time = animationDuration - 0.0001;

        aiMatrix4x4 node_transform_action = aiMatrix4x4();
        //rotation
        aiQuaternion rotate_quat = calcInterpolatedRotation(animation_time, node_anim_action);
        aiMatrix4x4 rotate_matr = aiMatrix4x4(rotate_quat.GetMatrix());

        //translation
        aiVector3D translate_vector = calcInterpolatedPosition(animation_time, node_anim_action);
        aiMatrix4x4 translate_matr;
        aiMatrix4x4::Translation(translate_vector, translate_matr);

        /*
        // TBD: Eye Movement
        if ( string(node_anim->mNodeName.data) == string("eye.L") || string(node_anim->mNodeName.data) == string("eye.R"))
        {
        aiQuaternion eye_Direction = aiQuaternion(rotate_head_xz.w, rotate_head_xz.x, rotate_head_xz.y, rotate_head_xz.z);
        rotate_matr = rotate_matr * aiMatrix4x4(rotate_head.GetMatrix());
        }
        // TBD: Eye Movement
        if ( string(node_anim->mNodeName.data) == string("head"))
        {
        aiQuaternion eye_Direction = aiQuaternion(rotate_head_xz.w, rotate_head_xz.x, rotate_head_xz.y, rotate_head_xz.z);
        rotate_matr = rotate_matr * aiMatrix4x4(rotate_head.GetMatrix());
        }
        */

        node_transform_action = translate_matr * rotate_matr;

        if (isNodeMoved(node_transform, node_transform_action))
        {
            isNodeProcessed = true;
            node_transform = node_transform_action;
        }
    }
	
	aiMatrix4x4 global_transform = parent_transform * node_transform;

	if (m_bone_mapping.find(node_name) != m_bone_mapping.end()) 
	{
		uint bone_index = m_bone_mapping[node_name];
		m_bone_matrices[bone_index].final_world_transform = m_global_inverse_transform * global_transform * m_bone_matrices[bone_index].offset_matrix;
	}

	for (uint i = 0; i < p_node->mNumChildren; i++)
	{
		readNodeHierarchy(currentTime, p_node->mChildren[i], global_transform, animationSettings);
	}

}

bool Model::isNodeMoved(aiMatrix4x4 matrix1, aiMatrix4x4 matrix2)
{
    bool isNodePositionSame = false;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (fabs(matrix1[i][j] - matrix2[i][j]) > 0.0005f)
                isNodePositionSame = true;

    return isNodePositionSame;
}

void Model::boneTransform(vector<aiMatrix4x4>& transforms, ANIMATION_SETTINGS animationSettings)
{
    aiMatrix4x4 identity_matrix; // = mat4(1.0f);

    float animation_time = 1.0f;
    //if (animationSettings.action.actionSequence != NONE)
    //    animation_time = (float)fmod(time_in_ticks, importedAnimationScenes[animationSettings.action.actionSequence]->scene->mAnimations[0]->mDuration);
    //readNodeHierarchy(animation_time, scene->mRootNode, identity_matrix, animationSettings);

    readNodeHierarchy((float)GetTickCount(), scene->mRootNode, identity_matrix, animationSettings);

	transforms.resize(m_num_bones);

	for (uint i = 0; i < m_num_bones; i++)
	{
		transforms[i] = m_bone_matrices[i].final_world_transform;
	}
}

glm::mat4 Model::aiToGlm(aiMatrix4x4 matr)
{
	glm::mat4 result;
	result[0].x = matr.a1; result[0].y = matr.b1; result[0].z = matr.c1; result[0].w = matr.d1;
	result[1].x = matr.a2; result[1].y = matr.b2; result[1].z = matr.c2; result[1].w = matr.d2;
	result[2].x = matr.a3; result[2].y = matr.b3; result[2].z = matr.c3; result[2].w = matr.d3;
	result[3].x = matr.a4; result[3].y = matr.b4; result[3].z = matr.c4; result[3].w = matr.d4;

	//g_log_file << " " << result[0].x << "		 " << result[0].y << "		 " << result[0].z << "		 " << result[0].w << endl;
	//g_log_file << " " << result[1].x << "		 " << result[1].y << "		 " << result[1].z << "		 " << result[1].w << endl;
	//g_log_file << " " << result[2].x << "		 " << result[2].y << "		 " << result[2].z << "		 " << result[2].w << endl;
	//g_log_file << " " << result[3].x << "		 " << result[3].y << "		 " << result[3].z << "		 " << result[3].w << endl;
	//g_log_file << endl;

	//g_log_file << " " << matr.a1 << "		 " << matr.b1 << "		 " << matr.c1 << "		 " << matr.d1 << endl;
	//g_log_file << " " << matr.a2 << "		 " << matr.b2 << "		 " << matr.c2 << "		 " << matr.d2 << endl;
	//g_log_file << " " << matr.a3 << "		 " << matr.b3 << "		 " << matr.c3 << "		 " << matr.d3 << endl;
	//g_log_file << " " << matr.a4 << "		 " << matr.b4 << "		 " << matr.c4 << "		 " << matr.d4 << endl;
	//g_log_file << endl;

	return result;
}

aiQuaternion Model::nlerp(aiQuaternion a, aiQuaternion b, float blend)
{
	//g_log_file << a.w + a.x + a.y + a.z << endl;
	a.Normalize();
	b.Normalize();

	aiQuaternion result;
	float dot_product = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	float one_minus_blend = 1.0f - blend;

	if (dot_product < 0.0f)
	{
		result.x = a.x * one_minus_blend + blend * -b.x;
		result.y = a.y * one_minus_blend + blend * -b.y;
		result.z = a.z * one_minus_blend + blend * -b.z;
		result.w = a.w * one_minus_blend + blend * -b.w;
	}
	else
	{
		result.x = a.x * one_minus_blend + blend * b.x;
		result.y = a.y * one_minus_blend + blend * b.y;
		result.z = a.z * one_minus_blend + blend * b.z;
		result.w = a.w * one_minus_blend + blend * b.w;
	}

	return result.Normalize();
}


map<string, unsigned int> Model::getBoneMapping()
{
    return m_bone_mapping;
}