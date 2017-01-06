/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Dissolve"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Merges polygons, which share the same attribute value, and "
		"(optionally) dissolves borders between adjacent polygon parts. "
		"If no attribute or combination of attributes is chosen, all polygons will be merged. "
		"Uses the free and open source software library <b>Clipper</b> created by Angus Johnson."
	));

	Add_Reference("http://www.angusj.com/delphi/clipper.php"     , SG_T("Clipper Homepage"      ));
	Add_Reference("http://sourceforge.net/projects/polyclipping/", SG_T("Clipper at SourceForge"));

	//-----------------------------------------------------
	Parameters.Add_Shapes(NULL,
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(NULL,
		"DISSOLVED"	, _TL("Dissolved Polygons"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Fields(Parameters("POLYGONS"),
		"FIELDS"	, _TL("Dissolve Field(s)"),
		_TL("")
	);

	Parameters.Add_Table_Fields(Parameters("POLYGONS"),
		"STATISTICS", _TL("Statistics Field(s)"),
		_TL("")
	);

	Parameters.Add_Bool(Parameters("STATISTICS"), "STAT_SUM", _TL("Sum"      ), _TL(""), false);
	Parameters.Add_Bool(Parameters("STATISTICS"), "STAT_AVG", _TL("Mean"     ), _TL(""), true );
	Parameters.Add_Bool(Parameters("STATISTICS"), "STAT_MIN", _TL("Minimum"  ), _TL(""), false);
	Parameters.Add_Bool(Parameters("STATISTICS"), "STAT_MAX", _TL("Maximum"  ), _TL(""), false);
	Parameters.Add_Bool(Parameters("STATISTICS"), "STAT_RNG", _TL("Range"    ), _TL(""), false);
	Parameters.Add_Bool(Parameters("STATISTICS"), "STAT_DEV", _TL("Deviation"), _TL(""), false);
	Parameters.Add_Bool(Parameters("STATISTICS"), "STAT_VAR", _TL("Variance" ), _TL(""), false);
	Parameters.Add_Bool(Parameters("STATISTICS"), "STAT_LST", _TL("Listing"  ), _TL(""), false);
	Parameters.Add_Bool(Parameters("STATISTICS"), "STAT_NUM", _TL("Count"    ), _TL(""), false);

	Parameters.Add_Choice(Parameters("STATISTICS"),
		"STAT_NAMING", _TL("Field Naming"),
		_TL(""), 
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("variable type + original name"),
			_TL("original name + variable type"),
			_TL("original name"),
			_TL("variable type")
		), 0
	);

	Parameters.Add_Bool(NULL,
		"BND_KEEP"	, _TL("Keep Boundaries"),
		_TL(""),
		false
	);

	Parameters.Add_Double(NULL,
		"MIN_AREA"	, _TL("Minimum Area"),
		_TL(""),
		0.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygon_Dissolve::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CPolygon_Dissolve::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "STATISTICS") )
	{
		pParameters->Set_Enabled("STAT_SUM", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_AVG", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_MIN", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_MAX", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_RNG", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_DEV", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_VAR", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_LST", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_NUM", pParameter->asInt() > 0);

		pParameters->Set_Enabled("STAT_NAMING", pParameter->asInt() > 0);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "BND_KEEP") )
	{
		pParameters->Set_Enabled("MIN_AREA", pParameter->asBool() == false);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Dissolve::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS" )->asShapes();
	CSG_Shapes	*pDissolved	= Parameters("DISSOLVED")->asShapes();

	if(	!pPolygons->is_Valid() || pPolygons->Get_Count() < 2 )
	{
		Error_Set(_TL("invalid or empty polygons layer"));

		return( false );
	}

	//-----------------------------------------------------
	pDissolved->Create(SHAPE_TYPE_Polygon);

	CSG_Parameter_Table_Fields	&Fields	= *Parameters("FIELDS")->asTableFields();

	CSG_Table	Dissolve;

	if( Fields.Get_Count() == 0 )
	{
		pDissolved->Set_Name(CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("Dissolved")));
	}
	else
	{
		Dissolve.Add_Field("INDEX", SG_DATATYPE_Int   );
		Dissolve.Add_Field("VALUE", SG_DATATYPE_String);

		Dissolve.Set_Record_Count(pPolygons->Get_Count());

		for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
		{
			CSG_Shape	*pPolygon	= pPolygons->Get_Shape(i);

			CSG_String	Value;

			for(int iField=0; iField<Fields.Get_Count(); iField++)
			{
				Value	+= pPolygon->asString(Fields.Get_Index(iField));
			}

			Dissolve[i].Set_Value(0, i);
			Dissolve[i].Set_Value(1, Value);
		}

		Dissolve.Set_Index(1, TABLE_INDEX_Ascending);

		//-------------------------------------------------
		CSG_String	Name;

		for(int iField=0; iField<Fields.Get_Count(); iField++)
		{
			if( iField > 0 )
			{
				Name	+= "; ";
			}

			Name	+= pPolygons->Get_Field_Name(Fields.Get_Index(iField));

			pDissolved->Add_Field(
				pPolygons->Get_Field_Name(Fields.Get_Index(iField)),
				pPolygons->Get_Field_Type(Fields.Get_Index(iField))
			);
		}

		pDissolved->Set_Name(CSG_String::Format("%s [%s: %s]", pPolygons->Get_Name(), _TL("Dissolved"), Name.c_str()));
	}

	Statistics_Initialize(pDissolved, pPolygons);

	//-----------------------------------------------------
	bool	bDissolve	= Parameters("BND_KEEP")->asBool() == false;
	double	minArea		= Parameters("MIN_AREA")->asDouble();

	CSG_String	Value;

	CSG_Shape	*pDissolve	= NULL;

	//-----------------------------------------------------
	for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(
			!Dissolve.Get_Count() ? i : Dissolve[i].asInt(0));

		if( !pDissolve || (Dissolve.Get_Count() && Value.Cmp(Dissolve[i].asString(1))) )
		{
			Get_Dissolved(pDissolve, bDissolve, minArea);

			if( Dissolve.Get_Count() )
			{
				Value	= Dissolve[i].asString(1);
			}

			pDissolve	= pDissolved->Add_Shape(pPolygon, SHAPE_COPY_GEOM);

			for(int iField=0; iField<Fields.Get_Count(); iField++)
			{
				*pDissolve->Get_Value(iField)	= *pPolygon->Get_Value(Fields.Get_Index(iField));
			}

			Statistics_Add(pDissolve, pPolygon, true);
		}
		else
		{
			for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
			{
				pDissolve->Add_Part(pPolygon->Get_Part(iPart));
			}

			Statistics_Add(pDissolve, pPolygon, false);
		}
	}

	Get_Dissolved(pDissolve, bDissolve, minArea);

	//-----------------------------------------------------
	if( m_Statistics )
	{
		delete[](m_Statistics);
	}

	m_List.Clear();

	return( pDissolved->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Dissolve::Get_Dissolved(CSG_Shape *pDissolve, bool bDissolve, double minArea)
{
	if( !pDissolve )
	{
		return( false );
	}

	if( bDissolve )
	{
		SG_Polygon_Dissolve(pDissolve);

		if( minArea > 0.0 )
		{
			for(int iPart=pDissolve->Get_Part_Count()-1; iPart>=0; iPart--)
			{
				if( ((CSG_Shape_Polygon *)pDissolve)->Get_Area(iPart) < minArea )
				{
					pDissolve->Del_Part(iPart);
				}
			}
		}
	}

	if( m_Statistics )
	{
		for(int iField=0, jField=m_Stat_Offset; iField<m_Stat_pFields->Get_Count(); iField++)
		{
			if( m_bSUM ) pDissolve->Set_Value(jField++, m_Statistics[iField].Get_Sum     ());
			if( m_bAVG ) pDissolve->Set_Value(jField++, m_Statistics[iField].Get_Mean    ());
			if( m_bMIN ) pDissolve->Set_Value(jField++, m_Statistics[iField].Get_Minimum ());
			if( m_bMAX ) pDissolve->Set_Value(jField++, m_Statistics[iField].Get_Maximum ());
			if( m_bRNG ) pDissolve->Set_Value(jField++, m_Statistics[iField].Get_Range   ());
			if( m_bDEV ) pDissolve->Set_Value(jField++, m_Statistics[iField].Get_StdDev  ());
			if( m_bVAR ) pDissolve->Set_Value(jField++, m_Statistics[iField].Get_Variance());
			if( m_bNUM ) pDissolve->Set_Value(jField++, m_Statistics[iField].Get_Count   ());
			if( m_bLST ) pDissolve->Set_Value(jField++, m_List      [iField]);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Dissolve::Statistics_Initialize(CSG_Shapes *pDissolved, CSG_Shapes *pPolygons)
{
	m_Stat_pFields	= Parameters("STATISTICS")->asTableFields();

	m_bSUM	= Parameters("STAT_SUM")->asBool();
	m_bAVG	= Parameters("STAT_AVG")->asBool();
	m_bMIN	= Parameters("STAT_MIN")->asBool();
	m_bMAX	= Parameters("STAT_MAX")->asBool();
	m_bRNG	= Parameters("STAT_RNG")->asBool();
	m_bDEV	= Parameters("STAT_DEV")->asBool();
	m_bVAR	= Parameters("STAT_VAR")->asBool();
	m_bLST	= Parameters("STAT_LST")->asBool();
	m_bNUM	= Parameters("STAT_NUM")->asBool();

	if( m_Stat_pFields->Get_Count() > 0 && (m_bSUM || m_bAVG || m_bMIN || m_bMAX || m_bRNG || m_bDEV || m_bVAR || m_bLST || m_bNUM) )
	{
		m_Statistics	= new CSG_Simple_Statistics[m_Stat_pFields->Get_Count()];

		m_Stat_Offset	= pDissolved->Get_Field_Count();

		for(int iField=0; iField<m_Stat_pFields->Get_Count(); iField++)
		{
			CSG_String	s	= pPolygons->Get_Field_Name(m_Stat_pFields->Get_Index(iField));

			if( m_bSUM ) pDissolved->Add_Field(Statistics_Get_Name("SUM", s), SG_DATATYPE_Double);
			if( m_bAVG ) pDissolved->Add_Field(Statistics_Get_Name("AVG", s), SG_DATATYPE_Double);
			if( m_bMIN ) pDissolved->Add_Field(Statistics_Get_Name("MIN", s), SG_DATATYPE_Double);
			if( m_bMAX ) pDissolved->Add_Field(Statistics_Get_Name("MAX", s), SG_DATATYPE_Double);
			if( m_bRNG ) pDissolved->Add_Field(Statistics_Get_Name("RNG", s), SG_DATATYPE_Double);
			if( m_bDEV ) pDissolved->Add_Field(Statistics_Get_Name("STD", s), SG_DATATYPE_Double);
			if( m_bVAR ) pDissolved->Add_Field(Statistics_Get_Name("VAR", s), SG_DATATYPE_Double);
			if( m_bNUM ) pDissolved->Add_Field(Statistics_Get_Name("NUM", s), SG_DATATYPE_Int   );
			if( m_bLST ) pDissolved->Add_Field(Statistics_Get_Name("LST", s), SG_DATATYPE_String);
		}

		if( m_bLST )
		{
			m_List.Set_Count(m_Stat_pFields->Get_Count());
		}

		return( true );
	}

	m_Statistics	= NULL;

	if( pDissolved->Get_Field_Count() == 0 )
	{
		pDissolved->Add_Field("OID", SG_DATATYPE_Int);
	}

	return( false );
}

//---------------------------------------------------------
CSG_String CPolygon_Dissolve::Statistics_Get_Name(const CSG_String &Type, const CSG_String &Name)
{
	CSG_String	s;
	
	switch( Parameters("STAT_NAMING")->asInt() )
	{
	default: s.Printf("%s_%s", Type.c_str(), Name.c_str());	break;
	case  1: s.Printf("%s_%s", Name.c_str(), Type.c_str());	break;
	case  2: s.Printf("%s"   , Name.c_str()              );	break;
	case  3: s.Printf("%s"   , Type.c_str()              );	break;
	}

	return( s );
}

//---------------------------------------------------------
bool CPolygon_Dissolve::Statistics_Add(CSG_Shape *pDissolve, CSG_Shape *pPolygon, bool bReset)
{
	if( m_Statistics )
	{
		for(int iField=0; iField<m_Stat_pFields->Get_Count(); iField++)
		{
			if( bReset )
			{
				m_Statistics[iField].Create();

				if( iField < m_List.Get_Count() )
				{
					m_List[iField].Clear();
				}
			}

			if( !pPolygon->is_NoData(m_Stat_pFields->Get_Index(iField)) )
			{
				m_Statistics[iField]	+= pPolygon->asDouble(m_Stat_pFields->Get_Index(iField));
			}

			if( iField < m_List.Get_Count() )
			{
				if( !m_List[iField].is_Empty() )
				{
					m_List[iField]	+= "|";
				}

				if( !pPolygon->is_NoData(m_Stat_pFields->Get_Index(iField)) )
				{
					m_List[iField]	+= pPolygon->asString(m_Stat_pFields->Get_Index(iField));
				}
			}
		}

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
