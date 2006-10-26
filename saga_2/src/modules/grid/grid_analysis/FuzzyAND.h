
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 contrib_boggia_massei                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//						FuzzyAND.h						 //
//                                                       //
//                 Copyright (C) 2003 by                 //
//           Antonio Boggia and Gianluca Massei          //
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
//    e-mail:     boggia@unipg.it						 //
//				  g_massa@libero.it				     	 //
//                                                       //
//    contact:    Antonio Boggia                         //
//                Gianluca Massei                        //
//                Department of Economics and Appraisal  //
//                University of Perugia - Italy			 //
//                www.unipg.it                           //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //												
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if !defined(AFX_FuzzyAND_H__4147D540_3A5F_11D6_92B0_0050BA1B4192__INCLUDED_)
#define AFX_FuzzyAND_H__4147D540_3A5F_11D6_92B0_0050BA1B4192__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CFuzzyAND : public CSG_Module_Grid  
{
public:
	CFuzzyAND(void);
	virtual ~CFuzzyAND(void);

	virtual const char *	Get_MenuPath	(void)	{	return( _TL("R:Fuzzy Logic") );	}

protected:

	virtual bool			On_Execute(void);

};

#endif // !defined(AFX_FuzzyAND_H__4147D540_3A5F_11D6_92B0_0050BA1B4192__INCLUDED_)
