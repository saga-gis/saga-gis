
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
//                      RivBasin.h                       //
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
#ifndef HEADER_INCLUDED__RivBasin_H
#define HEADER_INCLUDED__RivBasin_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>
#include <string>
using namespace std;
//---------------------------------------------------------
//#########################################################

class CRivBasin : public CSG_Module_Grid
{
public: ////// public members and functions: //////////////

	CRivBasin(void);							// constructor


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute	(void);		// always overwrite this function


private: ///// private members and functions: /////////////
	CSG_Grid *m_pDTM, *m_pGrad, *m_pDirec, *m_pRivGrids, *m_pRivGrad, *m_pRaster, *m_pRSpeed, *m_pBasinShare, *m_pSharedRivCons, *m_pWCons, *m_pNumInFlowCells;
	double pCr, BasinGrids,	statV, m_pVTresh;
	int nCr, statN;
	void Set_RivParam(int x, int y);
	bool Set_BasinShare(void);
	bool WriteLog(string s);
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__RivBasin_H
