///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   sim_geomorphology                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     GPP_Model.h                       //
//                                                       //
//                 Copyright (C) 2016 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@alps-gmbh.com                 //
//                wichmann@laserdata.at                  //
//                                                       //
//    contact:    alpS GmbH                              //
//                Grabenweg 68                           //
//                A-6020 Innsbruck                       //
//                www.alps-gmbh.com                      //
//                                                       //
//                LASERDATA GmbH                         //
//                Technikerstr. 21a                      //
//                A-6020 Innsbruck                       //
//                www.laserdata.at                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__GPP_Model_H
#define HEADER_INCLUDED__GPP_Model_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"

#include "GPP_Model_BASE.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGPP_Model : public CSG_Tool_Grid, protected CGPP_Model_BASE
{
public:
	CGPP_Model(void);
	virtual ~CGPP_Model(void);


protected:

	virtual bool	On_Execute(void);

	virtual int		On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

private:


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__GPP_Model_H

