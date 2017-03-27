/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//    System for an Automated Geo-Scientific Analysis    //
//                                                       //
//                     Tool Library                      //
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
const CSG_String	Classes_USDA[][4]	=
{ {	"000, 000, 255", "C"   , _TL("Clay"           ), "  0 100,   0   60,  20  40,  45  40,  45  55,   0 100"
},{	"000, 200, 255", "SiC" , _TL("Silty Clay"     ), "  0 100,   0   60,  20  40,   0  40,   0 100"
},{	"000, 200, 200", "SiCL", _TL("Silty Clay Loam"), "  0  40,   0   27,  20  27,  20  40,   0  40"
},{	"200, 000, 255", "SC"  , _TL("Sandy Clay"     ), " 45  55,  45   35,  65  35,  45  55"
},{	"200, 200, 200", "SCL" , _TL("Sandy Clay Loam"), " 45  35,  45   27,  52  20,  80  20,  65  35,  45  35"
},{	"127, 127, 200", "CL"  , _TL("Clay Loam"      ), " 20  40,  20   27,  45  27,  45  40,  20  40"
},{	"000, 255, 000", "Si"  , _TL("Silt"           ), "  0  12,   0    0,  20   0,   8  12,   0  12"
},{	"127, 255, 127", "SiL" , _TL("Silt Loam"      ), "  8  12,  20    0,  50   0,  23  27,   0  27,   0  12,   8  12"
},{	"127, 127, 127", "L"   , _TL("Loam"           ), " 23  27,  43    7,  52   7,  52  20,  45  27,  23  27"
},{	"255, 000, 000", "S"   , _TL("Sand"           ), " 85   0, 100    0,  90  10,  85   0"
},{	"255, 000, 127", "LS"  , _TL("Loamy Sand"     ), " 70   0,  85    0,  90  10,  85  15,  70   0"
},{	"200, 127, 127", "SL"  , _TL("Sandy Loam"     ), " 43   7,  50    0,  70   0,  85  15,  80  20,  52  20,  52   7,  43   7"
},{	"", "", "", ""	}	};

//---------------------------------------------------------
const CSG_String	Classes_KA5[][4]	=
{ {	"", "Ss" , _TL("Reinsand"                ), "100  0, 95   5, 85  5, 90  0, 100  0"
},{	"", "Sl2", _TL("schwach lehmiger Sand"   ), " 85  5, 82   8, 67  8, 70  5,  85  5"
},{	"", "Sl3", _TL("mittel lehmiger Sand"    ), " 82  8, 78  12, 48 12, 52  8,  82  8"
},{	"", "Sl4", _TL("stark lehmiger Sand"     ), " 78 12, 73  17, 43 17, 48 12,  78 12"
},{	"", "Slu", _TL("schluffig-lemiger Sand"  ), " 52  8, 43  17, 33 17, 42  8,  52  8"
},{	"", "St2", _TL("schwach toniger Sand"    ), " 95  5, 83  17, 73 17, 85  5,  95  5"
},{	"", "St3", _TL("mittel toniger Sand"     ), " 83 17, 75  25, 60 25, 68 17,  83 17"
},{	"", "Su2", _TL("schwach schluffiger Sand"), " 90  0, 85   5, 70  5, 75  0,  90  0"
},{	"", "Su3", _TL("mittel schluffiger Sand" ), " 75  0, 67   8, 52  8, 60  0,  75  0"
},{	"", "Su4", _TL("stark schluffiger Sand"  ), " 60  0, 52   8, 42  8, 50  0,  60  0"
},{	"", "Ls2", _TL("schwach sandiger Lehm"   ), " 43 17, 35  25, 25 25, 33 17,  43 17"
},{	"", "Ls3", _TL("mittel sandiger Lehm"    ), " 53 17, 45  25, 35 25, 43 17,  53 17"
},{	"", "Ls4", _TL("stark sandiger Lehm"     ), " 68 17, 60  25, 45 25, 53 17,  68 17"
},{	"", "Lt2", _TL("schwach toniger Lehm"    ), " 45 25, 35  35, 15 35, 25 25,  45 25"
},{	"", "Lt3", _TL("mittel toniger Lehm"     ), " 35 35, 25  45,  5 45, 15 35,  35 35"
},{	"", "Lts", _TL("sandig-toniger Lehm"     ), " 60 25, 40  45, 25 45, 45 25,  60 25"
},{	"", "Lu" , _TL("schluffiger Lehm"        ), " 33 17, 20  30,  5 30, 18 17,  33 17"
},{	"", "Uu" , _TL("reiner schluff"          ), " 20  0, 12   8,  0  8,  0  0,  20  0"
},{	"", "Uls" , _TL("sandig-lehmiger Schluff"), " 42  8, 33  17, 18 17, 27  8,  42  8"
},{	"", "Us" , _TL("sandiger Schluff"        ), " 50  0, 42   8, 12  8, 20  0,  50  0"
},{	"", "Ut2", _TL("schwach toniger Schluff" ), " 27  8, 23  12,  0 12,  0  8,  27  8"
},{	"", "Ut3", _TL("mittel toniger Schluff"  ), " 23 12, 18  17,  0 17,  0 12,  23 12"
},{	"", "Ut4", _TL("stark toniger Schluff"   ), " 18 17, 10  25,  0 25,  0 17,  18 17"
},{	"", "Tt" , _TL("reiner Ton"              ), " 35 65,  0 100,  0 65, 35 65"
},{	"", "Tl" , _TL("lehmiger Ton"            ), " 40 45, 20  65,  5 65, 25 45,  40 45"
},{	"", "Tu2", _TL("schwach schluffiger Ton" ), " 25 45,  5  65,  0 65,  0 45,  25 45"
},{	"", "Tu3", _TL("mittel schluffiger Ton"  ), " 20 30,  5  45,  0 45,  0 35,   5 30, 20 30"
},{	"", "Tu4", _TL("stark schluffiger Ton"   ), " 10 25,  0  35,  0 25, 10 25"
},{	"", "Ts2" , _TL("schwach sandiger Ton"   ), " 55 45, 35  65, 20 65, 40 45,  55 45"
},{	"", "Ts3" , _TL("mittel sandiger Ton"    ), " 65 35, 55  45, 40 45, 50 35,  65 35"
},{	"", "Ts4"  , _TL("stark sandiger Ton"    ), " 75 25, 65  35, 50 35, 60 25,  75 25"
},{	"", "", "", ""	}	};

//---------------------------------------------------------
CSG_String	Classes_Belgium[][4]	=
{ {	"", "L", _TL("L"), " 50.000   0.000, 15.000  0.000, 15.000 22.500, 20.000  20.000,  25.000 18.125, 30.000 17.500, 67.500 17.500, 67.500 11.250, 50.000 11.250, 50.000  0.000"
},{	"", "P", _TL("P"), " 67.500   0.000, 50.000  0.000, 50.000 11.250, 67.500  11.250,  67.500  0.000"
},{	"", "S", _TL("S"), " 82.500   0.000, 67.500  0.000, 67.500 11.250, 67.500  17.500,  82.500 17.500, 91.250  8.750, 82.500  8.750, 82.500  0.000"
},{	"", "U", _TL("U"), " 65.000  35.000, 10.000 35.000,  0.000 45.000,  0.000 100.000,  65.000 35.000"
},{	"", "Z", _TL("Z"), "100.000   0.000, 82.500  0.000, 82.500  8.750, 91.250   8.750, 100.000  0.000"
},{	"", "A", _TL("A"), " 15.000   0.000,  0.000  0.000,  0.000 30.000, 15.000  22.500,  15.000  0.000"
},{	"", "E", _TL("E"), " 82.500  17.500, 67.500 17.500, 30.000 17.500, 25.000  18.125,  20.000 20.000, 15.000 22.500,  0.000 30.000,  0.000 45.000, 10.000 35.000, 65.000 35.000, 82.500 17.500"
},{	"", "", "", ""	}	};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSoil_Texture_Classifier
{
public:
	CSoil_Texture_Classifier(void)	{}

	CSoil_Texture_Classifier(int Scheme)	{	Initialize(Scheme);	}

	//-----------------------------------------------------
	static CSG_String		Get_Description	(void)
	{
		return( _TW(
			"Derive soil texture classes from sand, silt and clay contents. "
			"Currently supported schemes are USDA and German Kartieranleitung 5. "
		));
	}

	//-----------------------------------------------------
	int						Get_Count		(void)	const	{	return( m_Classes.Get_Count() );	}

	int						Get_ID			(int i)	const	{	return( i >= 0 && i < Get_Count() ? m_Classes[i].asInt   (0) :        0 );	}
	int						Get_Color		(int i)	const	{	return( i >= 0 && i < Get_Count() ? m_Classes[i].asInt   (1) :        0 );	}
	CSG_String				Get_Key			(int i)	const	{	return( i >= 0 && i < Get_Count() ? m_Classes[i].asString(2) : SG_T("") );	}
	CSG_String				Get_Name		(int i)	const	{	return( i >= 0 && i < Get_Count() ? m_Classes[i].asString(3) : SG_T("") );	}

	bool					Get_Polygons	(CSG_Shapes *pPolygons, int Axes)	const
	{
		if( !pPolygons || !m_Classes.is_Valid() )
		{
			return( false );
		}

		pPolygons->Create(m_Classes);

		for(int i=0; Axes!=0 && i<pPolygons->Get_Count(); i++)
		{
			CSG_Shape	*pPolygon	= pPolygons->Get_Shape(i);

			for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(0); iPoint++)
			{
				TSG_Point	p	= pPolygon->Get_Point(iPoint);

				switch( Axes )
				{
				case 0: pPolygon->Set_Point(p.x,              p.y , iPoint); break;	// x=sand, y=clay
				case 1: pPolygon->Set_Point(p.x, 100 - (p.x + p.y), iPoint); break;	// x=sand, y=silt
				case 2: pPolygon->Set_Point(p.y, 100 - (p.x + p.y), iPoint); break;	// x=clay, y=silt
				}
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	static bool	Get_Table	(CSG_Table &Classes, int Definition)
	{
		Classes.Destroy();

		Classes.Add_Field("COLOR"  , SG_DATATYPE_String);
		Classes.Add_Field("KEY"    , SG_DATATYPE_String);
		Classes.Add_Field("NAME"   , SG_DATATYPE_String);
		Classes.Add_Field("POLYGON", SG_DATATYPE_String);

		for(int i=0; ; i++)
		{
			const CSG_String	*Class;

			switch( Definition )
			{
			default: Class = Classes_USDA   [i]; break;
			case  1: Class = Classes_KA5    [i]; break;
			case  2: Class = Classes_Belgium[i]; break;
			}

			if( Class[1].is_Empty() )	{	break;	}	else
			{
				CSG_Table_Record	*pClass	= Classes.Add_Record();

				pClass->Set_Value(0, Class[0]);
				pClass->Set_Value(1, Class[1]);
				pClass->Set_Value(2, Class[2]);
				pClass->Set_Value(3, Class[3]);
			}
		}

		return( Classes.Get_Count() > 0 );
	}

	//-----------------------------------------------------
	bool	Initialize	(int Definition)
	{
		CSG_Table	Classes;	return( Get_Table(Classes, Definition) && Initialize(Classes) );
	}

	//-----------------------------------------------------
	bool	Initialize	(const CSG_Table &Classes)
	{
		m_Classes.Create(SHAPE_TYPE_Polygon);

		m_Classes.Add_Field("ID"   , SG_DATATYPE_Int   );
		m_Classes.Add_Field("COLOR", SG_DATATYPE_Color );
		m_Classes.Add_Field("KEY"  , SG_DATATYPE_String);
		m_Classes.Add_Field("NAME" , SG_DATATYPE_String);

		//-------------------------------------------------
		for(int i=0; i<Classes.Get_Count(); i++)
		{
			CSG_String_Tokenizer	Items(Classes[i].asString(3), ",");

			if( Items.Get_Tokens_Count() >= 3 )
			{
				CSG_Shape_Polygon	*pClass	= (CSG_Shape_Polygon *)m_Classes.Add_Shape();

				pClass->Set_Value(0, i + 1);
				pClass->Set_Value(2, Classes[i].asString(1));
				pClass->Set_Value(3, Classes[i].asString(2));

				do
				{
					CSG_String	Point(Items.Get_Next_Token());	Point.Trim();

					double	Sand	= Point.BeforeFirst(' ').asDouble();
					double	Clay	= Point.AfterFirst (' ').asDouble();

					pClass->Add_Point(Sand, Clay);
				}
				while( Items.Has_More_Tokens() );

				Items.Set_String(Classes[i].asString(0), ",");

				if( Items.Get_Tokens_Count() == 3 )
				{
					int	r	= Items.Get_Next_Token().asInt();
					int	g	= Items.Get_Next_Token().asInt();
					int	b	= Items.Get_Next_Token().asInt();

					pClass->Set_Value(1, SG_GET_RGB(r, g, b));
				}
				else
				{
					TSG_Point	c	= pClass->Get_Centroid();

					int	r	= (255. / 100.) * c.x;
					int	g	= (255. / 100.) * (100 - (c.x + c.y));
					int	b	= (255. / 100.) * c.y;

					pClass->Set_Value(1, SG_GET_RGB(r, g, b));
				}
			}
		}

		return( Get_Count() > 0 );
	}

	//-----------------------------------------------------
	int						Get_Texture		(double Sand, double Silt, double Clay, double &Sum)	const
	{
		if( ((Sand < 0 ? 1 : 0) + (Silt < 0 ? 1 : 0) + (Clay < 0 ? 1 : 0)) > 1 )
		{
			return( -1 );
		}

		if( Sand < 0 ) Sand = 100 - (       Silt + Clay);
		if( Silt < 0 ) Silt = 100 - (Sand        + Clay);
		if( Clay < 0 ) Clay = 100 - (Sand + Silt       );

		Sum	= Sand + Silt + Clay;

		if( Sum > 0.0 && Sum != 100.0 )
		{
			Sand	*= 100.0 / Sum;
			Clay	*= 100.0 / Sum;
		}

		return( Get_Texture_From_Sand_and_Clay(Sand, Clay) );
	}

	//-----------------------------------------------------
	bool					Set_LUT			(CSG_Table *pClasses, bool bID)	const
	{
		pClasses->Set_Record_Count(Get_Count());

		for(int i=0; i<Get_Count(); i++)
		{
			CSG_Table_Record	*pClass	= pClasses->Get_Record(i);

			pClasses->Set_Value(i, 0, Get_Color(i));
			pClasses->Set_Value(i, 1, Get_Key  (i));
			pClasses->Set_Value(i, 2, Get_Name (i));

			if( bID )
			{
				pClasses->Set_Value(i, 3, Get_ID(i));
				pClasses->Set_Value(i, 4, Get_ID(i));
			}
			else
			{
				pClasses->Set_Value(i, 3, Get_Key(i));
				pClasses->Set_Value(i, 4, Get_Key(i));
			}
		}

		return( true );
	}


private: //////////////////////////////////////////////////

	CSG_Shapes		m_Classes;


	int				Get_Texture_From_Sand_and_Clay	(double Sand, double Clay)	const
	{
		if( Sand < 0.001 ) Sand = 0.001; else if( Sand > 99.99 ) Sand = 99.99;
		if( Clay < 0.001 ) Clay = 0.001; else if( Clay > 99.99 ) Clay = 99.99;

		if( Sand + Clay >= 99.99 )
		{
			double	Sum	= 99.99 / (Sand + Clay);

			Sand	*= Sum;
			Clay	*= Sum;
		}

		for(int i=0; i<Get_Count(); i++)
		{
			if( ((CSG_Shape_Polygon *)m_Classes.Get_Shape(i))->Contains(Sand, Clay) )
			{
				return( i );
			}
		}

		return( -1 );
	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSoil_Texture::CSoil_Texture(void)
{
	Set_Name		(_TL("Soil Texture Classification"));

	Set_Author		("Gianluca Massei (c) 2007 (g_massa@libero.it), L.Landschreiber, O.Conrad (c) 2017");

	Set_Description	(CSoil_Texture_Classifier::Get_Description());

	Add_Reference(
		"http://soils.usda.gov/technical/aids/investigations/texture/",
		SG_T("USDA NRCS Soils Website")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"SAND"		, _TL("Sand"),
		_TL("sand content given as percentage"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"SILT"		, _TL("Silt"),
		_TL("silt content given as percentage"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"CLAY"		, _TL("Clay"),
		_TL("clay content given as percentage"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"TEXTURE"	, _TL("Soil Texture"),
		_TL("soil texture"),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Grid("",
		"SUM"		, _TL("Sum"),
		_TL("Sum of percentages"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"SCHEME"	, _TL("Classification"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("USDA"),
			_TL("Germany KA5"),
			_TL("Belgium/France"),
			_TL("user defined")
		)
	);

	Parameters.Add_FixedTable("SCHEME",
		"USER"		, _TL("User Definition"),
		_TL("")
	);

	CSoil_Texture_Classifier::Get_Table(*Parameters("USER")->asTable(), 0);

	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Scheme as Polygons"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice("POLYGONS",
		"XY_AXES"		, _TL("X/Y Axes"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Sand and Clay"),
			_TL("Sand and Silt"),
			_TL("Clay and Silt")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSoil_Texture::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "POLYGONS") )
	{
		pParameters->Set_Enabled("XY_AXES", pParameter->asShapes() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SCHEME") )
	{
		pParameters->Set_Enabled("USER", pParameter->asInt() == 3);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
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
	CSoil_Texture_Classifier	Classifier(Parameters("SCHEME")->asInt());

	if( Parameters("SCHEME")->asInt() == 3 && !Classifier.Initialize(*Parameters("USER")->asTable()) )	// user defined
	{
		return( false );
	}

	//-----------------------------------------------------
	pClass->Set_NoData_Value(0.0);

	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pClass, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		Classifier.Set_LUT(pLUT->asTable(), true);

		DataObject_Set_Parameter(pClass, pLUT            );	// Lookup Table
		DataObject_Set_Parameter(pClass, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	if( Classifier.Get_Polygons(Parameters("POLYGONS")->asShapes(), Parameters("XY_AXES")->asInt()) && (pLUT = DataObject_Get_Parameter(pClass, "LUT")) != NULL && pLUT->asTable() )
	{
		Classifier.Set_LUT(pLUT->asTable(), true);

		DataObject_Set_Parameter(Parameters("POLYGONS")->asShapes(), pLUT            );	// Lookup Table
		DataObject_Set_Parameter(Parameters("POLYGONS")->asShapes(), "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
		DataObject_Set_Parameter(Parameters("POLYGONS")->asShapes(), "LUT_ATTRIB" , 0);	// Lookup Table Attribute
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
				double	Sum;

				int		Class	= Classifier.Get_Texture(
					pSand ? pSand->asDouble(x, y) : -1.0,
					pSilt ? pSilt->asDouble(x, y) : -1.0,
					pClay ? pClay->asDouble(x, y) : -1.0, Sum
				);

				SG_GRID_PTR_SAFE_SET_VALUE(pClass, x, y, Classifier.Get_ID(Class));
				SG_GRID_PTR_SAFE_SET_VALUE(pSum  , x, y, Sum);
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

	Set_Author		("Gianluca Massei (c) 2007 (g_massa@libero.it), L.Landschreiber, O.Conrad (c) 2017");

	Set_Description	(CSoil_Texture_Classifier::Get_Description());

	Add_Reference(
		"http://soils.usda.gov/technical/aids/investigations/texture/",
		SG_T("USDA NRCS Soils Website")
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"	, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"SAND"	, _TL("Sand"),
		_TL("sand content given as percentage"),
		true
	);

	Parameters.Add_Table_Field("TABLE",
		"SILT"	, _TL("Silt"),
		_TL("silt content given as percentage"),
		true
	);

	Parameters.Add_Table_Field("TABLE",
		"CLAY"	, _TL("Clay"),
		_TL("clay content given as percentage"),
		true
	);

	Parameters.Add_Table_Field("TABLE",
		"TEXTURE"	, _TL("Texture"),
		_TL("soil texture"),
		true
	);

	Parameters.Add_Choice("",
		"SCHEME"	, _TL("Classification"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("USDA"),
			_TL("Germany KA5"),
			_TL("Belgium/France"),
			_TL("user defined")
		)
	);

	Parameters.Add_FixedTable("SCHEME",
		"USER"		, _TL("User Definition"),
		_TL("")
	);

	CSoil_Texture_Classifier::Get_Table(*Parameters("USER")->asTable(), 0);

	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Scheme as Polygons"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice("POLYGONS",
		"XY_AXES"		, _TL("X/Y Axes"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Sand and Clay"),
			_TL("Sand and Silt"),
			_TL("Clay and Silt")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSoil_Texture_Table::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "POLYGONS") )
	{
		pParameters->Set_Enabled("XY_AXES", pParameter->asShapes() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SCHEME") )
	{
		pParameters->Set_Enabled("USER", pParameter->asInt() == 3);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
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
	CSoil_Texture_Classifier	Classifier(Parameters("SCHEME")->asInt());

	if( Parameters("SCHEME")->asInt() == 3 && !Classifier.Initialize(*Parameters("USER")->asTable()) )	// user defined
	{
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
			double	Sum;

			int		Class	= Classifier.Get_Texture(
				iSand >= 0 ? pRecord->asDouble(iSand) : -1.0,
				iSilt >= 0 ? pRecord->asDouble(iSilt) : -1.0,
				iClay >= 0 ? pRecord->asDouble(iClay) : -1.0, Sum
			);

			pRecord->Set_Value (iTexture, Classifier.Get_Key(Class));
		}
	}

	DataObject_Update(pTable);

	//-----------------------------------------------------
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pTable, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		Classifier.Set_LUT(pLUT->asTable(), false);

		DataObject_Set_Parameter(pTable, pLUT                   );	// Lookup Table
		DataObject_Set_Parameter(pTable, "COLORS_TYPE", 1       );	// Color Classification Type: Lookup Table
		DataObject_Set_Parameter(pTable, "LUT_ATTRIB" , iTexture);	// Lookup Table Attribute
	}

	if( Classifier.Get_Polygons(Parameters("POLYGONS")->asShapes(), Parameters("XY_AXES")->asInt()) && (pLUT = DataObject_Get_Parameter(pTable, "LUT")) != NULL && pLUT->asTable() )
	{
		Classifier.Set_LUT(pLUT->asTable(), true);

		DataObject_Set_Parameter(Parameters("POLYGONS")->asShapes(), pLUT            );	// Lookup Table
		DataObject_Set_Parameter(Parameters("POLYGONS")->asShapes(), "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
		DataObject_Set_Parameter(Parameters("POLYGONS")->asShapes(), "LUT_ATTRIB" , 0);	// Lookup Table Attribute
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
