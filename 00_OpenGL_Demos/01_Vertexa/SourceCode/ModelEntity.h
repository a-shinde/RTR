#pragma once
/****************************************************************************
10-Jul-2018    V1.0    RHS    $$1 Created
*****************************************************************************/

#include <vector>
#include <set>
#include <list>
#include <sstream>
#include <algorithm> 
#include <map>
#include <utility>
#include <fstream>
#include <GL\glew.h>
#include "ColladaReader.h"

using namespace std;

#define MAX_BONES_PER_VEREX 8

struct GeometryEntity  //one vertex or normal
{
  int      entityID;
  int      entityStride;
  double  *entityCords;
};

struct ELEMENT
{
  int numofAttributes;
  vector <vector<int>> AttributeIDs;
};

struct controllerVertexWeight
{
  int           numofVCount;
  vector<string> vertJointNames;
  vector<float> vertWeightValues;
};

struct ShaderBuffers
{
    GLuint vao;

    GLuint* vbo_vertex;
    int vbo_vertex_size;

    GLuint* vbo_bone;
    int vbo_bone_size;

    GLuint* ebo;
    int ebo_size;
    vector<unsigned int> elementCounts;
};

struct BoneInfo
{
    map<string, GLuint> bone_mapping;
};

struct ModelPart  //this is geometry tab
{
  string partID;
  bool   IsMorphedTargetAvailable;
  int    numGeomAttribs;
  int    numMorphedGeomAttribs;

  vector<vector<GeometryEntity>> GeometryEntityContainer; // vertex co-rd vector , normal vect and so on
  vector<vector<ELEMENT>> ElementContainer;
  vector<string> ElementSequence;
  vector<controllerVertexWeight> ControllerForGeom;

  ShaderBuffers shaderBuffer;
};
/*============================================================================*/



struct CONTROLLER
{
  string skinSource;
  vector<controllerVertexWeight> controllerVertexWeightVect;
};

/*============================================================================*/

typedef  ELEMENT Element;
typedef  CONTROLLER  Controller;

typedef  int STATUS;

class ModelEntity
{

private:
   ColladaReader       *reader;
   set<string>          morphedGeometryList;
   list <ModelPart *>   modelPartList;//multiple geometires
   list <Controller*>   controllerList;
   BoneInfo boneInfo;
   //Temporary
   int startTexIndex;

public:
    ModelEntity();
  ~ModelEntity();
  
  bool loadModel(string xmlFile);

  ModelPart* constructPart(string partID);
  void addPartToModel(ModelPart *modelPart);
  void addControllerToModel(Controller *controlllerData);
 
  list <ModelPart *> getPartList() {return modelPartList; }
  list <Controller *> getControllerList() { return controllerList; }

  void deleteAllPartsInModel();
  void deleteAllControllersInModel();
  
 
  Controller* getControllerForGeometry(string geometryID);
  GLuint morphMatrixUniform;

  vector<string> checkForMorphedTargetForPart(string partID);
  vector<vector<ELEMENT>> GetElementContainerFromGeometry(GeometryData geometry, vector<string> &ElementNames);
  vector<vector<GeometryEntity>>GetGeometryEntityContainerFromGeometry(GeometryData geometry);
  bool setMeshContainer();
  bool setControllerData();
  bool setBoneInfo(map<string, GLuint> bone_mapping);
  void getGeometryData();
  void ModelUninitialize(void);
  void Modeldisplay(GLuint shaders_program);
  void initShaders(GLuint shaderProgramObject);
};


/*Note:
Depend upon number of attributes such as Vertex, Normal ,
Texture, Color for each model, GeometryEntity will be filled
in a vector.Such list of vectors will be kept in GeometryEntityContainer
So first vector will be Vertex and then Normal and so on.
In some models we may have more than one geometry so this has been
stored against the geometry ID given in collada file.*/