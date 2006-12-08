
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        XYZ.cpp                        //
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
#include "xyz.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CXYZ_Export::CXYZ_Export(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Export Grid to XYZ"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description	(_TW(
		"Export grid to a table (text format), that contains for each grid cell "
		"the x/y-coordinates and additionally data from selected grids.\n")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File Name"),
		_TL("")
	);

	Parameters.Add_Value(
		NULL	, "CAPTION"		, _TL("Write Field Names"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}

//---------------------------------------------------------
CXYZ_Export::~CXYZ_Export(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CXYZ_Export::On_Execute(void)
{
	int						x, y, i;
	double					xMin, yMin;
	FILE					*aus;
	CSG_String				FileName;
	CSG_Parameter_Grid_List	*pGrids;

	pGrids		= Parameters("GRIDS")	->asGridList();
	FileName	= Parameters("FILENAME")->asString();

	if( pGrids->Get_Count() > 0 && (aus = fopen(FileName.b_str(), "w")) != NULL )
	{
		xMin	= pGrids->asGrid(0)->Get_XMin();
		yMin	= pGrids->asGrid(0)->Get_YMin();

		if( Parameters("CAPTION")->asBool() )
		{
			fprintf(aus, "\"X\"\t\"Y\"");

			for(i=0; i<pGrids->Get_Count(); i++)
			{
				fprintf(aus, "\t\"%s\"", pGrids->asGrid(i)->Get_Name());
			}

			fprintf(aus, "\n");
		}

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				fprintf(aus, "%f\t%f", xMin + x * Get_Cellsize(),  yMin + y * Get_Cellsize());

				for(i=0; i<pGrids->Get_Count(); i++)
				{
					fprintf(aus, "\t%f", pGrids->asGrid(i)->asDouble(x, y));
				}

				fprintf(aus, "\n");
			}
		}

		fclose(aus);

		return( true );
	}

	return( false );
}
