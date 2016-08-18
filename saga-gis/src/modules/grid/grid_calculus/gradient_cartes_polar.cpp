/**********************************************************
 * Version $Id: gradient_cartes_polar.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    gradient_cartes_polar.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gradient_cartes_polar.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGradient_Cartes_To_Polar::CGradient_Cartes_To_Polar(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Gradient Vector from Cartesian to Polar Coordinates"));

	Set_Author		(SG_T("Victor Olaya & Volker Wichmann (c) 2004-2010"));

	Set_Description	(_TW(
		"Converts gradient vector from directional components (Cartesian) "
		"to polar coordinates (direction or aspect angle and length or tangens of slope).\n"
		"The tool supports three conventions on how to measure and output the angle of direction:\n"
		"(a) mathematical: direction angle is zero in East direction and the angle increases counterclockwise\n"
		"(b) geographical: direction angle is zero in North direction and the angle increases clockwise\n"
		"(c) zero direction and orientation are user defined\n"
	));

	Parameters.Add_Grid(NULL, "DX"	, _TL("X Component")	, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "DY"	, _TL("Y Component")	, _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, "DIR"	, _TL("Direction")		, _TL(""), PARAMETER_OUTPUT);	
	Parameters.Add_Grid(NULL, "LEN"	, _TL("Length")			, _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice(
		NULL	, "UNITS"			, _TL("Polar Angle Units"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("radians"),
			_TL("degree")
		), 0
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "SYSTEM"			, _TL("Polar Coordinate System"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("mathematical"),
			_TL("geographical"),
			_TL("user defined")
		), 1
	);

	Parameters.Add_Value(
		pNode	, "SYSTEM_ZERO"		, _TL("User defined Zero Direction"),
		_TL("given in degree clockwise from North direction"),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 360.0, true
	);

	Parameters.Add_Choice(
		pNode	, "SYSTEM_ORIENT"	, _TL("User defined Orientation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("clockwise"),
			_TL("counterclockwise")
		), 0
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGradient_Cartes_To_Polar::On_Execute(void)
{
	bool		bDegree, bClockwise;
	int			Method;
	double		DX, DY, DIR, Zero;
	CSG_Grid	*pDX, *pDY, *pDIR, *pLEN;

	//-----------------------------------------------------
	pDX		= Parameters("DX")		->asGrid();
	pDY		= Parameters("DY")		->asGrid();
	pDIR	= Parameters("DIR")		->asGrid();
	pLEN	= Parameters("LEN")		->asGrid();

	bDegree	= Parameters("UNITS")	->asInt() == 1;
	Method	= Parameters("SYSTEM")	->asInt();

	if( Method == 0 )	// mathematic
	{
		Zero		= M_PI_090;
		bClockwise	= false;
	}
	else
	{
		Zero		= Parameters("SYSTEM_ZERO")->asDouble() * M_DEG_TO_RAD;
		bClockwise	= Parameters("SYSTEM_ORIENT")->asInt() == 0;
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{		
		for(int x=0; x<Get_NX(); x++)
		{
			if( pDX->is_NoData(x, y) || pDY->is_NoData(x, y) )
			{
				pLEN->Set_NoData(x, y);
				pDIR->Set_NoData(x, y);
			}
			else
			{
				DX	= pDX->asDouble(x, y);
			    DY	= pDY->asDouble(x, y);

				if( DX == 0.0 && DY == 0.0 )
				{
					pLEN->Set_Value (x, y, 0.0);
					pDIR->Set_NoData(x, y);
				}
				else
				{
					DIR	= DY != 0.0 ? fmod(M_PI_360 + atan2(DX, DY), M_PI_360) : DX < 0.0 ? M_PI_270 : M_PI_090;

					if( Method != 1 )	// not geographic
					{
						DIR	= fmod(M_PI_360 + (bClockwise ? DIR - Zero : Zero - DIR), M_PI_360);
					}

					pLEN->Set_Value(x, y, sqrt(DX*DX + DY*DY));
					pDIR->Set_Value(x, y, bDegree ? M_RAD_TO_DEG * DIR : DIR);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGradient_Polar_To_Cartes::CGradient_Polar_To_Cartes(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Gradient Vector from Polar to Cartesian Coordinates"));

	Set_Author		(SG_T("Victor Olaya & Volker Wichmann (c) 2004-2010"));

	Set_Description	(_TW(
		"Converts gradient vector from polar coordinates (direction or aspect angle and length or tangens of slope) "
		"to directional components (Cartesian).\n"
		"The tool supports three conventions on how the angle of direction can be supplied:\n"
		"(a) mathematical: direction angle is zero in East direction and the angle increases counterclockwise\n"
		"(b) geographical: direction angle is zero in North direction and the angle increases clockwise\n"
		"(c) zero direction and orientation are user defined\n"
	));

	Parameters.Add_Grid(NULL, "DIR"	, _TL("Direction")		, _TL(""), PARAMETER_INPUT);	
	Parameters.Add_Grid(NULL, "LEN"	, _TL("Length")			, _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, "DX"	, _TL("X Component")	, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "DY"	, _TL("Y Component")	, _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice(
		NULL	, "UNITS"			, _TL("Polar Angle Units"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("radians"),
			_TL("degree")
		), 0
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "SYSTEM"			, _TL("Polar Coordinate System"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("mathematical"),
			_TL("geographical"),
			_TL("user defined")
		), 1
	);

	Parameters.Add_Value(
		pNode	, "SYSTEM_ZERO"		, _TL("User defined Zero Direction"),
		_TL("given in degree clockwise from North direction"),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 360.0, true
	);

	Parameters.Add_Choice(
		pNode	, "SYSTEM_ORIENT"	, _TL("User defined Orientation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("clockwise"),
			_TL("counterclockwise")
		), 0
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGradient_Polar_To_Cartes::On_Execute(void)
{
	bool		bDegree, bClockwise;
	int			Method;
	double		LEN, DIR, Zero;
	CSG_Grid	*pDX, *pDY, *pDIR, *pLEN;

	//-----------------------------------------------------
	pDX		= Parameters("DX")		->asGrid();
	pDY		= Parameters("DY")		->asGrid();
	pDIR	= Parameters("DIR")		->asGrid();
	pLEN	= Parameters("LEN")		->asGrid();

	bDegree	= Parameters("UNITS")	->asInt() == 1;
	Method	= Parameters("SYSTEM")	->asInt();

	if( Method == 0 )	// mathematic
	{
		Zero		= M_PI_090;
		bClockwise	= false;
	}
	else
	{
		Zero		= Parameters("SYSTEM_ZERO")->asDouble() * M_DEG_TO_RAD;
		bClockwise	= Parameters("SYSTEM_ORIENT")->asInt() == 0;
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{		
		for(int x=0; x<Get_NX(); x++)
		{
			if( pLEN->is_NoData(x, y) || pDIR->is_NoData(x, y) )
			{
				pDX->Set_NoData(x, y);
				pDY->Set_NoData(x, y);
			}
			else
			{
				LEN	= pLEN->asDouble(x, y);
			    DIR	= pDIR->asDouble(x, y);

				if( bDegree )
				{
					DIR	*= M_DEG_TO_RAD;
				}

				if( Method != 1 )	// not geographic
				{
					DIR	= bClockwise ? DIR - Zero : Zero - DIR;
				}

				pDX->Set_Value(x, y, LEN * sin(DIR));
				pDY->Set_Value(x, y, LEN * cos(DIR));
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
