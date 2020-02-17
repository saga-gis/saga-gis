
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       pj_proj                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    globe_gores.h                      //
//                                                       //
//                 Olaf Conrad (C) 2020                  //
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
#ifndef HEADER_INCLUDED__globe_gores_H
#define HEADER_INCLUDED__globe_gores_H


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
class CGlobe_Gores : public CSG_Tool_Grid
{
public:
	CGlobe_Gores(void);

	virtual CSG_String				Get_MenuPath			(void)	{	return( _TL("Tools") );	}

	virtual bool					do_Sync_Projections		(void)	const	{	return( false );	}


protected:

	virtual int						On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool					On_Execute				(void);


private:

	CSG_Grid						*m_pGores;


	bool							Add_Gore				(int iGore, int nGores);

	bool							Add_Gore				(int iGore, int nGores, CSG_Grid *pGore);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__globe_gores_H
