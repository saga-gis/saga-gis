
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       image_io                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Geostat_Kriging.h                     //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    SAGA User Group Association            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#ifndef HEADER_INCLUDED__Geostat_Kriging_H
#define HEADER_INCLUDED__Geostat_Kriging_H


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
class CGeostat_Kriging : public CSG_Module
{
public:
	CGeostat_Kriging(void);
	virtual ~CGeostat_Kriging(void);


protected:

	virtual bool			On_Execute				(void);


	virtual bool			Get_Value				(double x, double y, double &z, double &Variance);

	double					Get_Weight				(double d);
	double					Get_Weight				(double dx, double dy);


private:

	bool					m_bBlock;

	int						m_zField;

	double					m_Block;

	CSG_Points_3D			m_Points;

	CSG_Vector				m_G;

	CSG_Matrix				m_W;

	CSG_Shapes_Search		m_Search;

	CSG_Trend				m_Variogram;

	CSG_Shapes				*m_pPoints;

	CSG_Grid				*m_pGrid, *m_pVariance;


	bool					_Initialise				(void);
	bool					_Initialise_Grids		(void);
	bool					_Finalise				(void);

	bool					_Interpolate			(void);

	bool					_Get_Variances			(void);
	bool					_Get_Differences		(CSG_Table *pTable, int zField, int nSkip, double maxDist);


private:

	int						m_nPoints_Min, m_nPoints_Max;

	double					m_Radius;


	int						Get_Weights				(double x, double y);

};

#endif // #ifndef HEADER_INCLUDED__Geostat_Kriging_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
