/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//    System for an Automated Geo-Scientific Analysis    //
//                                                       //
//                    Module Library:                    //
//                    grid_analysis                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Soil_Texture.cpp                   //
//                                                       //
//                 Copyright (C) 2007 by                 //
//                    Gianluca Massei                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for an Automated  //
// Geo-Scientific Analysis'. SAGA is free software; you  //
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
//    e-mail:     g_massa@libero.it				     	 //
//                                                       //
//    contact:    Gianluca Massei                        //
//                Department of Economics and Appraisal  //
//                University of Perugia - Italy			 //
//                www.unipg.it                           //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Soil_Texture.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
struct SClass
{
	int			ID, Color;

	CSG_String	Name;

	int			nPoints;

	double		Sand[8], Clay[8];
};

//---------------------------------------------------------
const struct SClass	Classes[12]	=
{	
	{
		 1, SG_GET_RGB(000, 000, 255), _TL("Clay"),
		 6,	{   0,   0,  20,  45,  45,   0	},
			{ 100,  60,  40,  40,  55, 100	}
	},	{
		 2, SG_GET_RGB(000, 200, 255), _TL("Silty Clay"),
		 4,	{   0,   0,  20,   0	},
			{ 100,  60,  40,  40	}
	},	{
		 3, SG_GET_RGB(000, 200, 200), _TL("Silty Clay-Loam"),
		 5,	{   0,   0,  20,  20,   0	},
			{  40,  27,  27,  40,  40	}
	},	{
		 4, SG_GET_RGB(200, 000, 255), _TL("Sandy Clay"),
		 4,	{  45,  45,  65,  45	},
			{  55,  35,  35,  55	}
	},	{
		 5, SG_GET_RGB(200, 200, 200), _TL("Sandy Clay-Loam"),
		 6,	{  45,  45,  52,  80,  65,  45	},
			{  35,  27,  20,  20,  35,  35	}
	},	{
		 6, SG_GET_RGB(127, 127, 200), _TL("Clay-Loam"),
		 5,	{  20,  20,  45,  45,  20	},
			{  40,  27,  27,  40,  40	}
	},	{
		 7, SG_GET_RGB(000, 255, 000), _TL("Silt"),
		 5,	{   0,   0,  20,   8,   0	},
			{  12,   0,   0,  12,  12	}
	},	{
		 8, SG_GET_RGB(127, 255, 127), _TL("Silt-Loam"),
		 7,	{   8,  20,  50,  23,   0,   0,   8	},
			{  12,   0,   0,  27,  27,  12,  12	}
	},	{
		 9, SG_GET_RGB(127, 127, 127), _TL("Loam"),
		 6,	{  23,  43,  52,  52,  45,  23	},
			{  27,   7,   7,  20,  27,  27	}
	},	{
		10, SG_GET_RGB(255, 000, 000), _TL("Sand"),
		 4,	{  85, 100,  90,  85	},
			{   0,   0,  10,   0	}
	},	{
		11, SG_GET_RGB(255, 000, 127), _TL("Loamy Sand"),
		 5,	{  70,  85,  90,  85,  70	},
			{   0,   0,  10,  15,   0	}
	},	{
		12, SG_GET_RGB(200, 127, 127), _TL("Sandy Loam"),
		 8,	{  43,  50,  70,  85,  80,  52,  52,  43	},
			{   7,   0,   0,  15,  20,  20,   7,   7	}
	}
};


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSoil_Texture::CSoil_Texture(void)
{
	Set_Name		(_TL("Soil Texture Classification"));

	Set_Author		(_TL("Gianluca Massei (c) 2007 (g_massa@libero.it)"));

	Set_Description	(_TW(
		"Derive soil texture classes with USDA scheme from sand, silt and clay contents.\n\n"
		"  1 - Clay\n"
		"  2 - Silty Clay\n"
		"  3 - Silty Clay-Loam\n"
		"  4 - Sandy Clay\n"
		"  5 - Sandy Clay-Loam\n"
		"  6 - Clay-Loam\n"
		"  7 - Silt\n"
		"  8 - Silt-Loam\n"
		"  9 - Loam\n"
		" 10 - Sand\n"
		" 11 - Loamy Sand\n"
		" 12 - Sandy Loam\n"
		"\nReference:\n"
		"<a target=\"_blank\" href=\"http://soils.usda.gov/technical/aids/investigations/texture/\">USDA NRCS Soils Website</a>\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "SAND"	, _TL("Sand"),
		_TL("sand content given as percentage"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "SILT"	, _TL("Silt"),
		_TL("silt content given as percentage"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "CLAY"	, _TL("Clay"),
		_TL("clay content given as percentage"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "TEXTURE"	, _TL("Soil Texture"),
		_TL("soil texture"),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Grid(
		NULL, "SUM"		, _TL("Sum"),
		_TL("Sum of percentages"),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Texture::On_Execute(void)
{
	CSG_Grid	*pSand, *pSilt, *pClay, *pTexture, *pSum;

	//-----------------------------------------------------
	pSand		= Parameters("SAND")	->asGrid();
	pSilt		= Parameters("SILT")	->asGrid();
	pClay		= Parameters("CLAY")	->asGrid();
	pTexture	= Parameters("TEXTURE")	->asGrid();
	pSum		= Parameters("SUM")		->asGrid();

	//-----------------------------------------------------
	if( (pSand ? 1 : 0) + (pSilt ? 1 : 0) + (pClay ? 1 : 0) < 2 )
	{
		Error_Set(_TL("at least two contents (sand, silt, clay) have to be given"));

		return( false );
	}

	//-----------------------------------------------------
	pTexture->Set_NoData_Value(0.0);

	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pTexture, P) && P("COLORS_TYPE") && P("LUT") )
	{
		CSG_Table	*pLUT	= P("LUT")->asTable();

		for(int iClass=0; iClass<12; iClass++)
		{
			CSG_Table_Record	*pClass;

			if( (pClass = pLUT->Get_Record(iClass)) == NULL )
			{
				pClass	= pLUT->Add_Record();
			}

			pClass->Set_Value(0, Classes[iClass].Color);
			pClass->Set_Value(1, Classes[iClass].Name);
			pClass->Set_Value(2, Classes[iClass].Name);
			pClass->Set_Value(3, Classes[iClass].ID);
			pClass->Set_Value(4, Classes[iClass].ID);
		}

		while( pLUT->Get_Record_Count() > 12 )
		{
			pLUT->Del_Record(pLUT->Get_Record_Count() - 1);
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pTexture, P);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int		Texture	= 0;
			double	Sum		= 0.0;

			if(	!(pSand && pSand->is_NoData(x, y))
			&&	!(pSilt && pSilt->is_NoData(x, y))
			&&	!(pClay && pClay->is_NoData(x, y)) )
			{
				double	Sand	= pSand ? pSand->asDouble(x, y) : 100.0 - (pSilt->asDouble(x, y) + pClay->asDouble(x, y));
				double	Silt	= pSilt ? pSilt->asDouble(x, y) : 100.0 - (pSand->asDouble(x, y) + pClay->asDouble(x, y));
				double	Clay	= pClay ? pClay->asDouble(x, y) : 100.0 - (pSand->asDouble(x, y) + pSilt->asDouble(x, y));

				if( (Sum = Sand + Silt + Clay) > 0.0 )
				{
					if( Sum != 100.0 )
					{
						Sand	*= 100.0 / Sum;
						Clay	*= 100.0 / Sum;
					}

					Texture	= Get_Texture(Sand, Clay);
				}
			}

			if( Texture )
			{
				pTexture->Set_Value(x, y, Texture);

				if( pSum )
				{
					pSum->Set_Value(x, y, Sum);
				}
			}
			else
			{
				pTexture->Set_NoData(x, y);

				if( pSum )
				{
					pSum->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSoil_Texture::Get_Texture(double Sand, double Clay)
{
	for(int iClass=0; iClass<12; iClass++)
	{
		const struct SClass	*c	= Classes + iClass;

		bool	bClass	= false;

		for(int i=0, j=c->nPoints-1; i<c->nPoints; j=i++)
		{
			if( ((c->Clay[i] <= Clay && Clay < c->Clay[j]) || (c->Clay[j] <= Clay && Clay < c->Clay[i]))
			&&	Sand < (c->Sand[j] - c->Sand[i]) * (Clay - c->Clay[i]) / (c->Clay[j] - c->Clay[i]) + c->Sand[i] )
			{
				bClass	= !bClass;
			}
		}

		if( bClass )
		{
			return( c->ID );
		}
	}

	//-----------------------------------------------------
	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
