#pragma once

/****************************************************************************
30-Jun-2018    V1.0    RHS    $$1 Created
*****************************************************************************/
#include <iostream>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/xinclude/XIncludeUtils.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>

#include <map>
#include <vector>


using namespace std;
using namespace xercesc;


/*=============================================================================
Function written for reading library_geometries have with LibGeom tag in their names.

library_animations
library_physics_scenes
library_lights
library_materials
library_effects
library_visual_scenes
library_controllers
==============================================================================*/




/*===========================================================================*/
struct GeometryData
{
  string  geomID;

  map<string, vector<int>>  countStrideMap;
 
  map<string, vector<int>> pElementData;     //actual element numbers 
  map<string, vector<int>>  pElementInfo;   //num of element, num of verts 
  map<string, string>  geometryAttributes; //number of vertices per emement, num of attributes per part ,
  vector<string> attributeSequence;       //as map will store data in sorted format so sequence is stored.

  int numofGeomAttribs;
  int *CountStride;
};

/*===========================================================================*/


struct AnimationSource
{
  string  sourceID;
  string  paramName;
  string  paramType;

  int     entityCount;
  int     count;
  int     stride;

  bool    floatArray;

  string  content;
};


struct AnimationSampler
{
  string samplerID;
  string Input;
  string Interpolation;
  string Output;
};

struct AnimationChannel
{
  string source;
  string Target;
};
/*===========================================================================*/


struct VisualSceneExtra
{
  string profile;
  map<string, string> mapVSExtra;
};


struct VisualSceneNode
{
  string VSNodeID;
  string VSNodeName;
  string VSNodetype;

  bool bRotateInfo;
  bool bMatrixInfo;
  bool bChildNodesInfo;
  bool bExtraInfo;

  string  sTranslate;
  string  sRotateX;
  string  sRotateY;
  string  sRotateZ;
  string  sScale;

  string  sMatrix;

  vector<VisualSceneNode> VSChildNodes;
};

/*===========================================================================*/

struct skinStruct
{
  string name;
  string key;//either Joint, Trasform, Weights  
  string dataType;
  int count;
  int stride;
  string content;
};

struct ControllerData
{
  string skinSourceName;//very imp will be same in geometry
  vector<skinStruct> skinStructVect;
  string BSMatrix;
  int    nVcountCount;
  string vCountContent;
  string vertWeightsContent;
  string inputs;
  int    numofInputs;
};
/*===========================================================================*/

class ColladaReader
{

private:
      /*******For libGeometry*****************/
      int numofGeomAttribs;
      map<string, GeometryData> geometryPartMap;
      vector<string> geometryPartIDSequence;

      /******For Animation *****************/
      map<string, vector<int>>   animCountStrideMap;
      map<string, string>        animationAttributes;
      int                        numofAnimAttribs;
      vector<string>             animationIDSequence;
      map<string, vector<string>>           animationSourceIDSequence;
      map<string, vector<AnimationSource>>  animationSourceMap;
      map<string, vector<AnimationSampler>> animationSamplerMap;
      map<string, vector<AnimationChannel>> animationChannelMap;

      /******For Controller *****************/
      vector<string> controllerIDSequence;
      map<string, ControllerData> controllerDataPartMap;
  
public:
  ColladaReader();
  virtual ~ColladaReader();

  /****Parsing functions for library_geometries***/

  char   *charTrim(char *str);
  string getNodeAttributeValue(DOMNode* node, string name);

  void   read_EntityInfoUnderSourceNode(DOMNode* entityNode, string sMeshEntName, GeometryData *geometryData);
  char   *read_TextNodeContent ( DOMText * textNode);
  void   set_LibGeom_ElementsList( DOMNode* elmntEntity,
                                  string sElementName, int nElementCnt,
                                  int numVertices, int numAttributes, GeometryData *geometryData);

  void read_LibGeom_MeshElementsInfo(DOMNode* elementNode, GeometryData *geometryData);

  void read_LibGeom_MeshInfo(DOMNode* meshNode, string geometryID);

  void read_LibGeom_Geometries(DOMNodeList *geomElemList);

  void readlibraryPhysicsScenes(DOMNodeList* phySceneElemList);

  int  readColladaXMLFile(string xmlFile);

  /***Parsing functions for library_animations **/
  void             read_LibAnim_Animation(DOMNodeList *animElemList);
  AnimationSource  read_LibAnim_EntityInfoUnderSourceNode(DOMNode* entityNode, string sourceEntName);
  AnimationSampler read_LibAnim_EntityInfoUnderSamplerNode(DOMNode* entityNode, string samplerEntName);

  /**Parsing functions for library_visual_scenes **/
  void read_LibVisScene_Visual_Scenes(DOMNodeList *vsceneElemList);
  VisualSceneNode read_LibVisScene_GetNode(DOMNode* entityNode);

  /**arsing functions for library_controllers **/
  void read_LibController_GetNode(DOMNodeList * controllerList);
  void read_LibController_SkinInfo(DOMNode* skinNode, string controllerID);
  void read_LibController_EntityInfoUnderSourceNode(DOMNode* entityNode, string sMeshEntName, skinStruct *SkinStructData);

  /*Map get functions for geometry */
  map<string, GeometryData> get_LibGeom_PartMap() { return geometryPartMap; }
  vector<string> get_LibGeom_PartIDSequence() {return geometryPartIDSequence;}

  map<string, ControllerData> get_LibController_PartMap() { return controllerDataPartMap; }
  vector<string> get_LibController_PartIDSequence() { return controllerIDSequence; }

 /* map<string, vector<int>>  get_LibGeom_ElementsList() { return pElementData; }
  map<string, vector<int>>  get_LibGeom_ElementsInfo() { return pElementInfo; }
  map<string, string>       get_LibGeom_GeomAttributes() { return geometryAttributes; }
  map<string, vector<int>>         get_LibGeom_CountOrStride() { return CountStrideMap; };

  int getNumofGeomAttribs() { return numofGeomAttribs; }
  vector<string> getAttributeSequence(){return attributeSequence;}*/

};


/*library_Animations
We will always have:
Number of sources = cahnels *3 or sampler*3.
3 is for Input, Interpolation, Output.
So if we have 1 chanel and 1 sample then we will have 3 sources.
If we have 8 chanel and 8 sample then we will have 8*3 sources.

<channel source="#ABC_root-transform_ABC_root-sampler"
target="ABC_root/transform(0)(0)"/>

<channel source="#Armature_Chest_pose_matrix-sampler" target="Chest/transform"/>

In the first case the "target" attribute will have "transform (X) (Y)"
after the last "/" in its value and in the second case the "target" attribute
will have "transform" after the last "/" in its value.

In the second form the values of the matrix which we backed in,
are given in one <source> out of those 3 <source>s, just like the
one <source> we read in reading inverse bind matrices from controller.
While in the first form values of each component of the 4 x 4 matrix are given
in different <source>s.
And we have to combine them in one matrix when we read the data.

We read the Joint matrices for each joint from the <visual_scene> node.
These values (which will be matrices) which we read from <animation> nodes
for those joints, which are targeted through the "target" attribute of the 
<channel> of that <animation> will replace the Joint matrices, we read
earlier from <visual_scene> for each key frame defined in the animation.
And to calculate the world transformation matrix for each joint we will have
to take this new Joint Matrix and multiply it with the parent Joint's
world transformation matrix.
*/
