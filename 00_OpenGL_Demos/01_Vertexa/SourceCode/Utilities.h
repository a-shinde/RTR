#pragma once
//This file will have common utility functions needed across components
#include<iostream>
#include<aws\core\external\cjson\cJSON.h>
#include<vector>
#include "Animation.h"
#include <mutex>
#define SPEECH_M_SUCCESS 1
#define SPEECH_M_ERROR 0

extern std::ofstream g_log_file;
extern bool isProgramTerminating; //To handle multithreaded cross components gracefull termination
extern std::mutex gmIsProgramTerminatingMutex;

int  parse_speech_marks(std::string &speechMarksJson, std::vector<LIP_SYNC_INPUT> &speechMarksOut);
void test_visme_parser();//Sample function to show how to use this parser

bool isProgramShuttingDown();
void setProgramShuttingDownToTrue();
