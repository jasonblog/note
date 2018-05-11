#include "runflag.h"

CRunFlag::CRunFlag()
{
	mRunFlag = ERF_RUNTIME;
}


CRunFlag::~CRunFlag()
{

}

void CRunFlag::SetRunFlag(ERunFlag eRunFlag)
{
	mRunFlag = eRunFlag;
}

bool CRunFlag::CheckRunFlag(ERunFlag eRunFlag)
{
	return (eRunFlag == mRunFlag);
}

