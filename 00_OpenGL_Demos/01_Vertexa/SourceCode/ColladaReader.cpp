/*===========================================================================
30-Jun-2018    V1.0    RHS    $$1 Created
/*===========================================================================*/
#include <vector>
#include <sstream>
#include <algorithm> 
#include <map>
#include <utility>

#include "ColladaReader.h"


ColladaReader::ColladaReader()
{
  
}

ColladaReader::~ColladaReader()
{
 
}
/*===========================================================================*/

string ColladaReader::getNodeAttributeValue(DOMNode* node, string name)
{
  DOMNamedNodeMap *attr = node->getAttributes();
  DOMNode *aN;
  string  paramValue;
  if ((aN = attr->getNamedItem(XMLString::transcode((char*)name.c_str()))) != 0)
  {
    paramValue = charTrim((char *)(XMLString::transcode(aN->getNodeValue())));
  }
  return paramValue;
}

/*===========================================================================*/

char* ColladaReader:: charTrim(char *str)
{
  char *inputChar, *outputChar;

  if (str)
  {
    for (inputChar = outputChar = str; *inputChar; )
    {
      while (*inputChar && (isspace(*inputChar)))
        inputChar++;
      if (*inputChar && (outputChar != str))
        *(outputChar++) = ' ';
      while (*inputChar && (!isspace(*inputChar)))
        *(outputChar++) = *(inputChar++);
    }
    *outputChar = '\0';
  }
  return (str);
}

/*===========================================================================
Functions for reading library_geometries
===========================================================================*/

void ColladaReader :: read_EntityInfoUnderSourceNode(DOMNode* entityNode, string sMeshEntName, GeometryData *geometryData)
{
  //Read the mesh entities information such as position, normal texture etc.
  DOMNodeList *meshEntityNodes = entityNode->getChildNodes();
  
  string mainEntityName = XMLString::transcode(entityNode->getNodeName());

  for (unsigned int ii = 0; ii < meshEntityNodes->getLength(); ii++)
  {
    DOMNode *meshEntity = meshEntityNodes->item(ii);
    if (meshEntity->getNodeType() != DOMNode::ELEMENT_NODE)
      continue;
    else
    {
      if (!strcmp(XMLString::transcode(meshEntity->getNodeName()), "float_array"))
      {
        DOMNamedNodeMap *entityAttr = meshEntity->getAttributes();
        DOMNode *aNMeshEntity;
        char* sEntityCnt = NULL;
        int   nEntityCnt = 0;
        char* content = NULL;
        if ((aNMeshEntity = entityAttr->getNamedItem(XMLString::transcode("count"))) != 0)
        {
          sEntityCnt = charTrim((char *)(XMLString::transcode(aNMeshEntity->getNodeValue())));
          nEntityCnt = atoi(sEntityCnt);
          if (nEntityCnt)
          {
            XMLCh* buffer = new XMLCh[XMLString::stringLen(static_cast<DOMText*>(meshEntity->getFirstChild())->getNodeValue()) + 1];
            XMLString::copyString(buffer, static_cast<DOMText*>(meshEntity->getFirstChild())->getNodeValue());
            XMLString::trim(buffer);
            content = XMLString::transcode(buffer);
            geometryData->geometryAttributes[sMeshEntName] = content;
            numofGeomAttribs++;
            delete[] buffer;
            XMLString::release(&content);
          }
          else
          {
           // geometryData->geometryAttributes[sMeshEntName] = "";
          }
        }
      }
      if (!strcmp(XMLString::transcode(meshEntity->getNodeName()), "technique_common"))
      {
        DOMNode* attrbNode = meshEntity->getFirstChild();
        while (attrbNode)
        {
          DOMNode* nextAttrNode = attrbNode->getNextSibling();
          if (!strcmp(XMLString::transcode(attrbNode->getNodeName()), "accessor"))
          {
            DOMNamedNodeMap *entityAttrbs = attrbNode->getAttributes();
            DOMNode *aNEntityAttrbNode;
            char* sEntityCnt = NULL;
            int   nEntityCnt = 0;
            char* sEntityStrids = NULL;
            int   nEntityStrides = 0;
            vector<int> CountStride;
            CountStride.resize(2);

            aNEntityAttrbNode = entityAttrbs->getNamedItem(XMLString::transcode("count"));
            sEntityCnt = charTrim((char *)(XMLString::transcode(aNEntityAttrbNode->getNodeValue())));
            nEntityCnt = atoi(sEntityCnt);
            CountStride[0] = nEntityCnt;

            aNEntityAttrbNode = entityAttrbs->getNamedItem(XMLString::transcode("stride"));
            sEntityStrids = charTrim((char *)(XMLString::transcode(aNEntityAttrbNode->getNodeValue())));
            nEntityStrides = atoi(sEntityStrids);
            CountStride[1] = nEntityStrides;           
            geometryData->countStrideMap[sMeshEntName] = CountStride;
            geometryData->attributeSequence.push_back(sMeshEntName);
          }
          attrbNode = nextAttrNode;
        }
      }
    }
  }//end of for loop for entity children
}

/*===========================================================================
Read contents of TextNode.
===========================================================================*/

char* ColladaReader :: read_TextNodeContent(DOMText * textNode)
{
  XMLCh* buffer = new XMLCh[XMLString::stringLen(static_cast<DOMText*>(textNode->getFirstChild())->getNodeValue()) + 1];
  XMLString::copyString(buffer, static_cast<DOMText*>(textNode->getFirstChild())->getNodeValue());
  XMLString::trim(buffer);
  return(XMLString::transcode(buffer));
}

/*===========================================================================
Returns count and stride for given map position
===========================================================================*/
/*
int ColladaReader :: get_LibGeom_CountOrStride(int mapPosition, int param)
{
  for (map < string, int*>::iterator itr = CountStrideMap.begin(); itr != CountStrideMap.end(); itr++)
  {
    int*temp = (*itr).second;
    if (param == 0)//return Count
      return (temp[0]);
    else //return Stride
      return (temp[1]);
  }
  return 0;
 
  Stride will decide how many Vertex,Normal and Texture will be in one elment,
  though the Vertices and Normals are fixed number 3 , texture will be many times 2.
  so strides will be stored first in map and then assessed and stored in another
  vector to use further
  vector<int> inputIDs;
  inputIDs.resize(numInputs);
  for (int ii = 0; ii < numInputs; ii++)
  {
  int stride = getCountOrStride(ii, 1);
  if (stride != 0)
  inputIDs.resize(stride);
  else
  return;//need some message
  }
}*/

/*===========================================================================
Get the Element list from content that read in xml file under "p" node.
Input : DOM  node received for triangle or polylist.
        Number of element count.
        Number of vertices which will be 3 in case of triangle and vcount for polylist.
        Number of attributes (Vertex, Normal, Texture , Color.)
Output : 2 Maps which will have Material name as its key and 
         i)Number of elment count, num of vertices and num of attribute will be filled in one map.
         ii) Raw elment data under P node in another map.
         These maps will be further processeded in setElementContainer.in ModelEntity class
 ===========================================================================*/

void ColladaReader::set_LibGeom_ElementsList( DOMNode* elmntEntity,
                                              string sElementName, int nElementCnt,
                                              int numVertices, int numAttributes,
                                              GeometryData *geometryData)
{
  char* content = read_TextNodeContent(static_cast<DOMText*>(elmntEntity));
  stringstream tokenizer;
  tokenizer.str(content);
  XMLString::release(&content);
  int numIndices = nElementCnt*numVertices * numAttributes;
  int nElementSize = numVertices * numAttributes;
  vector<int> tempVect;
  tempVect.resize(numIndices);
  for (int i = 0; i < numIndices; i++)
  {
    tokenizer >> tempVect[i];
  }
  geometryData->pElementData[sElementName] = tempVect;

  vector<int> elementInfoVec;
  elementInfoVec.resize(3);
  elementInfoVec[0] = nElementCnt;
  elementInfoVec[1] = numVertices;
  elementInfoVec[2] = numAttributes;
  geometryData->pElementInfo[sElementName] = elementInfoVec;
}


/*===========================================================================
Read the Mesh "P" (Element) information and process it for further stoarage.
===========================================================================*/

void ColladaReader:: read_LibGeom_MeshElementsInfo(DOMNode* elementNode, GeometryData *geometryData)
{
  DOMNamedNodeMap *entityAttrbs = elementNode->getAttributes();
  DOMNode *aNEntityAttrbNode;
  char* sElementCnt = NULL;
  int   nElementCnt = 0;
  string sElementName = "";
  stringstream tokenizer;
  char* content = NULL;

  string NodeName = XMLString::transcode(elementNode->getNodeName());

  aNEntityAttrbNode = entityAttrbs->getNamedItem(XMLString::transcode("count"));
  if (aNEntityAttrbNode != NULL)
  {
    sElementCnt = charTrim((char *)(XMLString::transcode(aNEntityAttrbNode->getNodeValue())));
    nElementCnt = atoi(sElementCnt);
  }

  aNEntityAttrbNode = entityAttrbs->getNamedItem(XMLString::transcode("material"));
  if (aNEntityAttrbNode != NULL)
  {
    sElementName = charTrim((char *)(XMLString::transcode(aNEntityAttrbNode->getNodeValue())));
  } 
  
  int numAttributes = 0;
  int numVertices = 0;
  int vCountNumber = 0;
  int numInputs = 0;

  DOMNodeList *childNodes = elementNode->getChildNodes();
  for (unsigned int ii = 0; ii < childNodes->getLength(); ii++)
  {
    DOMNode *elmntEntity = childNodes->item(ii);
    if (elmntEntity->getNodeType() != DOMNode::ELEMENT_NODE)
      continue;
    else
    {
      if (!strcmp(XMLString::transcode(elmntEntity->getNodeName()), "input"))
      {
        numAttributes++;
      }
      if (!strcmp(XMLString::transcode(elmntEntity->getNodeName()), "vcount"))
      {
        content = read_TextNodeContent(static_cast<DOMText*>(elmntEntity));
        tokenizer.clear();
        tokenizer.str(content);
        vector<int> tempVect;
        tempVect.resize(nElementCnt);
        for (int i = 0; i < nElementCnt; i++)
        {
          tokenizer >> tempVect[i];
        }
        vCountNumber = tempVect[0];
        XMLString::release(&content);
      }
      if (!strcmp(XMLString::transcode(elmntEntity->getNodeName()), "p"))
      {
        if (NodeName == "triangles")//element is consist of 3 verts
        {
          numVertices = 3;
        }
        else if (NodeName == "lines")
        {
          numVertices = 2;
        }
        else if (NodeName == "polylist")//element is consist of info in vcount
        {
          numVertices = vCountNumber;
        }
        set_LibGeom_ElementsList(elmntEntity, sElementName, nElementCnt, numVertices, numAttributes, geometryData);

      }//end of "p" node
    }//end of main else condition
  }//end of for loop for childnodes
}

/*===========================================================================
Read the Mesh information and process it for further stoarage.
===========================================================================*/


void ColladaReader:: read_LibGeom_MeshInfo(DOMNode* meshNode, string geometryID)
{
  DOMNodeList *meshChildNodes = meshNode->getChildNodes();
  std::vector<string> meshEntityVec = { "position","normal","map","colors","vertices" };
  int entityCount = 0;
  int childCount = (int)meshChildNodes->getLength();

  GeometryData * geometryData = new GeometryData[1];
  geometryData->geomID = geometryID;
  geometryData->numofGeomAttribs = 0;
  numofGeomAttribs = 0;

  for (unsigned int i = 0; i < meshChildNodes->getLength(); i++)
  {
    DOMNode *iN = meshChildNodes->item(i);
    if (iN->getNodeType() != DOMNode::ELEMENT_NODE)
      continue;
    else
    {
      DOMNamedNodeMap *meshAttr = iN->getAttributes();
      DOMNode*  aNMesh;
      char*     meshEntityName = NULL;
      string    strMeshEntName;
      char*     meshEntityID = NULL;
      string    strMeshEntID;

      /*Under mesh Node the position, normal and map node values such as name is not consistant
      There are differnt variables used. Sometime the name itself is not given.
      So the original mesh child node name as "source", the "name" attribute parameter are used.*/

      string NodeName = XMLString::transcode(iN->getNodeName());
      if (entityCount <= 3)
      {
        if ((aNMesh = meshAttr->getNamedItem(XMLString::transcode("name"))) != 0)
        {
          meshEntityName = charTrim((char *)(XMLString::transcode(aNMesh->getNodeValue())));
          strMeshEntName = meshEntityName;
        }

        if ((aNMesh = meshAttr->getNamedItem(XMLString::transcode("id"))) != 0)
        {
          meshEntityID = charTrim((char *)(XMLString::transcode(aNMesh->getNodeValue())));
          strMeshEntID = meshEntityID;
        }
        if ((strMeshEntName.find(meshEntityVec[entityCount]) != string::npos ||
          strMeshEntID.find(meshEntityVec[entityCount]) != string::npos) &&
          (NodeName == "source"))
        {
          if(strMeshEntName.empty())
            read_EntityInfoUnderSourceNode(iN, strMeshEntID, geometryData);
          else
            read_EntityInfoUnderSourceNode(iN, strMeshEntName,geometryData);
          entityCount++;
        }
      }

      if (NodeName == "vertices")
      {
        entityCount++;
      }

      if (entityCount >4 || NodeName == "triangles" || NodeName == "polylist"  || NodeName == "lines")
      {
        DOMNode* elmtNode = iN;

        //we can more than one element list so need to loop 
        //through remaing traingle or polylists

        while (elmtNode)
        {
          DOMNode* nextElemNode = elmtNode->getNextSibling();
          DOMNode *iN = meshChildNodes->item(i);
          if (elmtNode->getNodeType() != DOMNode::ELEMENT_NODE)
          {
            if (i == childCount) break;
            else
            {
              elmtNode = nextElemNode;
              i++;
            }
            continue;
          }
          else
          {
            read_LibGeom_MeshElementsInfo(elmtNode, geometryData);
            if (i == childCount) break;
          }
          elmtNode = nextElemNode;
        }
      }//end of triangle condition

    }//end of else
  }//end of for loop for mesh children

  geometryData->numofGeomAttribs = numofGeomAttribs;
  geometryPartMap[geometryID] = *geometryData;
  numofGeomAttribs = 0;
}

/*===========================================================================
Entry function to read library_geometries
===========================================================================*/


void ColladaReader:: read_LibGeom_Geometries(DOMNodeList *geomElemList)
{
  //DOMNodeList *geomElemList = geomNode->getChildNodes();

  DOMNode *geomNode = geomElemList->item(0);

  DOMNode* child = geomNode->getFirstChild();
  while (child)
  {
    DOMNode* next = child->getNextSibling();
    char* name = XMLString::transcode(child->getNodeName());
    if (!strcmp(XMLString::transcode(child->getNodeName()), "geometry"))
    {
      DOMNamedNodeMap *attr = child->getAttributes();
      DOMNode *aN;
      char* geometryID = NULL;
      string geometryNodeID;
      if ((aN = attr->getNamedItem(XMLString::transcode("id"))) != 0)
      {
        geometryID = charTrim((char *)(XMLString::transcode(aN->getNodeValue())));
        geometryPartIDSequence.push_back(geometryID);
      }
      //looking for "Mesh" Node.
      DOMNode* meshNode = child->getFirstChild();
      while (meshNode)
      {
        DOMNode* nextGeomNode = meshNode->getNextSibling();
        if (!strcmp(XMLString::transcode(meshNode->getNodeName()), "mesh"))
        {
          read_LibGeom_MeshInfo(meshNode, geometryID);
        }
        if (!strcmp(XMLString::transcode(meshNode->getNodeName()), "extra"))
        {

        }
        meshNode = nextGeomNode;
      }
    }//end of geometry check
    child = next;
  }
}
/*==========End of Functions for reading library_geometries==================*/


/*=========Functions for reading   library_animations=======================*/

AnimationSource ColladaReader::read_LibAnim_EntityInfoUnderSourceNode(DOMNode* entityNode, string sourceEntName)
{
  AnimationSource animationSource;

  animationSource.sourceID = sourceEntName;
  DOMNodeList *animSourceEntityNodes = entityNode->getChildNodes();

  for (unsigned int ii = 0; ii < animSourceEntityNodes->getLength(); ii++)
  {
    DOMNode *animSourceEntity = animSourceEntityNodes->item(ii);
    if (animSourceEntity->getNodeType() != DOMNode::ELEMENT_NODE)
      continue;
    else
    {
      if (!strcmp(XMLString::transcode(animSourceEntity->getNodeName()), "float_array") ||
        !strcmp(XMLString::transcode(animSourceEntity->getNodeName()), "Name_array"))
      {
        string arrayType = XMLString::transcode(animSourceEntity->getNodeName());

        if (arrayType == "float_array")
          animationSource.floatArray = true;
        else
          animationSource.floatArray = false;

        DOMNamedNodeMap *entityAttr = animSourceEntity->getAttributes();
        DOMNode *aNanimSourceEntity;
        char* sEntityCnt = NULL;
        int   nEntityCnt = 0;
        char* content = NULL;
        if ((aNanimSourceEntity = entityAttr->getNamedItem(XMLString::transcode("count"))) != 0)
        {
          sEntityCnt = charTrim((char *)(XMLString::transcode(aNanimSourceEntity->getNodeValue())));
          nEntityCnt = atoi(sEntityCnt);
          animationSource.entityCount = nEntityCnt;

          XMLCh* buffer = new XMLCh[XMLString::stringLen(static_cast<DOMText*>(animSourceEntity->getFirstChild())->getNodeValue()) + 1];
          XMLString::copyString(buffer, static_cast<DOMText*>(animSourceEntity->getFirstChild())->getNodeValue());
          XMLString::trim(buffer);
          content = XMLString::transcode(buffer);
          animationSource.content = content;
          delete[] buffer;
          XMLString::release(&content);
        }
      }
      if (!strcmp(XMLString::transcode(animSourceEntity->getNodeName()), "technique_common"))
      {
        DOMNode* attrbNode = animSourceEntity->getFirstChild();
        while (attrbNode)
        {
          DOMNode* nextAttrNode = attrbNode->getNextSibling();
          if (!strcmp(XMLString::transcode(attrbNode->getNodeName()), "accessor"))
          {
            DOMNamedNodeMap *entityAttrbs = attrbNode->getAttributes();
            DOMNode *aNEntityAttrbNode;
            char* sEntityCnt = NULL;
            int   nEntityCnt = 0;
            char* sEntityStrids = NULL;
            int   nEntityStrides = 0;


            aNEntityAttrbNode = entityAttrbs->getNamedItem(XMLString::transcode("count"));
            sEntityCnt = charTrim((char *)(XMLString::transcode(aNEntityAttrbNode->getNodeValue())));
            nEntityCnt = atoi(sEntityCnt);
            animationSource.count = nEntityCnt;

            aNEntityAttrbNode = entityAttrbs->getNamedItem(XMLString::transcode("stride"));
            sEntityStrids = charTrim((char *)(XMLString::transcode(aNEntityAttrbNode->getNodeValue())));
            nEntityStrides = atoi(sEntityStrids);
            animationSource.stride = nEntityStrides;

            DOMNode* paramNode = attrbNode->getFirstChild();
            while (paramNode)
            {
              DOMNode* nextNode = paramNode->getNextSibling();
              string NodeName = XMLString::transcode(paramNode->getNodeName());
              if (NodeName == "param")
              {
                DOMNamedNodeMap *paramAttr = paramNode->getAttributes();
                DOMNode *pN;
                char* paramName = NULL;
                char* paramType = NULL;
                animationSource.paramName = "";
                animationSource.paramType = "";

                if( (pN = paramAttr->getNamedItem(XMLString::transcode("name"))) != 0) 
                {            
                  paramName = charTrim((char *)(XMLString::transcode(pN->getNodeValue())));
                  animationSource.paramName = paramName;
                  delete paramName;
                }
                if ((pN = paramAttr->getNamedItem(XMLString::transcode("type"))) != 0)
                {
                  paramType = charTrim((char *)(XMLString::transcode(pN->getNodeValue())));
                  animationSource.paramType = paramType;
                  delete paramType;
                }
              }
              paramNode = nextNode;
            }
          }
          attrbNode = nextAttrNode;
        }
      }
    }
  }//end of for loop for entity children
  return animationSource;
}

/*===========================================================================*/

AnimationSampler ColladaReader::read_LibAnim_EntityInfoUnderSamplerNode(DOMNode* entityNode, string samplerEntName)
{
  AnimationSampler animationSampler;
  animationSampler.samplerID = samplerEntName;

  DOMNode* inputNode = entityNode->getFirstChild();
  while (inputNode)
  {
    DOMNode* nextNode = inputNode->getNextSibling();
    string NodeName = XMLString::transcode(inputNode->getNodeName());

    if (NodeName == "input")
    {
      DOMNamedNodeMap *inputAttr = inputNode->getAttributes();
      DOMNode *inp;
      inp = inputAttr->getNamedItem(XMLString::transcode("semantic"));
      string type = charTrim((char *)(XMLString::transcode(inp->getNodeValue())));

      inp = inputAttr->getNamedItem(XMLString::transcode("source"));
      string typeSource = charTrim((char *)(XMLString::transcode(inp->getNodeValue())));
      typeSource.erase(typeSource.begin() + typeSource.find("#"));
    
      if (type == "INPUT")
      {
        animationSampler.Input = typeSource;
      }

      if (type == "OUTPUT")
      {        
        animationSampler.Output = typeSource;
      }

      if (type == "INTERPOLATION")
      {
        animationSampler.Interpolation = typeSource;
      }

    }
    inputNode = nextNode;
  }
  return animationSampler;
}
/*===========================================================================*/

void ColladaReader::read_LibAnim_Animation(DOMNodeList *animElemList)
{
   DOMNode *animNode = animElemList->item(0);

  DOMNode* child = animNode->getFirstChild();
  
  while (child)
  {
    DOMNode* next = child->getNextSibling();
    char* name = XMLString::transcode(child->getNodeName());
    cout << name << endl;

    if (!strcmp(XMLString::transcode(child->getNodeName()), "animation"))
    {
      vector<AnimationSource> animationSourceVec;
      vector<AnimationSampler> animationSamplerVec;
      vector<AnimationChannel> animationChannelVec;
      
      vector<string> sourceIDVec;
      vector<string> sampleIDSequence;

      DOMNamedNodeMap *attr = child->getAttributes();
      DOMNode *aN;
      char* animationID = NULL;
      if ((aN = attr->getNamedItem(XMLString::transcode("id"))) != 0) //animation ID
      {
        animationID = charTrim((char *)(XMLString::transcode(aN->getNodeValue())));
        animationIDSequence.push_back(animationID);
      }
      DOMNode* sourceNode = child->getFirstChild();
      int animEntityCount = 0;
      while (sourceNode)
      {
        DOMNode* nextGeomNode = sourceNode->getNextSibling();
        string NodeName = XMLString::transcode(sourceNode->getNodeName());       
        if (NodeName == "source")
        {
          DOMNamedNodeMap *sourceAttr = sourceNode->getAttributes();
          DOMNode *sN;
          char* sourceID = NULL;
          if ((sN = sourceAttr->getNamedItem(XMLString::transcode("id"))) != 0) //source Node ID
          {
            sourceID = charTrim((char *)(XMLString::transcode(sN->getNodeValue())));
          }
          AnimationSource aniSource = read_LibAnim_EntityInfoUnderSourceNode(sourceNode, sourceID);
          animationSourceVec.push_back(aniSource);
          sourceIDVec.push_back(sourceID);
        }
        if (NodeName == "sampler")
        {
          DOMNamedNodeMap *sampleAttr = sourceNode->getAttributes();
          DOMNode *smN;
          char* sampleID = NULL;
          if ((smN = sampleAttr->getNamedItem(XMLString::transcode("id"))) != 0) //source Node ID
          {
           sampleID = charTrim((char *)(XMLString::transcode(smN->getNodeValue())));
          }
          AnimationSampler aniSampler = read_LibAnim_EntityInfoUnderSamplerNode(sourceNode, sampleID);
          animationSamplerVec.push_back(aniSampler);
          sampleIDSequence.push_back(sampleID);
        }
        if (NodeName == "channel")
        {
          AnimationChannel animChannel;
          DOMNamedNodeMap *channelAttr = sourceNode->getAttributes();
          DOMNode *chN;
          if ((chN = channelAttr->getNamedItem(XMLString::transcode("source"))) != 0) //channel Node ID
          {
            string chSource  = charTrim((char *)(XMLString::transcode(chN->getNodeValue())));
            animChannel.source = chSource;
          }
          if ((chN = channelAttr->getNamedItem(XMLString::transcode("target"))) != 0) //channel Node ID
          {
            string  chTarget = charTrim((char *)(XMLString::transcode(chN->getNodeValue())));
            animChannel.source = chTarget;
          }
          animationChannelVec.push_back(animChannel);
        }
        sourceNode = nextGeomNode;
      }
      if (animationID != NULL && !animationSourceVec.empty())
      {
        animationSourceMap[animationID] = animationSourceVec;
        animationSourceIDSequence[animationID] = sourceIDVec;
        animationSamplerMap[animationID] = animationSamplerVec;
        animationChannelMap[animationID] = animationChannelVec;
      }
    }//end of animation check
    child = next;
  }
}
/*==============End of Functions for reading   library_animations=============*/

/*==============Functions for reading   library_controllers==================*/

void ColladaReader::read_LibController_EntityInfoUnderSourceNode ( DOMNode* entityNode, string sSkinEntName,
                                                                   skinStruct *SkinStructData)
{
  //Read the skin source  entities information such as joints , weights , poses etc.
  DOMNodeList *sourceEntityNodes = entityNode->getChildNodes();

  string mainEntityName = XMLString::transcode(entityNode->getNodeName());

  SkinStructData->name = sSkinEntName;

  for (unsigned int ii = 0; ii < sourceEntityNodes->getLength(); ii++)
  {
    DOMNode *sourceEntity = sourceEntityNodes->item(ii);
    if (sourceEntity->getNodeType() != DOMNode::ELEMENT_NODE)
      continue;
    else
    {
      if (!strcmp(XMLString::transcode(sourceEntity->getNodeName()), "float_array") || 
          !strcmp(XMLString::transcode(sourceEntity->getNodeName()), "Name_array"))
      {
        DOMNamedNodeMap *entityAttr = sourceEntity->getAttributes();
        DOMNode *aNsourceEntity;
        char* sEntityCnt = NULL;
        int   nEntityCnt = 0;
        char* content = NULL;
        if ((aNsourceEntity = entityAttr->getNamedItem(XMLString::transcode("count"))) != 0)
        {
          sEntityCnt = charTrim((char *)(XMLString::transcode(aNsourceEntity->getNodeValue())));
          nEntityCnt = atoi(sEntityCnt);
          if (nEntityCnt)
          {
            XMLCh* buffer = new XMLCh[XMLString::stringLen(static_cast<DOMText*>(sourceEntity->getFirstChild())->getNodeValue()) + 1];
            XMLString::copyString(buffer, static_cast<DOMText*>(sourceEntity->getFirstChild())->getNodeValue());
            XMLString::trim(buffer);
            content = XMLString::transcode(buffer);
            SkinStructData->content = content;         
            delete[] buffer;
            XMLString::release(&content);
          }
          else
          {
            // geometryData->geometryAttributes[sSkinEntName] = "";
          }
        }
      }
      if (!strcmp(XMLString::transcode(sourceEntity->getNodeName()), "technique_common"))
      {
        DOMNode* attrbNode = sourceEntity->getFirstChild();
        while (attrbNode)
        {
          DOMNode* nextAttrNode = attrbNode->getNextSibling();
          if (!strcmp(XMLString::transcode(attrbNode->getNodeName()), "accessor"))
          {
            DOMNamedNodeMap *entityAttrbs = attrbNode->getAttributes();
            DOMNode *aNEntityAttrbNode;
            char* sEntityCnt = NULL;
            int   nEntityCnt = 0;
            char* sEntityStrids = NULL;
            int   nEntityStrides = 0;
           
            if( (aNEntityAttrbNode = entityAttrbs->getNamedItem(XMLString::transcode("count"))) != 0)
            {           
              sEntityCnt = charTrim((char *)(XMLString::transcode(aNEntityAttrbNode->getNodeValue())));
              nEntityCnt = atoi(sEntityCnt);
              SkinStructData->count = nEntityCnt;
            }
            if ((aNEntityAttrbNode = entityAttrbs->getNamedItem(XMLString::transcode("stride"))) != 0)
            {
              sEntityStrids = charTrim((char *)(XMLString::transcode(aNEntityAttrbNode->getNodeValue())));
              nEntityStrides = atoi(sEntityStrids);
              SkinStructData->stride = nEntityStrides;
            }
            ///////////////////////////////////
            DOMNode* paramNode = attrbNode->getFirstChild();
            while (paramNode)
            {
              DOMNode* nextNode = paramNode->getNextSibling();
              string NodeName = XMLString::transcode(paramNode->getNodeName());
              if (NodeName == "param")
              {
                DOMNamedNodeMap *paramAttr = paramNode->getAttributes();
                DOMNode *pN;
                //e.g. <param name="JOINT" type="name"/>
                if ((pN = paramAttr->getNamedItem(XMLString::transcode("name"))) != 0)
                {
                  char* paramName = NULL;
                  paramName = charTrim((char *)(XMLString::transcode(pN->getNodeValue())));
                  SkinStructData->key = paramName;
                }
                if ((pN = paramAttr->getNamedItem(XMLString::transcode("type"))) != 0)
                {
                  char* paramType = NULL;
                  paramType = charTrim((char *)(XMLString::transcode(pN->getNodeValue())));
                  SkinStructData->dataType = paramType;
                }
              }
              paramNode = nextNode;
            }
            //////////////////////////////////
          }
          attrbNode = nextAttrNode;
        }
      }
    }
  }//end of for loop for entity children
}

/*============================================================================*/

void ColladaReader :: read_LibController_SkinInfo(DOMNode* skinNode, string controllerID)
{
  DOMNodeList *skinChildNodes = skinNode->getChildNodes();

  string skinSource = getNodeAttributeValue(skinNode, "source");

  skinSource.erase(std::remove(skinSource.begin(), skinSource.end(), '#'), skinSource.end());


  ControllerData * controllerData = new ControllerData[1];
  controllerData->skinSourceName = skinSource;

  for (unsigned int i = 0; i < skinChildNodes->getLength(); i++)
  {
    DOMNode *iN = skinChildNodes->item(i);
    if (iN->getNodeType() != DOMNode::ELEMENT_NODE)
      continue;
    else
    {
      string strSkinEntName, strSkinEntID;

      strSkinEntName = getNodeAttributeValue(iN, "name");
      strSkinEntID = getNodeAttributeValue(iN, "id");
      string NodeName = XMLString::transcode(iN->getNodeName());

      if (NodeName == "bind_shape_matrix")
      {

      }
      if (NodeName == "source")
      {
        skinStruct *SkinStructData = new skinStruct[1];
        if (strSkinEntName.empty())
        {
          read_LibController_EntityInfoUnderSourceNode(iN, strSkinEntID, SkinStructData);
        }
        else
        {
          read_LibController_EntityInfoUnderSourceNode(iN, strSkinEntName, SkinStructData);
        }
        controllerData->skinStructVect.push_back(*SkinStructData);

        //TBD
      }
      if (NodeName == "joints")
      {

      }
      if (NodeName == "vertex_weights")
      {
        DOMNodeList *skinVertWeightNodes = iN->getChildNodes();
        // string mainEntityName = XMLString::transcode(iN->getNodeName());

        DOMNamedNodeMap *entityAttrbs = iN->getAttributes();
        DOMNode *aNEntityAttrbNode;
        int nVertWtCnt = 0;
        aNEntityAttrbNode = entityAttrbs->getNamedItem(XMLString::transcode("count"));
        if (aNEntityAttrbNode != NULL)
        {
          char* sEntityCnt = charTrim((char *)(XMLString::transcode(aNEntityAttrbNode->getNodeValue())));
          controllerData->nVcountCount = atoi(sEntityCnt);
        }
        int numVertWtInputs = 0;

        for (unsigned int ii = 0; ii < skinVertWeightNodes->getLength(); ii++)
        {
          DOMNode *vertWeightNode = skinVertWeightNodes->item(ii);
          if (vertWeightNode->getNodeType() != DOMNode::ELEMENT_NODE)
            continue;
          else
          {
            if (!strcmp(XMLString::transcode(vertWeightNode->getNodeName()), "input"))
            {
              numVertWtInputs++;
            }
            if (!strcmp(XMLString::transcode(vertWeightNode->getNodeName()), "vcount"))
            {
              controllerData->vCountContent = read_TextNodeContent(static_cast<DOMText*>(vertWeightNode));
            }
            if (!strcmp(XMLString::transcode(vertWeightNode->getNodeName()), "v"))
            {
              controllerData->vertWeightsContent = read_TextNodeContent(static_cast<DOMText*>(vertWeightNode));
            }
          }
        }
        controllerData->numofInputs = numVertWtInputs;
      }//end of vertex_weights

    }
  }//end of for loop for skin children 
 
  controllerDataPartMap[controllerID] = *controllerData; 
}

/*===========================================================================*/


void ColladaReader::read_LibController_GetNode(DOMNodeList * controllerList)
{
    DOMNode *controllerNode = controllerList->item(0);
    DOMNode* child = controllerNode->getFirstChild();
    while (child)
    {
      DOMNode* next = child->getNextSibling();
      if (!strcmp(XMLString::transcode(child->getNodeName()), "controller"))
      {
        string controllerID = getNodeAttributeValue(child, "id");
        controllerIDSequence.push_back(controllerID);
       
        //looking for "Skin" Node.
        DOMNode* skinNode = child->getFirstChild();
        while (skinNode)
        {
          DOMNode* nextGeomNode = skinNode->getNextSibling();
          if (!strcmp(XMLString::transcode(skinNode->getNodeName()), "skin"))
          {
            read_LibController_SkinInfo(skinNode, controllerID);
          }
          skinNode = nextGeomNode;
        }
      }
      child = next;
    }
}


/*==============Functions for reading   library_visual_scenes================*/
VisualSceneNode ColladaReader::read_LibVisScene_GetNode(DOMNode* node)
{

  VisualSceneNode vsNode;
  vsNode.VSNodeID = "";  vsNode.VSNodeName = "";  vsNode.VSNodetype = "";
  vsNode.bRotateInfo     = false;  vsNode.bMatrixInfo = false;
  vsNode.bChildNodesInfo = false;  vsNode.bExtraInfo  = false;
  vsNode.sTranslate = "";  vsNode.sRotateX = "";  vsNode.sRotateY = "";
  vsNode.sRotateZ   = "";  vsNode.sScale = "";    vsNode. sMatrix = "";
 
  DOMNamedNodeMap *vsNodeAttr = node->getAttributes();
  DOMNode *vsN; 
  if ((vsN = vsNodeAttr->getNamedItem(XMLString::transcode("id"))) != 0) //source Node ID
  {
    vsNode.VSNodeID = charTrim((char *)(XMLString::transcode(vsN->getNodeValue())));
  }
  if ((vsN = vsNodeAttr->getNamedItem(XMLString::transcode("name"))) != 0) //source Node ID
  {
    vsNode.VSNodeName = charTrim((char *)(XMLString::transcode(vsN->getNodeValue())));
  }
  if ((vsN = vsNodeAttr->getNamedItem(XMLString::transcode("type"))) != 0) //source Node ID
  {
    vsNode.VSNodetype = charTrim((char *)(XMLString::transcode(vsN->getNodeValue())));
  }
  
  DOMNode* childNode = node->getFirstChild();
  while (childNode)
  {
    DOMNode* nextNode = childNode->getNextSibling();
    string NodeName = XMLString::transcode(childNode->getNodeName());
 
    if (childNode->getNodeType() != DOMNode::ELEMENT_NODE)
    {
      childNode = nextNode;
      continue;
    } 

    if (NodeName == "translate")
    {
      vsNode.bRotateInfo = true;
      vsNode.sTranslate = read_TextNodeContent(static_cast<DOMText*>(childNode));
    }
    if (NodeName == "translate")
    {
      vsNode.bRotateInfo = true;
      vsNode.sTranslate = read_TextNodeContent(static_cast<DOMText*>(childNode));
    }
    if (NodeName == "rotate"  || NodeName == "scale")
    {
      vsNode.bRotateInfo = true;
      DOMNamedNodeMap *vsNodeRotAttr = childNode->getAttributes();
      DOMNode *vsRotN;
      string rotType = "";
      if ((vsRotN = vsNodeRotAttr->getNamedItem(XMLString::transcode("sid"))) != 0)
      {
        rotType = charTrim((char *)(XMLString::transcode(vsRotN->getNodeValue())));       
      }
      if (rotType == "rotationX")
      {
        vsNode.sRotateX = read_TextNodeContent(static_cast<DOMText*>(childNode));
      }
      if (rotType == "rotationY")
      {
        vsNode.sRotateY = read_TextNodeContent(static_cast<DOMText*>(childNode));
      }
      if (rotType == "rotationZ")
      {
        vsNode.sRotateZ = read_TextNodeContent(static_cast<DOMText*>(childNode));
      }
      if (rotType == "scale")
      {
        vsNode.sScale = read_TextNodeContent(static_cast<DOMText*>(childNode));
      }
    }
    if (NodeName == "matrix")
    {
      vsNode.bMatrixInfo = true;
      vsNode.sMatrix = read_TextNodeContent(static_cast<DOMText*>(childNode));     
    }    
    if (NodeName == "node")
    {
      cout << NodeName << endl;
      vsNode.bChildNodesInfo = true;
      VisualSceneNode childVSNode = read_LibVisScene_GetNode(childNode);
      vsNode.VSChildNodes.push_back(childVSNode);
    }

    if (NodeName == "extra")
    {
      vsNode.bExtraInfo = true;
    
    }

    childNode = nextNode;
  }
  return vsNode;
}


/*===========================================================================*/

/*The <visual_scene> element forms the root of the scene graph topology.
<visual_scene> contains one or more <node> elements.
The <node> element embodies the hierarchical relationship of elements in a scene.
<node> can have a wide range of child elements, including <node> elements themselves.
The transformation elements transform the coordinate system of the <node> element.
Transformation elements are:
<lookat>, <matrix>, <rotate>, <scale>, <skew>, <translate>
<instance_geometry> allows the node to instantiate a geometry object*/

void ColladaReader::read_LibVisScene_Visual_Scenes ( DOMNodeList *vsceneElemList)
{
  DOMNode *vsceneNode = vsceneElemList->item(0);

  DOMNode* child = vsceneNode->getFirstChild();
  while (child)
  {
    DOMNode* next = child->getNextSibling();
    char* name = XMLString::transcode(child->getNodeName());
    cout << name << endl;
    vector<VisualSceneNode> vSNodeVec;

    if (child->getNodeType() != DOMNode::ELEMENT_NODE)
    {
      child = next;
      continue;
    }   

    if (!strcmp(XMLString::transcode(child->getNodeName()), "visual_scene"))
    {
     
      DOMNode* node = child->getFirstChild();
      while (node)
      {
        DOMNode* nextVSNode = node->getNextSibling();
        if (!strcmp(XMLString::transcode(node->getNodeName()), "node"))
        {
          cout << node->getNodeName() << endl;
          VisualSceneNode vsNode = read_LibVisScene_GetNode(node);
          vSNodeVec.push_back(vsNode);
        }
        if (!strcmp(XMLString::transcode(node->getNodeName()), "extra"))
        {

        }
        node = nextVSNode;
      }
    }//end of visual_scene check
    child = next;
  }
}
/*===========================================================================*/

void ColladaReader::readlibraryPhysicsScenes(DOMNodeList* phySceneElemList)
{

  for (unsigned int i = 0; i < phySceneElemList->getLength(); i++)
  {
    DOMNode *iN = phySceneElemList->item(i);
  }
}
/*===========================================================================*/


int ColladaReader ::readColladaXMLFile(string xmlFile)
{

  try
  {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch)
  {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Error during initialization! :\n" << message << "\n";
    XMLString::release(&message);
    return 0;
  }

  XercesDOMParser parser;
  parser.setValidationScheme(XercesDOMParser::Val_Auto);
  parser.setIncludeIgnorableWhitespace(true);

  ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
  parser.setErrorHandler(errHandler);

  //const char* xmlFile = "./model.dae";

  try
  {
    parser.parse((char*)xmlFile.c_str());
    DOMDocument *doc = parser.getDocument();
    DOMElement *root = doc->getDocumentElement();
    if (root == NULL)
    {
      return 0;
    }
    // write(root);

    // Read the asset
    /*DOMNodeList *assetElemList = root->getElementsByTagName(XMLString::transcode("asset"));
    DOMNode *node0 = assetElemList->item(0);

    DOMNodeList *physicsSceneElemList = root->getElementsByTagName(XMLString::transcode("library_physics_scenes"));
    readlibraryPhysicsScenes(physicsSceneElemList);*/

    XMLCh* l_ch = XMLString::transcode("library_geometries");
    DOMNodeList *geomElemList = root->getElementsByTagName(l_ch);
    read_LibGeom_Geometries(geomElemList);
   
  //  l_ch = XMLString::transcode("library_animations");
  //  DOMNodeList *animElemList = root->getElementsByTagName(l_ch);
  //  if(animElemList->getLength())
  //   read_LibAnim_Animation(animElemList);

 //   l_ch = XMLString::transcode("library_visual_scenes");
 //   DOMNodeList *vsceneElemList = root->getElementsByTagName(l_ch);
 //   if (vsceneElemList->getLength())
 //     read_LibVisScene_Visual_Scenes(vsceneElemList);

    l_ch = XMLString::transcode("library_controllers");
    DOMNodeList *controllerList = root->getElementsByTagName(l_ch);
    if (controllerList->getLength())
      read_LibController_GetNode(controllerList);


    XMLString::release(&l_ch);
  }
  catch (const XMLException& toCatch)
  {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Exception message is: \n" << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (const DOMException& toCatch)
  {
    char* message = XMLString::transcode(toCatch.msg);
    cout << "Exception message is: \n" << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (...)
  {
    cout << "Unexpected Exception \n";
    return -1;
  }
  delete errHandler;
  return 1;
}

