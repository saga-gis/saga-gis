/**********************************************************
 * Version $Id: KinWav_D8.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     sim_hydrology                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     KinWav_D8.h                       //
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
#ifndef HEADER_INCLUDED__KinWav_D8_H
#define HEADER_INCLUDED__KinWav_D8_H


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
class CKinWav_D8 : public CSG_Tool_Grid  
{
public:
	CKinWav_D8(void);


protected:

	virtual int			On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool		On_Execute				(void);


private:

	int					m_Routing, m_MaxIter;

	double				m_dTime, m_Epsilon;

	CSG_Grid			*m_pDEM, *m_pFlow, m_Flow_t0, *m_dFlow, m_Length, m_Alpha;

	CSG_Table			*m_pGauges_Flow;

	CSG_Shapes			*m_pGauges;


	bool				Initialize				(void);
	bool				Finalize				(void);

	void				Set_D8					(int x, int y);
	void				Set_MFD					(int x, int y);

	void				Set_Flow				(double Time);

	void				Set_Runoff				(int x, int y);
	void				Set_Runoff				(int x, int y, double Q);

	bool				Gauges_Initialise		(void);
	bool				Gauges_Set_Flow			(double Time);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__KinWav_D8_H
