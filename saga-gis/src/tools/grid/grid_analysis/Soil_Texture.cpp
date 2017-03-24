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
struct SClass
{
	int			Color;
	CSG_String	Key, Name;
	double		Sand[10], Clay[10];
};

//---------------------------------------------------------
const struct SClass	Classes_USDA[]	= {
{	SG_GET_RGB(000, 000, 255), "C"   , _TL("Clay"           ),
	{   0,   0,  20,  45,  45,   0          ,  -1	},
	{ 100,  60,  40,  40,  55, 100          ,  -1	}	},
{	SG_GET_RGB(000, 200, 255), "SiC" , _TL("Silty Clay"     ),
	{   0,   0,  20,   0                    ,  -1	},
	{ 100,  60,  40,  40                    ,  -1	}	},
{	SG_GET_RGB(000, 200, 200), "SiCL", _TL("Silty Clay Loam"),
	{   0,   0,  20,  20,   0               ,  -1	},
	{  40,  27,  27,  40,  40               ,  -1	}	},
{	SG_GET_RGB(200, 000, 255), "SC"  , _TL("Sandy Clay"     ),
	{  45,  45,  65,  45                    ,  -1	},
	{  55,  35,  35,  55                    ,  -1	}	},
{	SG_GET_RGB(200, 200, 200), "SCL" , _TL("Sandy Clay Loam"),
	{  45,  45,  52,  80,  65,  45          ,  -1	},
	{  35,  27,  20,  20,  35,  35          ,  -1	}	},
{	SG_GET_RGB(127, 127, 200), "CL"  , _TL("Clay Loam"      ),
	{  20,  20,  45,  45,  20               ,  -1	},
	{  40,  27,  27,  40,  40               ,  -1	}	},
{	SG_GET_RGB(000, 255, 000), "Si"  , _TL("Silt"           ),
	{   0,   0,  20,   8,   0               ,  -1	},
	{  12,   0,   0,  12,  12               ,  -1	}	},
{	SG_GET_RGB(127, 255, 127), "SiL" , _TL("Silt Loam"      ),
	{   8,  20,  50,  23,   0,   0,   8     ,  -1	},
	{  12,   0,   0,  27,  27,  12,  12     ,  -1	}	},
{	SG_GET_RGB(127, 127, 127), "L"   , _TL("Loam"           ),
	{  23,  43,  52,  52,  45,  23          ,  -1	},
	{  27,   7,   7,  20,  27,  27          ,  -1	}	},
{	SG_GET_RGB(255, 000, 000), "S"   , _TL("Sand"           ),
	{  85, 100,  90,  85                    ,  -1	},
	{   0,   0,  10,   0                    ,  -1	}	},
{	SG_GET_RGB(255, 000, 127), "LS"  , _TL("Loamy Sand"     ),
	{  70,  85,  90,  85,  70               ,  -1	},
	{   0,   0,  10,  15,   0               ,  -1	}	},
{	SG_GET_RGB(200, 127, 127), "SL"  , _TL("Sandy Loam"     ),
	{  43,  50,  70,  85,  80,  52,  52,  43,  -1	},
	{   7,   0,   0,  15,  20,  20,   7,   7,  -1	}	},
{	0, "", "", NULL, NULL	}	};

//---------------------------------------------------------
const struct SClass	Classes_KA5[]	= {	
{	SG_GET_RGB(000, 000, 255), "Ss"  , _TL("Reinsand"),
	{   0,   5,   5,   0,   0               ,  -1	},
	{   0,   0,  10,  10,   0               ,  -1	}	},
{	SG_GET_RGB(000, 200, 255), "Sl2", _TL("schwach lehmiger Sand"),
	{   5,   8,   8,   5,   5               ,  -1	},
	{  10,  10,  25,  25,  10               ,  -1	}	},
{	SG_GET_RGB(000, 200, 200), "Sl3", _TL("mittel lehmiger Sand"),
	{   8,  12,  12,   8,   8               ,  -1	},
	{  10,  10,  40,  40,  10               ,  -1	}	},
{	SG_GET_RGB(200, 000, 255), "Sl4", _TL("stark lehmiger Sand"),
	{  12,  17,  17,  12,  12               ,  -1	},
	{  10,  10,  40,  40,  10               ,  -1	}	},
{	SG_GET_RGB(200, 200, 200), "Slu", _TL("schluffig-lemiger Sand"),
	{   8,  17,  17,   8,   8               ,  -1	},
	{  40,  40,  50,  50,  40               ,  -1	}	},
{	SG_GET_RGB(127, 127, 200), "St2", _TL("schwach toniger Sand"),
	{   5,  17,  17,   5,   5               ,  -1	},
	{   0,   0,  10,  10,   0               ,  -1	}	},
{	SG_GET_RGB(000, 255, 000), "St3", _TL("mittel toniger Sand"),
	{  17,  25,  25,  17,  17               ,  -1	},
	{   0,   0,  15,  15,   0               ,  -1	}	},
{	SG_GET_RGB(127, 255, 127), "Su2", _TL("schwach schluffiger Sand"),
	{   0,   5,   5,   0,   0               ,  -1	},
	{  10,  10,  25,  25,  10               ,  -1	}	},
{	SG_GET_RGB(127, 127, 127), "Su3", _TL("mittel schluffiger Sand"),
	{   0,   8,   8,   0,   0               ,  -1	},
	{  25,  25,  40,  40,  25               ,  -1	}	},
{	SG_GET_RGB(255, 000, 000), "Su4", _TL("stark schluffiger Sand"),
	{   0,   8,   8,   0,   0               ,  -1	},
	{  40,  40,  50,  50,  40               ,  -1	}	},
{	SG_GET_RGB(255, 000, 127), "Ls2", _TL("schwach sandiger Lehm"),
	{  17,  25,  25,  17,  17               ,  -1	},
	{  40,  40,  50,  50,  40               ,  -1	}	},
{	SG_GET_RGB(200, 127, 127), "Ls3", _TL("mittel sandiger Lehm"),
	{  17,  25,  25,  17,  17               ,  -1	},
	{  30,  30,  40,  40,  30               ,  -1	}	},
{	SG_GET_RGB(032, 178, 170), "Ls4", _TL("stark sandiger Lehm"),
	{  17,  25,  25,  17,  17               ,  -1	},
	{  15,  15,  30,  30,  15               ,  -1	}	},
{	SG_GET_RGB(152, 251, 152), "Lt2", _TL("schwach toniger Lehm"),
	{  25,  35,  35,  25,  25               ,  -1	},
	{  30,  30,  50,  50,  30               ,  -1	}	},
{	SG_GET_RGB(255, 160, 122), "Lt3", _TL("mittel toniger Lehm"),
	{  35,  45,  45,  35,  35               ,  -1	},
	{  30,  30,  50,  50,  30               ,  -1	}	},
{	SG_GET_RGB(238, 213, 183), "Lts", _TL("sandig-toniger Lehm"),
	{  25,  45,  45,  25,  25               ,  -1	},
	{  15,  15,  30,  30,  15               ,  -1	}	},
{	SG_GET_RGB(255, 165, 000), "Lu" , _TL("schluffiger Lehm"),
	{  17,  30,  30,  17,  17               ,  -1	},
	{  50,  50,  65,  65,  50               ,  -1	}	},
{	SG_GET_RGB(255, 106, 106), "Uu" , _TL("reiner schluff"),
	{   0,   8,   8,   0,   0               ,  -1	},
	{  80,  80,  92, 100,  80               ,  -1	}	},
{	SG_GET_RGB(255, 69, 000), "Uls" , _TL("sandig-lehmiger Schluff"),
	{   8,  17,  17,   8,   8               ,  -1	},
	{  50,  50,  65,  65,  50               ,  -1	}	},
{	SG_GET_RGB(178, 034, 034), "Us" , _TL("sandiger Schluff"),
	{   0,   8,   8,   0,   0               ,  -1	},
	{  50,  50,  80,  80,  50               ,  -1	}	},
{	SG_GET_RGB(255, 215, 000), "Ut2", _TL("schwach toniger Schluff"),
	{   8,  12,  12,   8,   8               ,  -1	},
	{  65,  65,  88,  92,  65               ,  -1	}	},
{	SG_GET_RGB(255, 228, 181), "Ut3", _TL("mittel toniger Schluff"),
	{  12,  17,  17,  12,  12               ,  -1	},
	{  65,  65,  83,  88,  65               ,  -1	}	},
{	SG_GET_RGB(255, 000, 127), "Ut4", _TL("stark toniger Schluff"),
	{  17,  25,  25,  17,  17               ,  -1	},
	{  65,  65,  75,  83,  65               ,  -1	}	},
{	SG_GET_RGB(255, 133, 000), "Tt" , _TL("reiner Ton"),
	{  65, 100,  65,  65                    ,  -1	},
	{   0,   0,  35,   0                    ,  -1	}	},
{	SG_GET_RGB(000, 205, 000), "Tl" , _TL("lehmiger Ton"),
	{  45,  65,  65,  45,  45               ,  -1	},
	{  15,  15,  30,  30,  15               ,  -1	}	},
{	SG_GET_RGB(202, 255, 112), "Tu2", _TL("schwach schluffiger Ton"),
	{  45,  65,  65,  45,  45               ,  -1	},
	{  30,  30,  35,  55,  30               ,  -1	}	},
{	SG_GET_RGB(173, 255, 047), "Tu3", _TL("mittel schluffiger Ton"),
	{  30,  45,  45,  35,  30,  30          ,  -1	},
	{  50,  50,  55,  65,  65,	50          ,  -1	}	},
{	SG_GET_RGB(255, 228, 196), "Tu4", _TL("stark schluffiger Ton"),
	{  25,  35,  25,  25                    ,  -1	},
	{  65,  65,  75,  65                    ,  -1	}	},
{	SG_GET_RGB(139, 90, 000), "Ts2" , _TL("schwach sandiger Ton"),
	{  45,  65,  65,  45,  45               ,  -1	},
	{   0,   0,  15,  15,   0               ,  -1	}	},
{	SG_GET_RGB(160, 82, 045), "Ts3" , _TL("mittel sandiger Ton"),
	{  35,  45,  45,  35,  35               ,  -1	},
	{   0,   0,  15,  15,   0               ,  -1	}	},
{	SG_GET_RGB(205, 92, 92), "Ts4"  , _TL("stark sandiger Ton"),
	{  25,  35,  35,  25,  25               ,  -1	},
	{   0,   0,  15,  15,   0               ,  -1	}	},
{	0, "", "", NULL, NULL	}	};


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

	const CSG_Shapes &		Get_Polygons	(void)	const	{	return( m_Classes );	}

	//-----------------------------------------------------
	bool	Initialize	(int Classification)
	{
		m_Classes.Create(SHAPE_TYPE_Polygon);

		m_Classes.Add_Field("ID"   , SG_DATATYPE_Int   );
		m_Classes.Add_Field("COLOR", SG_DATATYPE_Color );
		m_Classes.Add_Field("KEY"  , SG_DATATYPE_String);
		m_Classes.Add_Field("NAME" , SG_DATATYPE_String);

		//-------------------------------------------------
		const struct SClass	*Classes;

		switch( Classification )
		{
		default: Classes = Classes_USDA; break;
		case  1: Classes = Classes_KA5 ; break;
		}

		//-------------------------------------------------
		for(int i=0; ; i++)
		{
			if( Classes[i].Key.is_Empty() )	{	break;	}	else
			{
				CSG_Shape	*pClass	= m_Classes.Add_Shape();

				pClass->Set_Value(0, i + 1           );
				pClass->Set_Value(1, Classes[i].Color);
				pClass->Set_Value(2, Classes[i].Key  );
				pClass->Set_Value(3, Classes[i].Name );

				for(int j=0; j<8; j++)
				{
					if( Classes[i].Sand[j] < 0 )	{	break;	}	else
					{
						pClass->Add_Point(Classes[i].Sand[j], Classes[i].Clay[j]);
					}
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
			pClasses->Set_Value(i, 1, Get_Name (i));
			pClasses->Set_Value(i, 2, Get_Key  (i));

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
		CSG_String::Format("%s|%s|",
			_TL("USDA"),
			_TL("Kartieranleitung 5")
		)
	);

	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Scheme as Polygons"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
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
	CSoil_Texture_Classifier	Classifier(Parameters("SCHEME")->asInt());

	//-----------------------------------------------------
	pClass->Set_NoData_Value(0.0);

	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pClass, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		Classifier.Set_LUT(pLUT->asTable(), true);

		DataObject_Set_Parameter(pClass, pLUT            );	// Lookup Table
		DataObject_Set_Parameter(pClass, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	if( Parameters("POLYGONS")->asShapes() && (pLUT = DataObject_Get_Parameter(pClass, "LUT")) != NULL && pLUT->asTable() )
	{
		Parameters("POLYGONS")->asShapes()->Create(Classifier.Get_Polygons());

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
		CSG_String::Format("%s|%s|",
			_TL("USDA"),
			_TL("Kartieranleitung 5")
		)
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
	CSoil_Texture_Classifier	Classifier(Parameters("SCHEME")->asInt());

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

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
