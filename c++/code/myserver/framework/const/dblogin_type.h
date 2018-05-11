//
// Created by dguco on 17-9-12.
//

#ifndef SERVER_DBLOGINTYPE_H
#define SERVER_DBLOGINTYPE_H


// 行数
enum emRowNumber
{
    emRow1	= 1,
    emRow2,
    emRow3,
    emRow4,
    emRow5,
    emRow6,
    emRow7,
    emRow8,
    emRow9,
    emRow10,
    emRow11,
    emRow12,
    emRow13,
    emRow14,
    emRow15,
    emRow16,
    emRow17,
    emRow18,
    emRow19,
    emRow20,
    emRow21,
    emRow22,
    emRow23,
    emRow24,
    emRow25,
    emRow26,
    emRow27,
    emRow28,
    emRow29,
    emRow30,
    emRow31,
    emRow32,
    emRow33,
    emRow34,
    emRow35,
    emRow36,
    emRow37,
    emRow38,
    emRow39,
    emRow40,
    emRow41,
    emRow42,
    emRow43,
    emRow44,
    emRow45,
    emRow46,
    emRow47,
    emRow48,
    emRow49,
    emRow50,

};

// 列数
enum emColNumber
{
    emCol1	= 1,
    emCol2,
    emCol3,
    emCol4,
    emCol5,
    emCol6,
    emCol7,
    emCol8,
    emCol9,
    emCol10,
    emCol11,
    emCol12,
    emCol13,
    emCol14,
    emCol15,
    emCol16,
    emCol17,
    emCol18,
    emCol19,
    emCol20,
    emCol21,
    emCol22,
    emCol23,
    emCol24,
    emCol25,
    emCol26,
    emCol27,
    emCol28,
    emCol29,
    emCol30,
    emCol31,
    emCol32,
    emCol33,
    emCol34,
    emCol35,
    emCol36,
    emCol37,
    emCol38,
    emCol39,
    emCol40,
    emCol41,
    emCol42,
    emCol43,
    emCol44,
    emCol45,
    emCol46,
    emCol47,
    emCol48,
    emCol49,
    emCol50,
};


// 从Dbserver取数据， 返回到gameserver以后根据不同类型解析
enum emDBLogicType
{
    emDBTypeFindOrCreateUser					= 0,		// 查询创建玩家帐号
//    emDBTypeLoadRoleData						= 1,		// 拉取角色数据
//    emDBTypeLoadRechargeOrder					= 2,		// 获取充值数据
//    emDBTypeSelectSession						= 3,		// session验证
//    emDBTypeSelectRename						= 4,		// 玩家修改名字
//    emDBTypeLoadServerInfo						= 5, 		// 拉取ServerInfo数据
//    emDBTypeLoadOrderGold						= 6,		// 拉取金币排行榜
//    emDBTypeSaveRoleData						= 20000,		// 存储角色信息
//    emDBTypeSaveSlotsPool						= 20001,		// 存储服务器信息
//    emDBTypeSaveOrderGold						= 20002,		// 存储金币排行榜
};

// 玩家上线从数据库分批Load玩家数据, 以及分批save数据
enum ELoadSaveFlag
{
//    ELSF_ACCOUNTDATA		    	= 0x00000001,		// load或save帐号信息						0000 0000 0000 0000 0000 0000 0000 0001
//    ELSF_BASICDATA					= 0x00000002,		// load或save基础数据						0000 0000 0000 0000 0000 0000 0000 0010
//    ELSF_HERODATA					= 0x00000004,		// load或save英雄数据						0000 0000 0000 0000 0000 0000 0000 0100
//    ELSF_PACKAGEDATA				= 0x00000008,		// load或save背包数据						0000 0000 0000 0000 0000 0000 0000 1000
//    ELSF_SCENESDATA		 			= 0x00000010,		// load或save关卡数据						0000 0000 0000 0000 0000 0000 0001 0000
//    ELSF_SHOPDATA					= 0x00000020,		// load或save商城数据						0000 0000 0000 0000 0000 0000 0010 0000
//    ELSF_MAILDATA					= 0x00000040,		// load或save邮件数据						0000 0000 0000 0000 0000 0000 0100 0000
//    ELSF_TASKDATA					= 0x00000080,		// load或save任务数据						0000 0000 0000 0000 0000 0000 1000 0000
//    ELSF_SIGNDATA					= 0x00000100,		// load或save任务数据						0000 0000 0000 0000 0000 0001 0000 0000
//    ELSF_EVENTDATA					= 0x00000200,		// load或save事件数据						0000 0000 0000 0000 0000 0010 0000 0000
//    ELSF_ARENAOFFLINEDATA		    = 0x00000400,		// load或save青铜竞技场离线数据	0000 0000 0000 0000 0000 0100 0000 0000
//    ELSF_ARENAOFFLINEDATAII		    = 0x00000800,		// load或save白银竞技场离线数据	0000 0000 0000 0000 0000 1000 0000 0000
//    ELSF_ARENAOFFLINEDATAIII	    = 0x00001000,		// load或save黄金竞技场离线数据	0000 0000 0000 0000 0001 0000 0000 0000
};

#endif //SERVER_DBLOGINTYPE_H
