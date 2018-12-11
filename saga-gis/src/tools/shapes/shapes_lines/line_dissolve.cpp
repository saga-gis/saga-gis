/**********************************************************
 * Version $Id: line_dissolve.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   line_dissolve.cpp                   //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include "line_dissolve.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Dissolve::CLine_Dissolve(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Line Dissolve"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Dissolves line shapes, which share the same attribute value(s)."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"DISSOLVED"	, _TL("Dissolved Lines"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Table_Fields("LINES",
		"FIELDS"	, _TL("Dissolve Field(s)"),
		_TL("")
	);

	Parameters.Add_Table_Fields("LINES",
		"STATISTICS", _TL("Statistics Field(s)"),
		_TL("")
	);

	Parameters.Add_Bool("STATISTICS", "STAT_SUM", _TL("Sum"      ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_AVG", _TL("Mean"     ), _TL(""), true );
	Parameters.Add_Bool("STATISTICS", "STAT_MIN", _TL("Minimum"  ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_MAX", _TL("Maximum"  ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_RNG", _TL("Range"    ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_DEV", _TL("Deviation"), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_VAR", _TL("Variance" ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_LST", _TL("Listing"  ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_NUM", _TL("Count"    ), _TL(""), false);

	Parameters.Add_Choice("STATISTICS",
		"STAT_NAMING", _TL("Field Naming"),
		_TL(""), 
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("variable type + original name"),
			_TL("original name + variable type"),
			_TL("original name"),
			_TL("variable type")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLine_Dissolve::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CLine_Dissolve::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("STATISTICS") )
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

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Dissolve::On_Execute(void)
{
	CSG_Shapes	*pLines	= Parameters("LINES")->asShapes();

	if(	!pLines->is_Valid() || pLines->Get_Count() < 2 )
	{
		Error_Set(_TL("invalid or empty lines layer"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pDissolved	= Parameters("DISSOLVED")->asShapes();

	pDissolved->Create(SHAPE_TYPE_Line);

	CSG_Parameter_Table_Fields	&Fields	= *Parameters("FIELDS")->asTableFields();

	CSG_Table	Dissolve;

	//-----------------------------------------------------
	if( Fields.Get_Count() == 0 )
	{
		pDissolved->Fmt_Name("%s [%s]", pLines->Get_Name(), _TL("Dissolved"));
	}
	else
	{
		Dissolve.Add_Field("INDEX", SG_DATATYPE_Int   );
		Dissolve.Add_Field("VALUE", SG_DATATYPE_String);

		Dissolve.Set_Record_Count(pLines->Get_Count());

		for(int i=0; i<pLines->Get_Count() && Set_Progress(i, pLines->Get_Count()); i++)
		{
			CSG_Shape	*pLine	= pLines->Get_Shape(i);

			CSG_String	Value;

			for(int iField=0; iField<Fields.Get_Count(); iField++)
			{
				Value	+= pLine->asString(Fields.Get_Index(iField));
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

			Name	+= pLines->Get_Field_Name(Fields.Get_Index(iField));

			pDissolved->Add_Field(
				pLines->Get_Field_Name(Fields.Get_Index(iField)),
				pLines->Get_Field_Type(Fields.Get_Index(iField))
			);
		}

		pDissolved->Fmt_Name("%s [%s: %s]", pLines->Get_Name(), _TL("Dissolved"), Name.c_str());
	}

	//-----------------------------------------------------
	Statistics_Initialize(pDissolved, pLines);

	//-----------------------------------------------------
	CSG_String	Value;	CSG_Shape	*pDissolve	= NULL;

	for(int i=0; i<pLines->Get_Count() && Set_Progress(i, pLines->Get_Count()); i++)
	{
		CSG_Shape	*pLine	= pLines->Get_Shape(!Dissolve.Get_Count() ? i : Dissolve[i].asInt(0));

		if( !pDissolve || (Dissolve.Get_Count() && Value.Cmp(Dissolve[i].asString(1))) )
		{
			if( Dissolve.Get_Count() )
			{
				Value	= Dissolve[i].asString(1);
			}

			pDissolve	= pDissolved->Add_Shape(pLine, SHAPE_COPY_GEOM);

			for(int iField=0; iField<Fields.Get_Count(); iField++)
			{
				*pDissolve->Get_Value(iField)	= *pLine->Get_Value(Fields.Get_Index(iField));
			}

			Statistics_Add(pDissolve, pLine, true);
		}
		else
		{
			Add_Line(pDissolve, pLine);

			Statistics_Add(pDissolve, pLine, false);
		}
	}

	//-----------------------------------------------------
	return( pDissolved->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Dissolve::Add_Line(CSG_Shape *pLine, CSG_Shape *pAdd)
{
	if( pLine && pAdd )
	{
		int	nParts	= pLine->Get_Part_Count();

		for(int iPart=0; iPart<pAdd->Get_Part_Count(); iPart++)
		{
			if( pAdd->Get_Point_Count(iPart) > 1 )
			{
				int	jPart	= pLine->Get_Part_Count();

				for(int iPoint=0; iPoint<pAdd->Get_Point_Count(iPart); iPoint++)
				{
					pLine->Add_Point(pAdd->Get_Point(iPoint, iPart), jPart);
				}
			}
		}

		return( nParts < pLine->Get_Part_Count() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Dissolve::Statistics_Initialize(CSG_Shapes *pDissolved, CSG_Shapes *pLines)
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
			CSG_String	s	= pLines->Get_Field_Name(m_Stat_pFields->Get_Index(iField));

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
CSG_String CLine_Dissolve::Statistics_Get_Name(const CSG_String &Type, const CSG_String &Name)
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
bool CLine_Dissolve::Statistics_Add(CSG_Shape *pDissolve, CSG_Shape *pLine, bool bReset)
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

			if( !pLine->is_NoData(m_Stat_pFields->Get_Index(iField)) )
			{
				m_Statistics[iField]	+= pLine->asDouble(m_Stat_pFields->Get_Index(iField));
			}

			if( iField < m_List.Get_Count() )
			{
				if( !m_List[iField].is_Empty() )
				{
					m_List[iField]	+= "|";
				}

				if( !pLine->is_NoData(m_Stat_pFields->Get_Index(iField)) )
				{
					m_List[iField]	+= pLine->asString(m_Stat_pFields->Get_Index(iField));
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
