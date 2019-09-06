
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                    sim_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               timed_flow_accumulation.h               //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__timed_flow_accumulation_H
#define HEADER_INCLUDED__timed_flow_accumulation_H


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
class CTimed_Flow_Accumulation : public CSG_Tool_Grid
{
public:
	CTimed_Flow_Accumulation(void);

//	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:timed_flow_accumulation") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	double						m_K, m_R, m_Time;

	CSG_Grid					*m_pDEM, *m_pK, *m_pAccu, *m_pFlow, *m_pTime, *m_pConc;


	double						Get_K					(int x, int y);
	double						Get_R					(int x, int y);
	double						Get_Velocity			(double k, double R, double I);
	double						Get_Velocity			(int x, int y);
	double						Get_Travel_Time			(int x, int y);
	double						Get_Travel_Time			(int x, int y, int Direction);

	void						Add_Flow				(int x, int y, int Direction, double Proportion = 1.);

	bool						Get_D8					(int x, int y, int &Direction);
	bool						Set_D8					(int x, int y);

	bool						Get_MFD					(int x, int y, double Flow[8]);
	bool						Set_MFD					(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__timed_flow_accumulation_H
