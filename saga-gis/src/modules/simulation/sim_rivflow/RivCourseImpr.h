
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       RivFlow                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   RivCourseImpr.h                     //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                   Christian Alwardt                   //
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
//    e-mail:     alwardt@ifsh.de                        //
//                                                       //
//    contact:    Christian Alwardt                      //
//                Institute for Peace Research           //
//                and Security Policy (IFSH)             //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//		RivTool 0.1										 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__RivCourseImpr_H
#define HEADER_INCLUDED__RivCourseImpr_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>

//---------------------------------------------------------
//#########################################################

class CRivCourseImpr : public CSG_Module_Grid_Interactive {

public:
	CRivCourseImpr(void);

protected:
	virtual bool On_Execute(void);
	virtual bool On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode);

private:
	CSG_Grid *m_pDTM;
	double m_pDiffH, minVal;
	int nX, nY;
	void getNeighFlowGridValue(int x, int y, double minVal);


};

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__RivCourseImpr_H
