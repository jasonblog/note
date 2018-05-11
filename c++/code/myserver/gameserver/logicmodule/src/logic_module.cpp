//
// Created by DGuco on 17-6-21.
//

#include "../inc/logic_module.h"
#include "server_tool.h"

template<> CLogicModule* CSingleton<CLogicModule>::spSingleton = NULL;

CLogicModule::CLogicModule()
{
    mRegist = false;
    mModueType = EMODULETYPE_INVALID;
}

CLogicModule::~CLogicModule()
{

}

bool CLogicModule::IsRegist()
{
    return mRegist;
}

void CLogicModule::RegistModule(EModuleType eType)
{
    mRegist = true;
    mModueType = eType;
}

