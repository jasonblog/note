//
//  object.h
//  游戏对象父类
//  Created by DGuco on 16/12/19.
//  Copyright © 2016年 DGuco. All rights reserved.
//


#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "base.h"

//OBJ_ID_BITS占位,时间戳(4字节)+类型(1字节)+同一秒序列id(3字节),同一秒同类型的对象ID的范围最大为2^^24,
//即每秒可生成16777216个同类型对象(足够了) (注意:服务器时间不能往回调)
#define OBJ_ID_BITS (24)
#define INVALID_OBJ_ID	0
typedef unsigned long  OBJ_ID;

//同一秒id范围
#define OBJ_ID_START(type)			(((type) << OBJ_ID_BITS) + 1)
//这里预留10个保证不同类型id不会相同
#define OBJ_ID_END(type)			((((type) + 1) << OBJ_ID_BITS) - 10)

// 类型取值 0 ~ 255 为了防止越界，去掉头尾边界，所以有效取值范围为 1 ~ 254,所以这里取值从254（0xFE）开始递减
typedef enum _ObjType
{
    OBJ_SESSION_TIMER = 0xFE,
    OBJ_GAMER_TIMER   = 0xFD,
    OBJ_PLAYER_ENTRY  = 0xFC,
    OBJ_INVALID
}EnObjType;

class CObj
{
public:
	CObj()
		/*: id_( 0 )*/
	{
	}

	virtual ~CObj()
	{
		// 在析构的时候，设置id_，可以保证内存中id_数据的合法性
		// 这是因为有时候需要通过id来访问数据，而在共享内存中的数据，并不会真正析构
		id_ = INVALID_OBJ_ID;
	}

	OBJ_ID get_id() const
	{
		return id_;
	}

	void set_id(OBJ_ID id)
	{
		id_ = id;
	}

	static int ID2TYPE(OBJ_ID id)
	{
		return (id >> OBJ_ID_BITS) & 0X00000000000000FF;
	}

	// 判断object是否合法
	// 由于开启异常处理不太方便，在这里加上这个函数。如果构造函数失败，只需要设置id =INVALID_OBJ_ID即可
	// 这种方式虽然比较丑类,但是可以比较有效的不使用异常来处理构造函数中发现的错误
	int valid() const
	{
		return INVALID_OBJ_ID != id_;
	}

	// 设置对象非法
	void invalid()
	{
		id_ = INVALID_OBJ_ID;
	}

	// 初始化对象数据
	virtual int Initialize() = 0;

private:
	OBJ_ID id_; // 所有对象的唯一标识
};                                               
                                                           
#endif // _OBJECT_H_
