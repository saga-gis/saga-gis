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

	CSG_String	Key, Name;

	int			nPoints;

	double		Sand[8], Clay[8];
};

//---------------------------------------------------------
const struct SClass	Classes[12]	=
{	
	{
		 1, SG_GET_RGB(000, 000, 255), "C"   , _TL("Clay"),
		 6,	{   0,   0,  20,  45,  45,   0				},
			{ 100,  60,  40,  40,  55, 100				}
	},	{
		 2, SG_GET_RGB(000, 200, 255), "SiC" , _TL("Silty Clay"),
		 4,	{   0,   0,  20,   0						},
			{ 100,  60,  40,  40						}
	},	{
		 3, SG_GET_RGB(000, 200, 200), "SiCL", _TL("Silty Clay Loam"),
		 5,	{   0,   0,  20,  20,   0					},
			{  40,  27,  27,  40,  40					}
	},	{
		 4, SG_GET_RGB(200, 000, 255), "SC"  , _TL("Sandy Clay"),
		 4,	{  45,  45,  65,  45						},
			{  55,  35,  35,  55						}
	},	{
		 5, SG_GET_RGB(200, 200, 200), "SCL" , _TL("Sandy Clay Loam"),
		 6,	{  45,  45,  52,  80,  65,  45				},
			{  35,  27,  20,  20,  35,  35				}
	},	{
		 6, SG_GET_RGB(127, 127, 200), "CL"  , _TL("Clay Loam"),
		 5,	{  20,  20,  45,  45,  20					},
			{  40,  27,  27,  40,  40					}
	},	{
		 7, SG_GET_RGB(000, 255, 000), "Si"  , _TL("Silt"),
		 5,	{   0,   0,  20,   8,   0					},
			{  12,   0,   0,  12,  12					}
	},	{
		 8, SG_GET_RGB(127, 255, 127), "SiL" , _TL("Silt Loam"),
		 7,	{   8,  20,  50,  23,   0,   0,   8			},
			{  12,   0,   0,  27,  27,  12,  12			}
	},	{
		 9, SG_GET_RGB(127, 127, 127), "L"   , _TL("Loam"),
		 6,	{  23,  43,  52,  52,  45,  23				},
			{  27,   7,   7,  20,  27,  27				}
	},	{
		10, SG_GET_RGB(255, 000, 000), "S"   , _TL("Sand"),
		 4,	{  85, 100,  90,  85						},
			{   0,   0,  10,   0						}
	},	{
		11, SG_GET_RGB(255, 000, 127), "LS"  , _TL("Loamy Sand"),
		 5,	{  70,  85,  90,  85,  70					},
			{   0,   0,  10,  15,   0					}
	},	{
		12, SG_GET_RGB(200, 127, 127), "SL"  , _TL("Sandy Loam"),
		 8,	{  43,  50,  70,  85,  80,  52,  52,  43	},
			{   7,   0,   0,  15,  20,  20,   7,   7	}
	}
};

//---------------------------------------------------------
const CSG_String	Description	= _TW(
	"Derive soil texture classes with USDA scheme from sand, silt and clay contents.\n\n"
	"  1 - Clay\n"
	"  2 - Silty Clay\n"
	"  3 - Silty Clay Loam\n"
	"  4 - Sandy Clay\n"
	"  5 - Sandy Clay Loam\n"
	"  6 - Clay Loam\n"
	"  7 - Silt\n"
	"  8 - Silt Loam\n"
	"  9 - Loam\n"
	" 10 - Sand\n"
	" 11 - Loamy Sand\n"
	" 12 - Sandy Loam\n"
	"\nReference:\n"
	"<a target=\"_blank\" href=\"http://soils.usda.gov/technical/aids/investigations/texture/\">USDA NRCS Soils Website</a>\n"
);


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int Get_Texture_SandClay(double Sand, double Clay)
{
	if( Sand < 0.001 ) Sand = 0.001; else if( Sand > 99.99 ) Sand = 99.99;
	if( Clay < 0.001 ) Clay = 0.001; else if( Clay > 99.99 ) Clay = 99.99;

	if( Sand + Clay >= 99.99 )
	{
		double	Sum	= 99.99 / (Sand + Clay);

		Sand	*= Sum;
		Clay	*= Sum;
	}

	//-----------------------------------------------------
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
			return( iClass );
		}
	}

	//-----------------------------------------------------
	return( -1 );
}

//---------------------------------------------------------
int Get_Texture_SandSilt(double Sand, double Silt)
{
	return( Get_Texture_SandClay(Sand, 100.0 - (Sand + Silt)) );
}

//---------------------------------------------------------
int Get_Texture_SiltClay(double Silt, double Clay)
{
	return( Get_Texture_SandClay(100.0 - (Silt + Clay), Clay) );
}

//---------------------------------------------------------
int Get_Texture(double Sand, double Silt, double Clay, double &Sum)
{
	if( Sand < 0.0 ) Sand = 0.0; else if( Sand > 100.0 ) Sand = 100.0;
	if( Silt < 0.0 ) Silt = 0.0; else if( Silt > 100.0 ) Silt = 100.0;
	if( Clay < 0.0 ) Clay = 0.0; else if( Clay > 100.0 ) Clay = 100.0;

	Sum	= Sand + Silt + Clay;

	if( Sum > 0.0 && Sum != 100.0 )
	{
		Sand	*= 100.0 / Sum;
		Clay	*= 100.0 / Sum;
	}

	return( Get_Texture_SandClay(Sand, Clay) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSoil_Texture::CSoil_Texture(void)
{
	Set_Name		(_TL("Soil Texture Classification"));

	Set_Author		("Gianluca Massei (c) 2007 (g_massa@libero.it)");

	Set_Description	(Description);

	//-----------------------------------------------------
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
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Grid(
		NULL, "SUM"		, _TL("Sum"),
		_TL("Sum of percentages"),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Texture::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pSand	= Parameters("SAND"   )->asGrid();
	CSG_Grid	*pSilt	= Parameters("SILT"   )->asGrid();
	CSG_Grid	*pClay	= Parameters("CLAY"   )->asGrid();
	CSG_Grid	*pClass	= Parameters("TEXTURE")->asGrid();
	CSG_Grid	*pSum	= Parameters("SUM"    )->asGrid();

	//-----------------------------------------------------
	if( (pSand ? 1 : 0) + (pSilt ? 1 : 0) + (pClay ? 1 : 0) < 2 )
	{
		Error_Set(_TL("at least two contents (sand, silt, clay) have to be given"));

		return( false );
	}

	//-----------------------------------------------------
	pClass->Set_NoData_Value(-1.0);

	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pClass, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		CSG_Table	*pClasses	= pLUT->asTable();

		for(int iClass=0; iClass<12; iClass++)
		{
			CSG_Table_Record	*pClass	= pClasses->Get_Record(iClass);

			if( pClass == NULL )
			{
				pClass	= pClasses->Add_Record();
			}

			pClass->Set_Value(0, Classes[iClass].Color);
			pClass->Set_Value(1, Classes[iClass].Name);
			pClass->Set_Value(2, Classes[iClass].Key);
			pClass->Set_Value(3, iClass);
			pClass->Set_Value(4, iClass);
		}

		while( pClasses->Get_Count() > 12 )
		{
			pClasses->Del_Record(pClasses->Get_Count() - 1);
		}

		DataObject_Set_Parameter(pClass, pLUT);	// Lookup Table
		DataObject_Set_Parameter(pClass, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if(	(pSand && pSand->is_NoData(x, y))
			||	(pSilt && pSilt->is_NoData(x, y))
			||	(pClay && pClay->is_NoData(x, y)) )
			{
				SG_GRID_PTR_SAFE_SET_NODATA(pClass, x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(pSum  , x, y);
			}
			else
			{
				int		Class	= -1;
				double	Sum		= 100.0;

				if( pSand && pSilt && pClay )
				{
					Class	= Get_Texture(pSand->asDouble(x, y), pSilt->asDouble(x, y), pClay->asDouble(x, y), Sum);
				}
				else if( !pSilt )
				{
					Class	= Get_Texture_SandClay(pSand->asDouble(x, y), pClay->asDouble(x, y));
				}
				else if( !pClay )
				{
					Class	= Get_Texture_SandSilt(pSand->asDouble(x, y), pSilt->asDouble(x, y));
				}
				else if( !pSand )
				{
					Class	= Get_Texture_SiltClay(pSilt->asDouble(x, y), pClay->asDouble(x, y));
				}

				SG_GRID_PTR_SAFE_SET_VALUE(pClass, x, y, Class);
				SG_GRID_PTR_SAFE_SET_VALUE(pSum  , x, y, Sum  );
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
CSoil_Texture_Table::CSoil_Texture_Table(void)
{
	Set_Name		(_TL("Soil Texture Classification for Tables"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(Description);

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Table(
		NULL	, "TABLE"	, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "SAND"	, _TL("Sand"),
		_TL("sand content given as percentage"),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "SILT"	, _TL("Silt"),
		_TL("silt content given as percentage"),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "CLAY"	, _TL("Clay"),
		_TL("clay content given as percentage"),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "TEXTURE"	, _TL("Texture"),
		_TL("soil texture"),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Texture_Table::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable		= Parameters("TABLE")->asTable();

	int		iSand		= Parameters("SAND"   )->asInt();
	int		iSilt		= Parameters("SILT"   )->asInt();
	int		iClay		= Parameters("CLAY"   )->asInt();
	int		iTexture	= Parameters("TEXTURE")->asInt();

	//-----------------------------------------------------
	if( (iSand >= 0 ? 1 : 0) + (iSilt >= 0 ? 1 : 0) + (iClay >= 0 ? 1 : 0) < 2 )
	{
		Error_Set(_TL("at least two contents (sand, silt, clay) have to be given"));

		return( false );
	}

	//-----------------------------------------------------
	if( iTexture < 0 )
	{
		iTexture	= pTable->Get_Field_Count();

		pTable->Add_Field("TEXTURE", SG_DATATYPE_String);
	}

	//-----------------------------------------------------
	for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(i);

		if(	(iSand >= 0 && pRecord->is_NoData(iSand))
		||	(iSilt >= 0 && pRecord->is_NoData(iSilt))
		||	(iClay >= 0 && pRecord->is_NoData(iClay)) )
		{
			pRecord->Set_NoData(iTexture);
		}
		else
		{
			int		Class	= -1;

			if( iSand >= 0 && iSilt >= 0 && iClay >= 0 )
			{
				double	Sum;

				Class	= Get_Texture(pRecord->asDouble(iSand), pRecord->asDouble(iSilt), pRecord->asDouble(iClay), Sum);
			}
			else if( iSilt < 0 )
			{
				Class	= Get_Texture_SandClay(pRecord->asDouble(iSand), pRecord->asDouble(iClay));
			}
			else if( iClay < 0 )
			{
				Class	= Get_Texture_SandSilt(pRecord->asDouble(iSand), pRecord->asDouble(iSilt));
			}
			else if( iSand < 0 )
			{
				Class	= Get_Texture_SiltClay(pRecord->asDouble(iSilt), pRecord->asDouble(iClay));
			}

			pRecord->Set_Value (iTexture, Classes[Class].Key);
		}
	}

	DataObject_Update(pTable);

	//-----------------------------------------------------
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pTable, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		CSG_Table	*pClasses	= pLUT->asTable();

		for(int iClass=0; iClass<12; iClass++)
		{
			CSG_Table_Record	*pClass	= pClasses->Get_Record(iClass);

			if( pClass == NULL )
			{
				pClass	= pClasses->Add_Record();
			}

			pClass->Set_Value(0, Classes[iClass].Color);
			pClass->Set_Value(1, Classes[iClass].Name);
			pClass->Set_Value(2, Classes[iClass].Name);
			pClass->Set_Value(3, Classes[iClass].Key);
			pClass->Set_Value(4, Classes[iClass].Key);
		}

		while( pClasses->Get_Count() > 12 )
		{
			pClasses->Del_Record(pClasses->Get_Count() - 1);
		}

		DataObject_Set_Parameter(pTable, pLUT);	// Lookup Table
		DataObject_Set_Parameter(pTable, "LUT_ATTRIB" , iTexture);	// Lookup Table Attribute
		DataObject_Set_Parameter(pTable, "COLORS_TYPE", 1       );	// Color Classification Type: Lookup Table
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
