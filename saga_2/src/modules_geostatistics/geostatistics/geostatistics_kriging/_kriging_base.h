
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 Geostatistics_Kriging                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    _Kriging_Base.h                    //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//    contact:    Olaf Conrad                            //
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
#ifndef HEADER_INCLUDED___Kriging_Base_H
#define HEADER_INCLUDED___Kriging_Base_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class geostatistics_kriging_EXPORT C_Kriging_Base : public CSG_Module
{
public:
	C_Kriging_Base(void);
	virtual ~C_Kriging_Base(void);

	virtual const SG_Char *	Get_MenuPath	(void)	{	return( _TL("R:Kriging") );	}


protected:

	virtual bool			On_Execute		(void);


	bool					m_bBlock;

	int						m_zField;

	double					m_Block;

	CSG_Points_3D			m_Points;

	CSG_Vector				m_G;

	CSG_Matrix				m_W;

	CSG_PRQuadTree			m_Search;

	CSG_Grid				*m_pGrid, *m_pVariance;

	CSG_Shapes				*m_pShapes;


	virtual bool			On_Initialise	(void)	{	return( true );	}

	virtual bool			Get_Value		(double x, double y, double &z, double &Variance)	= 0;

	double					Get_Weight		(double Distance);
	double					Get_Weight		(double dx, double dy);


private:

	bool					m_bLog;

	int						m_Model;

	double					m_Nugget, m_Sill, m_Range, m_BLIN, m_BEXP, m_APOW, m_BPOW;


	bool					_Get_Points		(void);
	bool					_Get_Grid		(void);
	CSG_Grid *				_Get_Grid		(TSG_Rect Extent);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED___Kriging_Base_H
