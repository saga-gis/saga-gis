/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    shapes_polygons                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Polygon_Union.cpp                   //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "Polygon_Union.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Dissolve::CPolygon_Dissolve(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Dissolve"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Merges polygons, which share the same attribute value, and "
		"(optionally) dissolves borders between adjacent polygon parts. "
		"If no attribute or combination of attributes is chosen, all polygons will be merged. "
		"\nUses the free and open source software library <b>Clipper</b> created by Angus Johnson.\n"
		"<a target=\"_blank\" href=\"http://www.angusj.com/delphi/clipper.php\">Clipper Homepage</a>\n"
		"<a target=\"_blank\" href=\"http://sourceforge.net/projects/polyclipping/\">Clipper at SourceForge</a>\n"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(pNode, "FIELD_1", _TL("1. Attribute"), _TL(""), true);
	Parameters.Add_Table_Field(pNode, "FIELD_2", _TL("2. Attribute"), _TL(""), true);
	Parameters.Add_Table_Field(pNode, "FIELD_3", _TL("3. Attribute"), _TL(""), true);

	Parameters.Add_Shapes(
		NULL	, "DISSOLVED"	, _TL("Dissolved Polygons"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		NULL	, "BND_KEEP"	, _TL("Keep Boundaries"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	pNode	= Parameters.Add_Table_Fields(
		pNode	, "STAT_FIELDS"	, _TL("Statistics"),
		_TL("")
	);

	Parameters.Add_Value(pNode, "STAT_SUM", _TL("Sum"      ), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "STAT_AVG", _TL("Mean"     ), _TL(""), PARAMETER_TYPE_Bool, true );
	Parameters.Add_Value(pNode, "STAT_MIN", _TL("Minimum"  ), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "STAT_MAX", _TL("Maximum"  ), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "STAT_RNG", _TL("Range"    ), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "STAT_DEV", _TL("Deviation"), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "STAT_VAR", _TL("Variance" ), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "STAT_NUM", _TL("Count"    ), _TL(""), PARAMETER_TYPE_Bool, false);

	Parameters.Add_Choice(
		pNode, "STAT_NAMING"	, _TL("Field Naming"),
		_TL(""), 
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("variable type + original name"),
			_TL("original name + variable type"),
			_TL("original name"),
			_TL("variable type")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygon_Dissolve::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("POLYGONS")) && pParameters->Get_Parameter("POLYGONS")->asShapes() != NULL )
	{
		int	nFields	= pParameters->Get_Parameter("POLYGONS")->asShapes()->Get_Field_Count();

		pParameters->Get_Parameter("FIELD_2")->Set_Value(nFields);
		pParameters->Get_Parameter("FIELD_3")->Set_Value(nFields);
	}

	return( 0 );
}

//---------------------------------------------------------
int CPolygon_Dissolve::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FIELD_1")) )
	{
		pParameters->Get_Parameter("FIELD_2")->Set_Enabled(pParameter->asInt() >= 0);
		pParameters->Get_Parameter("FIELD_3")->Set_Enabled(pParameter->asInt() >= 0 && pParameters->Get_Parameter("FIELD_2")->asInt() >= 0);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FIELD_2")) )
	{
		pParameters->Get_Parameter("FIELD_3")->Set_Enabled(pParameter->asInt() >= 0 && pParameters->Get_Parameter("FIELD_1")->asInt() >= 0);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("STAT_FIELDS")) )
	{
		pParameters->Get_Parameter("STAT_SUM")->Set_Enabled(pParameter->asInt() > 0);
		pParameters->Get_Parameter("STAT_AVG")->Set_Enabled(pParameter->asInt() > 0);
		pParameters->Get_Parameter("STAT_MIN")->Set_Enabled(pParameter->asInt() > 0);
		pParameters->Get_Parameter("STAT_MAX")->Set_Enabled(pParameter->asInt() > 0);
		pParameters->Get_Parameter("STAT_RNG")->Set_Enabled(pParameter->asInt() > 0);
		pParameters->Get_Parameter("STAT_DEV")->Set_Enabled(pParameter->asInt() > 0);
		pParameters->Get_Parameter("STAT_VAR")->Set_Enabled(pParameter->asInt() > 0);
		pParameters->Get_Parameter("STAT_NUM")->Set_Enabled(pParameter->asInt() > 0);

		pParameters->Get_Parameter("STAT_NAMING")->Set_Enabled(pParameter->asInt() > 0);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Dissolve::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if(	!pPolygons->is_Valid() || pPolygons->Get_Count() < 2 )
	{
		Error_Set(_TL("invalid or empty polygons layer"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pUnions	= Parameters("DISSOLVED")->asShapes();

	pUnions->Create(SHAPE_TYPE_Polygon);

	int	Field_1	= Parameters("FIELD_1")->asInt();
	int	Field_2	= Parameters("FIELD_2")->asInt();	if( Field_1 < 0 )	Field_2	= -1;
	int	Field_3	= Parameters("FIELD_3")->asInt();	if( Field_2 < 0 )	Field_3	= -1;

	if( Field_1 >= 0 )
	{
		CSG_String	s	= pPolygons->Get_Field_Name(Field_1);
		pUnions->Add_Field(pPolygons->Get_Field_Name(Field_1), pPolygons->Get_Field_Type(Field_1));

		if( Field_2 >= 0 )
		{
			s	+= CSG_String(" | ") + pPolygons->Get_Field_Name(Field_2);
			pUnions->Add_Field(pPolygons->Get_Field_Name(Field_2), pPolygons->Get_Field_Type(Field_2));

			if( Field_3 >= 0 )
			{
				s	+= CSG_String(" | ") + pPolygons->Get_Field_Name(Field_3);
				pUnions->Add_Field(pPolygons->Get_Field_Name(Field_3), pPolygons->Get_Field_Type(Field_3));
			}
		}

		pPolygons->Set_Index(Field_1, TABLE_INDEX_Ascending, Field_2, TABLE_INDEX_Ascending, Field_3, TABLE_INDEX_Ascending);

		pUnions->Set_Name(CSG_String::Format(SG_T("%s [%s: %s]"), pPolygons->Get_Name(), _TL("Dissolved"), s.c_str()));
	}
	else // if( Field_1 < 0 )
	{
		pUnions->Set_Name(CSG_String::Format(SG_T("%s [%s: %s]"), pPolygons->Get_Name(), _TL("Dissolved"), _TL("All")));
	}

	Init_Statistics(pUnions, pPolygons);

	//-----------------------------------------------------
	CSG_String	Value;

	CSG_Shape	*pUnion		= NULL;

	bool		bDissolve	= Parameters("BND_KEEP")->asBool() == false;

	//-----------------------------------------------------
	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape	*pPolygon	= pPolygons->Get_Shape(pPolygons->Get_Record_byIndex(iPolygon)->Get_Index());

		CSG_String	s;

		if( Field_1 >= 0 )	s	 = pPolygon->asString(Field_1);
		if( Field_2 >= 0 )	s	+= pPolygon->asString(Field_2);
		if( Field_3 >= 0 )	s	+= pPolygon->asString(Field_3);

		if( pUnion == NULL || (Field_1 >= 0 && Value.Cmp(s)) )
		{
			Set_Union(pUnion, bDissolve);

			Value	= s;
			pUnion	= pUnions->Add_Shape(pPolygon, SHAPE_COPY_GEOM);

			if( Field_1 >= 0 )	pUnion->Set_Value(0, pPolygon->asString(Field_1));
			if( Field_2 >= 0 )	pUnion->Set_Value(1, pPolygon->asString(Field_2));
			if( Field_3 >= 0 )	pUnion->Set_Value(2, pPolygon->asString(Field_3));

			Add_Statistics(pUnion, pPolygon, true);
		}
		else
		{
			for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0, nParts=pUnion->Get_Part_Count(); iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
				{
					pUnion->Add_Point(pPolygon->Get_Point(iPoint, iPart), nParts);
				}
			}

			Add_Statistics(pUnion, pPolygon, false);
		}
	}

	Set_Union(pUnion, bDissolve);

	//-----------------------------------------------------
	if( m_Statistics )
	{
		delete[](m_Statistics);
	}

	return( pUnions->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Dissolve::Init_Statistics(CSG_Shapes *pUnions, CSG_Shapes *pPolygons)
{
	m_Stat_pFields	= Parameters("STAT_FIELDS")->asTableFields();

	m_bSUM	= Parameters("STAT_SUM")->asBool();
	m_bAVG	= Parameters("STAT_AVG")->asBool();
	m_bMIN	= Parameters("STAT_MIN")->asBool();
	m_bMAX	= Parameters("STAT_MAX")->asBool();
	m_bRNG	= Parameters("STAT_RNG")->asBool();
	m_bDEV	= Parameters("STAT_DEV")->asBool();
	m_bVAR	= Parameters("STAT_VAR")->asBool();
	m_bNUM	= Parameters("STAT_NUM")->asBool();

	if( m_Stat_pFields->Get_Count() > 0 && (m_bSUM || m_bAVG || m_bMIN || m_bMAX || m_bRNG || m_bDEV || m_bVAR || m_bNUM) )
	{
		m_Statistics	= new CSG_Simple_Statistics[m_Stat_pFields->Get_Count()];

		m_Stat_Offset	= pUnions->Get_Field_Count();

		for(int iField=0; iField<m_Stat_pFields->Get_Count(); iField++)
		{
			CSG_String	s	= pPolygons->Get_Field_Name(m_Stat_pFields->Get_Index(iField));

			if( m_bSUM )	pUnions->Add_Field(Get_Statistics_Name("SUM", s), SG_DATATYPE_Double);
			if( m_bAVG )	pUnions->Add_Field(Get_Statistics_Name("AVG", s), SG_DATATYPE_Double);
			if( m_bMIN )	pUnions->Add_Field(Get_Statistics_Name("MIN", s), SG_DATATYPE_Double);
			if( m_bMAX )	pUnions->Add_Field(Get_Statistics_Name("MAX", s), SG_DATATYPE_Double);
			if( m_bRNG )	pUnions->Add_Field(Get_Statistics_Name("RNG", s), SG_DATATYPE_Double);
			if( m_bDEV )	pUnions->Add_Field(Get_Statistics_Name("STD", s), SG_DATATYPE_Double);
			if( m_bVAR )	pUnions->Add_Field(Get_Statistics_Name("VAR", s), SG_DATATYPE_Double);
			if( m_bNUM )	pUnions->Add_Field(Get_Statistics_Name("NUM", s), SG_DATATYPE_Int   );
		}

		return( true );
	}

	m_Statistics	= NULL;

	if( pUnions->Get_Field_Count() == 0 )
	{
		pUnions->Add_Field("OID", SG_DATATYPE_Int);
	}

	return( false );
}

//---------------------------------------------------------
CSG_String CPolygon_Dissolve::Get_Statistics_Name(const CSG_String &Type, const CSG_String &Name)
{
	CSG_String	s;
	
	switch( Parameters("STAT_NAMING")->asInt() )
	{
	default:
	case 0:	s.Printf(SG_T("%s_%s"), Type.c_str(), Name.c_str());	break;
	case 1:	s.Printf(SG_T("%s_%s"), Name.c_str(), Type.c_str());	break;
	case 2:	s.Printf(SG_T("%s"   ), Name.c_str()              );	break;
	case 3:	s.Printf(SG_T("%s"   ), Type.c_str()              );	break;
	}

	return( s );
}

//---------------------------------------------------------
bool CPolygon_Dissolve::Add_Statistics(CSG_Shape *pUnion, CSG_Shape *pPolygon, bool bReset)
{
	if( m_Statistics )
	{
		for(int iField=0; iField<m_Stat_pFields->Get_Count(); iField++)
		{
			if( bReset )
			{
				m_Statistics[iField].Create();
			}

			m_Statistics[iField]	+= pPolygon->asDouble(m_Stat_pFields->Get_Index(iField));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CPolygon_Dissolve::Set_Union(CSG_Shape *pUnion, bool bDissolve)
{
	if( !pUnion )
	{
		return( false );
	}

	if( bDissolve )
	{
		SG_Polygon_Dissolve(pUnion);
	}

	if( m_Statistics )
	{
		for(int iField=0, jField=m_Stat_Offset; iField<m_Stat_pFields->Get_Count(); iField++)
		{
			if( m_bSUM )	pUnion->Set_Value(jField++, m_Statistics[iField].Get_Sum     ());
			if( m_bAVG )	pUnion->Set_Value(jField++, m_Statistics[iField].Get_Mean    ());
			if( m_bMIN )	pUnion->Set_Value(jField++, m_Statistics[iField].Get_Minimum ());
			if( m_bMAX )	pUnion->Set_Value(jField++, m_Statistics[iField].Get_Maximum ());
			if( m_bRNG )	pUnion->Set_Value(jField++, m_Statistics[iField].Get_Range   ());
			if( m_bDEV )	pUnion->Set_Value(jField++, m_Statistics[iField].Get_StdDev  ());
			if( m_bVAR )	pUnion->Set_Value(jField++, m_Statistics[iField].Get_Variance());
			if( m_bNUM )	pUnion->Set_Value(jField++, m_Statistics[iField].Get_Count   ());
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
