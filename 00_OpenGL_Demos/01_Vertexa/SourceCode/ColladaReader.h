#pragma once

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

