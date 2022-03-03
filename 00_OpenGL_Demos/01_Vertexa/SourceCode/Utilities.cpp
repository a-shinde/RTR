#include "Utilities.h"
#include<fstream>
#include<stdio.h>
#include<sstream>

#pragma comment(lib,"aws-cpp-sdk-core.lib")


void splitSpeechMarksFromOriginalJson(std::string originalJson, std::vector<std::string>& outArrayOfJsonSpeechMarksStrings)
{
	std::stringstream str_buff(originalJson);
	std::string speechMark;

	while (getline(str_buff, speechMark, '}'))
	{
		speechMark.append("}");
		outArrayOfJsonSpeechMarksStrings.push_back(speechMark);
	}
}
int parse_speech_marks(std::string &speechMarksJson, std::vector<LIP_SYNC_INPUT> &speechMarksOut)
{
	const cJSON *time = NULL;
	const cJSON *type = NULL;
	const cJSON *value = NULL;
	std::vector<std::string> speechMarks;
	g_log_file << "Parsing speech marks.." << std::endl;
	int status = 0;
	/* First split the input string to seperate out each speech marks JSON*/
	splitSpeechMarksFromOriginalJson(speechMarksJson, speechMarks);
	for (int i = 0; i < (speechMarks.size() - 1) ;i++)
	{
		g_log_file << "Parcing - index:" << i << " Value:" << speechMarks[i].c_str(); //Enable if want to debug parcing error occus
		cJSON *speech_mark_json = cJSON_Parse(speechMarks[i].c_str());
		if (speech_mark_json == NULL)
		{
			const char *error_ptr = cJSON_GetErrorPtr();
			if (error_ptr != NULL)
			{
					g_log_file << "SpeechMarksParser:Error before: " << error_ptr << std::endl;
			}
			status = 0;
			return SPEECH_M_ERROR;
		}
		type = cJSON_GetObjectItemCaseSensitive(speech_mark_json, "type"); //First check for word in the JSON, if present. Skip it.
		if (cJSON_IsString(type) && (type->valuestring != NULL))
		{
			if (strcmp(type->valuestring, "word") == 0)
			{
				//skip
				continue;
			}
			else if (strcmp(type->valuestring, "viseme") == 0)
			{
				//add it
				LIP_SYNC_INPUT speechMarkRec;
				//Extract time
				time = cJSON_GetObjectItemCaseSensitive(speech_mark_json, "time");
				if (cJSON_IsNumber(time) && (time->valueint != NULL)) //Time would in miliseconds(int)
				{
					speechMarkRec.visemeTime = time->valueint;
				}
				value = cJSON_GetObjectItemCaseSensitive(speech_mark_json, "value");
				if (cJSON_IsString(value) && (value->valuestring != NULL))
				{
					speechMarkRec.visemeValue = value->valuestring;
				}
				speechMarksOut.push_back(speechMarkRec);
			}
			

		}
		cJSON_Delete(speech_mark_json);
	}
	
	return SPEECH_M_SUCCESS;
}
//Sample test code to show how to use this parser
void test_visme_parser()
{
	std::string originalSpeechMarksJson = "{\"time\":6,\"type\":\"word\",\"start\":0,\"end\":4,\"value\":\"nice\"}\r\n{\"time\":6,\"type\":\"viseme\",\"value\":\"t\"}\r\n{\"time\":68,\"type\":\"viseme\",\"value\":\"a\"}\r\n{\"time\":205,\"type\":\"viseme\",\"value\":\"s\"}\r\n{\"time\":333,\"type\":\"word\",\"start\":5,\"end\":7,\"value\":\"to\"}\r\n{\"time\":333,\"type\":\"viseme\",\"value\":\"t\"}\r\n{\"time\":400,\"type\":\"viseme\",\"value\":\"u\"}\r\n{\"time\":437,\"type\":\"word\",\"start\":8,\"end\":12,\"value\":\"meet\"}\r\n{\"time\":437,\"type\":\"viseme\",\"value\":\"p\"}\r\n{\"time\":506,\"type\":\"viseme\",\"value\":\"i\"}\r\n{\"time\":609,\"type\":\"viseme\",\"value\":\"t\"}";
	std::vector<LIP_SYNC_INPUT> speechMarks;
	int ret = parse_speech_marks(originalSpeechMarksJson, speechMarks);
	if (ret == SPEECH_M_SUCCESS)
	{
		g_log_file << "Speech marks parsed successfully. Printing them." << std::endl;
		for (int i = 0;i < speechMarks.size();i++)
		{
			std::stringstream speechMarkRec;
			speechMarkRec << "Time:" << speechMarks[i].visemeTime << " value:" << speechMarks[i].visemeValue << std::endl;
			g_log_file << speechMarkRec.str();
		}
	}
}

bool isProgramShuttingDown() {
	gmIsProgramTerminatingMutex.lock();
	bool isProgrammShuttingDown = isProgramTerminating;
	gmIsProgramTerminatingMutex.unlock();
	return isProgrammShuttingDown;
}

void setProgramShuttingDownToTrue() {
	gmIsProgramTerminatingMutex.lock();
	isProgramTerminating = true; 
	gmIsProgramTerminatingMutex.unlock();
}