/**********************************************************
 * Version $Id: Mine_Sweeper.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                                                       //
//                     Mine_Sweeper                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Mine_Sweeper.h                     //
//                                                       //
//           Copyright (C) 2003 by Andre Ringeler        //
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
#ifndef HEADER_INCLUDED__Mine_Sweeper_H
#define HEADER_INCLUDED__Mine_Sweeper_H

//---------------------------------------------------------
#include "MLB_Interface.h"

#include <time.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

class CTimer
{
public:

	CTimer(void);
	~CTimer(void);

	int Time(void);

protected:

	time_t starttime;

};


class CMine_Sweeper : public CSG_Tool_Interactive
{
public:
	CMine_Sweeper(void);
	virtual ~CMine_Sweeper(void);


	CSG_Grid	*pInput;

protected:

	virtual bool			On_Execute(void);
	virtual bool			On_Execute_Finish	(void);
	virtual bool			On_Execute_Position	(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);

	bool					Get_Grid_Pos(int &x, int &y);

	void SetSprite(int xpos, int ypos, int nr);

	bool	MakeBoard(int level);

	void Make_GameBoard(int xpos,int ypos);

	void Show_GameBoard(bool ShowMines);

	int Get_Number_of_Bombs(int xpos, int ypos);

	bool Play(int xpos, int ypos,bool computer_move);

	void Mark(int xpos, int ypos);

	void ResetBoard(int xpos, int ypos);
	
	int Mine_NX,Mine_NY, N_Mines;

	int OpenFields, MarkedMines;

	CSG_Grid *GameBoard, *FlagBoard;

	bool First_Click;

	CTimer *Time; 

private:

};



#endif // #ifndef HEADER_INCLUDED__Mine_Sweeper_H
