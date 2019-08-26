
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  statistics_kriging                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   kriging_base.h                      //
//                                                       //
//                 Olaf Conrad (C) 2008                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__kriging_base_H
#define HEADER_INCLUDED__kriging_base_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CKriging_Base : public CSG_Tool
{
public:
	CKriging_Base(void);
	virtual ~CKriging_Base(void);


protected:

	CSG_Matrix						m_Points, m_W;

	CSG_KDTree_2D					m_Search;

	CSG_Parameters_PointSearch		m_Search_Options;


	virtual int						On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int						On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool					On_Execute				(void);

	virtual bool					Init_Points				(CSG_Shapes *pPoints, int Field, bool bLog);

	bool							Get_Points				(double x, double y, CSG_Matrix &Points);

	virtual bool					Get_Weights				(const CSG_Matrix &Points, CSG_Matrix &W)	= 0;

	virtual bool					Get_Value				(double x, double y, double &v, double &e)	= 0;
	virtual bool					Get_Value				(const double    *c, double &v, double &e)	{	return( Get_Value(c[0], c[1], v, e) );	}

	double							Get_Weight				(double d)				{	d = m_Model.Get_Value(d); return( d > 0. ? d : 0. );	}
	double							Get_Weight				(double dx, double dy)	{	return( Get_Weight(sqrt(dx*dx + dy*dy))           );	}
	double							Get_Weight				(double *a, double *b)	{	return( Get_Weight(a[0], a[1], b[0], b[1])        );	}
	double							Get_Weight				(double ax, double ay, double bx, double by)
	{
		if( m_Block > 0. )
		{
			return( ( Get_Weight((ax          ) - bx, (ay          ) - by)
					+ Get_Weight((ax + m_Block) - bx, (ay + m_Block) - by)
					+ Get_Weight((ax + m_Block) - bx, (ay - m_Block) - by)
					+ Get_Weight((ax - m_Block) - bx, (ay + m_Block) - by)
					+ Get_Weight((ax - m_Block) - bx, (ay - m_Block) - by)
				) / 5.
			);
		}

		return( Get_Weight(ax - bx, ay - by) );
	}

	void							Set_Value				(int x, int y, double v, double e)
	{
		if( m_pValue )	m_pValue->Set_Value(x, y, v);
		if( m_pError )	m_pError->Set_Value(x, y, e);
	}

	void							Set_NoData				(int x, int y)
	{
		if( m_pValue )	m_pValue->Set_NoData(x, y);
		if( m_pError )	m_pError->Set_NoData(x, y);
	}


private:

	double							m_Block;

	CSG_Trend						m_Model;

	CSG_Parameters_Grid_Target		m_Grid_Target;

	CSG_Grid						*m_pValue, *m_pError;

	class CVariogram_Dialog			*m_pVariogram;


	bool							_Init_Grids				(void);

	bool							_Init_Search			(bool bUpdate = false);

	bool							_Get_Cross_Validation	(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__kriging_base_H
