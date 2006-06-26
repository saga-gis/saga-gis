
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Fractals                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Pythagoras_Tree.h                   //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#ifndef HEADER_INCLUDED__Pythagoras_Tree_H
#define HEADER_INCLUDED__Pythagoras_Tree_H



//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPythagoras_Tree : public CModule
{
public:
	CPythagoras_Tree(void);
	virtual ~CPythagoras_Tree(void);


protected:

	virtual bool			On_Execute(void);


private:

	int						Iteration, Method;

	double					Min_Size, Size, sin_Angle, cos_Angle, var_Min, var_Range, var_Angle;

	CShapes					*pShapes;


	void					Set_Quadrat	(TGEO_Point pt_A, TGEO_Point pt_B);

	void					Add_Shape	(TGEO_Point pt_A, TGEO_Point pt_B, TGEO_Point pt_C, TGEO_Point pt_D);
	void					Add_Shape	(TGEO_Point pt_A, TGEO_Point pt_B, TGEO_Point pt_C);

};

#endif // #ifndef HEADER_INCLUDED__Pythagoras_Tree_H
