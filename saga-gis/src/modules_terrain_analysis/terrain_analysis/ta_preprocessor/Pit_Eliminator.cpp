
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Pit_Eliminator.cpp                   //
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
#include "Pit_Eliminator.h"
#include "Pit_Router.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPit_Eliminator::CPit_Eliminator(void)
{
	Set_Name		(_TL("Sink Removal"));

	Set_Author		(SG_T("O. Conrad (c) 2001"));

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(
		NULL, "DEM"			, _TL("DEM"),
		_TL("Digital Elevation Model that has to be processed"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "SINKROUTE"	, _TL("Sink Route"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "DEM_PREPROC", _TL("Preprocessed DEM"),
		_TL("Preprocessed DEM. If this is not set changes will be stored in the original DEM grid."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Deepen Drainage Routes"),
			_TL("Fill Sinks")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD"	, _TL("Threshold"),
		_TL(""),
		PARAMETER_TYPE_Bool
	);

	Parameters.Add_Value(
		NULL	, "THRSHEIGHT"	, _TL("Threshold Height"),
		_TL(""),
		PARAMETER_TYPE_Double	, 100
	);
}

//---------------------------------------------------------
CPit_Eliminator::~CPit_Eliminator(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPit_Eliminator::On_Execute(void)
{
	bool		bResult, bKillRoute;
	int			Method, nPits;
	CPit_Router	Router;

	//-----------------------------------------------------
	bResult	= true;

	pRoute	= Parameters("SINKROUTE")	->asGrid();
	Method	= Parameters("METHOD")		->asInt();
	pDTM	= Parameters("DEM_PREPROC")	->asGrid();

	if( pDTM == NULL )
	{
		pDTM	= Parameters("DEM")->asGrid();
	}
	else if( pDTM != Parameters("DEM")->asGrid() )
	{
		pDTM->Assign(Parameters("DEM")->asGrid());

		pDTM->Set_Name(CSG_String::Format(SG_T("%s [%s]"), Parameters("DEM")->asGrid()->Get_Name(), _TL("no sinks")));
	}

	//-----------------------------------------------------
	bKillRoute	= pRoute == NULL;

	if( bKillRoute )
	{
		pRoute	= SG_Create_Grid(pDTM);
		nPits	= Router.Get_Routes(pDTM, pRoute, Parameters("THRESHOLD")->asBool() ? Parameters("THRSHEIGHT")->asDouble() : -1.0);
	}
	else
	{
		nPits	= 1;
	}

	//-----------------------------------------------------
	if( nPits > 0 )
	{
		Process_Set_Text(_TL("Initializing direction matrix..."));

		Create_goRoute();

		//-------------------------------------------------
		switch( Method )
		{
		case 0:
			Process_Set_Text(_TL("I'm diggin'..."));
			bResult	= Dig_Channels();
			break;

		case 1:
			Process_Set_Text(_TL("I'm fillin'..."));
			bResult	= Fill_Sinks();
			break;

		default:
			bResult	= false;
			break;
		}

		delete( goRoute );
	}

	//-----------------------------------------------------
	if( bKillRoute )
	{
		delete(pRoute);
	}

	Lock_Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPit_Eliminator::Create_goRoute(void)
{
	int		x, y;

	goRoute	= SG_Create_Grid(pRoute);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !is_InGrid(x,y) )
			{
				goRoute->Set_NoData(x, y);
			}
			else if( pRoute->asChar(x, y) > 0 )
			{
				goRoute->Set_Value(x, y, pRoute->asChar(x, y) % 8 );
			}
			else
			{
				goRoute->Set_Value(x, y, pDTM->Get_Gradient_NeighborDir(x, y));
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPit_Eliminator::Dig_Channels(void)
{
	bool	bPit;
	int		x, y, i, ix, iy;
	double	z;

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			z		= pDTM->asDouble(x, y);

			for(i=0, bPit=true; i<8 && bPit; i++)
			{
				ix		= Get_xTo(i, x);
				iy		= Get_yTo(i, y);

				if( !is_InGrid(ix, iy) || z > pDTM->asDouble(ix, iy) )
				{
					bPit	= false;
				}
			}

			if( bPit )
			{
				Dig_Channel(x, y);
			}
		}
	}

	return( is_Progress() );
}


//---------------------------------------------------------
void CPit_Eliminator::Dig_Channel(int x, int y)
{
	bool	bContinue;
	int		goDir;
	double	z;

	z			= pDTM->asDouble(x, y);
	bContinue	= true;

	do
	{
		z		-= M_ALMOST_ZERO;
		goDir	= goRoute->asChar(x, y);

		if( goDir < 0 )
		{
			bContinue	= false;
		}
		else
		{
			x	= Get_xTo(goDir, x);
			y	= Get_yTo(goDir, y);

			if( !is_InGrid(x, y) || z > pDTM->asDouble(x, y) )
			{
				bContinue	= false;
			}
			else
			{
				pDTM->Set_Value(x, y, z);
			}
		}
	}
	while( bContinue );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPit_Eliminator::Fill_Sinks(void)
{
	int		x, y;

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			Fill_Check(x, y);
		}
	}

	return( is_Progress() );
}

//---------------------------------------------------------
void CPit_Eliminator::Fill_Check(int x, int y)
{
	bool	bOutlet;
	int		i, ix, iy, j;
	double	z;

	z	= pDTM		->asDouble	(x, y);
	i	= goRoute	->asChar	(x, y);

	ix	= Get_xTo(i, x);
	iy	= Get_yTo(i, y);

	if( !is_InGrid(ix, iy) || z > pDTM->asDouble(ix, iy) )
	{
		for(i=0, j=4, bOutlet=false; i<8 && !bOutlet; i++, j=(j+1)%8)
		{
			ix	= Get_xTo(i, x);
			iy	= Get_yTo(i, y);

			if( is_InGrid(ix, iy) && goRoute->asChar(ix, iy) == j && z > pDTM->asDouble(ix, iy) )
			{
				bOutlet	= true;
			}
		}

		if( bOutlet )
		{
			Lock_Create();
			Lock_Set(x, y);

			for(i=0, j=4; i<8; i++, j=(j+1)%8)
			{
				ix	= Get_xTo(i, x);
				iy	= Get_yTo(i, y);

				Fill_Sink(ix, iy, j, z);
			}
		}
	}
}

//---------------------------------------------------------
void CPit_Eliminator::Fill_Sink(int x, int y, int j, double z)
{
	int		i, ix, iy;

	if( is_InGrid(x, y) && !is_Locked(x, y) && goRoute->asChar(x, y) == j )
	{
		Lock_Set(x, y);

		z	+= M_ALMOST_ZERO * Get_UnitLength(j);

		if( pDTM->asDouble(x, y) < z )
		{
			pDTM->Set_Value(x, y, z);

			for(i=0, j=4; i<8; i++, j=(j+1)%8)
			{
				ix	= Get_xTo(i, x);
				iy	= Get_yTo(i, y);

				Fill_Sink(ix, iy, j, z);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
