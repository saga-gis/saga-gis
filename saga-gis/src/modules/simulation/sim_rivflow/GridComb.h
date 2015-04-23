
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
//                      GridComb.h                       //
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
#ifndef HEADER_INCLUDED__GridComb_H
#define HEADER_INCLUDED__GridComb_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>
#include <string>
using namespace std;
//---------------------------------------------------------
//#########################################################

class CGridComb : public CSG_Module_Grid
{
public: ////// public members and functions: //////////////

	CGridComb(void);							// constructor


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute	(void);		// always overwrite this function


private: ///// private members and functions: /////////////

int sY, eY;
int NX, NY;
int m_pFvA; //Auswahlplatzhalter ob Wasserentnahme oder Wassernutzung berücksichtigt werden soll

CSG_String m_pDataFolder;
CSG_String SPath;

CSG_Grid *m_pRefGrid;
CSG_Grid *m_pGridComb;
CSG_Grid *m_pDomW;
CSG_Grid *m_pElecW;
CSG_Grid *m_pLiveW;
CSG_Grid *m_pManW;
CSG_Grid *m_pIrrW;

bool SaveParameters();

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__GridComb_H
