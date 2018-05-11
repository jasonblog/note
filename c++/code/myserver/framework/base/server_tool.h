//
// Created by DGcuo on 17-7-5.
//

#ifndef __SERVER_TOOL_H__
#define __SERVER_TOOL_H__

#include <vector>
#include <string>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "object.h"
#include "log.h"

using namespace std;

// ********************************************************************** //
// CSingleton
// ********************************************************************** //

template<typename T>
class CSingleton
{
private:
	static T *spSingleton;

public:
	CSingleton()
	{
		spSingleton = static_cast< T * >( this );
	}

	//禁止拷贝
	CSingleton(const CSingleton &temp) = delete;
	CSingleton &operator=(const CSingleton &temp) = delete;

	static T *GetSingletonPtr()
	{
		return spSingleton;
	}

	static T &GetSingleton()
	{
		return *spSingleton;
	}
};

// ********************************************************************** //
// CCoord2D( 2D坐标 )
// ********************************************************************** //
class CCoord2D
{
public:
	float m_fX;
	float m_fY;

public:
	CCoord2D()
	{
		Initialize();
	}
	CCoord2D(float fX, float fY)
		: m_fX(fX), m_fY(fY)
	{}

	~CCoord2D()
	{}

	int Initialize()
	{
		m_fX = 0.0f;
		m_fY = 0.0f;

		return 0;
	}

	int Resume()
	{
		return 0;
	}

	CCoord2D &operator=(const CCoord2D &v)
	{
		m_fX = v.m_fX;
		m_fY = v.m_fY;

		return (*this);
	}
};

// ********************************************************************** //
// CCoord3D( 3D坐标 )
// ********************************************************************** //
class CCoord3D
{
public:
	float m_fX;
	float m_fY;
	float m_fZ;

public:
	CCoord3D()
	{
		Initialize();
	}
	CCoord3D(float fX, float fY, float fZ)
		: m_fX(fX), m_fY(fY), m_fZ(fZ)
	{}

	~CCoord3D()
	{}

	int Initialize()
	{
		m_fX = 0;
		m_fY = 0;
		m_fZ = 0;

		return 0;
	}

	int Resume()
	{
		return 0;
	}

	CCoord3D &operator=(const CCoord3D &v)
	{
		m_fX = v.m_fX;
		m_fY = v.m_fY;
		m_fZ = v.m_fZ;

		return (*this);
	}

	CCoord3D &operator+(const CCoord3D &v)
	{
		m_fX += v.m_fX;
		m_fY += v.m_fY;
		m_fZ += v.m_fZ;

		return (*this);
	}

	CCoord3D &operator-(const CCoord3D &v)
	{
		m_fX -= v.m_fX;
		m_fY -= v.m_fY;
		m_fZ -= v.m_fZ;

		return (*this);
	}

	CCoord3D &operator*(const float dis)
	{
		m_fX *= dis;
		m_fY *= dis;
		m_fZ *= dis;

		return (*this);
	}

	void normalize()
	{
		float magSq = m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ;
		if (magSq > 0.0f) {
			float oneOverMag = 1.0f / sqrt(magSq);
			m_fX *= oneOverMag;
			m_fY *= oneOverMag;
			m_fZ *= oneOverMag;
		}
	}

	float static distance(const CCoord3D &p1, const CCoord3D &p2)
	{
		float dx = p1.m_fX - p2.m_fX;
		float dy = p1.m_fY - p2.m_fY;
		float dz = p1.m_fZ - p2.m_fZ;
		return sqrt(dx * dx + dy + dy + dz * dz);
	}

	CCoord3D static curpos(const CCoord3D &vStart, const CCoord3D &vEnd, const float dis)
	{
		CCoord3D p1 = vStart;
		CCoord3D p2 = vEnd;
		CCoord3D p3 = p2 - p1;
		p3.normalize();
		CCoord3D p4 = p3 * dis;

		return p1 + p4;
	}
};

// ********************************************************************** //
// CWTimer
// ********************************************************************** //
class CWTimer
{
protected:
	time_t mNextTimeout;    // 下一次超时时间（毫秒）
	time_t mDuration;        // 单次超时间隔（毫秒）
public:
	CWTimer()
	{
		mDuration = 0;
		mNextTimeout = 0;
	}

	void Initialize(time_t vDuration)
	{
		mDuration = vDuration;
		mNextTimeout = (time(NULL) + mDuration);
	}

public:
	// 传入当前时间（毫秒）返回是否超时，如果超时将计算下一次超时时间，并补偿
	inline bool IsTimeout(time_t tNow)
	{
		if (mDuration <= 0) {
			return false;
		}

		if (tNow >= mNextTimeout) {
			if (mNextTimeout == 0) {
				mNextTimeout = (time(NULL) + mDuration);
			}
			else {
				mNextTimeout += mDuration;
			}

			return true;
		}

		return false;
	}

	// 重置timer超时时间
	void ResetTimeout(time_t tNow)
	{
		mNextTimeout = (tNow + mDuration);
	}
};

// ********************************************************************** //
// 唯一ID(保证所有大区唯一，可用于装备，宠物，工会等需要创建唯一标识)
// ********************************************************************** //
class CGuid
{
public:
	CGuid();
	~CGuid();

	bool operator==(const CGuid &__x) const;
	bool operator!=(const CGuid &__x) const;

	CGuid &operator=(const CGuid &__x);

	void CreateGuid(int iTime, unsigned short unZoneID, unsigned short unServerType, unsigned short unServerID);

public:
	int mCreateTime;    // 创建时间（单位：秒）
	unsigned short mZoneID;        // 大区ID
	unsigned short mServerType;    // 服务器类型
	unsigned short mServerID;        // 服务器ID
	unsigned short mMyAddCnt;        // 创建时的自增长计数

	static unsigned short mAddCnt;        // 自增长计数
};

// ********************************************************************** //
// 双向链表基类
// ********************************************************************** //
class CDoubleLinker
{
public:
	CDoubleLinker()
	{
		Clear();
	}
	~CDoubleLinker()
	{}

	int Init()
	{
		Clear();

		return 0;
	}

	int Resume()
	{
		return 0;
	}

	void Clear()
	{
		mpNext = NULL;
		mpPrev = NULL;
	}

	void SetNext(CDoubleLinker *pPtr)
	{ mpNext = pPtr; }
	CDoubleLinker *GetNext()
	{ return mpNext; }

	void SetPrev(CDoubleLinker *pPtr)
	{ mpPrev = pPtr; }
	CDoubleLinker *GetPrev()
	{ return mpPrev; }

	// 是否在链表中
	bool InLinker()
	{ return (mpPrev == NULL && mpNext == NULL) ? false : true; }

protected:
	CDoubleLinker *mpNext;
	CDoubleLinker *mpPrev;
};

// ********************************************************************** //
// 双向链表头尾信息
// ********************************************************************** //
class CDoubleLinkerInfo
{
public:
	CDoubleLinkerInfo()
	{
		Clear();
	}

	CDoubleLinkerInfo(const CDoubleLinkerInfo &info)
	{
		(*this) = info;
	}

	~CDoubleLinkerInfo()
	{}

	int Initialize()
	{
		Clear();

		return 0;
	}

	int Resume()
	{
		return 0;
	}

	void Clear()
	{
		ClearHead();
		ClearEnd();
		miCnt = 0;
	}

	bool operator==(const CDoubleLinkerInfo &__x) const;
	bool operator!=(const CDoubleLinkerInfo &__x) const;

	CDoubleLinkerInfo &operator=(const CDoubleLinkerInfo &__x);

	// 将结点插入链表
	void insert(CDoubleLinker *pNode);

	// 从链表中删除该结点
	// 这个函数应该是不安全的，但为了效率，所以这样处理
	// 如果该node不是这个链表中的元素，这里也会处理，当该元素是其他链表的中间元素，则不会出任何问题
	// 如果该node不是其他链表的中间元素，那么那个链表将被破坏，而且下面会崩溃，因为prev或next其中一个肯定为空
	// 所以在外层使用的是有一定得注意
	int erase(CDoubleLinker *pNode);

	void SetHead(CDoubleLinker *pPtr)
	{ mpHead = pPtr; }
	CDoubleLinker *GetHead()
	{ return mpHead; }
	void ClearHead()
	{ mpHead = NULL; }

	void SetEnd(CDoubleLinker *pPtr)
	{ mpEnd = pPtr; }
	CDoubleLinker *GetEnd()
	{ return mpEnd; }
	void ClearEnd()
	{ mpEnd = NULL; }

	int GetCnt()
	{ return miCnt; }

protected:
	CDoubleLinker *mpHead;
	CDoubleLinker *mpEnd;
	int miCnt;    // 链表中元素数量
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CWTSize;
class CWTPoint;
class CWTRect;

//! CWTSize 类
/*!
	CWTSize 比较类似 MFC CSize, 实现了相对坐标  mCX 和 mCY 成员是公开的，另外
	CWTSize 提供了维护这两个数据的成员函数
*/
class CWTSize
{
public:
	int mCX;            //! 横坐标
	int mCY;            //! 纵坐标

public:
	//! CWTSize 构造函数, 默认构造一个 CWTSize( 0, 0 ) 对象
	CWTSize()
		: mCX(0), mCY(0)
	{}

	//! CWTSize 构造函数, 通过两个整数构造一个 CWTSize 对象
	/*! \param int vCX 横坐标
        \param int vCY 纵坐标 */
	CWTSize(int vCX, int vCY)
		: mCX(vCX), mCY(vCY)
	{}

public:
	//! CWTSize 析造函数
	~CWTSize()
	{}

public:
	//! != 操作符, 判断两个CWTSize是否不一样
	/*! \param const CWTSize& rSize  对象引用
		\return 如果该对象和rSize不一样返回true, 否则返回false */
	bool operator!=(const CWTSize &rSize) const
	{ return mCX != rSize.mCX || mCY != rSize.mCY; }

	//! == 操作符, 判断两个CWTSize是否一样
	/*! \param rSize CWTSize 对象引用
		\return 如果该对象和rSize一样返回true, 否则返回false */
	bool operator==(const CWTSize &rSize) const
	{ return mCX == rSize.mCX && mCY == rSize.mCY; }

	//! -= 操作符, 该CWTSize对象减去一个CWTSize对象
	/*! \param rSize CWTSize 对象引用
		\return 没有返回值 */
	void operator-=(const CWTSize &rSize)
	{
		mCX -= rSize.mCX;
		mCY -= rSize.mCY;
	}

	//! += 操作符, 该CWTSize对象加上一个rSize对象
	/*! \param rSize CWTSize 对象引用
		\return 没有返回值 */
	void operator+=(const CWTSize &rSize)
	{
		mCX += rSize.mCX;
		mCY += rSize.mCY;
	}

	//! - 操作符, 这个操作返回rPoint的值减去该CWTSize对象的值
	/*! \param rSize CWTPoint 对象引用
		\return 返回rPoint的值减去该CWTSize对象的值 */
	CWTPoint operator-(const CWTPoint &rPoint) const;

	//! - 操作符, 这个操作返回rRect的值减去该CWTSize对象的值
	/*! \param rRect CWTRect 对象引用
		\return 返回rRect的值减去该CWTSize对象后，构造的CWTRect拷贝 */
	CWTRect operator-(const CWTRect &rRect) const;

	//! - 操作符, 这个操作返回该CWTSize对象的值减去CWTSize对象后，构造的CWTSize拷贝 */
	/*! \param rSize CWTSize 对象引用
		\return 返回该CWTSize对象的值减去 rSize的值 */
	CWTSize operator-(const CWTSize &rSize) const
	{ return CWTSize(mCX - rSize.mCX, mCY - rSize.mCY); }

	//! - 操作符, 这个操作返回该CWTSize对象的取反后，构造的CWTSize拷贝
	/*! \return 返回该CWTSize对象取反后的值 */
	CWTSize operator-() const
	{ return CWTSize(-mCX, -mCY); }

	//! + 操作符, 这个操作返回rPoint的值加上该CWTSize对象的值
	/*! \param rPoint CWTPoint 对象引用
		\return 返回rPoint的值加上该CWTSize对象后，构造的CWTPoint拷贝 */
	CWTPoint operator+(const CWTPoint &rPoint) const;

	//! + 操作符, 这个操作返回rRect的值加上该CWTSize对象的值
	/*! \param rRect CWTRect 对象引用
		\return 返回rRect的值加上该CWTSize对象后，构造的CWTRect拷贝 */
	CWTRect operator+(const CWTRect &rRect) const;

	//! + 操作符, 这个操作返回该CWTSize对象的值加上rSize后，构造的CWTSize拷贝
	/*! \param rSize CWTSize 对象引用
		\return 返回该CWTSize对象加上一个rSize对象后，构造的CWTSize拷贝 */
	CWTSize operator+(const CWTSize &rSize) const
	{ return CWTSize(mCX + rSize.mCX, mCY + rSize.mCY); }
};

//! CWTPoint 类
/*!
	CWTPoint 比较类似 MFC CPoint, 实现了相对坐标  mX 和 mY 成员是公开的，另外
	CWTPoint 提供了维护这两个数据的成员函数
*/
class CWTPoint
{
public:
	int mX;
	int mY;

public:
	//! CWTPoint 构造函数, 默认构造一个 CWTPoint( 0, 0 ) 对象
	CWTPoint()
	{}//: mX( 0 ), mY( 0 )						{	}
	//! CWTPoint 构造函数, 通过两个整数构造一个 CWTPoint 对象
	/*! \param vX int 型, 横坐标
        \param vY int 型, 纵坐标 */
	CWTPoint(int vX, int vY)
		: mX(vX), mY(vY)
	{}
public:
	//! CWTPoint 析造函数
	~CWTPoint()
	{}

public:
	//! 成员函数 Offset, 横坐标和纵坐标分别加上vXOffset, vYOffset
	/*! \param vX int 型, 横坐标偏移量
		\param vY int 型, 纵坐标偏移量 */
	void Offset(int vXOffset, int vYOffset)
	{
		mX += vXOffset;
		mY += vYOffset;
	}

	//! 成员函数 Offset, 横坐标和纵坐标分别加上rPoint.mX, rPoint.mY
	/*! \param rPoint const CWTPoint型引用, 存储了横向和纵向的偏移量 */
	void Offset(const CWTPoint &rPoint)
	{
		mX += rPoint.mX;
		mY += rPoint.mY;
	}

	//! 成员函数 Offset, 横坐标和纵坐标分别偏移rSize.mX, rSize.mY */
	/*! \param rSize const CWTSize型引用, 存储了横向和纵向的偏移量 */
	void Offset(const CWTSize &rSize)
	{
		mX += rSize.mCX;
		mY += rSize.mCY;
	}

	void Clear()
	{
		mX = -1;
		mY = -1;
	}

	//! 成员函数 LimitDistance, 判断目标点与本点距离是否在vAmbit内
	bool LimitDistance(const CWTPoint &rDesPos, unsigned int vAmbit)
	{
//		CWTPoint tLength = *this - rDesPos;
//		return tLength.mX * tLength.mX + tLength.mY * tLength.mY;
		if ((unsigned int) abs(mX - rDesPos.mX) > vAmbit) {
			return false;
		}
		else if ((unsigned int) abs(mY - rDesPos.mY) > vAmbit) {
			return false;
		}

		return true;
	}

	int Length(const CWTPoint &rDesPos)
	{
		return max(abs(mY - rDesPos.mY), abs(mX - rDesPos.mX));
	}
public:
	//! != 操作符, 判断两个CWTPoint是否不一样 */
	/*! \param rPoint const CWTPoint型引用
		\return 如果该对象和rPoint不一样返回true, 否则返回false */
	bool operator!=(const CWTPoint &rPoint) const
	{ return mX != rPoint.mX || mY != rPoint.mY; }

	//! == 操作符, 判断两个CWTPoint是否一样 */
	/*! \param rPoint const CWTPoint型引用
		\return 如果该对象和rPoint一样返回true, 否则返回false */
	bool operator==(const CWTPoint &rPoint) const
	{ return mX == rPoint.mX && mY == rPoint.mY; }

	//! += 操作符, 该操作加上一个CWTSize对象 */
	/*! \param rSize const CWTSize型引用
		\return 没有返回值 */
	void operator+=(const CWTSize &rSize)
	{
		mX += rSize.mCX;
		mY += rSize.mCY;
	}

	//! += 操作符, 该操作加上一个CWTPoint对象 */
	/*! \param rPoint const CWTPoint型引用
		\return 没有返回值 */
	void operator+=(const CWTPoint &rPoint)
	{
		mX += rPoint.mX;
		mY += rPoint.mY;
	}

	//! -= 操作符, 该操作减去一个CWTSize对象 */
	/*! \param rSize const CWTSize型引用
		\return 没有返回值 */
	void operator-=(const CWTSize &rSize)
	{
		mX -= rSize.mCX;
		mY -= rSize.mCY;
	}

	//! -= 操作符, 该操作减去一个CWTPoint对象 */
	/*! \param rPoint const CWTPoint型引用
		\return 没有返回值 */
	void operator-=(const CWTPoint &rPoint)
	{
		mX -= rPoint.mX;
		mY -= rPoint.mY;
	}

	//! - 操作符, 该操作返回该CWTPoint对象减去一个CWTPoint对象后，构造的CWTPoint拷贝 */
	/*! \param rPoint const CWTPoint型引用
		\return 返回该CWTPoint对象减去一个CWTPoint对象后，构造的CWTPoint拷贝 */
	CWTPoint operator-(const CWTPoint &rPoint) const
	{ return CWTPoint(mX - rPoint.mX, mY - rPoint.mY); }

	//! - 操作符, 该操作返回该CWTPoint对象减去一个CWTSize对象后，构造的CWTPoint拷贝
	/*! \param rSize const CWTSize型引用
		\return 返回该CWTPoint对象减去一个CWTSize对象后，构造的CWTPoint拷贝 */
	CWTPoint operator-(const CWTSize &rSize) const
	{ return CWTPoint(mX - rSize.mCX, mY - rSize.mCY); }

	//! - 操作符, 该操作返回CWTRect对象减去该CWTPoint对象后，构造的CWTRect拷贝
	/*! \param rRect const CWTRect型引用
		\return 返回CWTRect对象减去该CWTPoint对象后，构造的CWTRect拷贝 */
	CWTRect operator-(const CWTRect &rRect) const;

	//! - 操作符, 该操作返回该CWTPoint对象取反后，构造的CWTPoint拷贝
	/*! \return 返回该CWTPoint对象取反后，构造的CWTPoint拷贝 */
	CWTPoint operator-() const
	{ return CWTPoint(-mX, -mY); }

	//! + 操作符, 该操作返回该CWTPoint对象加上一个CWTPoint对象后，构造的CWTPoint拷贝
	/*! \param rPoint const CWTPoint型引用
		\return 返回该CWTPoint对象加上一个CWTPoint对象后，构造的CWTPoint拷贝 */
	CWTPoint operator+(const CWTPoint &rPoint) const
	{ return CWTPoint(mX + rPoint.mX, mY + rPoint.mY); }

	//! + 操作符, 该操作返回该CWTPoint对象加上一个CWTSize对象后，构造的CWTPoint拷贝
	/*! \param rSize const CWTSize型引用
		\return 返回该CWTPoint对象加上一个CWTSize对象后，构造的CWTPoint拷贝 */
	CWTPoint operator+(const CWTSize &rSize) const
	{ return CWTPoint(mX + rSize.mCX, mY + rSize.mCY); }

	//! + 操作符, 该操作返回CWTRect对象加上该CWTPoint对象后，构造的CWTRect拷贝
	/*! \param rRect const CWTRect型引用
		\return 返回CWTRect对象加上该CWTPoint对象后，构造的CWTRect拷贝 */
	CWTRect operator+(const CWTRect &rRect) const;
};

//! CWTRect 类
/*!
	CWTRect 比较类似 MFC CWTRect, 实现了相对矩形坐标  mTopLeft 和 mBottomRight 成员是公开的，另外
	CWTRect 提供了维护这两个数据的成员函数, CWTRect的构造函数默认将调用NormalizeRect, 该函数将使
	得一个CWTRect对象正常化, 比如:mTopLeft( 10, 10 ) 和 mBottomRight( 20, 20 )是一个正常的矩形, 但
	一个mTopLeft( 20, 20 ) 和 mBottomRight( 10, 10 )是一个不正常的矩形, NormalizeRect将使得这样的矩
	形正常化，一个不正常的矩形在使用某些CWTRect的成员函数时可能会发生错误！
*/
class CWTRect
{
public:
	CWTPoint mTopLeft;                    //! CWTPoint 型,矩形的左上角
	CWTPoint mBottomRight;                //! CWTPoint 型,矩形的右下角

public:
	//! CWTRect 构造函数, 默认构造一个 CWTRect( 0, 0, 0, 0 ) 对象
	CWTRect()
	{ NormalizeRect(); }

	//! \brief CWTRect 构造函数, 通过4个整数构造 CWTRect 对象,
	/*! \param vLeft int 型 左上角横坐标
		\param vTop int 型 左上角纵坐标
		\param vRight int 型 右下角横坐标
		\param vBottom int 型 右下角纵坐标
		\param vNormalize bool 型 是不是要正常化该 CWTRect 对象
		\sa CWTRect */
	CWTRect(int vLeft, int vTop, int vRight, int vBottom, bool vNormalize = true)
		: mTopLeft(vLeft, vTop), mBottomRight(vRight, vBottom)
	{ if (vNormalize) NormalizeRect(); }

	//! CWTRect 构造函数, 通过1个 CWTPoint 和一个 CWTSize 构造 CWTRect 对象,
	/*! \param rPoint const CWTPoint 型引用 左上角坐标
		\param rSize const CWTSize 型引用 CWTRect尺寸
		\param vNormalize bool 型 是不是要正常化该 CWTRect 对象
		\sa CWTRect */
	CWTRect(const CWTPoint &rPoint, const CWTSize &rSize, bool vNormalize = true)
		: mTopLeft(rPoint), mBottomRight(rPoint + rSize)
	{ if (vNormalize) NormalizeRect(); }

	//! CWTRect 构造函数, 通过2个 CWTPoint 构造 CWTRect 对象,
	/*! \param rTopLeft const CWTPoint 型引用 左上角坐标
		\param rBottomRight const CWTPoint 型引用 右下角坐标
		\param vNormalize bool 型 是不是要正常化该 CWTRect 对象
		\sa CWTRect */
	CWTRect(const CWTPoint &rTopLeft, const CWTPoint &rBottomRight, bool vNormalize = true)
		: mTopLeft(rTopLeft), mBottomRight(rBottomRight)
	{ if (vNormalize) NormalizeRect(); }
public:
	//! CWTRect 析造函数
	~CWTRect()
	{}

public:
	//! 成员函数 BottomLeft,
	/*! \return 返回一个CWTPoint 的拷贝 */
	CWTPoint BottomLeft() const
	{ return CWTPoint(mTopLeft.mX, mBottomRight.mY); }

	//! 成员函数 TopRight,
	/*! \return 返回一个CWTPoint 的拷贝 */
	CWTPoint TopRight() const
	{ return CWTPoint(mBottomRight.mX, mTopLeft.mY); }

	//! 成员函数 BottomRight,
	/*! \return 返回一个CWTPoint 的拷贝 */
	//CWTPoint BottomRight( ) const
	//	{ return mBottomRight; }

	const CWTPoint &BottomRight() const
	{ return mBottomRight; }

	CWTPoint &BottomRight()
	{ return mBottomRight; }

	//! 成员函数 TopLeft,
	/*! \return 返回一个CWTPoint 的拷贝 */
	//CWTPoint TopLeft( ) const
	//	{ return mTopLeft; }

	const CWTPoint &TopLeft() const
	{ return mTopLeft; }

	CWTPoint &TopLeft()
	{ return mTopLeft; }

	//! 成员函数 CenterPoint,
	/*! \return 返回该 CWTRect 对象的中心点, 一个CWTPoint 的拷贝 */
	CWTPoint CenterPoint() const
	{ return CWTPoint((mTopLeft.mX + mBottomRight.mX) >> 1, (mTopLeft.mY + mBottomRight.mY) >> 1); }

	//! 成员函数 EqualRect, 判断两个对象是否相等
	/*! \param rRect const CWTRect 型引用
		\return 返回该 CWTRect 对象和rRect是否相等, 如果相等返回true, 否则返回false */
	bool EqualRect(const CWTRect &rRect) const
	{ return mTopLeft == rRect.mTopLeft && mBottomRight == rRect.mBottomRight; }

	//! 成员函数 DeflateRect, 缩小该 CWTRect 对象, 缩小指的是像中心靠近
	/*! \param vX int 型 mTopLeft的横坐标增加和mBottomRight的横坐标减少量
		\param vY int 型 mTopLeft的纵坐标增加和mBottomRight的纵坐标减少量
		\return 没有返回值 */
	void DeflateRect(int vX, int vY)
	{
		mTopLeft.mX += vX;
		mTopLeft.mY += vY;
		mBottomRight.mX -= vX;
		mBottomRight.mY -= vY;
	}

	//! 成员函数 DeflateRect, 缩小该 CWTRect 对象, 缩小指的是像中心靠近
	/*! \param rSize const CWTSize 型 rSize的mCX指明了 mTopLeft的横坐标增加和mBottomRight的横坐标减少量
									   rSize的mCY指明了 mTopLeft的纵坐标增加和mBottomRight的纵坐标减少量
		\return 没有返回值 */
	void DeflateRect(const CWTSize &rSize)
	{
		mTopLeft.mX += rSize.mCX;
		mTopLeft.mY += rSize.mCY;
		mBottomRight.mX -= rSize.mCX;
		mBottomRight.mY -= rSize.mCY;
	}

	//! 成员函数 DeflateRect, 缩小该 CWTRect 对象, 缩小指的是像中心靠近
	/*! \param rRect const CWTRect 型 rRect 指明了每一个边的缩小量
		\return 没有返回值 */
	void DeflateRect(const CWTRect &rRect)
	{
		mTopLeft.mX += rRect.mTopLeft.mX;
		mTopLeft.mY += rRect.mTopLeft.mY;
		mBottomRight.mX -= rRect.mBottomRight.mX;
		mBottomRight.mY -= rRect.mBottomRight.mY;
	}

	//! 成员函数 DeflateRect, 缩小该 CWTRect 对象, 缩小指的是像中心靠近
	/*! \param vLeft int 型 vLeft 指明了 左边 缩小量
		\param vTop int 型 vTop 指明了 上边 缩小量
		\param vRight int 型 vRight 指明了 右边 缩小量
		\param vBottom int 型 vBottom 指明了 下边 缩小量
		\return 没有返回值 */
	void DeflateRect(int vLeft, int vTop, int vRight, int vBottom)
	{
		mTopLeft.mX += vLeft;
		mTopLeft.mY += vTop;
		mBottomRight.mX -= vRight;
		mBottomRight.mY -= vBottom;
	}

	//! 成员函数 InflateRect, 扩大该 CWTRect 对象, 扩大指的是像中心发散
	/*! \param vX int 型 mTopLeft的横坐标减少和mBottomRight的横坐标增加量
		\param vY int 型 mTopLeft的纵坐标减少和mBottomRight的纵坐标增加量
		\return 没有返回值 */
	void InflateRect(int vX, int vY)
	{
		mTopLeft.mX -= vX;
		mTopLeft.mY -= vY;
		mBottomRight.mX += vX;
		mBottomRight.mY += vY;
	}

	//! 成员函数 InflateRect, 扩大该 CWTRect 对象, 扩大指的是像中心发散
	/*! \param rSize const CWTSize 型 rSize的mCX指明了 mTopLeft的横坐标减少和mBottomRight的横坐标增加量
									   rSize的mCY指明了 mTopLeft的纵坐标减少和mBottomRight的纵坐标增加量
		\return 没有返回值 */
	void InflateRect(const CWTSize &rSize)
	{
		mTopLeft.mX -= rSize.mCX;
		mTopLeft.mY -= rSize.mCY;
		mBottomRight.mX += rSize.mCX;
		mBottomRight.mY += rSize.mCY;
	}

	//! 成员函数 InflateRect, 扩大该 CWTRect 对象, 扩大指的是像中心发散
	/*! \param rRect const CWTRect 型 rRect 指明了每一个边的扩大量
		\return 没有返回值 */
	void InflateRect(const CWTRect &rRect)
	{
		mTopLeft.mX -= rRect.mTopLeft.mX;
		mTopLeft.mY -= rRect.mTopLeft.mY;
		mBottomRight.mX += rRect.mBottomRight.mX;
		mBottomRight.mY += rRect.mBottomRight.mY;
	}

	//! 成员函数 InflateRect, 扩大该 CWTRect 对象, 扩大指的是像中心发散
	/*! \param vLeft int 型 vLeft 指明了 左边 扩大量
		\param vTop int 型 vTop 指明了 上边 扩大量
		\param vRight int 型 vRight 指明了 右边 扩大量
		\param vBottom int 型 vBottom 指明了 下边 扩大量
		\return 没有返回值 */
	void InflateRect(int vLeft, int vTop, int vRight, int vBottom)
	{
		mTopLeft.mX -= vLeft;
		mTopLeft.mY -= vTop;
		mBottomRight.mX += vRight;
		mBottomRight.mY += vBottom;
	}

	//! 成员函数 Height, 返回该 CWTRect 对象的高度
	/*! \return 返回该 CWTRect 对象的高度 */
	int Height() const
	{ return mBottomRight.mY - mTopLeft.mY + 1; }

	//! 成员函数 Width, 返回该 CWTRect 对象的宽度
	/*! \return 返回该 CWTRect 对象的宽度 */
	int Width() const
	{ return mBottomRight.mX - mTopLeft.mX + 1; }

	//! 成员函数 IsRectEmpty, 返回该 CWTRect 对象的宽和高是不是全大于0
	/*! \return 返回该 CWTRect 对象是否正常 */
	bool IsRectEmpty() const
	{ return Width() <= 0 || Height() <= 0; }

	//! 成员函数 IsRectNull, 返回该 CWTRect 对象的四个边是否全为0
	/*! \return 返回该 CWTRect 对象是否为0 */
	bool IsRectNull() const
	{ return mTopLeft.mX == 0 && mTopLeft.mY == 0 && mBottomRight.mX == 0 && mBottomRight.mY == 0; }

	//! 成员函数 MoveToX, 对其该 CWTRect 对象到vX,纵坐标不变
	/*! \param vX int 型 指明对齐的横坐标
		\return 没有返回值 */
	void MoveToX(int vX)
	{
		mBottomRight.mX = vX + Width();
		mTopLeft.mX = vX;
	}

	//! 成员函数 MoveToY, 对其该 CWTRect 对象到vY,横坐标不变
	/*! \param vY int 型 指明对齐的纵坐标
		\return 没有返回值 */
	void MoveToY(int vY)
	{
		mBottomRight.mY = vY + Height();
		mTopLeft.mY = vY;
	}

	//! 成员函数 MoveToXY, 对其该 CWTRect 对象到vX, vY
	/*! \param vX int 型 指明对齐的横坐标
		\param vY int 型 指明对齐的纵坐标
		\return 没有返回值 */
	void MoveToXY(int vX, int vY)
	{
		mBottomRight.mX = vX + Width();
		mTopLeft.mX = vX;
		mBottomRight.mY = vY + Height();
		mTopLeft.mY = vY;
	}

	//! 成员函数 MoveToXY, 对其该 CWTRect 对象到rPoint
	/*! \param rPoint const CWTPoint 型引用 指明对齐的坐标
		\return 没有返回值 */
	void MoveToXY(const CWTPoint &rPoint)
	{
		mBottomRight.mX = rPoint.mX + Width();
		mTopLeft.mX = rPoint.mX;
		mBottomRight.mY = rPoint.mY + Height();
		mTopLeft.mY = rPoint.mY;
	}

	//! 成员函数 OffsetRect, 移动该 CWTRect 对象
	/*! \param vX int 型 指明横坐标的偏移量
		\param vY int 型 指明纵坐标的偏移量
		\return 没有返回值 */
	void OffsetRect(int vX, int vY)
	{ *this += CWTPoint(vX, vY); }

	//! 成员函数 OffsetRect, 移动该 CWTRect 对象
	/*! \param rPoint const CWTPoint 型引用 指明坐标偏移量
		\return 没有返回值 */
	void OffsetRect(const CWTPoint &rPoint)
	{ *this += rPoint; }

	//! 成员函数 OffsetRect, 移动该 CWTRect 对象
	/*! \param rSize const CWTSize 型引用 指明坐标偏移量
		\return 没有返回值 */
	void OffsetRect(const CWTSize &rSize)
	{ *this += rSize; }

	//! 成员函数 SetRect, 设置 CWTRect 对象
	/*! \param vLeft int 型 CWTRect对象的左上角横坐标
		\param vTop int 型 CWTRect对象的左上角纵坐标
		\param vRight int 型 CWTRect对象的右下角横坐标
		\param vBottom int 型 CWTRect对象的右下角纵坐标
		\return 没有返回值 */
	void SetRect(int vLeft, int vTop, int vRight, int vBottom)
	{
		mTopLeft.mX = vLeft;
		mTopLeft.mY = vTop;
		mBottomRight.mX = vRight;
		mBottomRight.mY = vBottom;
	}

	//! 成员函数 SetRect, 通过1个 CWTPoint 和1个 CWTSize 设置 CWTRect 对象,
	/*! \param rPoint const CWTPoint 型引用 左上角坐标
		\param rSize const CWTSize 型引用 CWTRect尺寸
		\return 没有返回值 */
	void SetRect(const CWTPoint &rPoint, const CWTSize &rSize)
	{
		mTopLeft = rPoint;
		mBottomRight = rPoint + rSize;
	}

	//! 成员函数 SetRect, 通过2个 CWTPoint 设置 CWTRect 对象,
	/*! \param rTopLeft const CWTPoint 型引用 左上角坐标
		\param rBottomRight const CWTPoint 型引用 右下角坐标
		\return 没有返回值 */
	void SetRect(const CWTPoint &rTopLeft, const CWTPoint &rBottomRight)
	{
		mTopLeft = rTopLeft;
		mBottomRight = rBottomRight;
	}

	//! 成员函数 SetRectEmpty, 将该 CWTRect 对象置0,
	/*! \return 没有返回值 */
	void SetRectEmpty()
	{
		mTopLeft.mX = 0;
		mTopLeft.mY = 0;
		mBottomRight.mX = 0;
		mBottomRight.mY = 0;
	}

	//! 成员函数 Size, 返回该 CWTRect 对象的宽度和高度
	/*! \return 返回该 CWTRect 对象的宽度和高度 */
	CWTSize Size() const
	{ return CWTSize(Width(), Height()); }

	//! 成员函数 NormalizeRect, 正常化该 CWTRect 对象
	/*! \return 没有返回值 */
	void NormalizeRect();

	//! 成员函数 PtInRect, 判断指定坐标是否在该 CWTRect 对象中
	/*! \param rPoint const CWTPoint型引用 待判断的坐标
		\return 返回指定坐标是不是在该 CWTRect 对象中 */
	bool PtInRect(const CWTPoint &rPoint) const;

	//! 成员函数 IntersectRect, 计算两个 CWTRect 对象的交集
	/*! \brief 如果计算产生的矩形不正常, 那么该矩形将被清零
		\brief eg. CWTRect( ).( CWTRect( 0, 0, 10, 10 ), CWTRect( 5, 5, 15, 15 ) ) == CWTRect( 5, 5, 10, 10 )
		\param const CWTRect& rSrcRect1 源矩形
		\param const CWTRect& rSrcRect2 源矩形
		\return 如果传入的两个源矩形或是计算后产生的矩形不正常，将返回false, 否这返回true */
	bool IntersectRect(const CWTRect &rSrcRect1, const CWTRect &rSrcRect2);

	//! 成员函数 UnionRect, 计算两个 CWTRect 对象的并集
	/*! \brief 如果传入的两个源矩形有一个不正常, 函数将返回false
		\brief eg. CWTRect( ).( CWTRect( 0, 0, 10, 10 ), CWTRect( 5, 5, 15, 15 ) ) == CWTRect( 0, 0, 15, 15 )
		\param const CWTRect& rSrcRect1 源矩形
		\param const CWTRect& rSrcRect2 源矩形
		\return 如果传入的两个源矩形有一个不正常, 函数将返回false, 否这返回true */
	bool UnionRect(const CWTRect &rSrcRectl, const CWTRect &rSrcRect2);

	//! 矩形相减
	int SubtractRect(const CWTRect &rDesRect, CWTRect *pRectList);

public:
	//! != 操作符, 判断两个 CWTRect 对象是否不相等
	/*! \param const CWTRect& rRect
		\return 返回该 CWTRect 对象和rRect是否相等, 如果不相等返回true, 否则返回false */
	bool operator!=(const CWTRect &rRect) const
	{ return !EqualRect(rRect); }

	//! == 操作符, 判断两个 CWTRect 对象是否相等
	/*! \param const CWTRect& rRect
		\return 返回该 CWTRect 对象和rRect是否相等, 如果相等返回true, 否则返回false */
	bool operator==(const CWTRect &rRect) const
	{ return EqualRect(rRect); }

	//! &= 操作符, 计算该 CWTRect 对象和 指定 CWTRect 对象的交集
	/*! \param const CWTRect& rRect
		\return 没有返回值 */
	void operator&=(const CWTRect &rRect)
	{ IntersectRect(*this, rRect); }

	//! |= 操作符, 计算该 CWTRect 对象和 指定 CWTRect 对象的并集
	/*! \param const CWTRect& rRect
		\return 没有返回值 */
	void operator|=(const CWTRect &rRect)
	{ UnionRect(*this, rRect); }

	//! & 操作符, 计算该 CWTRect 对象和 指定 CWTRect 对象的交集
	/*! \param const CWTRect& rRect
		\return 计算该 CWTRect 对象和 指定 CWTRect 对象的并集, 返回 CWTRect 的拷贝  */
	CWTRect operator&(const CWTRect &rRect) const
	{
		CWTRect tRectResult;
		tRectResult.IntersectRect(*this, rRect);
		return tRectResult;
	}

	//! | 操作符, 计算该 CWTRect 对象和 指定 CWTRect 对象的并集
	/*! \param const CWTRect& rRect
		\return 计算该 CWTRect 对象和 指定 CWTRect 对象的并集, 返回 CWTRect 的拷贝  */
	CWTRect operator|(const CWTRect &rRect) const
	{
		CWTRect tRectResult;
		tRectResult.UnionRect(*this, rRect);
		return tRectResult;
	}

	//! - 操作符, 把该 CWTRect 反向偏移 rPoint 个单位
	/*! \param const CWTPoint& rPoint
		\return 把该 CWTRect 反向偏移 rPoint 个单位, 返回 CWTRect 的拷贝  */
	CWTRect operator-(const CWTPoint &rPoint) const
	{
		CWTRect tRectResult = *this;
		tRectResult.OffsetRect(-rPoint);
		return tRectResult;
	}

	//! - 操作符, 把该 CWTRect 反向偏移 rSize 个单位
	/*! \param const CWTSize& rSize
		\return 把该 CWTRect 反向偏移 rSize 个单位, 返回 CWTRect 的拷贝  */
	CWTRect operator-(const CWTSize &rSize) const
	{
		CWTRect tRectResult = *this;
		tRectResult.OffsetRect(-rSize);
		return tRectResult;
	}

	//! - 操作符, 缩小该 CWTRect 对象，缩小的尺寸由 rRect 指定
	/*! \param const CWTRect& rRect
		\return 缩小该 CWTRect 对象，缩小的尺寸由 rRect 指定, 返回 CWTRect 的拷贝  */
	CWTRect operator-(const CWTRect &rRect) const
	{
		CWTRect tRectResult = *this;
		tRectResult.DeflateRect(rRect);
		return tRectResult;
	}

	//! + 操作符, 把该 CWTRect 正向偏移 rPoint 个单位
	/*! \param const CWTPoint& rPoint
		\return 把该 CWTRect 正向偏移 rPoint 个单位, 返回 CWTRect 的拷贝  */
	CWTRect operator+(const CWTPoint &rPoint) const
	{
		CWTRect tRectResult = *this;
		tRectResult.OffsetRect(rPoint);
		return tRectResult;
	}

	//! + 操作符, 把该 CWTRect 正向偏移 rSize 个单位
	/*! \param const CWTSize& rSize
		\return 把该 CWTRect 正向偏移 rSize 个单位, 返回 CWTRect 的拷贝  */
	CWTRect operator+(const CWTSize &rSize) const
	{
		CWTRect tRectResult = *this;
		tRectResult.OffsetRect(rSize);
		return tRectResult;
	}

	//! + 操作符, 扩大该 CWTRect 对象，扩大的尺寸由 rRect 指定
	/*! \param const CWTRect& rRect
		\return 扩大该 CWTRect 对象，扩大的尺寸由 rRect 指定, 返回 CWTRect 的拷贝  */
	CWTRect operator+(const CWTRect &rRect) const
	{
		CWTRect tRectResult = *this;
		tRectResult.InflateRect(rRect);
		return tRectResult;
	}

	//! += 操作符, 把该 CWTRect 正向偏移 rPoint 个单位
	/*! \param const CWTPoint& rPoint
		\return 没有返回值  */
	void operator+=(const CWTPoint &rPoint)
	{
		mTopLeft += rPoint;
		mBottomRight += rPoint;
	}

	//! += 操作符, 把该 CWTRect 正向偏移 rSize 个单位
	/*! \param const CWTSize& rSize
		\return 没有返回值  */
	void operator+=(const CWTSize &rSize)
	{
		mTopLeft += rSize;
		mBottomRight += rSize;
	}

	//! += 操作符, 扩大该 CWTRect 对象，扩大的尺寸由 rRect 指定
	/*! \param const CWTRect& rRect
		\return 没有返回值  */
	void operator+=(const CWTRect &rRect)
	{ InflateRect(rRect); }

	//! += 操作符, 把该 CWTRect 反向偏移 rPoint 个单位
	/*! \param const CWTPoint& rPoint
		\return 没有返回值  */
	void operator-=(const CWTPoint &rPoint)
	{
		mTopLeft -= rPoint;
		mBottomRight -= rPoint;
	}

	//! += 操作符, 把该 CWTRect 反向偏移 rSize 个单位
	/*! \param const CWTSize& rSize
		\return 没有返回值  */
	void operator-=(const CWTSize &rSize)
	{
		mTopLeft -= rSize;
		mBottomRight -= rSize;
	}

	//! += 操作符, 缩小该 CWTRect 对象，缩小的尺寸由 rRect 指定
	/*! \param const CWTRect& rRect
		\return 没有返回值  */
	void operator-=(const CWTRect &rRect)
	{ DeflateRect(rRect); }
};


#endif // __SERVER_TOOL_H__

