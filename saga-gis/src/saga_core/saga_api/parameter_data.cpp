
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  parameter_data.cpp                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "parameters.h"
#include "data_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String SG_Parameter_Type_Get_Name(TSG_Parameter_Type Type)
{
	switch( Type )
	{
	default                              :	return( _TL("Parameter"       ) );

	case PARAMETER_TYPE_Node             :	return( _TL("Node"            ) );

	case PARAMETER_TYPE_Bool             :	return( _TL("Boolean"         ) );
	case PARAMETER_TYPE_Int              :	return( _TL("Integer"         ) );
	case PARAMETER_TYPE_Double           :	return( _TL("Floating point"  ) );
	case PARAMETER_TYPE_Degree           :	return( _TL("Degree"          ) );
	case PARAMETER_TYPE_Date             :	return( _TL("Date"            ) );
	case PARAMETER_TYPE_Range            :	return( _TL("Value range"     ) );
	case PARAMETER_TYPE_Choice           :	return( _TL("Choice"          ) );
	case PARAMETER_TYPE_Choices          :	return( _TL("Choices"         ) );

	case PARAMETER_TYPE_String           :	return( _TL("Text"            ) );
	case PARAMETER_TYPE_Text             :	return( _TL("Long text"       ) );
	case PARAMETER_TYPE_FilePath         :	return( _TL("File path"       ) );

	case PARAMETER_TYPE_Font             :	return( _TL("Font"            ) );
	case PARAMETER_TYPE_Color            :	return( _TL("Color"           ) );
	case PARAMETER_TYPE_Colors           :	return( _TL("Colors"          ) );
	case PARAMETER_TYPE_FixedTable       :	return( _TL("Static table"    ) );
	case PARAMETER_TYPE_Grid_System      :	return( _TL("Grid system"     ) );
	case PARAMETER_TYPE_Table_Field      :	return( _TL("Table field"     ) );
	case PARAMETER_TYPE_Table_Fields     :	return( _TL("Table fields"    ) );

	case PARAMETER_TYPE_DataObject_Output:	return( _TL("Data Object"     ) );
	case PARAMETER_TYPE_Grid             :	return( _TL("Grid"            ) );
	case PARAMETER_TYPE_Grids            :	return( _TL("Grids"           ) );
	case PARAMETER_TYPE_Table            :	return( _TL("Table"           ) );
	case PARAMETER_TYPE_Shapes           :	return( _TL("Shapes"          ) );
	case PARAMETER_TYPE_TIN              :	return( _TL("TIN"             ) );
	case PARAMETER_TYPE_PointCloud       :	return( _TL("Point Cloud"     ) );

	case PARAMETER_TYPE_Grid_List        :	return( _TL("Grid list"       ) );
	case PARAMETER_TYPE_Grids_List       :	return( _TL("Grids list"      ) );
	case PARAMETER_TYPE_Table_List       :	return( _TL("Table list"      ) );
	case PARAMETER_TYPE_Shapes_List      :	return( _TL("Shapes list"     ) );
	case PARAMETER_TYPE_TIN_List         :	return( _TL("TIN list"        ) );
	case PARAMETER_TYPE_PointCloud_List  :	return( _TL("Point Cloud list") );

	case PARAMETER_TYPE_Parameters       :	return( _TL("Parameters"      ) );
	}
}

//---------------------------------------------------------
CSG_String SG_Parameter_Type_Get_Identifier(TSG_Parameter_Type Type)
{
	switch( Type )
	{
	default                              :	return( "parameter"    );

	case PARAMETER_TYPE_Node             :	return( "node"         );

	case PARAMETER_TYPE_Bool             :	return( "boolean"      );
	case PARAMETER_TYPE_Int              :	return( "integer"      );
	case PARAMETER_TYPE_Double           :	return( "double"       );
	case PARAMETER_TYPE_Degree           :	return( "degree"       );
	case PARAMETER_TYPE_Date             :	return( "date"         );
	case PARAMETER_TYPE_Range            :	return( "range"        );
	case PARAMETER_TYPE_Choice           :	return( "choice"       );
	case PARAMETER_TYPE_Choices          :	return( "choices"      );

	case PARAMETER_TYPE_String           :	return( "text"         );
	case PARAMETER_TYPE_Text             :	return( "long_text"    );
	case PARAMETER_TYPE_FilePath         :	return( "file"         );

	case PARAMETER_TYPE_Font             :	return( "font"         );
	case PARAMETER_TYPE_Color            :	return( "color"        );
	case PARAMETER_TYPE_Colors           :	return( "colors"       );
	case PARAMETER_TYPE_FixedTable       :	return( "static_table" );
	case PARAMETER_TYPE_Grid_System      :	return( "grid_system"  );
	case PARAMETER_TYPE_Table_Field      :	return( "table_field"  );
	case PARAMETER_TYPE_Table_Fields     :	return( "table_fields" );

	case PARAMETER_TYPE_DataObject_Output:	return( "data_object"  );
	case PARAMETER_TYPE_Grid             :	return( "grid"         );
	case PARAMETER_TYPE_Grids            :	return( "grids"        );
	case PARAMETER_TYPE_Table            :	return( "table"        );
	case PARAMETER_TYPE_Shapes           :	return( "shapes"       );
	case PARAMETER_TYPE_TIN              :	return( "tin"          );
	case PARAMETER_TYPE_PointCloud       :	return( "points"       );

	case PARAMETER_TYPE_Grid_List        :	return( "grid_list"    );
	case PARAMETER_TYPE_Grids_List       :	return( "grids_list"   );
	case PARAMETER_TYPE_Table_List       :	return( "table_list"   );
	case PARAMETER_TYPE_Shapes_List      :	return( "shapes_list"  );
	case PARAMETER_TYPE_TIN_List         :	return( "tin_list"     );
	case PARAMETER_TYPE_PointCloud_List  :	return( "points_list"  );

	case PARAMETER_TYPE_Parameters       :	return( "parameters"   );
	}
}

//---------------------------------------------------------
TSG_Parameter_Type SG_Parameter_Type_Get_Type(const CSG_String &Identifier)
{
	if( !Identifier.Cmp("node"        ) )	{	return( PARAMETER_TYPE_Node             );	}
	if( !Identifier.Cmp("boolean"     ) )	{	return( PARAMETER_TYPE_Bool             );	}
	if( !Identifier.Cmp("integer"     ) )	{	return( PARAMETER_TYPE_Int              );	}
	if( !Identifier.Cmp("double"      ) )	{	return( PARAMETER_TYPE_Double           );	}
	if( !Identifier.Cmp("degree"      ) )	{	return( PARAMETER_TYPE_Degree           );	}
	if( !Identifier.Cmp("date"        ) )	{	return( PARAMETER_TYPE_Date             );	}
	if( !Identifier.Cmp("range"       ) )	{	return( PARAMETER_TYPE_Range            );	}
	if( !Identifier.Cmp("choice"      ) )	{	return( PARAMETER_TYPE_Choice           );	}
	if( !Identifier.Cmp("choices"     ) )	{	return( PARAMETER_TYPE_Choices          );	}

	if( !Identifier.Cmp("text"        ) )	{	return( PARAMETER_TYPE_String           );	}
	if( !Identifier.Cmp("long_text"   ) )	{	return( PARAMETER_TYPE_Text             );	}
	if( !Identifier.Cmp("file"        ) )	{	return( PARAMETER_TYPE_FilePath         );	}

	if( !Identifier.Cmp("font"        ) )	{	return( PARAMETER_TYPE_Font             );	}
	if( !Identifier.Cmp("color"       ) )	{	return( PARAMETER_TYPE_Color            );	}
	if( !Identifier.Cmp("colors"      ) )	{	return( PARAMETER_TYPE_Colors           );	}
	if( !Identifier.Cmp("static_table") )	{	return( PARAMETER_TYPE_FixedTable       );	}
	if( !Identifier.Cmp("grid_system" ) )	{	return( PARAMETER_TYPE_Grid_System      );	}
	if( !Identifier.Cmp("table_field" ) )	{	return( PARAMETER_TYPE_Table_Field      );	}
	if( !Identifier.Cmp("table_fields") )	{	return( PARAMETER_TYPE_Table_Fields     );	}

	if( !Identifier.Cmp("data_object" ) )	{	return( PARAMETER_TYPE_DataObject_Output);	}
	if( !Identifier.Cmp("grid"        ) )	{	return( PARAMETER_TYPE_Grid             );	}
	if( !Identifier.Cmp("grids"       ) )	{	return( PARAMETER_TYPE_Grids            );	}
	if( !Identifier.Cmp("table"       ) )	{	return( PARAMETER_TYPE_Table            );	}
	if( !Identifier.Cmp("shapes"      ) )	{	return( PARAMETER_TYPE_Shapes           );	}
	if( !Identifier.Cmp("tin"         ) )	{	return( PARAMETER_TYPE_TIN              );	}
	if( !Identifier.Cmp("points"      ) )	{	return( PARAMETER_TYPE_PointCloud       );	}

	if( !Identifier.Cmp("grid_list"   ) )	{	return( PARAMETER_TYPE_Grid_List        );	}
	if( !Identifier.Cmp("grids_list"  ) )	{	return( PARAMETER_TYPE_Grids_List       );	}
	if( !Identifier.Cmp("table_list"  ) )	{	return( PARAMETER_TYPE_Table_List       );	}
	if( !Identifier.Cmp("shapes_list" ) )	{	return( PARAMETER_TYPE_Shapes_List      );	}
	if( !Identifier.Cmp("tin_list"    ) )	{	return( PARAMETER_TYPE_TIN_List         );	}
	if( !Identifier.Cmp("points_list" ) )	{	return( PARAMETER_TYPE_PointCloud_List  );	}

	if( !Identifier.Cmp("parameters"  ) )	{	return( PARAMETER_TYPE_Parameters       );	}

	return( PARAMETER_TYPE_Undefined );
}


///////////////////////////////////////////////////////////
//														 //
//						Node							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Node::CSG_Parameter_Node(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}


///////////////////////////////////////////////////////////
//														 //
//						Bool							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Bool::CSG_Parameter_Bool(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Value	= false;
}

//---------------------------------------------------------
int CSG_Parameter_Bool::_Set_Value(int Value)
{
	bool	bValue = Value != 0;

	if( m_Value != bValue )
	{
		m_Value	= bValue;

		return( SG_PARAMETER_DATA_SET_CHANGED );
	}

	return( SG_PARAMETER_DATA_SET_TRUE );
}

int CSG_Parameter_Bool::_Set_Value(double Value)
{
	return( _Set_Value((int)Value) );
}

int CSG_Parameter_Bool::_Set_Value(const CSG_String &Value)
{
	if( !Value.CmpNoCase("true") || !Value.CmpNoCase("yes") )
	{
		return( _Set_Value(1) );
	}

	if( !Value.CmpNoCase("false") || !Value.CmpNoCase("no") )
	{
		return( _Set_Value(0) );
	}

	int		i;

	if( Value.asInt(i) )
	{
		return( _Set_Value(i) );
	}

	return( SG_PARAMETER_DATA_SET_FALSE );
}

//---------------------------------------------------------
void CSG_Parameter_Bool::_Set_String(void)
{
	m_String	= m_Value ? _TL("true") : _TL("false");
}

//---------------------------------------------------------
bool CSG_Parameter_Bool::_Assign(CSG_Parameter *pSource)
{
	m_Value		= pSource->asBool();

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Bool::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(m_Value ? "true" : "false");
	}
	else
	{
		m_Value	= Entry.Cmp_Content("true", true);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Value							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Value::CSG_Parameter_Value(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Minimum	= 0.;
	m_bMinimum	= false;

	m_Maximum	= 0.;
	m_bMaximum	= false;
}

//---------------------------------------------------------
bool CSG_Parameter_Value::Set_Valid_Range(double Minimum, double Maximum)
{
	if( m_bMinimum && m_bMaximum && Minimum > Maximum )
	{
		m_Minimum	= Maximum;
		m_Maximum	= Minimum;
	}
	else
	{
		m_Minimum	= Minimum;
		m_Maximum	= Maximum;
	}

	int	Result;

	switch( Get_Type() )
	{
	case PARAMETER_TYPE_Double:
	case PARAMETER_TYPE_Degree: Result = _Set_Value(asDouble()); break;
	case PARAMETER_TYPE_Int   : Result = _Set_Value(asInt   ()); break;
	default                   : return( false );
	}

	if( Result == SG_PARAMETER_DATA_SET_CHANGED )
	{
		has_Changed();
	}

	return( Result != SG_PARAMETER_DATA_SET_FALSE );
}

//---------------------------------------------------------
void CSG_Parameter_Value::Set_Minimum(double Minimum, bool bOn)
{
	if( bOn == false )
	{
		m_bMinimum	= false;
	}
	else
	{
		m_bMinimum	= true;

		if( m_bMaximum && m_Maximum < Minimum )
		{
			m_Maximum	= Minimum;
		}

		Set_Valid_Range(Minimum, m_Maximum);
	}
}

//---------------------------------------------------------
void CSG_Parameter_Value::Set_Maximum(double Maximum, bool bOn)
{
	if( bOn == false )
	{
		m_bMaximum	= false;
	}
	else
	{
		m_bMaximum	= true;

		if( m_bMinimum && m_Minimum > Maximum )
		{
			m_Minimum	= Maximum;
		}

		Set_Valid_Range(m_Minimum, Maximum);
	}
}

//---------------------------------------------------------
bool CSG_Parameter_Value::_Assign(CSG_Parameter *pSource)
{
	m_Minimum	= pSource->asValue()->m_Minimum;
	m_bMinimum	= pSource->asValue()->m_bMinimum;

	m_Maximum	= pSource->asValue()->m_Maximum;
	m_bMaximum	= pSource->asValue()->m_bMaximum;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Int								 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Int::CSG_Parameter_Int(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Value(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Value		= 0;
}

//---------------------------------------------------------
int CSG_Parameter_Int::_Set_Value(int Value)
{
	if( m_bMinimum && Value < (int)m_Minimum )
	{
		return( _Set_Value((int)m_Minimum) );
	}

	if( m_bMaximum && Value > (int)m_Maximum )
	{
		return( _Set_Value((int)m_Maximum) );
	}

	if( m_Value != Value )
	{
		m_Value	= Value;

		return( SG_PARAMETER_DATA_SET_CHANGED );
	}

	return( SG_PARAMETER_DATA_SET_TRUE );
}

int CSG_Parameter_Int::_Set_Value(double Value)
{
	return( _Set_Value((int)Value) );
}

int CSG_Parameter_Int::_Set_Value(const CSG_String &Value)
{
	int		i;

	if( Value.asInt(i) )
	{
		return( _Set_Value(i) );
	}

	return( SG_PARAMETER_DATA_SET_FALSE );
}

//---------------------------------------------------------
void CSG_Parameter_Int::_Set_String(void)
{
	m_String.Printf("%d", m_Value);
}

//---------------------------------------------------------
bool CSG_Parameter_Int::_Assign(CSG_Parameter *pSource)
{
	CSG_Parameter_Value::_Assign(pSource);

	return( _Set_Value(((CSG_Parameter_Value *)pSource)->asInt()) != 0 );
}

//---------------------------------------------------------
bool CSG_Parameter_Int::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(asString());
	}
	else
	{
		return( Entry.Get_Content().asInt(m_Value) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Double							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Double::CSG_Parameter_Double(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Value(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Value		= 0.0;
}

//---------------------------------------------------------
int CSG_Parameter_Double::_Set_Value(int Value)
{
	return( _Set_Value((double)Value) );
}

int CSG_Parameter_Double::_Set_Value(double Value)
{
	if( m_bMinimum && Value < m_Minimum )
	{
		return( _Set_Value(m_Minimum) );
	}

	if( m_bMaximum && Value > m_Maximum )
	{
		return( _Set_Value(m_Maximum) );
	}

	if( m_Value != Value )
	{
		m_Value	= Value;

		return( SG_PARAMETER_DATA_SET_CHANGED );
	}

	return( SG_PARAMETER_DATA_SET_TRUE );
}

int  CSG_Parameter_Double::_Set_Value(const CSG_String &Value)
{
	double	d;

	if( Value.asDouble(d) )
	{
		return( _Set_Value(d) );
	}

	return( SG_PARAMETER_DATA_SET_FALSE );
}

//---------------------------------------------------------
void CSG_Parameter_Double::_Set_String(void)
{
	m_String.Printf("%g", m_Value);
}

//---------------------------------------------------------
bool CSG_Parameter_Double::_Assign(CSG_Parameter *pSource)
{
	CSG_Parameter_Value::_Assign(pSource);

	return( _Set_Value(pSource->asDouble()) != 0 );
}

//---------------------------------------------------------
bool CSG_Parameter_Double::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(asString());
	}
	else
	{
		return( Entry.Get_Content().asDouble(m_Value) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Degree							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Degree::CSG_Parameter_Degree(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Double(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
int CSG_Parameter_Degree::_Set_Value(const CSG_String &Value)
{
	return( CSG_Parameter_Double::_Set_Value(SG_Degree_To_Double(Value)) );
}

//---------------------------------------------------------
void CSG_Parameter_Degree::_Set_String(void)
{
	m_String	= SG_Double_To_Degree(asDouble());
}


///////////////////////////////////////////////////////////
//														 //
//						Date							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Date::CSG_Parameter_Date(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	_Set_Value(CSG_DateTime::Now().Get_JDN());
}

//---------------------------------------------------------
int CSG_Parameter_Date::_Set_Value(int Value)
{
	return( _Set_Value((double)Value) );
}

int CSG_Parameter_Date::_Set_Value(double Value)
{
	Value	= 0.5 + floor(Value);	// always adjust to high noon, prevents rounding problems (we're not intested in time, just date!)

	if( Value != asDouble() )
	{
		m_Date.Set(Value);

		return( SG_PARAMETER_DATA_SET_CHANGED );
	}

	return( SG_PARAMETER_DATA_SET_TRUE );
}

int CSG_Parameter_Date::_Set_Value(const CSG_String &Value)
{
	CSG_DateTime	Date;

	if( Date.Parse_Date(Value) )
	{
		return( _Set_Value(Date.Get_JDN()) );
	}

	return( SG_PARAMETER_DATA_SET_FALSE );
}

//---------------------------------------------------------
void CSG_Parameter_Date::_Set_String(void)
{
	m_String	= m_Date.Format_ISODate();
}

//---------------------------------------------------------
int CSG_Parameter_Date::_asInt(void)	const
{
	return( (int)asDouble() );
}

//---------------------------------------------------------
double CSG_Parameter_Date::_asDouble(void)	const
{
	return( m_Date.Get_JDN() );
}

//---------------------------------------------------------
void CSG_Parameter_Date::Set_Date(const CSG_DateTime &Date)
{
	m_Date	= Date;
}

//---------------------------------------------------------
bool CSG_Parameter_Date::_Assign(CSG_Parameter *pSource)
{
	m_Date	= ((CSG_Parameter_Date *)pSource)->m_Date;

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Date::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(asString());
	}
	else
	{
		_Set_Value(Entry.Get_Content());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Range							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Range::CSG_Parameter_Range(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_pRange	= new CSG_Parameters;

	if( is_Information() )
	{
		m_pMin	= (CSG_Parameter_Double *)m_pRange->Add_Info_Value(ID, "MIN", "Minimum", Description, PARAMETER_TYPE_Double);
		m_pMax	= (CSG_Parameter_Double *)m_pRange->Add_Info_Value(ID, "MAX", "Maximum", Description, PARAMETER_TYPE_Double);
	}
	else
	{
		m_pMin	= (CSG_Parameter_Double *)m_pRange->Add_Double    (ID, "MIN", "Minimum", Description);
		m_pMax	= (CSG_Parameter_Double *)m_pRange->Add_Double    (ID, "MAX", "Maximum", Description);
	}
}

CSG_Parameter_Range::~CSG_Parameter_Range(void)
{
	delete(m_pRange);
}

//---------------------------------------------------------
void CSG_Parameter_Range::_Set_String(void)
{
	m_String.Printf("%g; %g", Get_Min(), Get_Max());
}

//---------------------------------------------------------
int CSG_Parameter_Range::_Set_Value(const CSG_String &Value)
{
	return( Set_Range(Value.BeforeFirst(';').asDouble(), Value.AfterFirst(';').asDouble()) ? SG_PARAMETER_DATA_SET_CHANGED : SG_PARAMETER_DATA_SET_FALSE );
}

bool CSG_Parameter_Range::Set_Range(double Min, double Max)
{
	bool	bResult;

	if( Min > Max )
	{
		bResult	 = m_pMin->Set_Value(Max);
		bResult	|= m_pMax->Set_Value(Min);
	}
	else
	{
		bResult	 = m_pMin->Set_Value(Min);
		bResult	|= m_pMax->Set_Value(Max);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_Min(double Value)
{
	if( m_pMin->Set_Value(Value) )
	{
		has_Changed();

		return( true );
	}

	return( false );
}

double CSG_Parameter_Range::Get_Min(void)	const
{
	return( m_pMin->asDouble() );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_Max(double Value)
{
	if( m_pMax->Set_Value(Value) )
	{
		has_Changed();

		return( true );
	}

	return( false );
}

double CSG_Parameter_Range::Get_Max(void)	const
{
	return( m_pMax->asDouble() );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Restore_Default(void)
{
	return( m_pMin->Restore_Default() && m_pMax->Restore_Default() );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::_Assign(CSG_Parameter *pSource)
{
	m_pMin->Assign(pSource->asRange()->m_pMin);
	m_pMax->Assign(pSource->asRange()->m_pMax);

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Fmt_Content("%g; %g", Get_Min(), Get_Max());

		return( true );
	}
	else
	{
		CSG_String	s(Entry.Get_Content());	double	Min, Max;

		if( s.BeforeFirst(';').asDouble(Min) && s.AfterFirst(';').asDouble(Max) )
		{
			return( Set_Range(Min, Max) );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Choice							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Choice::CSG_Parameter_Choice(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Value	= -1;
}

//---------------------------------------------------------
void CSG_Parameter_Choice::Set_Items(const SG_Char *String)
{
	m_Items.Clear();

	if( String && *String != '\0' )
	{
		m_Items	= SG_String_Tokenize(String, "|");
	}

	if( m_Value < 0 && m_Items.Get_Count() > 0 )
	{
		m_Value	= 0;
	}

	if( m_Value >= m_Items.Get_Count() )
	{
		m_Value	= m_Items.Get_Count() - 1;
	}

	_Set_String();	// m_Items have changed
}

//---------------------------------------------------------
CSG_String CSG_Parameter_Choice::Get_Items(void) const
{
	CSG_String	Items;

	for(int i=0; i<m_Items.Get_Count(); i++)
	{
		if( i > 0 )
		{
			Items	+= "|";
		}

		Items	+= m_Items[i];
	}

	return( Items );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Choice::Get_Item(int Index)	const
{
	if( Index >= 0 && Index < m_Items.Get_Count() )
	{
		const SG_Char	*Item	= m_Items[Index].c_str();

		if( *Item == '{' )
		{
			do	{	Item++;	}	while( *Item != '\0' && *Item != '}' );

			if( *Item == '\0' )
			{
				return( m_Items[Index].c_str() );
			}

			Item++;
		}

		return( Item );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_String CSG_Parameter_Choice::Get_Item_Data(int Index)	const
{
	CSG_String	Data;

	if( Index >= 0 && Index < m_Items.Get_Count() )
	{
		Data	= m_Items[Index];	Data.Trim();

		if( Data.Find('{') == 0 )	// data entry within leading curly brackets: '{data} item text'
		{
			Data	= Data.AfterFirst('{').BeforeFirst('}');
		}
	}

	return( Data );
}

bool CSG_Parameter_Choice::Get_Data(int        &Value)	const
{
	CSG_String	String;

	return( Get_Data(String) && String.asInt   (Value) );
}

bool CSG_Parameter_Choice::Get_Data(double     &Value)	const
{
	CSG_String	String;

	return( Get_Data(String) && String.asDouble(Value) );
}

bool CSG_Parameter_Choice::Get_Data(CSG_String &Value)	const
{
	Value	= Get_Item_Data(m_Value);

	return( !Value.is_Empty() );
}

CSG_String CSG_Parameter_Choice::Get_Data(void)	const
{
	return( Get_Item_Data(m_Value) );
}

//---------------------------------------------------------
int CSG_Parameter_Choice::_Set_Value(int               Value)
{
	if( Value >= 0 && Value < m_Items.Get_Count() )
	{
		if( m_Value != Value )
		{
			m_Value	= Value;

			return( SG_PARAMETER_DATA_SET_CHANGED );
		}

		return( SG_PARAMETER_DATA_SET_TRUE );
	}

	return( SG_PARAMETER_DATA_SET_FALSE );
}

//---------------------------------------------------------
int CSG_Parameter_Choice::_Set_Value(double            Value)
{
	return( _Set_Value((int)Value) );
}

//---------------------------------------------------------
int CSG_Parameter_Choice::_Set_Value(const CSG_String &Value)
{
	if( !Value.is_Empty() )
	{
		int	Index;	// first test, if value string is an integer specifying the index position

		if( Value.asInt(Index) && Index >= 0 && Index < m_Items.Get_Count() )
		{
			return( _Set_Value(Index) );
		}

		CSG_String	_Value(Value[0] == '\"' ? Value.AfterFirst('\"').BeforeFirst('\"') : Value);	// quotations can be used to enforce comparison with item's data/name (overpassing index based selection)

		for(int i=0; i<m_Items.Get_Count(); i++)
		{
			if( !_Value.Cmp(Get_Item_Data(i)) || !_Value.Cmp(Get_Item(i)) )
			{
				return( _Set_Value(i) );
			}
		}
	}

	return( SG_PARAMETER_DATA_SET_FALSE );
}

//---------------------------------------------------------
void CSG_Parameter_Choice::_Set_String(void)
{
	m_String	= m_Value >= 0 && m_Value < m_Items.Get_Count() ? Get_Item(m_Value) : _TL("<no choice available>");
}

//---------------------------------------------------------
bool CSG_Parameter_Choice::_Assign(CSG_Parameter *pSource)
{
	m_Items	= pSource->asChoice()->m_Items;
	m_Value	= pSource->asChoice()->m_Value;

	_Set_String();	// m_Items have changed

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Choice::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Add_Property("index", m_Value);

		Entry.Set_Content(asString());

		return( true );
	}
	else
	{
		int	Index;

		return( (Entry.Get_Property("index", Index) || Entry.Get_Content().asInt(Index)) && _Set_Value(Index) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Choice							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Choices::CSG_Parameter_Choices(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
int CSG_Parameter_Choices::_Set_Value(const CSG_String &Value)
{
	CSG_String_Tokenizer	Tokens(Value, ";");

	m_Selection.Destroy();

	while( Tokens.Has_More_Tokens() )
	{
		int	Index;

		if( Tokens.Get_Next_Token().asInt(Index) )
		{
			Select(Index);
		}
	}

	return( SG_PARAMETER_DATA_SET_CHANGED );
}

//---------------------------------------------------------
void CSG_Parameter_Choices::_Set_String(void)
{
	m_String.Clear();

	for(size_t i=0; i<m_Selection.Get_Size(); i++)
	{
		m_String	+= CSG_String::Format("%d;", m_Selection[i]);
	}
}

//---------------------------------------------------------
void CSG_Parameter_Choices::Set_Items(const CSG_String &Items)
{
	Del_Items();

	CSG_String_Tokenizer	Tokens(Items, "|");

	while( Tokens.Has_More_Tokens() )
	{
		Add_Item(Tokens.Get_Next_Token());
	}
}

//---------------------------------------------------------
void CSG_Parameter_Choices::Set_Items(const CSG_Strings &Items)
{
	Del_Items();

	for(int i=0; i<Items.Get_Count(); i++)
	{
		Add_Item(Items[i]);
	}
}

//---------------------------------------------------------
CSG_String CSG_Parameter_Choices::Get_Items(void) const
{
	CSG_String	Items;

	for(int i=0; i<m_Items[0].Get_Count(); i++)
	{
		if( i > 0 )
		{
			Items	+= "|";
		}

		Items	+= m_Items[0][i];
	}

	return( Items );
}

//---------------------------------------------------------
void CSG_Parameter_Choices::Del_Items(void)
{
	m_Items[0].Clear();
	m_Items[1].Clear();

	m_Selection.Destroy();
}

//---------------------------------------------------------
void CSG_Parameter_Choices::Add_Item(const CSG_String &Item, const CSG_String &Data)
{
	m_Items[0]	+= Item;
	m_Items[1]	+= Data;
}

//---------------------------------------------------------
bool CSG_Parameter_Choices::is_Selected(int Index)
{
	for(size_t i=0; i<m_Selection.Get_Size(); i++)
	{
		if( Index == m_Selection[i] )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter_Choices::Select(int Index, bool bSelect)
{
	if( Index >= 0 && Index < Get_Item_Count() )
	{
		if( bSelect && !is_Selected(Index) )
		{
			m_Selection	+= Index;
		}
		else if( !bSelect )
		{
			for(size_t i=0; i<m_Selection.Get_Size(); i++)
			{
				if( Index == m_Selection[i] )
				{
					for(size_t j=i+1; j<m_Selection.Get_Size(); i++, j++)
					{
						m_Selection[i]	= m_Selection[j];
					}

					m_Selection.Dec_Array();
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter_Choices::Clr_Selection(void)
{
	m_Selection.Destroy();

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Choices::_Assign(CSG_Parameter *pSource)
{
	m_Items[0]	= pSource->asChoices()->m_Items[0];
	m_Items[1]	= pSource->asChoices()->m_Items[1];

	m_Selection	= pSource->asChoices()->m_Selection;

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Choices::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(asString());

		return( true );
	}
	else
	{
		return( _Set_Value(Entry.Get_Content()) != 0 );
	}
}


///////////////////////////////////////////////////////////
//														 //
//						String							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_String::CSG_Parameter_String(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_bPassword	= false;
}

//---------------------------------------------------------
bool CSG_Parameter_String::is_Valid(void)	const
{
	return( !m_String.is_Empty() );
}

//---------------------------------------------------------
void CSG_Parameter_String::Set_Password(bool bOn)
{
	m_bPassword	= bOn;
}

//---------------------------------------------------------
bool CSG_Parameter_String::is_Password(void)	const
{
	return( m_bPassword );
}

//---------------------------------------------------------
int CSG_Parameter_String::_Set_Value(const CSG_String &Value)
{
	if( m_String.Cmp(Value) )
	{
		m_String	= Value;

		return( SG_PARAMETER_DATA_SET_CHANGED );
	}

	return( SG_PARAMETER_DATA_SET_TRUE );
}

//---------------------------------------------------------
bool CSG_Parameter_String::_Assign(CSG_Parameter *pSource)
{
	m_String	= ((CSG_Parameter_String *)pSource)->m_String;
	m_bPassword	= ((CSG_Parameter_String *)pSource)->m_bPassword;

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_String::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(m_String);
	}
	else
	{
		m_String	= Entry.Get_Content();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Text							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Text::CSG_Parameter_Text(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_String(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}


///////////////////////////////////////////////////////////
//														 //
//						FilePath						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_File_Name::CSG_Parameter_File_Name(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_String(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Filter.Printf("%s|*.*", _TL("All Files"));

	m_bSave			= false;
	m_bMultiple		= false;
	m_bDirectory	= false;
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::Set_Filter(const SG_Char *Filter)
{
	if( Filter )
	{
		m_Filter	= Filter;
	}
	else
	{
		m_Filter.Printf("%s|*.*", _TL("All Files"));
	}
}

const SG_Char *  CSG_Parameter_File_Name::Get_Filter(void)	const
{
	return( m_Filter.c_str() );
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::Set_Flag_Save(bool bFlag)
{
	m_bSave			= bFlag;
}

void CSG_Parameter_File_Name::Set_Flag_Multiple(bool bFlag)
{
	m_bMultiple		= bFlag;
}

void CSG_Parameter_File_Name::Set_Flag_Directory(bool bFlag)
{
	m_bDirectory	= bFlag;
}

//---------------------------------------------------------
bool CSG_Parameter_File_Name::Get_FilePaths(CSG_Strings &FilePaths)	const
{
	FilePaths.Clear();

	if( m_String.Length() > 0 )
	{
		if( !m_bMultiple )
		{
			FilePaths.Add(m_String);
		}
		else if( m_String[0] != '\"' )
		{
			FilePaths.Add(m_String);
		}
		else
		{
			CSG_String	s(m_String);

			while( s.Length() > 2 )
			{
				s	= s.AfterFirst('\"');
				FilePaths.Add(s.BeforeFirst('\"'));
				s	= s.AfterFirst('\"');
			}
		}
	}

	return( FilePaths.Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Parameter_File_Name::_Assign(CSG_Parameter *pSource)
{
	CSG_Parameter_String::_Assign(pSource);

	Set_Filter(((CSG_Parameter_File_Name *)pSource)->m_Filter.c_str());

	m_bSave			= pSource->asFilePath()->m_bSave;
	m_bMultiple		= pSource->asFilePath()->m_bMultiple;
	m_bDirectory	= pSource->asFilePath()->m_bDirectory;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Font							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Font::CSG_Parameter_Font(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	Restore_Default();
}

//---------------------------------------------------------
bool CSG_Parameter_Font::Restore_Default(void)
{
	m_Color		= SG_GET_RGB(0, 0, 0);
	m_Font		= "0;-13;0;0;0;400;0;0;0;0;3;2;1;34;Arial";
	m_String	= "Arial";

	return( true );
}

//---------------------------------------------------------
int CSG_Parameter_Font::_Set_Value(int Value)
{
	if( m_Color != Value )
	{
		m_Color	= Value;

		return( SG_PARAMETER_DATA_SET_CHANGED );
	}

	return( SG_PARAMETER_DATA_SET_TRUE );
}

//---------------------------------------------------------
int CSG_Parameter_Font::_Set_Value(const CSG_String &Value)
{
	if( Value.is_Empty() )
	{
		Restore_Default();

		return( SG_PARAMETER_DATA_SET_CHANGED );
	}

	m_Font		= Value;
	m_String	= m_Font.AfterLast(';');

	return( SG_PARAMETER_DATA_SET_CHANGED );
}

//---------------------------------------------------------
int CSG_Parameter_Font::_asInt(void)	const
{
	return( m_Color );
}

//---------------------------------------------------------
void * CSG_Parameter_Font::_asPointer(void)	const
{
	return( (void *)m_Font.c_str() );
}

//---------------------------------------------------------
bool CSG_Parameter_Font::_Assign(CSG_Parameter *pSource)
{
	m_Color		= ((CSG_Parameter_Font *)pSource)->m_Color;
	m_Font		= ((CSG_Parameter_Font *)pSource)->m_Font;
	m_String	= ((CSG_Parameter_Font *)pSource)->m_String;

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Font::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Add_Child("COLOR", CSG_String::Format("R%03d G%03d B%03d",
			SG_GET_R(m_Color),
			SG_GET_G(m_Color),
			SG_GET_B(m_Color)
		));

		Entry.Add_Child("FONT", m_Font);
	}
	else
	{
		if( Entry("COLOR") != NULL )
		{
			_Set_Value((int)SG_GET_RGB(
				Entry("COLOR")->Get_Content().AfterFirst('R').asInt(),
				Entry("COLOR")->Get_Content().AfterFirst('G').asInt(),
				Entry("COLOR")->Get_Content().AfterFirst('B').asInt()
			));
		}

		if( Entry("FONT") != NULL )
		{
			_Set_Value(Entry("FONT")->Get_Content());
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Color							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Color::CSG_Parameter_Color(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Int(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
int CSG_Parameter_Color::_Set_Value(const CSG_String &Value)
{
	long	l;

	if( SG_Color_From_Text(Value, l) )
	{
		return( CSG_Parameter_Int::_Set_Value((int)l) );
	}

	int		i;

	if( Value.asInt(i) )
	{
		return( CSG_Parameter_Int::_Set_Value(i) );
	}

	return( SG_PARAMETER_DATA_SET_FALSE );
}

//---------------------------------------------------------
bool CSG_Parameter_Color::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Fmt_Content("R%03d G%03d B%03d", SG_GET_R(m_Value), SG_GET_G(m_Value), SG_GET_B(m_Value));
	}
	else
	{
		m_Value	= SG_GET_RGB(
			Entry.Get_Content().AfterFirst('R').asInt(),
			Entry.Get_Content().AfterFirst('G').asInt(),
			Entry.Get_Content().AfterFirst('B').asInt()
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Colors							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Colors::CSG_Parameter_Colors(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
void CSG_Parameter_Colors::_Set_String(void)
{
	m_String.Printf("%d %s", m_Colors.Get_Count(), _TL("colors"));
}


//---------------------------------------------------------
int CSG_Parameter_Colors::_asInt(void)	const
{
	return( m_Colors.Get_Count() );
}

//---------------------------------------------------------
void * CSG_Parameter_Colors::_asPointer(void)	const
{
	return( (void *)&m_Colors );
}

//---------------------------------------------------------
bool CSG_Parameter_Colors::_Assign(CSG_Parameter *pSource)
{
	return( m_Colors.Assign(pSource->asColors()) );
}

//---------------------------------------------------------
bool CSG_Parameter_Colors::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		for(int i=0; i<m_Colors.Get_Count(); i++)
		{
			Entry.Add_Child("COLOR", CSG_String::Format("R%03d G%03d B%03d",
				m_Colors.Get_Red  (i),
				m_Colors.Get_Green(i),
				m_Colors.Get_Blue (i))
			);
		}
	}
	else
	{
		if( Entry.Get_Children_Count() <= 1 )
		{
			return( false );
		}

		m_Colors.Set_Count(Entry.Get_Children_Count());

		for(int i=0; i<m_Colors.Get_Count(); i++)
		{
			CSG_String	s(Entry(i)->Get_Content());

			m_Colors.Set_Red  (i, s.AfterFirst('R').asInt());
			m_Colors.Set_Green(i, s.AfterFirst('G').asInt());
			m_Colors.Set_Blue (i, s.AfterFirst('B').asInt());
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						FixedTable						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Fixed_Table::CSG_Parameter_Fixed_Table(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Table.Set_Name(_TL("Table"));
}

//---------------------------------------------------------
void CSG_Parameter_Fixed_Table::_Set_String(void)
{
	m_String.Printf("%s (%s: %d, %s: %d)", m_Table.Get_Name(), _TL("columns"), m_Table.Get_Field_Count(), _TL("rows"), m_Table.Get_Record_Count());
}

//---------------------------------------------------------
void * CSG_Parameter_Fixed_Table::_asPointer(void)	const
{
	return( (void *)&m_Table );
}

//---------------------------------------------------------
bool CSG_Parameter_Fixed_Table::_Assign(CSG_Parameter *pSource)
{
	return( m_Table.Create(*pSource->asTable()) );
}

//---------------------------------------------------------
bool CSG_Parameter_Fixed_Table::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	int	iField;

	if( bSave )
	{
		CSG_MetaData	*pNode	= Entry.Add_Child("FIELDS");

		for(iField=0; iField<m_Table.Get_Field_Count(); iField++)
		{
			CSG_MetaData	*pEntry	= pNode->Add_Child("FIELD", m_Table.Get_Field_Name(iField));

			pEntry->Set_Property("type", gSG_Data_Type_Identifier[m_Table.Get_Field_Type(iField)]);
		}

		pNode	= Entry.Add_Child("RECORDS");

		for(int iRecord=0; iRecord<m_Table.Get_Count(); iRecord++)
		{
			CSG_MetaData	*pEntry	= pNode->Add_Child("RECORD");

			CSG_Table_Record	*pRecord	= m_Table.Get_Record(iRecord);

			for(iField=0; iField<m_Table.Get_Field_Count(); iField++)
			{
				pEntry->Add_Child("FIELD", pRecord->asString(iField));
			}
		}
	}
	else
	{
		CSG_MetaData	*pNode;

		if( (pNode = Entry("FIELDS")) == NULL )
		{
			return( false );
		}

		CSG_Table	Table;

		for(iField=0; iField<pNode->Get_Children_Count(); iField++)
		{
			TSG_Data_Type	type	= SG_DATATYPE_String;

			CSG_String	s;

			if( pNode->Get_Child(iField)->Get_Property("type", s) )
			{
				type	= SG_Data_Type_Get_Type(s);
			}

			Table.Add_Field(pNode->Get_Child(iField)->Get_Content(), type == SG_DATATYPE_Undefined ? SG_DATATYPE_String : type);
		}

		if( (pNode = Entry("RECORDS")) == NULL )
		{
			return( false );
		}

		for(int iRecord=0; iRecord<pNode->Get_Children_Count(); iRecord++)
		{
			CSG_MetaData	*pEntry	= pNode->Get_Child(iRecord);

			CSG_Table_Record	*pRecord	= Table.Add_Record();

			for(iField=0; iField<pEntry->Get_Children_Count(); iField++)
			{
				pRecord->Set_Value(iField, pEntry->Get_Child(iField)->Get_Content());
			}
		}

		return( m_Table.Assign_Values(&Table) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid_System						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grid_System::CSG_Parameter_Grid_System(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
int CSG_Parameter_Grid_System::_Set_Value(void *Value)
{
	CSG_Grid_System	System;

	if( Value != NULL )
	{
		System.Create(*((CSG_Grid_System *)Value));
	}

	if( m_System == System )
	{
		return( SG_PARAMETER_DATA_SET_TRUE );
	}

	m_System	= System;

	//-----------------------------------------------------
	CSG_Data_Manager *pManager    = Get_Manager();

	for(int i=0; i<Get_Children_Count(); i++)
	{
		CSG_Parameter	*pParameter	= Get_Child(i);

		if( pParameter->is_DataObject() )
		{
			CSG_Data_Object	*pObject	= pParameter->asDataObject();

			bool	bInvalid	= !m_System.is_Valid() || !(pManager && pManager->Exists(pObject));

			if( !bInvalid && pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE )
			{
				switch( pObject->Get_ObjectType() )
				{
				case SG_DATAOBJECT_TYPE_Grid : bInvalid = !m_System.is_Equal(((CSG_Grid  *)pObject)->Get_System()); break;
				case SG_DATAOBJECT_TYPE_Grids: bInvalid = !m_System.is_Equal(((CSG_Grids *)pObject)->Get_System()); break;
				default: break;
				}
			}

			if( bInvalid && pObject != DATAOBJECT_CREATE )
			{
				pParameter->Set_Value(DATAOBJECT_NOTSET);
			}
		}

		//-------------------------------------------------
		else if( pParameter->is_DataObject_List() )
		{
			CSG_Parameter_List	*pList	= pParameter->asList();

			if( !m_System.is_Valid() )
			{
				pList->Del_Items();
			}
			else for(int j=pList->Get_Item_Count()-1; j>=0; j--)
			{
				CSG_Data_Object	*pObject	= pList->Get_Item(j);

				bool	bInvalid	= !(pManager && pManager->Exists(pObject));

				if( !bInvalid && pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE )
				{
					switch( pObject->Get_ObjectType() )
					{
					case SG_DATAOBJECT_TYPE_Grid : bInvalid = !m_System.is_Equal(((CSG_Grid  *)pObject)->Get_System()); break;
					case SG_DATAOBJECT_TYPE_Grids: bInvalid = !m_System.is_Equal(((CSG_Grids *)pObject)->Get_System()); break;
					default: break;
					}
				}

				if( bInvalid )
				{
					pList->Del_Item(j);
				}
			}

			pParameter->has_Changed();
		}
	}

	//-----------------------------------------------------
	return( SG_PARAMETER_DATA_SET_CHANGED );
}

//---------------------------------------------------------
void CSG_Parameter_Grid_System::_Set_String(void)
{
	m_String	= m_System.Get_Name();
}

//---------------------------------------------------------
void * CSG_Parameter_Grid_System::_asPointer(void)	const
{
	return( (void *)&m_System );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_System::_Assign(CSG_Parameter *pSource)
{
	m_System	= ((CSG_Parameter_Grid_System *)pSource)->m_System;

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_System::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Add_Child("CELLSIZE", m_System.Get_Cellsize());
		Entry.Add_Child("XMIN"    , m_System.Get_Extent().Get_XMin());
		Entry.Add_Child("XMAX"    , m_System.Get_Extent().Get_XMax());
		Entry.Add_Child("YMIN"    , m_System.Get_Extent().Get_YMin());
		Entry.Add_Child("YMAX"    , m_System.Get_Extent().Get_YMax());
	}
	else
	{
		double		Cellsize;
		TSG_Rect	Extent;

		Cellsize	= Entry("CELLSIZE")->Get_Content().asDouble();
		Extent.xMin	= Entry("XMIN"    )->Get_Content().asDouble();
		Extent.xMax	= Entry("XMAX"    )->Get_Content().asDouble();
		Extent.yMin	= Entry("YMIN"    )->Get_Content().asDouble();
		Extent.yMax	= Entry("YMAX"    )->Get_Content().asDouble();

		m_System.Assign(Cellsize, CSG_Rect(Extent));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Table Field							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Table_Field::CSG_Parameter_Table_Field(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Int(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Default	= -1;
	m_Value		= -1;
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Field::Add_Default(double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	if( m_Default < 0 && is_Optional() )
	{
		m_Default	= Get_Children_Count();

		Get_Parameters()->Add_Double(Get_Identifier(), CSG_String::Format("%s_DEFAULT", Get_Identifier()),
			_TL("Default"), _TL("default value if no attribute has been selected"),
			Value, Minimum, bMinimum, Maximum, bMaximum
		);
	}

	return( m_Default >= 0 );
}

//---------------------------------------------------------
CSG_Table * CSG_Parameter_Table_Field::Get_Table(void)	const
{
	CSG_Table	*pTable	= Get_Parent() ? Get_Parent()->asTable() : NULL;

	return( pTable && pTable != DATAOBJECT_CREATE && pTable->Get_Field_Count() > 0 ? pTable : NULL );
}

//---------------------------------------------------------
int CSG_Parameter_Table_Field::_Set_Value(int Value)
{
	CSG_Table	*pTable	= Get_Table();

	if( pTable != NULL && pTable->Get_Field_Count() > 0 && Value >= 0 )
	{
		if( Value >= pTable->Get_Field_Count() )
		{
			Value	= !is_Optional() ? pTable->Get_Field_Count() - 1 : -1;
		}
	}
	else
	{
		Value	= -1;
	}

	if( Get_Child(m_Default) )
	{
		Get_Child(m_Default)->Set_Enabled(Value < 0);
	}

	if( m_Value != Value )
	{
		m_Value	= Value;

		return( SG_PARAMETER_DATA_SET_CHANGED );
	}

	return( SG_PARAMETER_DATA_SET_TRUE );
}

//---------------------------------------------------------
int CSG_Parameter_Table_Field::_Set_Value(const CSG_String &Value)
{
	CSG_Table	*pTable	= Get_Table();

	if( pTable != NULL )
	{
		int	Index;

		for(Index=0; Index<pTable->Get_Field_Count(); Index++)
		{
			if( !Value.CmpNoCase(pTable->Get_Field_Name(Index)) )
			{
				return( _Set_Value(Index) );
			}
		}

		if( Value.asInt(Index) )
		{
			return( _Set_Value(Index) );
		}
	}

	return( _Set_Value(-1) );
}

//---------------------------------------------------------
void CSG_Parameter_Table_Field::_Set_String(void)
{
	CSG_Table	*pTable	= Get_Table();

	if( !pTable || pTable->Get_Field_Count() < 1 )
	{
		m_String	= _TL("<no attributes>");
	}
	else if( m_Value < 0 || m_Value >= pTable->Get_Field_Count() )
	{
		m_String	= _TL("<not set>");
	}
	else
	{
		m_String	= pTable->Get_Field_Name(m_Value);
	}
}

//---------------------------------------------------------
double CSG_Parameter_Table_Field::_asDouble(void) const
{
	return( Get_Child(m_Default) ? Get_Child(m_Default)->asDouble() : CSG_Parameter_Int::asDouble() );
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Field::_Assign(CSG_Parameter *pSource)
{
	m_Value		= ((CSG_Parameter_Table_Field *)pSource)->m_Value;
	m_Default	= ((CSG_Parameter_Table_Field *)pSource)->m_Default;

	_Set_String();

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Field::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Property("index", asInt());
		Entry.Set_Content(asString());

		return( true );
	}
	else
	{
		int	Index;

		if( Entry.Get_Property("index", Index) )	// we require this check for backward compatibility, "index" was first introduced with SAGA 2.2.3 (r2671)
		{
			return( _Set_Value(Index) != 0 );
		}

		return( _Set_Value(Entry.Get_Content()) != 0 );
	}
}


///////////////////////////////////////////////////////////
//														 //
//					Table Fields						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Table_Fields::CSG_Parameter_Table_Fields(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_nFields	= 0;
	m_Fields	= NULL;
}

//---------------------------------------------------------
CSG_Parameter_Table_Fields::~CSG_Parameter_Table_Fields(void)
{
	SG_FREE_SAFE(m_Fields);
}

//---------------------------------------------------------
int CSG_Parameter_Table_Fields::_Set_Value(const CSG_String &Value)
{
	CSG_Table	*pTable	= Get_Table();

	if( !pTable || pTable->Get_Field_Count() <= 0 )
	{
		SG_FREE_SAFE(m_Fields);
		m_nFields	= 0;
		m_String	= _TL("<no attributes>");

		return( SG_PARAMETER_DATA_SET_CHANGED );
	}

	m_Fields	= (int *)SG_Realloc(m_Fields, pTable->Get_Field_Count() * sizeof(int));

	memset(m_Fields, 0, pTable->Get_Field_Count() * sizeof(int));

	//-----------------------------------------------------
	int		iField;

	CSG_String	List(Value);	List.Replace(";", ",");

	while( List.Length() > 0 )
	{
		CSG_String	sValue	= List.BeforeFirst(',');

		if( sValue.asInt(iField) && iField >= 0 && iField < pTable->Get_Field_Count() )
		{
			m_Fields[iField]	= 1;
		}
		else if( sValue.Length() > 0 )
		{
			sValue.Trim();

			if( sValue[0] == '[' )
			{
				sValue	= sValue.AfterFirst('[').BeforeLast(']');
			}

			for(iField=0; iField<pTable->Get_Field_Count(); iField++)
			{
				if( sValue.CmpNoCase(pTable->Get_Field_Name(iField)) == 0 )
				{
					m_Fields[iField]	= 1;

					break;
				}
			}
		}

		List	= List.AfterFirst(',');
	}

	//-----------------------------------------------------
	m_String.Clear();

	for(iField=0, m_nFields=0; iField<pTable->Get_Field_Count(); iField++)
	{
		if( m_Fields[iField] != 0 )
		{
			m_Fields[m_nFields++]	= iField;

			m_String	+= CSG_String::Format(m_String.is_Empty() ? "%d" : ",%d", iField);
		}
	}

	if( m_nFields <= 0 )
	{
		m_String	= _TL("<no attributes>");
	}

	return( SG_PARAMETER_DATA_SET_CHANGED );
}

//---------------------------------------------------------
int CSG_Parameter_Table_Fields::_asInt(void)	const
{
	return( m_nFields );
}

//---------------------------------------------------------
void * CSG_Parameter_Table_Fields::_asPointer(void)	const
{
	return( m_Fields );
}

//---------------------------------------------------------
CSG_Table * CSG_Parameter_Table_Fields::Get_Table(void)	const
{
	CSG_Table	*pTable	= Get_Parent() ? Get_Parent()->asTable() : NULL;

	return( pTable && pTable != DATAOBJECT_CREATE && pTable->Get_Field_Count() > 0 ? pTable : NULL );
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Fields::_Assign(CSG_Parameter *pSource)
{
	return( _Set_Value(pSource->asString()) != 0 );
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Fields::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(m_String);
	}
	else
	{
		m_String	= Entry.Get_Content();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						DataObject						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data_Object::CSG_Parameter_Data_Object(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_pDataObject	= NULL;
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::is_Valid(void)	const
{
	return(	is_Optional() || (m_pDataObject && m_pDataObject->is_Valid()) );
}

//---------------------------------------------------------
int CSG_Parameter_Data_Object::_Set_Value(void *Value)
{
	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CSG_Data_Object *)Value;

		return( SG_PARAMETER_DATA_SET_CHANGED );
	}

	return( SG_PARAMETER_DATA_SET_TRUE );
}

//---------------------------------------------------------
void CSG_Parameter_Data_Object::_Set_String(void)
{
	if( m_pDataObject == DATAOBJECT_NOTSET )
	{
		m_String	= is_Output() && !is_Optional() ? _TL("<create>") : _TL("<not set>");
	}
	else if( m_pDataObject == DATAOBJECT_CREATE )
	{
		m_String	= _TL("<create>");
	}
	else
	{
		m_String	= m_pDataObject->Get_Name();
	}
}

//---------------------------------------------------------
void * CSG_Parameter_Data_Object::_asPointer(void)	const
{
	return( m_pDataObject );
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::_Assign(CSG_Parameter *pSource)
{
	m_pDataObject	= ((CSG_Parameter_Data_Object *)pSource)->m_pDataObject;

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		if(	m_pDataObject == DATAOBJECT_CREATE )
		{
			Entry.Set_Content("CREATE");

			return( true );
		}

		if( m_pDataObject == DATAOBJECT_NOTSET )//|| !SG_File_Exists(m_pDataObject->Get_File_Name(false)) )
		{
			Entry.Set_Content("NOT SET");

			return( true );
		}

		if( !m_pDataObject->Get_Owner() )
		{
			Entry.Set_Content(m_pDataObject->Get_File_Name(false));

			return( true );
		}

		if( m_pDataObject->Get_Owner()->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
		{
			CSG_Grids	*pGrids	= m_pDataObject->Get_Owner()->asGrids();

			for(int i=0; i<pGrids->Get_NZ(); i++)
			{
				if( pGrids->Get_Grid_Ptr(i) == m_pDataObject )
				{
					Entry.Set_Content(m_pDataObject->Get_Owner()->Get_File_Name(false));

					Entry.Add_Property("index", i);

					return( true );
				}
			}
		}
	}
	else
	{
		CSG_Data_Object	*pDataObject = NULL; int Index = -1;

		if( Entry.Cmp_Content("CREATE") )
		{
			_Set_Value(DATAOBJECT_CREATE);

			return( true );
		}

		if( Entry.Cmp_Content("NOT SET") || !Get_Manager() || !(pDataObject = Get_Manager()->Find(Entry.Get_Content(), false)) )
		{
			_Set_Value(DATAOBJECT_NOTSET);

			return( true );
		}

		if( !Entry.Get_Property("index", Index) )
		{
			_Set_Value(pDataObject);

			return( true );
		}

		if( pDataObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
		{
			_Set_Value(pDataObject->asGrids()->Get_Grid_Ptr(Index));

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//					DataObject_Output					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data_Object_Output::CSG_Parameter_Data_Object_Output(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Data_Object(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Type	= SG_DATAOBJECT_TYPE_Undefined;
}

//---------------------------------------------------------
int CSG_Parameter_Data_Object_Output::_Set_Value(void *Value)
{
	CSG_Data_Object	*pDataObject	= (CSG_Data_Object *)Value;

	if( pDataObject == DATAOBJECT_CREATE )
	{
		pDataObject	= NULL;
	}

	if( m_pDataObject != pDataObject && (pDataObject == NULL || pDataObject->Get_ObjectType() == m_Type) )
	{
		m_pDataObject	= pDataObject;

		if( Get_Manager() )
		{
			Get_Manager()->Add(m_pDataObject);

			if( Get_Manager() == &SG_Get_Data_Manager() )	// prevent that local data manager send their data objects to gui
			{
				SG_UI_DataObject_Add(m_pDataObject, false);
			}
		}
	}

	return( SG_PARAMETER_DATA_SET_CHANGED );
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object_Output::Set_DataObject_Type(TSG_Data_Object_Type Type)
{
	if( m_Type == SG_DATAOBJECT_TYPE_Undefined )	// m_Type should not be changed after set once!!!...
	{
		switch( Type )
		{
		case SG_DATAOBJECT_TYPE_Grid      :
		case SG_DATAOBJECT_TYPE_Grids     :
		case SG_DATAOBJECT_TYPE_Table     :
		case SG_DATAOBJECT_TYPE_Shapes    :
		case SG_DATAOBJECT_TYPE_TIN       :
		case SG_DATAOBJECT_TYPE_PointCloud:
			m_Type	= Type;

			return( true );

		default:
			return( false );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grid::CSG_Parameter_Grid(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Data_Object(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Type		= SG_DATATYPE_Undefined;

	m_Default	= -1;
}

//---------------------------------------------------------
void CSG_Parameter_Grid::Set_Preferred_Type(TSG_Data_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
CSG_Grid_System * CSG_Parameter_Grid::Get_System(void)	const
{
	if( Get_Parent() && Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( Get_Parent()->asGrid_System() );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid::Add_Default(double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	if( m_Default < 0 && is_Input() && is_Optional() )
	{
		m_Default	= Get_Children_Count();

		Get_Parameters()->Add_Double(Get_Identifier(), CSG_String::Format("%s_DEFAULT", Get_Identifier()),
			_TL("Default"), _TL("default value if no grid has been selected"),
			Value, Minimum, bMinimum, Maximum, bMaximum
		);
	}

	return( m_Default >= 0 );
}

//---------------------------------------------------------
int CSG_Parameter_Grid::_Set_Value(void *Value)
{
	if( Value == m_pDataObject )	// nothing to do
	{
		return( SG_PARAMETER_DATA_SET_TRUE );
	}

	//-----------------------------------------------------
	if( Value != DATAOBJECT_NOTSET && Value != DATAOBJECT_CREATE && Get_System() )// && Get_Manager()
	{
		CSG_Grid_System	System	= Get_Type() == PARAMETER_TYPE_Grid
			? ((CSG_Grid  *)Value)->Get_System()
			: ((CSG_Grids *)Value)->Get_System();

		if( !Get_System()->is_Equal(System) )
		{
			for(int i=0; i<Get_Parent()->Get_Children_Count(); i++)
			{
				CSG_Parameter	*pChild	= Get_Parent()->Get_Child(i);

				if( pChild->Get_Type() == PARAMETER_TYPE_Grid
				||  pChild->Get_Type() == PARAMETER_TYPE_Grids )
				{
					if( pChild->asDataObject() != DATAOBJECT_NOTSET
					&&  pChild->asDataObject() != DATAOBJECT_CREATE
					&&  pChild->asDataObject() != m_pDataObject )
					{
						return( false );
					}
				}

				if( pChild->is_DataObject_List() && pChild->asList()->Get_Item_Count() > 0 )
				{
					if( (pChild->Get_Type() == PARAMETER_TYPE_Grid_List  && pChild->asGridList ()->Get_System())
					||  (pChild->Get_Type() == PARAMETER_TYPE_Grids_List && pChild->asGridsList()->Get_System()) )
					{
						return( false );
					}
				}
			}

			Get_Parent()->Set_Value((void *)&System);
		}
	}

	//-----------------------------------------------------
	m_pDataObject	= (CSG_Data_Object *)Value;

	if( Get_Child(m_Default) )
	{
		Get_Child(m_Default)->Set_Enabled(m_pDataObject == DATAOBJECT_NOTSET);
	}

	return( SG_PARAMETER_DATA_SET_CHANGED );
}

//---------------------------------------------------------
int	CSG_Parameter_Grid::_asInt(void) const
{
	return( Get_Child(m_Default) ? Get_Child(m_Default)->asInt() : CSG_Parameter_Data_Object::asInt() );
}

//---------------------------------------------------------
double CSG_Parameter_Grid::_asDouble(void) const
{
	return( Get_Child(m_Default) ? Get_Child(m_Default)->asDouble() : CSG_Parameter_Data_Object::asDouble() );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid::_Assign(CSG_Parameter *pSource)
{
	m_Type		= ((CSG_Parameter_Grid *)pSource)->m_Type;
	m_Default	= ((CSG_Parameter_Grid *)pSource)->m_Default;

	return( _Set_Value(pSource->asPointer()) != 0 );
}


///////////////////////////////////////////////////////////
//														 //
//						Grids							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grids::CSG_Parameter_Grids(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Grid(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}


///////////////////////////////////////////////////////////
//														 //
//						Table							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Table::CSG_Parameter_Table(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Data_Object(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
int CSG_Parameter_Table::_Set_Value(void *Value)
{
	if( m_pDataObject == Value )
	{
		return( SG_PARAMETER_DATA_SET_TRUE );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	for(int i=0; i<Get_Children_Count(); i++)
	{
		CSG_Parameter	*pChild	= Get_Child(i);

		if( pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
		{
			pChild->Set_Value(m_pDataObject && pChild->is_Optional() ? ((CSG_Table *)m_pDataObject)->Get_Field_Count() : 0);
		}
		else if( pChild->Get_Type() == PARAMETER_TYPE_Table_Fields )
		{
			pChild->Set_Value(CSG_String(""));
		}
	}

	return( SG_PARAMETER_DATA_SET_CHANGED );
}


///////////////////////////////////////////////////////////
//														 //
//						Shapes							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Shapes::CSG_Parameter_Shapes(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Data_Object(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Type	= SHAPE_TYPE_Undefined;
}

//---------------------------------------------------------
int CSG_Parameter_Shapes::_Set_Value(void *Value)
{
	if(	Value != DATAOBJECT_NOTSET && Value != DATAOBJECT_CREATE
	&&	m_Type != SHAPE_TYPE_Undefined && m_Type != ((CSG_Shapes *)Value)->Get_Type() )
	{
		return( SG_PARAMETER_DATA_SET_FALSE );
	}

	if( m_pDataObject == Value )
	{
		return( SG_PARAMETER_DATA_SET_TRUE );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	for(int i=0; i<Get_Children_Count(); i++)
	{
		CSG_Parameter	*pChild	= Get_Child(i);

		if(	pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
		{
			pChild->Set_Value(m_pDataObject && m_pDataObject != DATAOBJECT_CREATE && pChild->is_Optional() ? ((CSG_Table *)m_pDataObject)->Get_Field_Count() : 0);
		}
		else if( pChild->Get_Type() == PARAMETER_TYPE_Table_Fields )
		{
			pChild->Set_Value("");
		}
	}

	return( SG_PARAMETER_DATA_SET_CHANGED );
}

//---------------------------------------------------------
void CSG_Parameter_Shapes::Set_Shape_Type(TSG_Shape_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
bool CSG_Parameter_Shapes::_Assign(CSG_Parameter *pSource)
{
	m_Type	= ((CSG_Parameter_Shapes *)pSource)->m_Type;

	return( CSG_Parameter_Data_Object::_Assign(pSource) );
}


///////////////////////////////////////////////////////////
//														 //
//						TIN								 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_TIN::CSG_Parameter_TIN(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Data_Object(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
int CSG_Parameter_TIN::_Set_Value(void *Value)
{
	if( m_pDataObject == Value )
	{
		return( SG_PARAMETER_DATA_SET_TRUE );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	for(int i=0; i<Get_Children_Count(); i++)
	{
		CSG_Parameter	*pChild	= Get_Child(i);

		if( pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
		{
			pChild->Set_Value(m_pDataObject && pChild->is_Optional() ? ((CSG_Table *)m_pDataObject)->Get_Field_Count() : 0);
		}
		else if( pChild->Get_Type() == PARAMETER_TYPE_Table_Fields )
		{
			pChild->Set_Value("");
		}
	}

	return( SG_PARAMETER_DATA_SET_CHANGED );
}


///////////////////////////////////////////////////////////
//														 //
//						PointCloud						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_PointCloud::CSG_Parameter_PointCloud(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_Data_Object(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
int CSG_Parameter_PointCloud::_Set_Value(void *Value)
{
	if( m_pDataObject == Value )
	{
		return( SG_PARAMETER_DATA_SET_TRUE );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	for(int i=0; i<Get_Children_Count(); i++)
	{
		CSG_Parameter	*pChild	= Get_Child(i);

		if( pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
		{
			pChild->Set_Value(m_pDataObject && pChild->is_Optional() ? ((CSG_Table *)m_pDataObject)->Get_Field_Count() : 0);
		}
		else if( pChild->Get_Type() == PARAMETER_TYPE_Table_Fields )
		{
			pChild->Set_Value("");
		}
	}

	return( SG_PARAMETER_DATA_SET_CHANGED );
}

//---------------------------------------------------------
bool CSG_Parameter_PointCloud::_Assign(CSG_Parameter *pSource)
{
	return( CSG_Parameter_Data_Object::_Assign(pSource) );
}


///////////////////////////////////////////////////////////
//														 //
//						List							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_List::CSG_Parameter_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
bool CSG_Parameter_List::Add_Item(CSG_Data_Object *pObject)
{
	if( pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE && m_Objects.Add(pObject) )
	{
		_Set_String();

		return( true );
	}

	return( false );
}

bool CSG_Parameter_List::Del_Item(CSG_Data_Object *pObject, bool bUpdateData)
{
	if( m_Objects.Del(pObject) > 0 )
	{
		if( bUpdateData )
		{
			_Set_String();
		}

		return( true );
	}

	return( false );
}

bool CSG_Parameter_List::Del_Item(int Index, bool bUpdateData)
{
	if( m_Objects.Del(Index) )
	{
		if( bUpdateData )
		{
			_Set_String();
		}

		return( true );
	}

	return( false );
}

bool CSG_Parameter_List::Del_Items(void)
{
	if( m_Objects.Set_Array(0) )
	{
		_Set_String();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Parameter_List::_Set_String(void)
{
	if( Get_Item_Count() > 0 )
	{
		m_String.Printf("%d %s (", Get_Item_Count(), Get_Item_Count() == 1 ? _TL("object") : _TL("objects"));

		for(int i=0; i<Get_Item_Count(); i++)
		{
			if( i > 0 )
			{
				m_String	+= ", ";
			}

			m_String	+= Get_Item(i)->Get_Name();
		}

		m_String	+= ")";
	}
	else
	{
		m_String	= _TL("No objects");
	}
}

//---------------------------------------------------------
int CSG_Parameter_List::_asInt(void)	const
{
	return( Get_Item_Count() );
}

//---------------------------------------------------------
void * CSG_Parameter_List::_asPointer(void)	const
{
	return( m_Objects.Get_Array() );
}

//---------------------------------------------------------
bool CSG_Parameter_List::_Assign(CSG_Parameter *pSource)
{
	Del_Items();

	for(int i=0; i<((CSG_Parameter_List *)pSource)->Get_Item_Count(); i++)
	{
		if( Get_Manager() != &SG_Get_Data_Manager() || SG_Get_Data_Manager().Exists(((CSG_Parameter_List *)pSource)->Get_Item(i)) )
		{
			Add_Item(((CSG_Parameter_List *)pSource)->Get_Item(i));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_List::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		for(int i=0; i<Get_Item_Count(); i++)
		{
			CSG_String	File	= Get_Item(i)->Get_File_Name(false);

			if( File.BeforeFirst(':').Cmp("PGSQL") || SG_File_Exists(File) )
			{
				Entry.Add_Child("DATA", File);
			}
		}
	}
	else
	{
		Del_Items();

		for(int i=0; i<Entry.Get_Children_Count(); i++)
		{
			CSG_Data_Object	*pObject	= Get_Manager() ? Get_Manager()->Find(Entry.Get_Content(i), false) : NULL;

			if( pObject )
			{
				Add_Item(pObject);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid_List						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grid_List::CSG_Parameter_Grid_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_List(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
CSG_Grid_System * CSG_Parameter_Grid_List::Get_System(void) const
{
	if( Get_Parent() && Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( Get_Parent()->asGrid_System() );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_List::Add_Item(CSG_Data_Object *pObject)
{
	if( pObject == DATAOBJECT_NOTSET || pObject == DATAOBJECT_CREATE
	|| (pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_Grid
	&&  pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_Grids)  )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid_System *pSystem = Get_System();

	if( pSystem )	// check grid system compatibility
	{
		CSG_Grid_System	System	= pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid
			? ((CSG_Grid  *)pObject)->Get_System()
			: ((CSG_Grids *)pObject)->Get_System();

		if( !pSystem->is_Valid() )
		{
			Get_Parent()->Set_Value((void *)&System);
		}
		else if( !pSystem->is_Equal(System) )
		{
			for(int i=0; i<Get_Parent()->Get_Children_Count(); i++)
			{
				CSG_Parameter	*pChild	= Get_Parent()->Get_Child(i);

				if( pChild->Get_Type() == PARAMETER_TYPE_Grid
				||  pChild->Get_Type() == PARAMETER_TYPE_Grids )
				{
					if( pChild->asDataObject() != DATAOBJECT_NOTSET
					&&  pChild->asDataObject() != DATAOBJECT_CREATE )
					{
						return( false );
					}
				}

				if( pChild->is_DataObject_List() && pChild->asList()->Get_Item_Count() > 0 )
				{
					if( (pChild->Get_Type() == PARAMETER_TYPE_Grid_List  && pChild->asGridList ()->Get_System())
					||  (pChild->Get_Type() == PARAMETER_TYPE_Grids_List && pChild->asGridsList()->Get_System()) )
					{
						return( false );
					}
				}
			}

			Get_Parent()->Set_Value((void *)&System);
		}
	}

	return( CSG_Parameter_List::Add_Item(pObject) && Update_Data() );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_List::Del_Item(CSG_Data_Object *pItem, bool bUpdateData)
{
	return( CSG_Parameter_List::Del_Item(pItem, bUpdateData) );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_List::Del_Item(int Index, bool bUpdateData)
{
	return( CSG_Parameter_List::Del_Item(Index, bUpdateData) );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_List::Del_Items(void)
{
	return( CSG_Parameter_List::Del_Items() && Update_Data() );
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_List::Update_Data(void)
{
	m_Grids.Set_Array(0);

	for(int i=0; i<Get_Item_Count(); i++)
	{
		switch( Get_Item(i)->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid: {
			m_Grids.Add(Get_Item(i));
			break; }

		case SG_DATAOBJECT_TYPE_Grids: {
			CSG_Grids	*pGrids	= (CSG_Grids *)Get_Item(i);

			for(int j=0; j<pGrids->Get_Grid_Count(); j++)
			{
				m_Grids.Add(pGrids->Get_Grid_Ptr(j));
			}
			break; }

		default: {
			break; }
		}

	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Grids_List						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grids_List::CSG_Parameter_Grids_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_List(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}

//---------------------------------------------------------
CSG_Grid_System * CSG_Parameter_Grids_List::Get_System(void) const
{
	if( Get_Parent() && Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( Get_Parent()->asGrid_System() );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Parameter_Grids_List::Add_Item(CSG_Data_Object *pObject)
{
	if( pObject == NULL || pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_Grids )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid_System *pSystem = Get_System();

	if( pSystem )	// check grid system compatibility
	{
		CSG_Grid_System	System	= Get_Type() == PARAMETER_TYPE_Grid_List
			? ((CSG_Grid  *)pObject)->Get_System()
			: ((CSG_Grids *)pObject)->Get_System();

		if( !pSystem->is_Valid() )
		{
			Get_Parent()->Set_Value((void *)&System);
		}
		else if( !pSystem->is_Equal(System) )
		{
			for(int i=0; i<Get_Parent()->Get_Children_Count(); i++)
			{
				CSG_Parameter	*pChild	= Get_Parent()->Get_Child(i);

				if( pChild->Get_Type() == PARAMETER_TYPE_Grids )
				{
					if( pChild->asDataObject() != DATAOBJECT_NOTSET
					&&  pChild->asDataObject() != DATAOBJECT_CREATE )
					{
						return( false );
					}
				}

				if( pChild->is_DataObject_List() && pChild->asList()->Get_Item_Count() > 0 )
				{
					if( (pChild->Get_Type() == PARAMETER_TYPE_Grids_List && pChild->asGridsList()->Get_System()) )
					{
						return( false );
					}
				}
			}

			Get_Parent()->Set_Value((void *)&System);
		}
	}

	return( CSG_Parameter_List::Add_Item(pObject) );
}


///////////////////////////////////////////////////////////
//														 //
//						Table_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_Table_List::CSG_Parameter_Table_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_List(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}


///////////////////////////////////////////////////////////
//														 //
//						Shapes_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_Shapes_List::CSG_Parameter_Shapes_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_List(pOwner, pParent, ID, Name, Description, Constraint)
{
	m_Type	= SHAPE_TYPE_Undefined;
}

//---------------------------------------------------------
void CSG_Parameter_Shapes_List::Set_Shape_Type(TSG_Shape_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
bool CSG_Parameter_Shapes_List::_Assign(CSG_Parameter *pSource)
{
	m_Type	= ((CSG_Parameter_Shapes_List *)pSource)->m_Type;

	return( CSG_Parameter_List::_Assign(pSource) );
}


///////////////////////////////////////////////////////////
//														 //
//						TIN_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_TIN_List::CSG_Parameter_TIN_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_List(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}


///////////////////////////////////////////////////////////
//														 //
//					PointCloud_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_PointCloud_List::CSG_Parameter_PointCloud_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter_List(pOwner, pParent, ID, Name, Description, Constraint)
{
	// nop
}


///////////////////////////////////////////////////////////
//														 //
//						Parameters						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Parameters::CSG_Parameter_Parameters(CSG_Parameters *pParameters, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	: CSG_Parameter(pParameters, pParent, ID, Name, Description, Constraint)
{
	m_pParameters	= new CSG_Parameters(pParameters->Get_Owner(), Name, Description, ID);
}

CSG_Parameter_Parameters::~CSG_Parameter_Parameters(void)
{
	delete(m_pParameters);
}

//---------------------------------------------------------
bool CSG_Parameter_Parameters::Restore_Default(void)
{
	return( m_pParameters->Restore_Defaults() );
}

//---------------------------------------------------------
void CSG_Parameter_Parameters::_Set_String(void)
{
	m_String.Printf("%d %s", m_pParameters->Get_Count(), _TL("parameters"));
}

//---------------------------------------------------------
void * CSG_Parameter_Parameters::_asPointer(void)	const
{
	return( m_pParameters );
}

//---------------------------------------------------------
bool CSG_Parameter_Parameters::_Assign(CSG_Parameter *pSource)
{
	m_pParameters->Assign(pSource->asParameters());

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Parameters::_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( m_pParameters->Serialize(Entry, bSave) )
	{
		if( bSave )
		{
			Entry.Set_Property("id"  , Get_Identifier     ());
			Entry.Set_Property("type", Get_Type_Identifier());
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
