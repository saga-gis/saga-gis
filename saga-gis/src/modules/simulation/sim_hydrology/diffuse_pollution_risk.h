/**********************************************************
 * Version $Id: diffuse_pollution_risk.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     sim_hydrology                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                diffuse_pollution_risk.h               //
//                                                       //
//                 Copyright (C) 2015 by                 //
//                     Cosima Berger                     //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Cosima Berger                          //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#ifndef HEADER_INCLUDED__diffuse_pollution_risk_H
#define HEADER_INCLUDED__diffuse_pollution_risk_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDiffuse_Pollution_Risk : public CSG_Module_Grid
{
public:
	CDiffuse_Pollution_Risk(void);
	

protected:

	virtual int					On_Parameters_Enable		(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute					(void);


private:

	bool						m_bSingle;

	CSG_Grid					*m_pDEM, m_FlowDir, m_RainAcc, m_TWI, *m_pDelivery, *m_pRisk_Point, *m_pRisk_Diffuse;
	

	bool						Get_Flow_Proportions		(int x, int y, double Proportion[8]);

	bool						Set_Flow					(void);
	bool						Set_Flow					(int x, int y, double Rain);

	bool						Set_Delivery_Index			(void);

	bool						Get_Risk_Diffuse			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__diffuse_pollution_risk_H
