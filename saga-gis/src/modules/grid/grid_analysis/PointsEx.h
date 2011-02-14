/**********************************************************
 * Version $Id$
 *********************************************************/
#include "MLB_Interface.h"

class CPointsEx  
{
public:
	CPointsEx();
	virtual ~CPointsEx();

	void							Clear				(void);

	bool							Add					(int x, int y, int iClosestPt);

	int								Get_Count			(void)		{	return( m_nPoints );	}

	int								Get_X				(int Index)	{	return( m_Points[Index].x );	}
	int								Get_Y				(int Index)	{	return( m_Points[Index].y );	}
	int								Get_ClosestPoint		(int Index)	{	return( m_ClosestPts[Index] );	}

private:

	int								m_nPoints;

	TSG_Point_Int					*m_Points;
	int								*m_ClosestPts;

};