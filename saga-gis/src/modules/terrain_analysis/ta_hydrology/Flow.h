/**********************************************************
 * Version $Id: Flow.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
//                                                       //
//														 //
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
#include "MLB_Interface.h"


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
class ta_hydrology_EXPORT CFlow : public CSG_Module_Grid  
{
public:
	CFlow(void);
	virtual ~CFlow(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("R:Catchment Area" ));	}

	void					Set_Point		(int x, int y);


protected:

	bool					bKill_Slope, bKill_Aspect;

	int						Step;

	double					MFD_Converge;

	CSG_Grid					*pDTM, *pRoute, *pWeight,
							*pCatch,
							*pCatch_Slope,
							*pCatch_Height,
							*pCatch_Aspect,
							*pFlowPath,
							*pCatch_AspectY,
							*pMaterial,
							*pTarget,
							*pAccu_Tot,
							*pAccu_Left,
							*pAccu_Right;


	virtual bool			On_Execute(void);

	virtual void			On_Initialize	(void)			= 0;
	virtual void			On_Finalize		(void)			{}

	virtual bool			Calculate		(void)			= 0;
	virtual bool			Calculate		(int x, int y)	= 0;

	void					Init_Cell		(int x, int y);

	void					Get_Gradient	(int x, int y, double &Slope, double &Aspect);

	void					Add_Fraction	(int x, int y, int Direction, double Fraction = 1);
	void					Add_Portion		(int x, int y, int ix, int iy, int Direction);
	void					Find_Sides		(int x, int y, int Direction, bool &left, bool &right);


private:

	bool					bPoint, delete_pMaterial;

	int						xPoint, yPoint;


	void					Finalize		(void);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Flow_H
