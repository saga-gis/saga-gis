
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
//                     DataTrans.h                       //
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

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__DataTrans_H
#define HEADER_INCLUDED__DataTrans_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>
#include <string>
using namespace std;

//---------------------------------------------------------
//#########################################################

class CDataTrans : public CSG_Module_Grid
{
public: ////// public members and functions: //////////////

	CDataTrans(void);							// constructor


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute	(void);		// always overwrite this function


private: ///// private members and functions: /////////////
	CSG_Grid *pSource1, *pSource2;
	
	int NX, NY, m_pID, i;
	bool CDataTrans::Set_TFile(CSG_Grid *pSource1, CSG_Grid *pSource2);
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__DataTrans_H