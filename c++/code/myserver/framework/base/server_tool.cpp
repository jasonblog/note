#include "server_tool.h"
#include <string.h>

// 阻挡线 
// ********************************************************************** //
//bool CLine::IsCrossPoint( CLine& nLine1, CLine& nLine2 , CWTPoint& nCrossPoint)
//{
//	CWTPoint d1 = CWTPoint( nLine1.mP2.mX - nLine1.mP1.mX, nLine1.mP2.mY-nLine1.mP1.mY );
//	CWTPoint d2 = CWTPoint( nLine2.mP2.mX - nLine2.mP1.mX, nLine2.mP2.mY-nLine2.mP1.mY );
//	CWTPoint pd = CWTPoint( nLine2.mP1.mX - nLine1.mP1.mX, nLine2.mP1.mY-nLine1.mP1.mY );
//
//	float pd1  = pd.mX * d2.mY - pd.mY * d2.mX;
//	float pd2  = pd.mX * d1.mY - pd.mY * d1.mX;
//	float dx12 = d1.mX * d2.mY - d2.mX * d1.mY;
//
//	float fz1 = pd1 * dx12;
//	float fz2 = pd2 * dx12;
//
//	float fm = dx12 * dx12;
//
//	// 线段平行
//	if ( FLOAT_EQ(fm)  )
//	{
//		return false;
//	}
//
//	// 线段不相交
//	float t1 = float(fz1 / fm);
//	if ( t1 > 1.0 || t1 < 0.0 )
//	{
//		return false;
//	}
//
//	// 线段不相交
//	float t2 = float(fz2 / fm);
//	if ( t2 > 1.0 || t2 < 0.0 )
//	{
//		return false;
//	}
//
//	// 线段相交 并返回交点
//	nCrossPoint.mX = nLine1.mP1.mX + d1.mX*t1;
//	nCrossPoint.mY = nLine1.mP1.mY + d1.mY*t1;
//
//	return true;
//}


// ********************************************************************** //
// 唯一ID
// ********************************************************************** //
unsigned short CGuid::mAddCnt = 0;

CGuid::CGuid()
{
	mCreateTime	= 0;
	mZoneID		= 0;
	mServerType	= 0;
	mServerID	= 0;
	mMyAddCnt	= 0;
}


CGuid::~CGuid()
{

}


bool CGuid::operator == (const CGuid& __x) const
{
	if ((__x.mCreateTime == mCreateTime)
			&& (__x.mZoneID == mZoneID)
			&& (__x.mServerType == mServerType)
			&& (__x.mServerID == mServerID)
			&& (__x.mMyAddCnt == mMyAddCnt))
	{
		return true;
	}

	return false;
}


bool CGuid::operator != (const CGuid& __x) const
{
	return !(*this == __x);
}


CGuid& CGuid::operator = (const CGuid& __x)
{
	if (*this != __x)
	{
		mCreateTime	= __x.mCreateTime;
		mZoneID		= __x.mZoneID;
		mServerType	= __x.mServerType;
		mServerID	= __x.mServerID;
		mMyAddCnt	= __x.mMyAddCnt;
	}

	return *this;
}


void CGuid::CreateGuid(int iTime, unsigned short unZoneID, unsigned short unServerType, unsigned short unServerID)
{
	mCreateTime	= iTime;
	mZoneID		= unZoneID;
	mServerType	= unServerType;
	mServerID	= unServerID;
	mMyAddCnt	= (mAddCnt++);
}


// ********************************************************************** //
// 双向链表头尾信息
// ********************************************************************** //
bool CDoubleLinkerInfo::operator == (const CDoubleLinkerInfo& __x) const
{
	if ((__x.mpHead == mpHead) && (__x.mpEnd == mpEnd) && (__x.miCnt == miCnt))
	{
		return true;
	}

	return false;
}


bool CDoubleLinkerInfo::operator != (const CDoubleLinkerInfo& __x) const
{
	return !(*this == __x);
}


CDoubleLinkerInfo& CDoubleLinkerInfo::operator = (const CDoubleLinkerInfo& __x)
{
	mpHead = __x.mpHead;
	mpEnd = __x.mpEnd;
	miCnt = __x.miCnt;

	return (*this);
}


// 将结点插入链表
void CDoubleLinkerInfo::insert(CDoubleLinker* pNode)
{
	if (pNode == NULL)
	{
		return;
	}

	// 没有Next和Prev的节点才能插入链表(无效节点),防止重复插入
	if (pNode->GetNext() != NULL || pNode->GetPrev() != NULL)
	{
		return;
	}

	if (mpEnd != NULL)
	{
		// 有数据，则直接将数据插入尾部，并设为尾指针
		mpEnd->SetNext(pNode);
		pNode->SetPrev(mpEnd);
		pNode->SetNext(NULL);
		SetEnd(pNode);
	}
	else
	{
		// 尾指针没数据，那么头指针也肯定没数据
		SetHead(pNode);
		SetEnd(pNode);
		pNode->Clear();
	}
	miCnt++;
}

// 从链表中删除该结点
// 这个函数应该是不安全的，但为了效率，所以这样处理
// 如果该node不是这个链表中的元素，这里也会处理，当该元素是其他链表的中间元素，则不会出任何问题
// 如果该node不是其他链表的中间元素，那么那个链表将被破坏，而且下面会崩溃，因为prev或next其中一个肯定为空
// 所以在外层使用的是有一定得注意
int CDoubleLinkerInfo::erase(CDoubleLinker* pNode)
{
	int iRet = -1;

	if (pNode == NULL)
	{
		return iRet;
	}

	if (pNode == mpHead && pNode == mpEnd)
	{
		// 只有一个元素
		Clear();
		iRet = 0;
	}
	else if (pNode == mpHead)
	{
		// 头指针
		if (pNode->GetNext() == NULL) return -1;
		pNode->GetNext()->SetPrev(NULL);
		SetHead(pNode->GetNext());
		iRet = 1;
	}
	else if (pNode == mpEnd)
	{
		// 尾指针
		if (pNode->GetPrev() == NULL) return -1;
		pNode->GetPrev()->SetNext(NULL);
		SetEnd(pNode->GetPrev());
		iRet = 2;
	}
	else
	{
		// 有Next和Prev的节点才能从链表中删除(有效节点)
		if (pNode->GetNext() == NULL || pNode->GetPrev() == NULL) return -1;
		// 在链表中间
		pNode->GetPrev()->SetNext(pNode->GetNext());
		pNode->GetNext()->SetPrev(pNode->GetPrev());
		iRet = 3;
	}

	if (miCnt > 0)
		miCnt--;

	pNode->Clear();

	return iRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************************ //
// CWTSize
// ************************************************************ //

CWTPoint CWTSize::operator - ( const CWTPoint& rPoint ) const
{
	return rPoint - *this;
}

CWTRect CWTSize::operator - ( const CWTRect& rRect ) const
{
	return rRect - *this;
}

CWTPoint CWTSize::operator + ( const CWTPoint& rPoint ) const
{
	return rPoint + *this;
}

CWTRect CWTSize::operator + ( const CWTRect& rRect ) const
{
	return rRect + *this;
}
// ************************************************************ //
// CWTPoint
// 2D 坐标
// ************************************************************ //

CWTRect CWTPoint::operator - ( const CWTRect& rRect ) const
{
	return rRect - *this;
}

CWTRect CWTPoint::operator + ( const CWTRect& rRect ) const
{
	return rRect + *this;
}

// ************************************************************ //
// CWTRect
// 2D 矩形区域
// ************************************************************ //

bool CWTRect::IntersectRect( const CWTRect& rSrcRect1, const CWTRect& rSrcRect2 )
{
	if ( rSrcRect1.IsRectEmpty( ) || rSrcRect2.IsRectEmpty( ) )
		return false;

	int tLeft	= max( rSrcRect1.mTopLeft.mX, rSrcRect2.mTopLeft.mX );
	int tTop	= max( rSrcRect1.mTopLeft.mY, rSrcRect2.mTopLeft.mY );
	int tRight	= min( rSrcRect1.mBottomRight.mX, rSrcRect2.mBottomRight.mX );
	int tBottom	= min( rSrcRect1.mBottomRight.mY, rSrcRect2.mBottomRight.mY );

	if ( CWTRect( tLeft, tTop, tRight, tBottom, false ).IsRectEmpty( ) )
	{
		SetRectEmpty( );
		return false;
	}

	mTopLeft.mX		= tLeft;
	mTopLeft.mY		= tTop;
	mBottomRight.mX = tRight;
	mBottomRight.mY = tBottom;
	return true;
}

void CWTRect::NormalizeRect( )
{
	mTopLeft.mX		= min( mTopLeft.mX, mBottomRight.mX );
	mTopLeft.mY		= min( mTopLeft.mY, mBottomRight.mY );
	mBottomRight.mX	= max( mTopLeft.mX, mBottomRight.mX );
	mBottomRight.mY	= max( mTopLeft.mY, mBottomRight.mY );
}

bool CWTRect::PtInRect( const CWTPoint& rPoint ) const
{
	if ( IsRectEmpty( ) )
		return false;

	if ( rPoint.mX < mTopLeft.mX || rPoint.mX > mBottomRight.mX )
		return false;

	if ( rPoint.mY < mTopLeft.mY || rPoint.mY > mBottomRight.mY )
		return false;

	return true;
}

bool CWTRect::UnionRect( const CWTRect& rSrcRectl, const CWTRect& rSrcRect2 )
{
	int tLeft	= rSrcRectl.mTopLeft.mX < rSrcRect2.mTopLeft.mX ? rSrcRectl.mTopLeft.mX : rSrcRect2.mTopLeft.mX;
	int tTop	= rSrcRectl.mTopLeft.mY < rSrcRect2.mTopLeft.mY ? rSrcRectl.mTopLeft.mY : rSrcRect2.mTopLeft.mY;
	int tRight	= rSrcRectl.mBottomRight.mX > rSrcRect2.mBottomRight.mX ? rSrcRectl.mBottomRight.mX : rSrcRect2.mBottomRight.mX;
	int tBottom	= rSrcRectl.mBottomRight.mY > rSrcRect2.mBottomRight.mY ? rSrcRectl.mBottomRight.mY : rSrcRect2.mBottomRight.mY;

	mTopLeft.mX		= tLeft;
	mTopLeft.mY		= tTop;
	mBottomRight.mX = tRight;
	mBottomRight.mY = tBottom;
	return true;
}

int CWTRect::SubtractRect( const CWTRect& rDesRect, CWTRect* pRectList )
{
	CWTRect tRectResult;
	if ( tRectResult.IntersectRect( *this, rDesRect ) == false )
	{
		pRectList[ 0 ] = rDesRect;
		return 1;
	}

	CWTRect tTopRect;
	tTopRect.mTopLeft			= rDesRect.mTopLeft;
	tTopRect.mBottomRight.mX	= rDesRect.mBottomRight.mX;
	tTopRect.mBottomRight.mY	= mTopLeft.mY - 1;

	CWTRect tLeftRect;
	tLeftRect.mTopLeft.mX		= rDesRect.mTopLeft.mX;
	tLeftRect.mTopLeft.mY		= max( mTopLeft.mY, rDesRect.mTopLeft.mY );
	tLeftRect.mBottomRight.mX	= mTopLeft.mX - 1;
	tLeftRect.mBottomRight.mY	= min( mBottomRight.mY, rDesRect.mBottomRight.mY );

	CWTRect tBottomRect;
	tBottomRect.mTopLeft.mX		= rDesRect.mTopLeft.mX;
	tBottomRect.mTopLeft.mY		= mBottomRight.mY + 1;
	tBottomRect.mBottomRight	= rDesRect.mBottomRight;

	CWTRect tRightRect;
	tRightRect.mTopLeft.mX		= mBottomRight.mX + 1;
	tRightRect.mTopLeft.mY		= max( mTopLeft.mY, rDesRect.mTopLeft.mY );
	tRightRect.mBottomRight.mX	= rDesRect.mBottomRight.mX;
	tRightRect.mBottomRight.mY	= min( mBottomRight.mY, rDesRect.mBottomRight.mY );
	int tRectIndex = 0;
	if ( tTopRect.IsRectEmpty( ) == false )
		pRectList[ tRectIndex ++ ] = tTopRect;

	if ( tLeftRect.IsRectEmpty( ) == false )
		pRectList[ tRectIndex ++ ] = tLeftRect;

	if ( tBottomRect.IsRectEmpty( ) == false )
		pRectList[ tRectIndex ++ ] = tBottomRect;

	if ( tRightRect.IsRectEmpty( ) == false )
		pRectList[ tRectIndex ++ ] = tRightRect;

	return tRectIndex;
}





















