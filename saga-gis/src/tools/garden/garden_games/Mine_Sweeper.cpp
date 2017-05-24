/**********************************************************
 * Version $Id: Mine_Sweeper.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                   Mine_Sweeper.cpp                    //
//                                                       //
//            Copyright (C) 2003 Andre Ringeler          //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//														 //	
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Mine_Sweeper.h"

///////////////////////////////////////////////////////////
//														 //
//				Import Resourcen	   					 //
//														 //
///////////////////////////////////////////////////////////

extern unsigned char	mine_res[];
extern unsigned char	mine_res_color[];
extern unsigned int		SPRITE_SIZE;

///////////////////////////////////////////////////////////
//														 //
//				Defines				   					 //
//														 //
///////////////////////////////////////////////////////////

#define FLAG				1
#define QUESTION			2

#define isBomb				1
#define isOpen				8
#define isBumm				16

#define SPRITE_CLOSE		0
#define SPRITE_FLAG			1
#define SPRITE_QUESTION		2
#define SPRITE_BOMB_BUMM	3
#define SPRITE_BOMB_NO		4
#define SPRITE_BOMB			5
#define SPRITE_OPEN			15

#define SPRITE_NUMMER(x)    (15-x)

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMine_Sweeper::CMine_Sweeper(void)
{
	Set_Name	(_TL("Mine Sweeper"));

	Set_Author	(_TL("Copyrights (c) 2003 by Andre Ringeler"));

	Set_Description	(_TW(
		"A Mine Sweeper Clone\n"
		"(created by Andre Ringeler).")
	);

	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	,"LEVEL"	,_TL("Level"),
		_TL("Game Level"),
		_TW(	
		"Beginer|"
		"Advanced|"
		"Profi|")	,1
	);	
}

//---------------------------------------------------------
CMine_Sweeper::~CMine_Sweeper(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMine_Sweeper::MakeBoard(int level)
{
	int		i, x, y;
	CSG_Colors	Colors;

	switch( level )
	{
		case 0:	Mine_NX = 8;	Mine_NY = 8;	N_Mines=10;
			break;
		
		case 1: Mine_NX = 16;	Mine_NY = 16;	N_Mines=40;
			break;
		
		case 2: Mine_NX = 30;	Mine_NY = 16;	N_Mines=99;
			break;
	}

	pInput	= SG_Create_Grid(SG_DATATYPE_Int,SPRITE_SIZE*Mine_NX, SPRITE_SIZE*Mine_NY);
	pInput->Set_Name(_TL("Mine Sweeper"));
	Parameters("GRID")->Set_Value(pInput);

	//-----------------------------------------------------
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pInput, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		pLUT->asTable()->Del_Records();

		for(i=0; i<16; i++)
		{
			CSG_Table_Record	*pRecord	= pLUT->asTable()->Add_Record();
			
			pRecord->Set_Value(0, SG_GET_RGB(mine_res_color[i*3], mine_res_color[i*3+1], mine_res_color[i*3+2]));
			pRecord->Set_Value(3, i);
		}

		DataObject_Set_Parameter(pInput, pLUT);
		DataObject_Set_Parameter(pInput, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	Colors.Set_Count(16);
	for ( i=0;i<16; i++)
	{
		Colors.Set_Color(i, SG_GET_RGB(mine_res_color[i*3],	mine_res_color[i*3+1],	mine_res_color[i*3+2]));
	}
	DataObject_Set_Colors(pInput, Colors);
	DataObject_Update(pInput, 0.0, 15.0, true);

	//-----------------------------------------------------
	for(  y = 0; y <  Mine_NY; y++)	
	for(  x = 0; x <  Mine_NX; x++)
	{
		SetSprite(x,y,SPRITE_CLOSE);
	}

	pInput->Set_Value(0, 0);

	return true;			
}

bool CMine_Sweeper::On_Execute(void)
{
	MakeBoard(Parameters("LEVEL")->asInt());

	GameBoard	= (CSG_Grid *) new CSG_Grid(SG_DATATYPE_Int,Mine_NX,Mine_NY);
	FlagBoard	= (CSG_Grid *) new CSG_Grid(SG_DATATYPE_Int,Mine_NX,Mine_NY);

	First_Click	= true;
	Time		= NULL;	

	return( true );
}


bool CMine_Sweeper::On_Execute_Finish(void)
{
	delete GameBoard;
	delete FlagBoard;

	if (Time) delete Time;

	return true;
}

	
void CMine_Sweeper::ResetBoard(int xpos, int ypos)
{
	OpenFields = 0;			MarkedMines = 0;

	FlagBoard->Assign();	GameBoard->Assign();

	if(Time) delete Time;

	Make_GameBoard( xpos , ypos);

	Time = new CTimer();
}

//---------------------------------------------------------
bool CMine_Sweeper::Get_Grid_Pos(int &x, int &y)
{
	bool	bResult;

	if( pInput && pInput->is_Valid() )
	{
		bResult	= true;

		//-------------------------------------------------
		x		= (int)(0.5 + (Get_xPosition() - pInput->Get_XMin()) / pInput->Get_Cellsize());

		if( x < 0 )
		{
			bResult	= false;
			x		= 0;
		}
		else if( x >= pInput->Get_NX() )
		{
			bResult	= false;
			x		= pInput->Get_NX() - 1;
		}

		//-------------------------------------------------
		y		= (int)(0.5 + (Get_yPosition() - pInput->Get_YMin()) / pInput->Get_Cellsize());

		if( y < 0 )
		{
			bResult	= false;
			y		= 0;
		}
		else if( y >= pInput->Get_NY() )
		{
			bResult	= false;
			y		= pInput->Get_NY() - 1;
		}

		return( bResult );
	}

	//-----------------------------------------------------
	x		= 0;
	y		= 0;

	return( false );
}


bool CMine_Sweeper::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	int ok = true;
	
	int time;
	int redraw = false;
	int xpos; 	int ypos;

	if( !Get_Grid_Pos(xpos, ypos) )
		return( false );

	xpos/=SPRITE_SIZE; 	ypos/=SPRITE_SIZE;

	ypos=Mine_NY-1-ypos;
	
	switch( Mode )
	{
	default:
		return( false );

	case TOOL_INTERACTIVE_LDOWN:
		if(First_Click)
		{
			ResetBoard(xpos, ypos);
			First_Click=false;
		}

		ok = Play(xpos, ypos, false);
		redraw = true;
		break;

	case TOOL_INTERACTIVE_RDOWN:
		Mark(xpos, ypos);
		redraw = true;
		break;
	}

	if (redraw)
	{
		if(ok)
		{
			Show_GameBoard(false);
			
			time= Time->Time();
			
			Message_Add(CSG_String::Format(SG_T(":-) Time:%ds Mines:%d\n"),time,N_Mines-MarkedMines));
			
			if (OpenFields == Mine_NX*Mine_NY-N_Mines  )
			{
				Message_Add(CSG_String::Format(_TL(":-) :-) you are a winner :-) :-) Time:%ds\n"),time));
				Message_Dlg(CSG_String::Format(_TL(":-) :-) you are a winner :-) :-) Time:%ds\n"),time));
			
				Show_GameBoard(true);

				First_Click=true;
			}
		}
		else 
		{
			Show_GameBoard(true);

			Message_Dlg(CSG_String::Format(_TL(":-( :-( you are a loser :-( :-(")));
			Message_Add(CSG_String::Format(_TL(":-( :-( you are a loser :-( :-(")));

			First_Click=true;
		}
	}
	return true;
}



void CMine_Sweeper::Make_GameBoard(int xpos,int ypos)
{
	int mines = 0;

	srand( (unsigned)time( NULL ) );

	while(mines < N_Mines)
	{
		int mx,my;

		mx = rand()%Mine_NX;
		my = rand()%Mine_NY;

		if( !(GameBoard->asInt(mx, my) & isBomb) && !((mx==xpos)&&(my==ypos)) )
		{
			GameBoard->Set_Value(mx,my,isBomb);
			mines ++;
		}
	}
}


void CMine_Sweeper::Show_GameBoard(bool ShowMines)
{
	int x,y;

	if (ShowMines)
	{
		for(  y = 0; y <  Mine_NY; y++)	
		for(  x = 0; x <  Mine_NX; x++)
		{
			if(GameBoard->asInt(x, y) & isBomb)
			{
				if (FlagBoard->asInt(x,y) == FLAG)
					SetSprite(x,y,SPRITE_FLAG);
				else
					SetSprite(x,y,SPRITE_BOMB);
			}
			else
			{
				if (GameBoard->asInt(x, y) == isOpen )
					SetSprite(x,y,SPRITE_NUMMER(Get_Number_of_Bombs(x,y)));
				else			
					SetSprite(x,y,SPRITE_CLOSE);
			}
			
			if (GameBoard->asInt(x, y) & isBumm) SetSprite(x,y,SPRITE_BOMB_BUMM);

			if ( !(GameBoard->asInt(x, y) & isBomb) && (FlagBoard->asInt(x,y) == FLAG)) 
				SetSprite(x,y,SPRITE_BOMB_NO);
		}
	}
	else
	for(  y = 0; y <  Mine_NY; y++)	
	for(  x = 0; x <  Mine_NX; x++)
	{
		if(GameBoard->asInt(x, y) == isOpen)
		{
			SetSprite(x,y,SPRITE_NUMMER(Get_Number_of_Bombs(x,y)));
		}
		else
		{
			if(FlagBoard->asInt(x,y))
			SetSprite(x,y,FlagBoard->asInt(x,y));
			else
			SetSprite(x,y,SPRITE_CLOSE);
		}
	}

	DataObject_Update(pInput, 0.0, 15.0);
}


void CMine_Sweeper::SetSprite(int xpos, int ypos, int nr)
{
	unsigned int x,y;
	
	for(y=0;y<SPRITE_SIZE;y++)
	for(x=0;x<SPRITE_SIZE;x++)
		pInput->Set_Value( xpos * SPRITE_SIZE + x, pInput->Get_NY() - ypos * SPRITE_SIZE - y -1,
		                   (double)mine_res[ y * SPRITE_SIZE + x + nr * SPRITE_SIZE * SPRITE_SIZE]);
}


int CMine_Sweeper::Get_Number_of_Bombs(int xpos, int ypos)
{ 
	int i;
	int number = 0;

	for( i = 0; i<8; i++) 
		if (GameBoard->is_InGrid(pInput->Get_System().Get_xTo(i,xpos),pInput->Get_System().Get_yTo(i,ypos)))
			if (GameBoard->asInt(pInput->Get_System().Get_xTo(i,xpos),pInput->Get_System().Get_yTo(i,ypos)) & isBomb)
				number ++;
		
	return number;
}

bool CMine_Sweeper::Play(int xpos, int ypos,bool computer_move)
{
	int i;

	if ( (!GameBoard->is_InGrid(xpos,ypos))||
	   (FlagBoard->asInt(xpos, ypos)==1) ||	
       ((GameBoard->asInt(xpos, ypos)) & isBomb && (computer_move))
	)
	{
		return true;
	}

	if ( (GameBoard->asInt(xpos, ypos)) & isBomb && (!computer_move) )
	{
		GameBoard->Set_Value(xpos, ypos,isBomb|isBumm);
		return false;
	}

	if (  GameBoard->asInt(xpos, ypos) == 0 )
	{
		GameBoard->Set_Value(xpos, ypos, isOpen);
		
		OpenFields ++;

		if (Get_Number_of_Bombs(xpos, ypos) )
			return true;

		for( i = 0; i<8; i++) 
			Play(pInput->Get_System().Get_xTo(i,xpos),pInput->Get_System().Get_yTo(i,ypos) ,true);
	}
	return true;
}


void CMine_Sweeper::Mark(int xpos, int ypos)
{ 
	int val;

	val = FlagBoard->asInt(xpos , ypos);
	
	if (val == FLAG) MarkedMines --;
	
	val ++;	val%=3;

	if (val == FLAG) MarkedMines ++ ;

	FlagBoard->Set_Value(xpos, ypos, val);
}


CTimer::CTimer()
{
	time(&starttime);
}

CTimer::~CTimer()
{
}

int CTimer::Time()
{
	time_t nowtime;
	
	time(&nowtime);

	return (int) (nowtime-starttime);
}
