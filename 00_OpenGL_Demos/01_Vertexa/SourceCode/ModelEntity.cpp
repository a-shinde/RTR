
#include "ModelEntity.h"
extern std::ofstream g_log_file;

ModelEntity::ModelEntity()
{
  morphedGeometryList.clear();
  reader = new ColladaReader();
}


ModelEntity::~ModelEntity()
{
  deleteAllPartsInModel();
  delete reader;
}



void ModelEntity::initShaders(GLuint shaderProgramObject)
{
    morphMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_morph_weight");
}

/*===========================================================================*/
ModelPart* ModelEntity :: constructPart(string partID)
{
  ModelPart *mPart = new ModelPart[1];
  mPart->partID = partID;
  return mPart;
}

/*===========================================================================*/

void ModelEntity::addPartToModel(ModelPart *modelPart)
{
  modelPartList.push_back(modelPart);
}

/*===========================================================================*/
void ModelEntity::deleteAllPartsInModel()
{
    for (list <ModelPart *>::iterator item = modelPartList.begin(); item != modelPartList.end(); ++item)
    {
        //TBD getting error here        
        //delete (*item);
    }
    
  modelPartList.clear();
}

/*===========================================================================*/

void ModelEntity::addControllerToModel(Controller *controlllerData)
{
  controllerList.push_back(controlllerData);
}

/*===========================================================================*/
void ModelEntity::deleteAllControllersInModel()
{
  for (list <Controller *>::iterator item = controllerList.begin(); item != controllerList.end(); ++item)
    delete (*item);
  controllerList.clear();
}

/*===========================================================================*/

Controller* ModelEntity:: getControllerForGeometry(string geometryID)
{
  for (list <Controller *>::iterator item = controllerList.begin(); item != controllerList.end(); ++item)
  {
    string skinSource = (*item)->skinSource;
    if (!strcmp(skinSource.c_str(), geometryID.c_str()))
    {
      return ((*item));
    }
  }
}


/*===========================================================================*/

vector<string> ModelEntity:: checkForMorphedTargetForPart(string partID)
{
  vector<string> morphedTargetForGeom;
  morphedTargetForGeom.clear();
  vector<string> geometryPartIDSeqence = reader->get_LibGeom_PartIDSequence();
  for (int i = 0; i < geometryPartIDSeqence.size(); i++)
  {
    size_t found = geometryPartIDSeqence[i].find("morph");
    if (found != string::npos)
    {      
      string str = geometryPartIDSeqence[i].substr(0, found-1);
      if (partID == str)
      {
        morphedGeometryList.insert(geometryPartIDSeqence[i]);
        morphedTargetForGeom.push_back(geometryPartIDSeqence[i]);
      }     
    }
  }
  return morphedTargetForGeom;
}

/*===========================================================================*/

vector<vector<ELEMENT>>ModelEntity::GetElementContainerFromGeometry(GeometryData geometry, vector<string> &ElementNames)
{
  vector<vector<ELEMENT>> ElementContainer;

  map<string, vector<int>> elementMap = geometry.pElementData;
  map<string, vector<int>> elementInfo = geometry.pElementInfo;
 
  for (map<string, vector<int>> ::iterator itr = elementMap.begin(); itr != elementMap.end(); itr++)
  {
    string elementName = (*itr).first;
    vector<int> pElementRawData = (*itr).second;

    map<string, vector<int>> ::iterator itrInfo;
    for (itrInfo = elementInfo.begin(); itrInfo != elementInfo.end(); itrInfo++)
    {
      if ((*itrInfo).first == elementName)
      {
        break;
      }
      else if (itrInfo == elementInfo.end())
      {
        elementInfo.erase(itrInfo);
        return ElementContainer;
      }
    }
    vector<int> infoVector = (*itrInfo).second;
    int nElementCnt = infoVector[0];
    int numVertices = infoVector[1];
    int numAttributes = infoVector[2];

    int numIndices = nElementCnt*numVertices * numAttributes;
    int nElementSize = numVertices * numAttributes;
    int index = 0;
    vector<ELEMENT> Elements;
    Elements.resize(nElementCnt);
    ElementNames.push_back(elementName);
    for (int i = 1; i < nElementCnt + 1; i++)
    {
      vector<vector<int>> entityVect;
      entityVect.resize(numAttributes);
      Element element;
      element.numofAttributes = numAttributes;
      element.AttributeIDs.resize(numAttributes);

      /*we may have 2,3 or 4 entity attributes like Vertex,
      Normal, Texture Color etc.so setting them dynamically.
      element.AttributeIDs[0] will have VERTEX id vector
      element.AttributeIDs[1] will have NORMAL id vector
      element.AttributeIDs[2] will have TEXTURE id vector
      */

      for (int ii = 0; ii < numAttributes; ii++)
      {
        entityVect[ii].resize(numVertices);
        element.AttributeIDs[ii].resize(numVertices);
      }
      int count = 0;
      for (int j = 0; j < nElementSize; j++)
      {
        if (numVertices == 3)
        {
          /*If there are 3 Verts, 3 Norms, 3 Text then elment size is 9,
          entityVect[0] will have 3 verts at entityVect[0][0], entityVect[0][1], entityVect[0][2], similarly
          entityVect[1] will have 3 norms at entityVect[1][0], entityVect[1][1], entityVect[1][2] and so on.
          so count is for second index of entityVect i.e. entityVect[][count].*/
          index = ((i - 1)*nElementSize + j) % numAttributes;
          if (j > 2 && index == 0)
            count++;
          entityVect[index][count] = pElementRawData[((i - 1)*nElementSize + j)];
        }
        else if (numVertices == 2)
        {
          index = ((i - 1)*nElementSize + j) % numAttributes;
          if (j == numAttributes)
            count++;
          entityVect[index][count] = pElementRawData[((i - 1)*nElementSize + j)];
        }
      }
      for (int ii = 0; ii < numAttributes; ii++)
      {
        element.AttributeIDs[ii] = entityVect[ii];
      }
      Elements[i - 1] = element;
    }//end of for loop for elements
    ElementContainer.push_back(Elements);
  }//map iterator loop
  return ElementContainer;
  
}
/*===========================================================================*/

vector<vector<GeometryEntity>> ModelEntity::GetGeometryEntityContainerFromGeometry(GeometryData geometry)
{
  vector<vector<GeometryEntity>> GeometryEntityContainer;

  map<string, vector<int>>  countStrideMap = geometry.countStrideMap;
  map<string, string>  geomAttributes = geometry.geometryAttributes;
  vector<string> attributeSequence = geometry.attributeSequence;
  int NumOfGeomAttribs = geometry.numofGeomAttribs;
  GeometryData geometryMorphed;  
  GeometryEntityContainer.resize(NumOfGeomAttribs);
  if (!countStrideMap.empty() && !geomAttributes.empty())
  {
    int attributeNum = 0;
    for (int jj = 0; jj < attributeSequence.size(); jj++)
    {
      map<string, string> ::iterator itrGeomAttrb
        = geomAttributes.find(attributeSequence[jj]);

      map<string, vector<int>> ::iterator itrCount
        = countStrideMap.find(attributeSequence[jj]);

      vector<int> temp = (*itrCount).second;
      int attributeCount = temp[0];
      int attribStride = temp[1];

      if (attributeCount == 0)//sometimes there are no attributes provided but given only name 
        continue;

      if (itrGeomAttrb != geomAttributes.end() &&
        itrCount != countStrideMap.end())
      {
        string content = (*itrGeomAttrb).second;
        stringstream tokenizer;
        tokenizer.str(content);

        vector<double> tempVect;
        tempVect.resize(attributeCount*attribStride);
        for (int i = 0; i < (attributeCount*attribStride); i++)
        {
          tokenizer >> tempVect[i];
        }

        vector<GeometryEntity> geomEntites;
        geomEntites.resize(attributeCount);

        int entityID = 0;
        for (int i = 0; i < attributeCount; i++)
        {
          GeometryEntity newGeomEntity;
          newGeomEntity.entityCords = new double[attribStride];
          for (int j = 0; j < attribStride; j++)
          {
            newGeomEntity.entityCords[j] = tempVect[i*attribStride + j];
          }
          newGeomEntity.entityID = entityID;
          newGeomEntity.entityStride = attribStride;
          entityID++;
          geomEntites[i] = newGeomEntity;
        }
        GeometryEntityContainer[attributeNum] = geomEntites;
        attributeNum++;
      }
      else
      {
        return GeometryEntityContainer;
      }
    }//end of for loop for attributeSequence
  }//end of countstride if condition 
  return GeometryEntityContainer;
}


/*===========================================================================*/
/*Filled data in maps in reader class will be retrived here.
The maps will have key as position, normal, color etc.
Since map will store the data in sorted order.
Sequence of these geometry attributes has been stored attributeSequence.
And then this vector is looped and string stored will be used as a
search key in map.
This way same sequence will be maintained while storing the data in model.*/

bool ModelEntity :: setMeshContainer()
{
  map<string, GeometryData> geometryPartMap = reader->get_LibGeom_PartMap();
  vector<string> geometryPartIDSeqence = reader->get_LibGeom_PartIDSequence();

  int partcount = 0;
  for (vector<string> ::iterator geometryPartIDSeqItr = geometryPartIDSeqence.begin();
       geometryPartIDSeqItr != geometryPartIDSeqence.end(); 
       geometryPartIDSeqItr++)
  {
    string geometryID = "";
    vector<vector<ELEMENT>> ElementContainer;
    vector<string> ElementNames;
    vector<vector<GeometryEntity>> GeometryEntityContainer;
    
    ModelPart* part;

    map<string, GeometryData>::iterator geometryPartMapItr  = geometryPartMap.find(*geometryPartIDSeqItr);
    if (geometryPartMapItr != geometryPartMap.end())
    {
      geometryID = geometryPartMapItr->first;
      GeometryData geometry = geometryPartMapItr->second;
      
      bool morphedTargetAvailabel = false;
      vector<string> morphedTargetStrVec = checkForMorphedTargetForPart(geometryID);
      if (morphedTargetStrVec.size() != 0)
        morphedTargetAvailabel = true;

      if (morphedGeometryList.find(geometryID) != morphedGeometryList.end())
      {
        continue;
      }
      part  = constructPart(geometryID);
      part->IsMorphedTargetAvailable = morphedTargetAvailabel;      
      
      part->numGeomAttribs = geometry.numofGeomAttribs;
      part->numMorphedGeomAttribs = 0;
      GeometryEntityContainer = GetGeometryEntityContainerFromGeometry(geometry);
      ElementContainer = GetElementContainerFromGeometry(geometry, ElementNames);

      //Assumption : The elements for geometry and its morphed target are same 
      if (part->IsMorphedTargetAvailable)
      {
        part->numGeomAttribs = geometry.numofGeomAttribs;
        //we can have multiple morphed target for one geometry so all these targets were found aand stoared 
        for (vector<string> ::iterator morpTargtItr = morphedTargetStrVec.begin();
             morpTargtItr < morphedTargetStrVec.end(); morpTargtItr++)
        {
          GeometryData geometryMorphed = geometryPartMap.find((*morpTargtItr))->second;
          if (geometryMorphed.geomID != (*morpTargtItr)) 
              return false;

         // vector<vector<ELEMENT>> morphedElementContainer;
         // vector<string> morphedElementNames;
         vector<vector<GeometryEntity>> morphedGeometryEntityContainer;

          part->numMorphedGeomAttribs += geometryMorphed.numofGeomAttribs;
          morphedGeometryEntityContainer = GetGeometryEntityContainerFromGeometry(geometryMorphed);
          //morphedElementContainer = GetElementContainerFromGeometry(geometryMorphed, morphedElementNames);

          GeometryEntityContainer.insert(GeometryEntityContainer.end(),
            morphedGeometryEntityContainer.begin(),
            morphedGeometryEntityContainer.end());

         /* ElementContainer.insert(ElementContainer.end(),
            morphedElementContainer.begin(),
            morphedElementContainer.end());

          ElementNames.insert(ElementNames.end(),
            morphedElementNames.begin(),
            morphedElementNames.end());*/
        }
      }
      ///////////////For controller///////////////////////////////////////////
     
      Controller* controllerForgeom = getControllerForGeometry(geometryID);
      if (controllerForgeom != NULL)
      {
        part->ControllerForGeom = controllerForgeom->controllerVertexWeightVect;        
      }
      ////////////////////////////////////////////////////////////////////////
      part->GeometryEntityContainer = GeometryEntityContainer;
      part->ElementContainer = ElementContainer;
      part->ElementSequence = ElementNames;
      addPartToModel(part);
      partcount++;
    }//end of if condition for geometryPartMap
  }//end of geometryPartIDSeqence for loop


  ////////////////Output////////////////////////////////////////////////////
/*  char elementOutPutFile[] = "./ElementOutput.txt";
  ofstream  elementFile;
  elementFile.open(elementOutPutFile, ios::out);
  
  char GeomOutPutFile[] = "./GeometryOutput.txt";
  ofstream  geomFile;
  geomFile.open(GeomOutPutFile, ios::out);
  
  
  list <ModelPart *> partList = getPartList();

  for (list <ModelPart *>::iterator itrPart = partList.begin(); itrPart != partList.end(); itrPart++)
  {
    elementFile << (*itrPart)->partID <<endl;
    geomFile << (*itrPart)->partID <<endl;

    vector<vector<GeometryEntity>> GeometryEntityContainer = (*itrPart)->GeometryEntityContainer;
    int attributeCount = 0;
    for (vector<vector<GeometryEntity>>::iterator itrMain = GeometryEntityContainer.begin(); itrMain != GeometryEntityContainer.end(); itrMain++)
    {

      vector<GeometryEntity> GeometryEntityVect = (*itrMain);
     
      switch (attributeCount)
      {
     
      case 0:
        geomFile <<"vertices"<<endl;
        break;

      case 1:
        geomFile << "normals" << endl;
        break;

      case 2:
        geomFile << "textures" << endl;
         break;
      case 3:
        geomFile << "colors" << endl;
        break;

      default:
        geomFile << "Unknown" << endl;
        break;
      }
     
      for (vector<GeometryEntity>::iterator itrEnt = GeometryEntityVect.begin(); itrEnt != GeometryEntityVect.end(); itrEnt++)
      {
        GeometryEntity geomEntity = (*itrEnt);
        int      ntityID = geomEntity.entityID;
        int      ntityStride = geomEntity.entityStride;
       
        for (int jj = 0; jj < ntityStride; jj++)
        {
          geomFile << geomEntity.entityCords[jj] << "    ";
        }
        geomFile << endl;
       
      }
      geomFile << "/*******************************************" << endl;
      attributeCount++;
    }

    
    //Elment Extraction.
    vector<vector<ELEMENT>> ElementContainer = (*itrPart)->ElementContainer;
    vector<string> ElmentLabels = (*itrPart)->ElementSequence;
    int cnt = 0;
    for (vector<vector<ELEMENT>>::iterator itrElemt = ElementContainer.begin(); itrElemt != ElementContainer.end(); itrElemt++)
    {
      vector<ELEMENT> Elements = (*itrElemt);
      elementFile << ElmentLabels[cnt] << endl;
      for (vector<ELEMENT>::iterator itr = Elements.begin(); itr != Elements.end(); itr++)
      {
        int num = (*itr).numofAttributes;
        vector <vector<int>> attrIDs = (*itr).AttributeIDs;

        for (int ii = 0; ii < num; ii++)
        {
          for (vector<int>::iterator elemItr = attrIDs[ii].begin(); elemItr != attrIDs[ii].end(); elemItr++)
          {
            elementFile << (*elemItr) << "   ";
          }
        }
        elementFile << endl;
      }
      elementFile << "/*******************************************" << endl;
      cnt++;
    }   
  } 
  elementFile.close();
  geomFile.close();*/
  /////////////////////////////////////////////////////////////////////////////
  return true;
}
/*===========================================================================*/


bool ModelEntity::setControllerData()
{
  map<string, ControllerData> controllerDataMap = reader->get_LibController_PartMap();
  vector<string> controllerIDSequence = reader->get_LibController_PartIDSequence();

  int partcount = 0;
  for (vector<string> ::iterator controllerIDSeqItr = controllerIDSequence.begin();
    controllerIDSeqItr != controllerIDSequence.end();
    controllerIDSeqItr++)
  {
    string controllerID = "";
    vector<vector<ELEMENT>> ElementContainer;
    vector<string> ElementNames;
    vector<vector<GeometryEntity>> GeometryEntityContainer;

    map<string, ControllerData>::iterator ControllerMapItr = controllerDataMap.find(*controllerIDSeqItr);
    if (ControllerMapItr != controllerDataMap.end())
    {
      controllerID = ControllerMapItr->first;
      ControllerData controller = ControllerMapItr->second;

      Controller *newController = new Controller[1];
      newController->skinSource = controller.skinSourceName;
      vector<controllerVertexWeight> controllerVertWtVector;

      vector<string> JointVector;
      vector<vector<float>> TrasformPosesMatricsVector;//inside vec will be of 16 sizes
      vector<float> weightCordinateVector;

      vector<int> vcountVector;
      vector<int> vertexWtVector;

      stringstream tokenizer;

      /////////getting contents from source tab//////////////////
      for (int i = 0; i < controller.skinStructVect.size(); i++)
      {
        string type     = controller.skinStructVect[i].key;
        int    count    = controller.skinStructVect[i].count;
        int    stride   = controller.skinStructVect[i].stride;
        string datatype = controller.skinStructVect[i].dataType;
        int numIndices  = count*stride;      

        if (!strcmp(type.c_str(),"JOINT")&& !strcmp(datatype.c_str(),"name"))
        {
          tokenizer.clear();
          tokenizer.str(controller.skinStructVect[i].content);         
          JointVector.resize(numIndices);
          for (int i = 0; i < numIndices; i++)
          {
            tokenizer >> JointVector[i];
          }
        }
        else if (!strcmp(type.c_str(), "TRANSFORM") && !strcmp(datatype.c_str(), "float4x4"))
        {
          tokenizer.clear();
          tokenizer.str(controller.skinStructVect[i].content);
          for (int i = 0; i < count; i++)
          {
            vector<float> tempVect;
            tempVect.resize(stride);
            for (int j = 0; j < stride; j++)
            {
              tokenizer >> tempVect[j];
            }
            TrasformPosesMatricsVector.push_back(tempVect);
          }
        }
        else if (!strcmp(type.c_str(), "WEIGHT") && !strcmp(datatype.c_str(), "float"))
        {
          tokenizer.clear();
          tokenizer.str(controller.skinStructVect[i].content);
          weightCordinateVector.resize(numIndices);
          for (int i = 0; i < numIndices; i++)
          {
            tokenizer >> weightCordinateVector[i];
          }
        }
      }
      //////////vertex_weights///////////////////////

      string vcountContent = controller.vCountContent;
      tokenizer.clear();
      tokenizer.str(vcountContent);
      int sum = 0;
      vcountVector.resize(controller.nVcountCount);
      for (int i = 0; i < controller.nVcountCount; i++)
      {
        tokenizer >> vcountVector[i];
        sum += vcountVector[i];
      }
    
      /*Note :
        Sum of all the ints from vcount vector multiplied by number of inputs 
        will be the total num of ints availble in "v"
        e.g.
        <vertex_weights count="4">
        <input semantic="JOINT" source="#joints" offset="0"/>
        <input semantic="WEIGHT" source="#weights" offset="1"/>
        <vcount>3 2 2 3</vcount>
        <v> 1 0 0 1 1 2  1 3 1 4   1 3 2 4  1 0 3 1 2 2  </v> </vertex_weights> 
        In this case number of inputs are 2 , joint and weights and sum of all ints in vcount 
        is 10 so total num of int under v will be 20.
      */

      int nVertWtCount = sum*controller.numofInputs;

      string vertexwtContent = controller.vertWeightsContent;
      tokenizer.clear();
      tokenizer.str(vertexwtContent);
      vertexWtVector.resize(nVertWtCount);

      for (int i = 0; i < nVertWtCount; i++)
      {
        tokenizer >> vertexWtVector[i];
      }

      //Construct a struct with vcount weightIndex and weightvalues
      //that will be appened to Geometry Vertex Positions.


      //Infulencing vert wt num for each vcount index = vocunt index * num of inputs
      //In above example for first vcount index 3 there will be first 6 ints from v.
      
      int indexVertWt = 0;
      for (int i = 0; i < controller.nVcountCount; i++)
      {
        int vcount = vcountVector[i];
        int influencingIndexCnt = vcount*controller.numofInputs;
        controllerVertexWeight cntrlWtStruct;
        cntrlWtStruct.numofVCount = vcount;
        for (int j = 0; j < influencingIndexCnt; j++)
        {
          int index = vertexWtVector[indexVertWt];
          if (j == 0 || ((j >= controller.numofInputs) && (j%controller.numofInputs == 0)))
          {
              cntrlWtStruct.vertJointNames.push_back(JointVector[index]);
          }
          else
          {
              cntrlWtStruct.vertWeightValues.push_back(weightCordinateVector[index]);
          }
          indexVertWt++;
        }
        controllerVertWtVector.push_back(cntrlWtStruct);
      }
      newController->controllerVertexWeightVect = controllerVertWtVector;
      addControllerToModel(newController);

    }//end of map's if condition
  }//end of main for loop


  return true;
}
/*===========================================================================*/

bool ModelEntity::setBoneInfo(map<string, GLuint> input_bone_mapping)
{
    if (!input_bone_mapping.empty())
    {
        for (map<string, GLuint>::iterator itr = input_bone_mapping.begin();
            itr != input_bone_mapping.end(); itr++)
        {
            string boneName = itr->first;
            GLuint boneID = itr->second;
            std::replace(boneName.begin(), boneName.end(), '.', '_');
            boneInfo.bone_mapping.insert(std::pair<string, GLuint>(boneName, boneID));
        }
        return true;
    }
    else
        return false;
}


bool ModelEntity::loadModel(string xmlFile)
{
 // ModelEntity *model = NULL;
  int success = reader->readColladaXMLFile(xmlFile) ;
  if (success != 1)
  {
      g_log_file << "error at Importing meshes from ModelEntity." << endl;
      return false;
  }
  setControllerData();
  setMeshContainer();  
  return true;
}


/*===========================================================================*/
void ModelEntity::getGeometryData()
{
    //auto start = std::chrono::system_clock::now();

    //std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
    //g_log_file << "ModelEntity Loading time " << elapsed_seconds.count() << endl;

    int NumParts = (int)modelPartList.size();

    vector<vector<int>> NumGeomAttributeByPart; //first index for part second type count
    vector<vector<int>> NumElementsByMesh;//first index for part second type count

    NumGeomAttributeByPart.resize(size_t(NumParts));
    NumElementsByMesh.resize(size_t(NumParts));

    int partCount = 0;
    for (list <ModelPart *>::iterator itrPart = modelPartList.begin(); itrPart != modelPartList.end(); itrPart++)
    {
        vector<vector<GeometryEntity>> GeometryEntityContainer = (*itrPart)->GeometryEntityContainer;
        vector<vector<GLfloat>> gGeometryEntity; //vector which will contain all the data in single array 

        bool morphTargetAvailable = (*itrPart)->IsMorphedTargetAvailable;
        int numEntityCount = (int)(*itrPart)->GeometryEntityContainer.size();
        int elmentSizeCount = 0; //Each elment will have indices for vert, norm
        int    numMorphedGeomAttribs = 0;
        int    numofGeomAttribs = (*itrPart)->numGeomAttribs;
        if (morphTargetAvailable)
        {
            numMorphedGeomAttribs = (*itrPart)->numMorphedGeomAttribs;
        }
        gGeometryEntity.resize(numEntityCount);

        NumGeomAttributeByPart[partCount].resize(numEntityCount);
        //gVboGeometry[partCount] = new GLuint[numEntityCount];
        (*itrPart)->shaderBuffer.vbo_vertex = new GLuint[numEntityCount];
        (*itrPart)->shaderBuffer.vbo_vertex_size = numEntityCount;

        int attributeCount = 0;
        for (vector<vector<GeometryEntity>>::iterator itrMain = GeometryEntityContainer.begin(); itrMain != GeometryEntityContainer.end(); itrMain++)
        {
            vector<GeometryEntity> GeometryEntityVect = (*itrMain);

            NumGeomAttributeByPart[partCount][attributeCount] = (int)(*itrMain).size();

            for (vector<GeometryEntity>::iterator itrEnt = GeometryEntityVect.begin(); itrEnt != GeometryEntityVect.end(); itrEnt++)
            {
                GeometryEntity geomEntity = (*itrEnt);
                //int      ntityID = geomEntity.entityID;
                int      ntityStride = geomEntity.entityStride;
                int entityCount = 0;
                for (int jj = 0; jj < ntityStride; jj++)
                {
                    gGeometryEntity[attributeCount].push_back(geomEntity.entityCords[jj]);
                    entityCount++;
                } //end of for loop for each stride
            }//end of for loop for each geom entity process
            attributeCount++;
        }//end of for loop for geometry process

         //Element Extraction.
        vector<vector<ELEMENT>> ElementContainer = (*itrPart)->ElementContainer;
        vector<string> ElementLabels = (*itrPart)->ElementSequence;

        int perMeshElementCnt = 0;//we have more than 1 elments per mesh 
        int numofEleType = (int)ElementContainer.size();

        if (ElementContainer.size() != ElementLabels.size())
        {
            return;
        }
        //gVboElement[partCount] = new GLuint[numofEleType];
        (*itrPart)->shaderBuffer.ebo = new GLuint[numofEleType];
        (*itrPart)->shaderBuffer.ebo_size = numofEleType;
        (*itrPart)->shaderBuffer.elementCounts.reserve(numEntityCount);

        NumElementsByMesh[partCount].resize(numofEleType);

        vector<vector<vector<unsigned short>>> elementVec;
        elementVec.resize(numofEleType);

        for (vector<vector<ELEMENT>>::iterator itrElemt = ElementContainer.begin(); itrElemt != ElementContainer.end(); itrElemt++)
        {
            vector<ELEMENT> Elements = (*itrElemt);
            int num = (*itrElemt).at(0).numofAttributes;
            elementVec[perMeshElementCnt].resize(num);
            int elementIndexCnt = 0;

            for (vector<ELEMENT>::iterator itr = Elements.begin(); itr != Elements.end(); itr++)
            {
                vector <vector<int>> attrIDs = (*itr).AttributeIDs;
                int eachElmentSizeCnt = 0;


                for (int ii = 0; ii < (*itr).numofAttributes; ii++)
                {
                    for (vector<int>::iterator elemItr = attrIDs[ii].begin(); elemItr != attrIDs[ii].end(); elemItr++)
                    {
                        elementVec[perMeshElementCnt][ii].push_back(*elemItr);
                    }
                }
                elementIndexCnt++;
            }

            NumElementsByMesh[partCount][perMeshElementCnt] = elementIndexCnt *num;
            perMeshElementCnt++;
        }//end of for loop for elment process

         //////////////////////////////////////////////////////////////////////////

         //////Processing of geometric entities other than vertex//////////////////

         /*The Vertex Index will be taken from vertexIndex vector and then corrosponding
         normal, texture etc indices will be colloected from their respective vectors.
         With these indices of normal and texture their corrosponding co-ordinates will
         be taken from respective co-ordinate vectors.
         These co-ordinates of noraml and texture and other attributes(except from verteices)
         will be stored in new vector at the Vertex Index.
         e.g. if I have 87 number vertex Index and its corrosponding normal Index is 8,
         then from normal cordinate vector co-ordinates will be collected and it will be
         stored against Index number 87 in newly created normal co-ordinate vector which
         will be used to send to glBufferData.*/

        vector<vector<vector<GLfloat>>> newGeomEntityCords;//1st attribute num, second at index third GLfloat 3 values.
        newGeomEntityCords.resize(attributeCount);
        int newAttributeCount = 0;
        int vectSize = 0;
        ////////////////////For morphed targets//////////////////////////////////////

        if (morphTargetAvailable)
        {
            int newGeomAttributeCount = 0;
            int newEleAttributeCount = 0;

            //Assumption : The elements for geometry and its morphed target are same 

            for (int kk = 0; kk < numofEleType; kk++)
            {
                if (elementVec[kk].size() > 1)// we should  have more than 1 geom attributes in element to process them 
                {
                    for (vector<vector<GeometryEntity>>::iterator itrMain
                        = GeometryEntityContainer.begin();
                        itrMain != GeometryEntityContainer.end();
                        itrMain++)
                    {

                        vector<GeometryEntity> GeometryEntityVect = (*itrMain);

                        if (newGeomAttributeCount == 0)
                        {
                            vectSize = GeometryEntityVect.size();//always assign same size of vertex vect to all other vects.
                            newGeomAttributeCount++;
                            newEleAttributeCount++;
                            continue;
                        }

                        newGeomEntityCords[newGeomAttributeCount].resize(vectSize);
                        int numMismatchVec = 0;
                        for (int i = 0; i < elementVec[kk][0].size(); i++)
                        {
                            int indexVert = elementVec[kk][0][i];
                            int indexEntity = elementVec[kk][newEleAttributeCount][i];


                            if (indexEntity >= GeometryEntityVect.size())
                            {
                                //TBD Someting is wrong here.
                                int needtoFix = 0;
                                indexEntity = GeometryEntityVect.size() - 1;
                            }

                            GeometryEntity geomEntity = GeometryEntityVect[indexEntity];

                            int  ntityStride = geomEntity.entityStride;

                            if (newGeomEntityCords[newGeomAttributeCount][indexVert].empty())
                            {
                                for (int jj = 0; jj < ntityStride; jj++)
                                {
                                    newGeomEntityCords[newGeomAttributeCount][indexVert].push_back(geomEntity.entityCords[jj]);
                                }
                            }
                            else
                            {
                                for (int index = 0; index < newGeomEntityCords[newGeomAttributeCount][indexVert].size(); index++)
                                {
                                }
                            }
                        }
                        newGeomAttributeCount++;
                        newEleAttributeCount++;
                        if (newGeomAttributeCount%numofGeomAttribs == 0 && newGeomAttributeCount >= numofGeomAttribs)
                        {
                            newEleAttributeCount = 0;
                        }

                    }//end of GeometryEntityContainer loop	

                }//end of if condition to check element size	

            }//end of kk loop 
            newAttributeCount = newGeomAttributeCount;
        }
        else
        {
            for (vector<vector<GeometryEntity>>::iterator itrMain =
                GeometryEntityContainer.begin();
                itrMain != GeometryEntityContainer.end();
                itrMain++)
            {
                vector<GeometryEntity> GeometryEntityVect = (*itrMain);
                if (newAttributeCount == 0)
                {
                    vectSize = GeometryEntityVect.size();//always assign same size of vertex vect to all other vects.
                    newAttributeCount++;
                    continue;
                }
                int entityCount = 0;
                newGeomEntityCords[newAttributeCount].resize(vectSize);
                for (int kk = 0; kk < numofEleType; kk++)
                {
                    if (elementVec[kk].size() > 1)
                    {
                        for (int i = 0; i < elementVec[kk][0].size(); i++)
                        {
                            int indexVert = elementVec[kk][0][i];
                            int indexEntity = elementVec[kk][newAttributeCount][i];
                            GeometryEntity geomEntity = GeometryEntityVect[indexEntity];
                            int  ntityStride = geomEntity.entityStride;
                            if (newGeomEntityCords[newAttributeCount][indexVert].empty())
                            {
                                for (int jj = 0; jj < ntityStride; jj++)
                                {
                                    newGeomEntityCords[newAttributeCount][indexVert].push_back(geomEntity.entityCords[jj]);
                                    entityCount++;
                                }
                            }
                        }
                    }
                }
                newAttributeCount++;
            }
        }
        //////////////////////////////////////////////////////////////////////////

        //Stoaring the above coolected co-ordinates in a plane gLfloat vector.
        vector <vector<GLfloat>> newGeomEntity;
        newGeomEntity.resize(newAttributeCount);

        for (int mm = 1; mm < newAttributeCount; mm++)
        {
            for (int nn = 0; nn < newGeomEntityCords[mm].size(); nn++)
            {
                for (int zz = 0; zz < newGeomEntityCords[mm][0].size(); zz++)
                {
                    if (newGeomEntityCords[mm][0].size() != newGeomEntityCords[mm][nn].size())
                    {
                        //TBD Someting is wrong here.
                        newGeomEntity[mm].push_back(newGeomEntityCords[mm][0][zz]);
                    }
                    else
                        newGeomEntity[mm].push_back(newGeomEntityCords[mm][nn][zz]);
                }
            }
        }

        if (morphTargetAvailable)
        {
            for (int mm = 0; mm < attributeCount; mm++)
            {
                if (mm >= numofGeomAttribs)//for getting differnce of vertices
                {
                    if (mm%numofGeomAttribs == 0)
                    {
                        for (size_t nn = 0; nn < newGeomEntity[mm].size(); nn++)
                        {
                            newGeomEntity[mm][nn] = newGeomEntity[mm][nn] - gGeometryEntity[0][nn];
                        }
                    }
                    else
                    {
                        for (size_t nn = 0; nn < newGeomEntity[mm].size(); nn++)
                        {
                            newGeomEntity[mm][nn] = newGeomEntity[mm][nn] - newGeomEntity[mm%numofGeomAttribs][nn];
                        }
                    }
                }
            }
        }

        //////////For controller////////////////////////////////////////////////////

        vector<controllerVertexWeight> controllerVertexWeightData = (*itrPart)->ControllerForGeom;
        if (!controllerVertexWeightData.empty())
        {
            (*itrPart)->shaderBuffer.vbo_bone = new GLuint[4];
            (*itrPart)->shaderBuffer.vbo_bone_size = 4;
        }
        /*  struct controllerVertexWeight
        {
        int           numofVCount;
        vector<string> vertJointNames;
        vector<float> vertWeightValues;
        };*/

        vector <int> boneIDEntity1;
        vector <GLfloat> weightEntity1;
        vector <int> boneIDEntity2;
        vector <GLfloat> weightEntity2;

        boneIDEntity1.reserve((gGeometryEntity[0].size() / 3) * 4);
        weightEntity1.reserve((gGeometryEntity[0].size() / 3) * 4);
        boneIDEntity2.reserve((gGeometryEntity[0].size() / 3) * 4);
        weightEntity2.reserve((gGeometryEntity[0].size() / 3) * 4);

        for (int vertexIndex = 0; vertexIndex < (gGeometryEntity[0].size() / 3); vertexIndex++)
        {
            for (int index = 0; index < MAX_BONES_PER_VEREX; index++)
            {
                unsigned int bone_index = 0;
                float weight = 0.0f;

                if (vertexIndex <= controllerVertexWeightData.size()
                    && index < controllerVertexWeightData[vertexIndex].numofVCount
                    && ((controllerVertexWeightData[vertexIndex].numofVCount == controllerVertexWeightData[vertexIndex].vertJointNames.size()) && (controllerVertexWeightData[vertexIndex].numofVCount == controllerVertexWeightData[vertexIndex].vertWeightValues.size())))
                {
                    if (boneInfo.bone_mapping.find(controllerVertexWeightData[vertexIndex].vertJointNames[index]) != boneInfo.bone_mapping.end())
                    {
                        bone_index = boneInfo.bone_mapping[controllerVertexWeightData[vertexIndex].vertJointNames[index]];
                        weight = controllerVertexWeightData[vertexIndex].vertWeightValues[index];
                    }
                }
                if (index < 4)
                {
                    boneIDEntity1.push_back(bone_index);
                    weightEntity1.push_back(weight);
                }
                else
                {
                    boneIDEntity2.push_back(bone_index);
                    weightEntity2.push_back(weight);
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////
        glGenVertexArrays(1, &((*itrPart)->shaderBuffer.vao));
        glBindVertexArray((*itrPart)->shaderBuffer.vao);
        
        int vertexAttrib = 0;
        for (int index = 0; index < attributeCount; index++)
        {
            int size = 3;
            if (((index+1) % 3) == 0)
            {
                if (index == 2)
                    size = 2;
                else
                    continue;
            }

            glGenBuffers(1, &(*itrPart)->shaderBuffer.vbo_vertex[index]);
            glBindBuffer(GL_ARRAY_BUFFER, (*itrPart)->shaderBuffer.vbo_vertex[index]);
            if (index == 0)
            {
                glBufferData(GL_ARRAY_BUFFER, gGeometryEntity[index].size() * sizeof(GLfloat), &gGeometryEntity[index].front(), GL_STATIC_DRAW);
            }
            else
            {
                glBufferData(GL_ARRAY_BUFFER, newGeomEntity[index].size() * sizeof(GLfloat), &newGeomEntity[index].front(), GL_STATIC_DRAW);
            }
            glVertexAttribPointer(vertexAttrib, size, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(vertexAttrib);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            if (index == 2)
                vertexAttrib += 5;
            else
                vertexAttrib++;
            
            if (vertexAttrib > 15)
                break;

            if (vertexAttrib > 7)
                index += 2;
        }


        //For Controller data
        int controllerAttrib = 3;
        for (int index = 0; index < 2; index++)
        {
            int size = 4;
            int stride = 0;

            glGenBuffers(1, &(*itrPart)->shaderBuffer.vbo_bone[index]);
            glBindBuffer(GL_ARRAY_BUFFER, (*itrPart)->shaderBuffer.vbo_bone[index]);
            if (index == 0)
            {
                glBufferData(GL_ARRAY_BUFFER, boneIDEntity1.size() * sizeof(GL_INT), &boneIDEntity1.front(), GL_STATIC_DRAW);
                glVertexAttribIPointer(controllerAttrib, size, GL_INT, stride, NULL);
                glEnableVertexAttribArray(controllerAttrib);

                //glVertexAttribIPointer((controllerAttrib + 2), size, GL_INT, stride * sizeof(GL_INT), (void*)(size * sizeof(GL_INT)));
                //glEnableVertexAttribArray(controllerAttrib + 2);

            }
            else
            {
                glBufferData(GL_ARRAY_BUFFER, weightEntity1.size() * sizeof(GLfloat), &weightEntity1.front(), GL_STATIC_DRAW);
                glVertexAttribPointer(controllerAttrib, size, GL_FLOAT, GL_FALSE, stride, NULL);
                glEnableVertexAttribArray(controllerAttrib);

                //glVertexAttribPointer((controllerAttrib + 2), size, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(size * sizeof(GLfloat)));
                //glEnableVertexAttribArray(controllerAttrib + 2);

            }
            controllerAttrib++;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        controllerAttrib = 5;
        for (int index = 2; index < 4; index++)
        {
            int size = 4;
            int stride = 0;

            glGenBuffers(1, &(*itrPart)->shaderBuffer.vbo_bone[index]);
            glBindBuffer(GL_ARRAY_BUFFER, (*itrPart)->shaderBuffer.vbo_bone[index]);
            if (index == 2)
            {
                glBufferData(GL_ARRAY_BUFFER, boneIDEntity2.size() * sizeof(GL_INT), &boneIDEntity2.front(), GL_STATIC_DRAW);
                glVertexAttribIPointer(controllerAttrib, size, GL_INT, 0, NULL);
                glEnableVertexAttribArray(controllerAttrib);
            }
            else
            {
                glBufferData(GL_ARRAY_BUFFER, weightEntity2.size() * sizeof(GLfloat), &weightEntity2.front(), GL_STATIC_DRAW);
                glVertexAttribPointer(controllerAttrib, size, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(controllerAttrib);
            }
            controllerAttrib++;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }


        for (int index = 0; index < numofEleType; index++)
        {
            glGenBuffers(1, &(*itrPart)->shaderBuffer.ebo[index]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*itrPart)->shaderBuffer.ebo[index]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementVec[index][0].size() * sizeof(unsigned short), &elementVec[index][0].front(), GL_STATIC_DRAW);
            (*itrPart)->shaderBuffer.elementCounts.push_back(elementVec[index][0].size());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        partCount++;
    }// end of for loop for part
}

/*===========================================================================*/
void ModelEntity::Modeldisplay(GLuint shaders_program)
{
    //To be Removed
    int i = 2;

    for (list <ModelPart *>::iterator itrPart = modelPartList.begin(); itrPart != modelPartList.end(); itrPart++)
    {
        //To be removed Start
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, i);
        glUniform1i(glGetUniformLocation(shaders_program, "material.texture_diffuse1"), i++);
        //To be removed End

        glBindVertexArray((*itrPart)->shaderBuffer.vao);
        for (int i = 0; i < (*itrPart)->shaderBuffer.ebo_size; i++)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*itrPart)->shaderBuffer.ebo[i]);
            glDrawElements(GL_TRIANGLES, (*itrPart)->shaderBuffer.elementCounts[i], GL_UNSIGNED_SHORT, 0);
        }
        glBindVertexArray(0);

        //To be removed Start
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
        //To be removed End
    }
}

/*===========================================================================*/

void ModelEntity::ModelUninitialize(void)
{
    
    for (list <ModelPart *>::iterator itrPart = modelPartList.begin(); itrPart != modelPartList.end(); itrPart++)
    {
        if ((*itrPart)->shaderBuffer.ebo)
        {
            for (int i = 0; i < (*itrPart)->shaderBuffer.ebo_size; i++)
            {
                glDeleteBuffers(1, &(*itrPart)->shaderBuffer.ebo[i]);
                (*itrPart)->shaderBuffer.ebo[i] = 0;
            }
        }
        if ((*itrPart)->shaderBuffer.vbo_vertex)
        {
            for (int i = 0; i < (*itrPart)->shaderBuffer.vbo_vertex_size; i++)
            {
                glDeleteBuffers(1, &(*itrPart)->shaderBuffer.vbo_vertex[i]);
                (*itrPart)->shaderBuffer.vbo_vertex[i] = 0;
            }
        }

        if ((*itrPart)->shaderBuffer.vao)
        {
            glDeleteVertexArrays(1, &(*itrPart)->shaderBuffer.vao);
            (*itrPart)->shaderBuffer.vao = 0;
        }
    }    
}


