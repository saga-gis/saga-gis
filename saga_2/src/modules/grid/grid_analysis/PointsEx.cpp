#include <memory.h>
#include "PointsEx.h"

CPointsEx::CPointsEx(void)
{
	m_nPoints	= 0;
	m_Points	= NULL;
	m_ClosestPts = NULL;
}

CPointsEx::~CPointsEx(void)
{
	Clear();
}

void CPointsEx::Clear(void)
{
	if( m_Points )
	{
		SG_Free(m_Points);
		SG_Free(m_ClosestPts);
	}

	m_nPoints	= 0;
	m_Points	= NULL;
	m_ClosestPts = NULL;
}

bool CPointsEx::Add(int x, int y, int iClosestPt)
{
	m_Points	= (TSG_Point_Int *)SG_Realloc(m_Points, (m_nPoints + 1) * sizeof(TSG_Point_Int));
	m_ClosestPts = (int*)SG_Realloc(m_ClosestPts, (m_nPoints + 1) * sizeof(int));
	
	m_Points[m_nPoints].x	= x;
	m_Points[m_nPoints].y	= y;
	m_ClosestPts[m_nPoints] = iClosestPt;
	m_nPoints++;

	return( true );
}

