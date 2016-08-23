/**********************************************************
 * Version $Id: kriging_base.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//            geostatistics_kriging_variogram            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    kriging_base.h                     //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
#include "MLB_Interface.h"


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
	~CKriging_Base(void);


protected:

	bool							m_bLog;

	int								m_zField;

	CSG_Points_Z					m_Data;

	CSG_Matrix						m_W;

	CSG_Shapes						*m_pPoints;

	CSG_Parameters_Search_Points	m_Search;


	virtual bool					On_Execute				(void);

	virtual int						On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int						On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool					On_Initialize			(void);

	virtual bool					Get_Weights				(const CSG_Points_Z &Points, CSG_Matrix &W)	= 0;

	virtual bool					Get_Value				(const TSG_Point &p, double &z, double &v)	= 0;

	double							Get_Weight				(double d)											{	return( m_Model.Get_Value(d) );	}
	double							Get_Weight				(double dx, double dy)								{	return( Get_Weight(sqrt(dx*dx + dy*dy)) );	}
	double							Get_Weight				(const TSG_Point_Z &a, const TSG_Point_Z &b)		{	return( Get_Weight(a.x - b.x, a.y - b.y) );	}
	double							Get_Weight				(const TSG_Point   &a, const TSG_Point_Z &b)		{	return( Get_Weight(a.x - b.x, a.y - b.y) );	}
	double							Get_Weight				(double ax, double ay, double bx, double by)
	{
		if( m_Block > 0.0 )
		{
			return( (	Get_Weight( ax            - bx,  ay            - by)
					+	Get_Weight((ax + m_Block) - bx, (ay + m_Block) - by)
					+	Get_Weight((ax + m_Block) - bx, (ay - m_Block) - by)
					+	Get_Weight((ax - m_Block) - bx, (ay + m_Block) - by)
					+	Get_Weight((ax - m_Block) - bx, (ay - m_Block) - by) ) / 5.0
			);
		}

		return( Get_Weight(ax - bx, ay - by) );
	}

	void							Set_Value				(int x, int y, double z, double v)
	{
		if( m_pGrid     )	m_pGrid    ->Set_Value(x, y, m_bLog    ? exp (z) : z);
		if( m_pVariance )	m_pVariance->Set_Value(x, y, m_bStdDev ? sqrt(v) : v);
	}

	void							Set_NoData				(int x, int y)
	{
		if( m_pGrid     )	m_pGrid    ->Set_NoData(x, y);
		if( m_pVariance )	m_pVariance->Set_NoData(x, y);
	}


private:

	bool							m_bStdDev;

	double							m_Block;

	CSG_Parameters_Grid_Target		m_Grid_Target;

	CSG_Trend						m_Model;

	CSG_Grid						*m_pGrid, *m_pVariance;

	class CVariogram_Dialog			*m_pVariogram;


	bool							_Initialise_Grids		(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__kriging_base_H
