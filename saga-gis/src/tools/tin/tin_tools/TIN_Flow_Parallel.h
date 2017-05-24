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
//                       TIN_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  TIN_Flow_Parallel.h                  //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
#ifndef HEADER_INCLUDED__TIN_Flow_Parallel_H
#define HEADER_INCLUDED__TIN_Flow_Parallel_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTIN_Flow_Parallel : public CSG_Tool
{
public:
	CTIN_Flow_Parallel(void);
	virtual ~CTIN_Flow_Parallel(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("Terrain Analysis") );	}


protected:

	virtual bool				On_Execute		(void);


private:

	int							m_iHeight, m_iArea, m_iFlow, m_iSpecific;

	CSG_TIN						*m_pFlow;


	void						Let_it_flow_single		(CSG_TIN_Node *pPoint);
	void						Let_it_flow_multiple	(CSG_TIN_Node *pPoint);

};

#endif // #ifndef HEADER_INCLUDED__TIN_Flow_Parallel_H
