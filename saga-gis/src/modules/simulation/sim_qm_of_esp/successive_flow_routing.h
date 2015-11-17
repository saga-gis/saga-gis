/**********************************************************
 * Version $Id: successive_flow_routing.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      qm_of_esp                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                successive_flow_routing.h              //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__successive_flow_routing_H
#define HEADER_INCLUDED__successive_flow_routing_H

///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "flow_routing.h"


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSuccessive_Flow_Routing : public CSG_Module_Grid
{
public:

	CSuccessive_Flow_Routing(void);

	virtual CSG_String	Get_MenuPath			(void)	{	return( _TL("A:Simulation|Hydrology" ));	}


protected:

	virtual bool		On_Execute				(void);


private:

	double				m_Manning;

	CSG_Grid			*m_pDEM, *m_pFlow, *m_pSlope;


	void				Fill_Sinks				(void);

	void				Set_Flow				(int x, int y);
	void				Set_Channel_Slope		(int x, int y);

};


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__successive_flow_routing_H
