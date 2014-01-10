/**********************************************************
 * Version $Id: D8_Flow_Analysis.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  D8_Flow_Analysis.h                   //
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
#ifndef HEADER_INCLUDED__D8_Flow_Analysis_H
#define HEADER_INCLUDED__D8_Flow_Analysis_H


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
class ta_channels_EXPORT CD8_Flow_Analysis : public CSG_Module_Grid
{
public:
	CD8_Flow_Analysis(void);


protected:

	virtual bool				On_Execute			(void);


private:

	int							m_Threshold;

	CSG_Grid					*m_pDEM, *m_pDir, *m_pOrder, *m_pBasins, m_Nodes;

	CSG_Shapes					*m_pSegments;


	void						Get_Direction		(void);

	void						Get_Order			(void);
	int							Get_Order			(int x, int y);

	void						Get_Nodes			(void);
	void						Set_Node			(int x, int y, int id, int type, CSG_Shape *pNode);

	void						Get_Basins			(void);
	int							Get_Basin			(int x, int y);

	void						Get_Segments		(void);
	void						Get_Segment			(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__D8_Flow_Analysis_H
