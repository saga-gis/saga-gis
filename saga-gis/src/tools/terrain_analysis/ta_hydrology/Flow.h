
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        Flow.h                         //
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
#ifndef HEADER_INCLUDED__Flow_H
#define HEADER_INCLUDED__Flow_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	flow_D8		= 0,
	flow_Rho8,
	flow_DInf,
	flow_MFD,
	flow_BRM,
	flow_KRA,
	flow_DEMON
};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CFlow : public CSG_Tool_Grid  
{
public:
	CFlow(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Flow Accumulation" ));	}

	void					Set_Point				(int x, int y);


protected:

	int						m_Step;

	bool					m_MFD_bContour;

	double					m_MFD_Converge;

	CSG_Grid				*m_pDTM, *m_pRoute, *m_pWeights, *m_pFlow, *m_pFlow_Length,
							*m_pVal_Input, *m_pVal_Mean,
							*m_pAccu_Material, *m_pAccu_Target, *m_pAccu_Total, *m_pAccu_Left, *m_pAccu_Right;


	virtual	int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);

	virtual void			On_Initialize			(void)			= 0;
	virtual void			On_Finalize				(void)			{}

	virtual bool			Calculate				(void)			= 0;
	virtual bool			Calculate				(int x, int y)	= 0;

	void					Init_Cell				(int x, int y);

	void					Get_Gradient			(int x, int y, double &Slope, double &Aspect);

	void					Add_Fraction			(int x, int y, int Direction, double Fraction = 1.0);
	void					Add_Portion				(int x, int y, int ix, int iy, int Direction);
	void					Find_Sides				(int x, int y, int Direction, bool &left, bool &right);


private:

	bool					m_bPoint, m_bKill_Material;

	int						m_xPoint, m_yPoint;


	void					_Finalize				(void);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Flow_H
