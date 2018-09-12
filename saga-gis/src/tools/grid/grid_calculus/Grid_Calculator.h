/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Grid_Calculator.h                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Andre Ringeler                     //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
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
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Calculator_H
#define HEADER_INCLUDED__Grid_Calculator_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Calculator_Base : public CSG_Tool_Grid
{
public:
	CGrid_Calculator_Base(void);


protected:

	bool						m_bUseNoData, m_bPosition[6];

	int							m_nValues;

	static double				m_NoData_Value;

	CSG_Formula					m_Formula;

	TSG_Grid_Resampling			m_Resampling;


	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	bool						Initialize				(int nGrids, int nXGrids);

	TSG_Data_Type				Get_Result_Type			(void);

	bool						Get_Result				(const CSG_Vector &Values, double &Result);

	static double				Get_NoData_Value		(void)	{	return( m_NoData_Value );	}


private:

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Calculator : public CGrid_Calculator_Base
{
public:
	CGrid_Calculator(void);


protected:

	virtual bool				On_Execute				(void);


private:

	CSG_Parameter_Grid_List		*m_pGrids, *m_pXGrids;


	bool						Get_Values				(int x, int y, CSG_Vector &Values);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrids_Calculator : public CGrid_Calculator_Base
{
public:
	CGrids_Calculator(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Grid|Grid Collection|Calculus") );	}


protected:

	virtual bool				On_Execute				(void);


private:

	CSG_Parameter_Grids_List	*m_pGrids, *m_pXGrids;


	bool						Get_Values				(int x, int y, int z, CSG_Vector &Values);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Calculator_H
