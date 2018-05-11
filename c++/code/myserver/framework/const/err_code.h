//
// Created by dguco on 17-9-12.
//

#ifndef SERVER_ERRCODE_H
#define SERVER_ERRCODE_H


// 系统错误码范围	-1000 ------	-1999
enum emSystemErrorCode
{
    emSystem_noservice				= -1000,		// 服务器未开启
    emSystem_parsemsgfailed		    = -1001,		// 解析消息失败
    emSystem_isfull					= -1002,		// 服务器已满
    emSystem_msginvalid				= -1003,		// 消息无效
    emSystem_createroleid			= -1004,		// 创建teamid失败
    emSystem_createrole				= -1005,		// 创建role失败
    emSystem_loginagain				= -1006,		// 其它地方登录被踢下线
    emSystem_loginlimit				= -1007,		// 禁止登陆 被踢下线
    emSystem_msgerr					= -1008,		// 禁止登陆 消息非法
    emSystem_offlinetimeout			= -1009,		// 离线服务器超时
    emSystem_logincheckfailed		= -1010,		// 平台验证失败
    emSystem_RoomClosed			    = -1011,		// 房间已关闭,请返回大厅
};

//开关号
//错误嘛 -5000-----10000 对应功能开关  0-----6000
#define FUNCTION_SWITCH( SWITCH_NUM ) ( (SWITCH_NUM > 0) ? (-1 * SWITCH_NUM - 5000):-5000 )

// 按照模块顺序添加
// 登陆错误码范围	-10000 ------  -10999
enum emLoginErrorCode
{
    emLogin_IsFull										= -10000,				// 服务器满人数已满， 请登陆新服
    emLogin_HasDirtyWord								= -10001,				// 角色名有脏字
    emLogin_SameName									= -10002,				// 角色名在本服务器与其他玩家的角色名重复了
    emLogin_CreateRole_NoAccount						= -10003, 				// 创建角色时， 没有帐号信息(只是逻辑上有这种可能)
    emLogin_CreateRole_SameTeamID						= -10004, 				// 创建角色时， 战队ID已经被占用(只是逻辑上有这种可能)
};




#endif //SERVER_ERRCODE_H
