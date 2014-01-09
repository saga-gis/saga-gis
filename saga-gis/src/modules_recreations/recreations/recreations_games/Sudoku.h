/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                        Sudoku                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       Sudoku.h                        //
//                                                       //
//           Copyright (C) 2006 by Victor Olaya          //
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
//    e-mail:     your@e-mail.abc                        //
//                                                       //
//    contact:    Your Name                              //
//                And Address                            //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Sudoku_H
#define HEADER_INCLUDED__Sudoku_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

class CSudoku : public CSG_Module_Grid_Interactive
{
public:
	CSudoku(void);
	virtual ~CSudoku(void);


protected:

	virtual bool			On_Execute			(void);
	virtual bool			On_Execute_Finish	(void);
	virtual bool			On_Execute_Position	(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode);


private:

	bool					**m_pFixedCells;

	int						**m_pSudoku;

	CSG_Grid					*m_pBoard;


	bool					Get_Grid_Pos		(int &x, int &y);

	void DrawBoard();
	void DrawCell(int iXCell, int iYCell, bool *pIsPossible);
	void CreateSudoku();
	void DrawSquare(int iX, int iY, int iColor, int iSize);
	void GetPossibleValues(int iX, int iY, bool *pIsPossible);

};

#endif // #ifndef HEADER_INCLUDED__Sudoku_H
