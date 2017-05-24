/**********************************************************
 * Version $Id: Exercise_03.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Exercise_03.h                      //
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
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Exercise_03_H
#define HEADER_INCLUDED__Exercise_03_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CExercise_03 : public CSG_Tool_Grid
{
public: ////// public members and functions: //////////////

	CExercise_03(void);							// constructor
	virtual ~CExercise_03(void);				// destructor


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute(void);		// always override this function


private: ///// private members and functions: /////////////

	CSG_Grid				*m_pInput, *m_pOutput;


	bool				Method_01(void);
	bool				Method_02(void);
	bool				Method_03(void);
	bool				Method_04(void);

};


//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Exercise_03_H
