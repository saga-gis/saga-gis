
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_To_Contour.cpp                  //
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_To_Contour.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_To_Contour::CGrid_To_Contour(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Contour Lines from Grid"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description	(_TW(
		"Create contour lines (isolines) from grid values. ")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "INPUT"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL, "CONTOUR"	, _TL("Contour Lines"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "ZMIN"	, _TL("Minimum Contour Value"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		NULL, "ZMAX"	, _TL("Maximum Contour Value"),
		_TL(""),
		PARAMETER_TYPE_Double, 10000.0
	);

	Parameters.Add_Value(
		NULL, "ZSTEP"	, _TL("Equidistance"),
		_TL(""),
		PARAMETER_TYPE_Double, 10.0, 0, true
	);
}

//---------------------------------------------------------
CGrid_To_Contour::~CGrid_To_Contour(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Contour::On_Execute(void)
{
	double				zMin, zMax, zStep;

	pGrid	= Parameters("INPUT"  )->asGrid();
	pLayer	= Parameters("CONTOUR")->asShapes();

	zMin	= Parameters("ZMIN"   )->asDouble()	/ pGrid->Get_ZFactor();
	zMax	= Parameters("ZMAX"   )->asDouble()	/ pGrid->Get_ZFactor();
	zStep	= Parameters("ZSTEP"  )->asDouble()	/ pGrid->Get_ZFactor();

	if( zMin <= zMax && zStep > 0 )
	{
		if( zMin < pGrid->Get_ZMin() )
		{
			zMin	+= zStep * (int)((pGrid->Get_ZMin() - zMin) / zStep);
		}

		if( zMax > pGrid->Get_ZMax() )
		{
			zMax	= pGrid->Get_ZMax();
		}

		pLayer->Create(SHAPE_TYPE_Line, pGrid->Get_Name());

		pLayer->Get_Table().Add_Field("ID", TABLE_FIELDTYPE_Int);
		pLayer->Get_Table().Add_Field(pGrid->Get_Name(), TABLE_FIELDTYPE_Double);

		Contour_Create(zMin, zMax, zStep);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_To_Contour::Contour_Create(double zMin, double zMax, double zStep)
{
	int		x, y, i, ID;

	double	zValue;

	//-----------------------------------------------------
	// Initialize...
	col		= (char **)SG_Calloc(Get_NY(), sizeof(char *));
	row		= (char **)SG_Calloc(Get_NY(), sizeof(char *));

	for(y=0; y<Get_NY(); y++)
	{
		col[y]	= (char *)SG_Calloc(Get_NX(), sizeof(char));
		row[y]	= (char *)SG_Calloc(Get_NX(), sizeof(char));
	}

	if( zStep <= 0 )
		zStep	= 1;

	//-----------------------------------------------------
	for(zValue=zMin, ID=0; zValue<=zMax && Set_Progress(zValue - zMin, zMax - zMin); zValue+=zStep)
	{
		//-------------------------------------------------
		// Step1: Find Border Cells
		for(y=0; y<Get_NY()-1; y++)
			for(x=0; x<Get_NX()-1; x++)
				if( pGrid->asDouble(x,y) >= zValue )
				{
					row[y][x]	= pGrid->asDouble(x+1,y  ) <  zValue ? 1 : 0;
					col[y][x]	= pGrid->asDouble(x  ,y+1) <  zValue ? 1 : 0;
				}
				else
				{
					row[y][x]	= pGrid->asDouble(x+1,y  ) >= zValue ? 1 : 0;
					col[y][x]	= pGrid->asDouble(x  ,y+1) >= zValue ? 1 : 0;
				}

		//-------------------------------------------------
		// Step2: Interpolation + Delineation
		for(y=0; y<Get_NY()-1; y++)
			for(x=0; x<Get_NX()-1; x++)
			{
				if(row[y][x])
				{
					for(i=0; i<2; i++)
						Contour_Find(x, y, zValue, true, ID++);
					row[y][x]	= 0;
				}

				if(col[y][x])
				{
					for(i=0; i<2; i++)
						Contour_Find(x, y, zValue, false, ID++);
					col[y][x]	= 0;
				}
			}
	}

	//-----------------------------------------------------
	// Finalize...
	for(y=0; y<Get_NY(); y++)
	{
		SG_Free(col[y]);
		SG_Free(row[y]);
	}

	SG_Free(col);
	SG_Free(row);
}

//---------------------------------------------------------
void CGrid_To_Contour::Contour_Find(int x, int y, double z, bool doRow, int ID)
{
	bool	doContinue	= true;

	int		zx			= doRow ? x + 1 : x,
			zy			= doRow ? y : y + 1,
			Dir			= 0;

	DWORD	n			= 0;

	double	d, xPos, yPos,
			xMin		= pGrid->Get_XMin(),
			yMin		= pGrid->Get_YMin();

	CSG_Shape	*pShape	= pLayer->Add_Shape();

	pShape->Get_Record()->Set_Value(0, ID);
	pShape->Get_Record()->Set_Value(1, z);

	do
	{
		//-------------------------------------------------
		// Interpolation...
		d		= pGrid->asDouble(x,y);
		d		= (d - z) / (d - pGrid->asDouble(zx,zy));

		xPos	= xMin + Get_Cellsize() * (x + d * (zx - x));
		yPos	= yMin + Get_Cellsize() * (y + d * (zy - y));
		pShape->Add_Point(xPos, yPos);

		//-------------------------------------------------
		// Naechstes (x/y) (col/row) finden...
		if( !Contour_FindNext(Dir, x ,y ,doRow) )
			doContinue	= Contour_FindNext(Dir, x, y, doRow);

		Dir		= (Dir + 5) % 8;

		//-------------------------------------------------
		// Loeschen und initialisieren...
		if(doRow)
		{
			row[y][x]	= 0;
			zx			= x + 1;
			zy			= y;
		}
		else
		{
			col[y][x]	= 0;
			zx			= x;
			zy			= y + 1;
		}
	}
	while(doContinue);
}

//---------------------------------------------------------
inline bool CGrid_To_Contour::Contour_FindNext(int &Dir, int &x, int &y, bool &doRow)
{
	bool	doContinue;

	if(doRow)
	{
		switch(Dir)
		{
		case 0:	// Norden
			if(row[y+1][x  ])
			{			y++;					Dir=0; doContinue=true; break; }
		case 1:	// Nord-Ost
			if(col[y  ][x+1])
			{	x++;			doRow=false;	Dir=1; doContinue=true; break; }
		case 2:	// Osten ist nicht...
		case 3:	// Sued-Ost
			if(y-1>=0) if(col[y-1][x+1])
			{	x++;	y--;	doRow=false;	Dir=3; doContinue=true; break; }
		case 4:	// Sueden
			if(y-1>=0) if(row[y-1][x  ])
			{			y--;					Dir=4; doContinue=true; break; }
		case 5:	// Sued-West
			if(y-1>=0) if(col[y-1][x  ])
			{			y--;	doRow=false;	Dir=5; doContinue=true; break; }
		case 6:	// Westen ist nicht...
		case 7:	// Nord-West
			if(col[y  ][x  ])
			{					doRow=false;	Dir=7; doContinue=true; break; }
		default:
			Dir=0; doContinue=false;
		}
	}
	else
	{
		switch(Dir)
		{
		case 0:	// Norden ist nicht...
		case 1:	// Nord-Ost
			if(row[y+1][x  ])
			{			y++;	doRow=true;		Dir=1; doContinue=true; break; }
		case 2:	// Osten
			if(col[y  ][x+1])
			{	x++;							Dir=2; doContinue=true; break; }
		case 3:	// Sued-Ost
			if(row[y  ][x  ])
			{					doRow=true;		Dir=3; doContinue=true; break; }
		case 4:	// Sueden ist nicht...
		case 5:	// Sued-West
			if(x-1>=0)	if(row[y  ][x-1])
			{	x--;			doRow=true;		Dir=5; doContinue=true; break; }
		case 6:	// Westen
			if(x-1>=0)	if(col[y  ][x-1])
			{	x--;							Dir=6; doContinue=true; break; }
		case 7:	// Nord-West
			if(x-1>=0)	if(row[y+1][x-1])
			{	x--;	y++;	doRow=true;		Dir=7; doContinue=true; break; }
		default:
			Dir=0; doContinue=false;
		}
	}

	return(doContinue);
}
