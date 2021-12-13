
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
//                    parameters.cpp                     //
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
#include "tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameters::CSG_Parameters(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_Parameters::CSG_Parameters(const CSG_Parameters &Parameters)
{
	_On_Construction();

	Create(Parameters);
}

//---------------------------------------------------------
CSG_Parameters::CSG_Parameters(const SG_Char *Name, const SG_Char *Description, const SG_Char *Identifier, bool bGrid_System)
{
	_On_Construction();

	Create(Name, Description, Identifier, bGrid_System);
}

//---------------------------------------------------------
CSG_Parameters::CSG_Parameters(void *pOwner, const SG_Char *Name, const SG_Char *Description, const SG_Char *Identifier, bool bGrid_System)
{
	_On_Construction();

	Create(pOwner, Name, Description, Identifier, bGrid_System);
}

//---------------------------------------------------------
CSG_Parameters::~CSG_Parameters(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Parameters::_On_Construction(void)
{
	m_pOwner		= NULL;
	m_pTool			= NULL;
	m_pManager		= &SG_Get_Data_Manager();

	m_Parameters	= NULL;
	m_nParameters	= 0;

	m_Callback		= NULL;
	m_bCallback		= true;

	m_pGrid_System	= NULL;
}

//---------------------------------------------------------
bool CSG_Parameters::Create(const CSG_Parameters &Parameters)
{
	Destroy();

	m_pOwner		= Parameters.m_pOwner;
	m_pTool			= Parameters.m_pTool;
	m_pManager		= Parameters.m_pManager;

	m_Callback		= Parameters.m_Callback;
	m_bCallback		= Parameters.m_bCallback;

	Set_Identifier	(Parameters.Get_Identifier ());
	Set_Name		(Parameters.Get_Name       ());
	Set_Description	(Parameters.Get_Description());

	//-----------------------------------------------------
	for(int i=0; i<Parameters.m_nParameters; i++)
	{
		_Add(Parameters.m_Parameters[i]);
	}

	if( Parameters.m_pGrid_System )
	{
		m_pGrid_System	= Get_Parameter(Parameters.m_pGrid_System->Get_Identifier());
	}

	return( m_nParameters == Parameters.m_nParameters );
}

//---------------------------------------------------------
bool CSG_Parameters::Create(const SG_Char *Name, const SG_Char *Description, const SG_Char *Identifier, bool bGrid_System)
{
	Destroy();

	Set_Identifier (Identifier);
	Set_Name       (Name);
	Set_Description(Description ? Description : SG_T(""));

	if( bGrid_System )
	{
		Use_Grid_System();
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameters::Create(void *pOwner, const SG_Char *Name, const SG_Char *Description, const SG_Char *Identifier, bool bGrid_System)
{
	if( Create(Name, Description, Identifier, bGrid_System) )
	{
		m_pOwner	= pOwner;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Parameters::Destroy(void)
{
	m_pOwner		= NULL;
	m_pTool			= NULL;
	m_pGrid_System	= NULL;

	Del_Parameters();

	m_References.Clear();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Set the responsible data manager for this parameter list.
  * If it is not NULL, the parameter list will perform checks
  * beforehand tool executions, e.g. check if all data sets
  * in its parameter list are really loaded.
*/
//---------------------------------------------------------
void CSG_Parameters::Set_Manager(CSG_Data_Manager *pManager)
{
	m_pManager		= pManager;

	for(int i=0; i<Get_Count(); i++)
	{
		if( m_Parameters[i]->Get_Type() == PARAMETER_TYPE_Parameters )
		{
			m_Parameters[i]->asParameters()->Set_Manager(pManager);
		}
	}
}

//---------------------------------------------------------
/**
* Let parameters list provide a default grid system after construction.
*/
//---------------------------------------------------------
bool CSG_Parameters::Use_Grid_System(void)
{
	if( !m_pGrid_System )
	{
		m_pGrid_System	= Add_Grid_System("", "PARAMETERS_GRID_SYSTEM", _TL("Grid System"), _TL(""));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
/**
* If parameters are owned by a tool the function returns the
* tool's GUI mode, or the presence of a GUI frame otherwise.
*/
//---------------------------------------------------------
bool CSG_Parameters::has_GUI(void) const
{
	return( Get_Tool() ? Get_Tool()->has_GUI() : SG_UI_Get_Window_Main() != NULL );
}

//---------------------------------------------------------
/**
  * Change the identifier of this parameter list after construction.
*/
//---------------------------------------------------------
void CSG_Parameters::Set_Identifier(const CSG_String &Identifier)
{
	m_Identifier	= Identifier;
}

//---------------------------------------------------------
/**
  * Change the identifier of this parameter list after construction.
*/
//---------------------------------------------------------
bool CSG_Parameters::Cmp_Identifier(const CSG_String &Identifier)	const
{
	return( m_Identifier.Cmp(Identifier) == 0 );
}

//---------------------------------------------------------
/**
  * Change the name of this parameter list after construction.
*/
//---------------------------------------------------------
void CSG_Parameters::Set_Name(const CSG_String &Name)
{
	m_Name			= Name;
}

//---------------------------------------------------------
/**
  * Change the description for this parameter list after construction.
*/
//---------------------------------------------------------
void CSG_Parameters::Set_Description(const CSG_String &Description)
{
	m_Description	= Description;
}

//---------------------------------------------------------
/**
  * Add a reference to the list of references.
*/
//---------------------------------------------------------
void CSG_Parameters::Add_Reference(const CSG_String &Authors, const CSG_String &Year, const CSG_String &Title, const CSG_String &Where, const SG_Char *Link, const SG_Char *Link_Text)
{
	CSG_String	Reference	= Authors;

	Reference.Printf("<b>%s (%s):</b> %s. %s", Authors.c_str(), Year.c_str(), Title.c_str(), Where.c_str());

	if( Link && *Link )
	{
		Reference	+= CSG_String::Format(" <a href=\"%s\">%s</a>.", Link, Link_Text && *Link_Text ? Link_Text : Link);
	}

	if( !Reference.is_Empty() )
	{
		m_References	+= Reference;
	}

	m_References.Sort();
}

//---------------------------------------------------------
/**
* Add a reference to the list of references.
*/
//---------------------------------------------------------
void CSG_Parameters::Add_Reference(const CSG_String &Link, const SG_Char *Link_Text)
{
	m_References	+= CSG_String::Format("<a href=\"%s\">%s</a>", Link.c_str(), Link_Text && *Link_Text ? Link_Text : Link.c_str());

	m_References.Sort();
}

//---------------------------------------------------------
/**
* Delete all references.
*/
//---------------------------------------------------------
void CSG_Parameters::Del_References(void)
{
	m_References.Clear();
}

//---------------------------------------------------------
/**
  * Enable or disable all parameters.
*/
//---------------------------------------------------------
void CSG_Parameters::Set_Enabled(bool bEnabled)
{
	for(int i=0; i<m_nParameters; i++)
	{
		m_Parameters[i]->Set_Enabled(bEnabled);
	}
}

//---------------------------------------------------------
/**
  * Enable or disable parameter with given identifier.
*/
//---------------------------------------------------------
void CSG_Parameters::Set_Enabled(const CSG_String &Identifier, bool bEnabled)
{
	CSG_Parameter	*pParameter	= Get_Parameter(Identifier);

	if( pParameter )
	{
		pParameter->Set_Enabled(bEnabled);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Parameter(CSG_Parameter *pParameter)
{
	return( _Add(pParameter) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Node(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
{
	return( _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Node, PARAMETER_INFORMATION) );
}

//---------------------------------------------------------
/**
  * Following parameter types can be used:
  * PARAMETER_TYPE_Bool
  * PARAMETER_TYPE_Int
  * PARAMETER_TYPE_Double
  *	PARAMETER_TYPE_Degree
  * PARAMETER_TYPE_Color
*/
CSG_Parameter * CSG_Parameters::Add_Value(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	return( _Add_Value(ParentID, ID, Name, Description, false, Type, Value, Minimum, bMinimum, Maximum, bMaximum) );
}

CSG_Parameter * CSG_Parameters::Add_Info_Value(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, double Value)
{
	return( _Add_Value(ParentID, ID, Name, Description,  true, Type, Value, 0.0, false, 0.0, false) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Bool  (const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool   Value)
{
	return( Add_Value(ParentID, ID, Name, Description, PARAMETER_TYPE_Bool  , Value ? 1.0 : 0.0) );
}

CSG_Parameter * CSG_Parameters::Add_Int   (const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int    Value, int    Minimum, bool bMinimum, int    Maximum, bool bMaximum)
{
	return( Add_Value(ParentID, ID, Name, Description, PARAMETER_TYPE_Int   , Value, Minimum, bMinimum, Maximum, bMaximum) );
}

CSG_Parameter * CSG_Parameters::Add_Double(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	return( Add_Value(ParentID, ID, Name, Description, PARAMETER_TYPE_Double, Value, Minimum, bMinimum, Maximum, bMaximum) );
}

CSG_Parameter * CSG_Parameters::Add_Degree(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	return( Add_Value(ParentID, ID, Name, Description, PARAMETER_TYPE_Degree, Value, Minimum, bMinimum, Maximum, bMaximum) );
}

CSG_Parameter * CSG_Parameters::Add_Date  (const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value)	// Julian Day Number
{
	if( !Value )
	{
		Value	= CSG_DateTime::Now().Get_JDN();
	}

	return( Add_Value(ParentID, ID, Name, Description, PARAMETER_TYPE_Date  , Value) );
}

CSG_Parameter * CSG_Parameters::Add_Color (const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int    Value)
{
	return( Add_Value(ParentID, ID, Name, Description, PARAMETER_TYPE_Color , Value) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Range (const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Range_Min, double Range_Max, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	return( _Add_Range(ParentID, ID, Name, Description, false, Range_Min, Range_Max, Minimum, bMinimum, Maximum, bMaximum) );
}

CSG_Parameter * CSG_Parameters::Add_Info_Range(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Range_Min, double Range_Max)
{
	return( _Add_Range(ParentID, ID, Name, Description,  true, Range_Min, Range_Max, 0.0, false, 0.0, false) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Choice(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &Items, int Default)
{
	CSG_Parameter	*pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Choice, 0);

	pParameter->asChoice()->Set_Items(Items);

	bool	bCallback	= Set_Callback(false);
	pParameter->Set_Value  (Default);
	pParameter->Set_Default(Default);
	Set_Callback(bCallback);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Choices(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &Items)
{
	CSG_Parameter	*pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Choices, 0);

	pParameter->asChoices()->Set_Items(Items);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_String(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &String, bool bLongText, bool bPassword)
{
	return( _Add_String(ParentID, ID, Name, Description, false, String, bLongText, bPassword) );
}

CSG_Parameter * CSG_Parameters::Add_Info_String(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &String, bool bLongText)
{
	return( _Add_String(ParentID, ID, Name, Description,  true, String, bLongText, false) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_FilePath(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const SG_Char *Filter, const SG_Char *Default, bool bSave, bool bDirectory, bool bMultiple)
{
	CSG_Parameter	*pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_FilePath, 0);

	pParameter->asFilePath()->Set_Filter        (Filter    );
	pParameter->asFilePath()->Set_Flag_Save     (bSave     );
	pParameter->asFilePath()->Set_Flag_Multiple (bMultiple );
	pParameter->asFilePath()->Set_Flag_Directory(bDirectory);

	bool	bCallback	= Set_Callback(false);
	pParameter->Set_Value  (Default);
	pParameter->Set_Default(Default);
	Set_Callback(bCallback);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Font(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const SG_Char *pInit)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Font, 0);

	if( pInit && *pInit )
	{
		bool	bCallback	= Set_Callback(false);
		pParameter->Set_Value  (pInit);
		pParameter->Set_Default(pInit);
		Set_Callback(bCallback);
	}

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Colors(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, CSG_Colors *pInit)
{
	CSG_Parameter			*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Colors, 0);

	pParameter->asColors()->Assign(pInit);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_FixedTable(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, CSG_Table *pTemplate)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_FixedTable, 0);

	pParameter->asTable()->Create(pTemplate);
	pParameter->asTable()->Set_Name(Name);
	pParameter->asTable()->Assign_Values(pTemplate);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grid_System(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, CSG_Grid_System *pInit)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Grid_System, 0);

	if( pInit )
	{
		pParameter->asGrid_System()->Assign(*pInit);
	}

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grid(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent, TSG_Data_Type Preferred_Type)
{
	CSG_Parameter	*pParameter, *pParent = Get_Parameter(ParentID);

	CSG_String	SystemID;

	if( pParent && pParent->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		SystemID	= pParent->Get_Identifier();
	}
	else if( bSystem_Dependent && m_pGrid_System )
	{
		SystemID	= m_pGrid_System->Get_Identifier();
	}
	else
	{
		pParent		= Add_Grid_System(pParent ? pParent->Get_Identifier() : SG_T(""), ID + "_GRIDSYSTEM", _TL("Grid system"), "");
		SystemID	= pParent->Get_Identifier();
	}

	pParameter	= _Add(SystemID, ID, Name, Description, PARAMETER_TYPE_Grid, Constraint);

	((CSG_Parameter_Grid *)pParameter)->Set_Preferred_Type(Preferred_Type);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grid_or_Const(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum, bool bSystem_Dependent)
{
	CSG_Parameter	*pParameter	= Add_Grid(ParentID, ID, Name, Description, PARAMETER_INPUT_OPTIONAL, bSystem_Dependent);

	((CSG_Parameter_Grid *)pParameter)->Add_Default(Value, Minimum, bMinimum, Maximum, bMaximum);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grid_Output(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter)->Set_DataObject_Type(SG_DATAOBJECT_TYPE_Grid);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grid_List(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent)
{
	CSG_Parameter	*pParameter, *pParent = Get_Parameter(ParentID);

	CSG_String	SystemID;

	if( pParent && pParent->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		SystemID	= pParent->Get_Identifier();
	}
	else if( bSystem_Dependent && m_pGrid_System && !((Constraint & PARAMETER_OUTPUT) && (Constraint & PARAMETER_OPTIONAL)) )
	{
		SystemID	= m_pGrid_System->Get_Identifier();
	}

	pParameter	= _Add(SystemID, ID, Name, Description, PARAMETER_TYPE_Grid_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grids(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent, TSG_Data_Type Preferred_Type)
{
	CSG_Parameter	*pParameter, *pParent = Get_Parameter(ParentID);

	CSG_String	SystemID;

	if( pParent && pParent->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		SystemID	= pParent->Get_Identifier();
	}
	else if( bSystem_Dependent && m_pGrid_System )
	{
		SystemID	= m_pGrid_System->Get_Identifier();
	}
	else
	{
		pParent		= Add_Grid_System(pParent ? pParent->Get_Identifier() : SG_T(""), ID + "_GRIDSYSTEM", _TL("Grid system"), "");
		SystemID	= pParent->Get_Identifier();
	}

	pParameter	= _Add(SystemID, ID, Name, Description, PARAMETER_TYPE_Grids, Constraint);

	((CSG_Parameter_Grids *)pParameter)->Set_Preferred_Type(Preferred_Type);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grids_Output(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter)->Set_DataObject_Type(SG_DATAOBJECT_TYPE_Grids);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grids_List(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent)
{
	CSG_Parameter	*pParameter, *pParent = Get_Parameter(ParentID);

	CSG_String	SystemID;

	if( pParent && pParent->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		SystemID	= pParent->Get_Identifier();
	}
	else if( bSystem_Dependent && m_pGrid_System && !((Constraint & PARAMETER_OUTPUT) && (Constraint & PARAMETER_OPTIONAL)) )
	{
		SystemID	= m_pGrid_System->Get_Identifier();
	}

	pParameter	= _Add(SystemID, ID, Name, Description, PARAMETER_TYPE_Grids_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table_Field(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool bAllowNone)
{
	CSG_Parameter	*pParent = Get_Parameter(ParentID);

	if( pParent && (
		pParent->Get_Type() == PARAMETER_TYPE_Table
	||	pParent->Get_Type() == PARAMETER_TYPE_Shapes
	||	pParent->Get_Type() == PARAMETER_TYPE_TIN
	||	pParent->Get_Type() == PARAMETER_TYPE_PointCloud) )
	{
		return( _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Table_Field, bAllowNone ? PARAMETER_OPTIONAL : 0) );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table_Field_or_Const(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	CSG_Parameter	*pParameter	= Add_Table_Field(ParentID, ID, Name, Description, true);

	if( pParameter )
	{
		((CSG_Parameter_Table_Field *)pParameter)->Add_Default(Value, Minimum, bMinimum, Maximum, bMaximum);
	}

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table_Fields(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParent = Get_Parameter(ParentID);

	if( pParent && (
		pParent->Get_Type() == PARAMETER_TYPE_Table
	||	pParent->Get_Type() == PARAMETER_TYPE_Shapes
	||	pParent->Get_Type() == PARAMETER_TYPE_TIN
	||	pParent->Get_Type() == PARAMETER_TYPE_PointCloud) )
	{
		return( _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Table_Fields, 0) );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Table, Constraint);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table_Output(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter)->Set_DataObject_Type(SG_DATAOBJECT_TYPE_Table);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table_List(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Table_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Shapes(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, TSG_Shape_Type Shape_Type)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Shapes, Constraint);

	((CSG_Parameter_Shapes *)pParameter)->Set_Shape_Type(Shape_Type);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Shapes_Output(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter)->Set_DataObject_Type(SG_DATAOBJECT_TYPE_Shapes);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Shapes_List(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, TSG_Shape_Type Type)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Shapes_List, Constraint);

	((CSG_Parameter_Shapes_List *)pParameter)->Set_Shape_Type(Type);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_TIN(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_TIN, Constraint);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_TIN_Output(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter)->Set_DataObject_Type(SG_DATAOBJECT_TYPE_TIN);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_TIN_List(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_TIN_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_PointCloud(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_PointCloud, Constraint);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_PointCloud_Output(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter)->Set_DataObject_Type(SG_DATAOBJECT_TYPE_PointCloud);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_PointCloud_List(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_PointCloud_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Parameters(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Parameters, 0);

	pParameter->asParameters()->m_Callback = m_Callback;
	pParameter->asParameters()->m_pTool    = m_pTool;

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::_Add_Value(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool bInformation, TSG_Parameter_Type Type, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	switch( Type )	// Check if Type is valid...
	{
	case PARAMETER_TYPE_Bool  :
	case PARAMETER_TYPE_Int   :
	case PARAMETER_TYPE_Double:
	case PARAMETER_TYPE_Degree:
	case PARAMETER_TYPE_Date  :
	case PARAMETER_TYPE_Color :
		break;

	default:	// if not valid set Type to [double]...
		Type	= PARAMETER_TYPE_Double;
	}

	CSG_Parameter	*pParameter	= _Add(ParentID, ID, Name, Description, Type, bInformation ? PARAMETER_INFORMATION : 0);

	bool	bCallback	= Set_Callback(false);

	if( !bInformation )
	{
		if( Type == PARAMETER_TYPE_Int
		||  Type == PARAMETER_TYPE_Double
		||  Type == PARAMETER_TYPE_Degree )
		{
			pParameter->asValue()->Set_Minimum(Minimum, bMinimum);
			pParameter->asValue()->Set_Maximum(Maximum, bMaximum);
		}
	}

	pParameter->Set_Value(Value);

	Set_Callback(bCallback);

	if( !bInformation )
	{
		switch( Type )
		{
		case PARAMETER_TYPE_Bool  :
		case PARAMETER_TYPE_Int   :
		case PARAMETER_TYPE_Color :
			pParameter->Set_Default((int)Value);
			break;

		case PARAMETER_TYPE_Date  :
			pParameter->Set_Default(pParameter->asString());
			break;

		default:
			pParameter->Set_Default(     Value);
		}
	}

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::_Add_Range(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool bInformation, double Default_Min, double Default_Max, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	//-----------------------------------------------------
	if( Default_Min > Default_Max )
	{
		double	d	= Default_Min;
		Default_Min	= Default_Max;
		Default_Max	= d;
	}

	//-----------------------------------------------------
	CSG_Parameter	*pParameter	= _Add(ParentID, ID, Name, Description, PARAMETER_TYPE_Range, bInformation ? PARAMETER_INFORMATION : 0);

	pParameter->asRange()->Get_Min_Parameter()->Set_Minimum(Minimum, bMinimum);
	pParameter->asRange()->Get_Min_Parameter()->Set_Maximum(Maximum, bMaximum);
	pParameter->asRange()->Get_Min_Parameter()->Set_Default(Default_Min);
	pParameter->asRange()->Set_Min(Default_Min);

	pParameter->asRange()->Get_Max_Parameter()->Set_Minimum(Minimum, bMinimum);
	pParameter->asRange()->Get_Max_Parameter()->Set_Maximum(Maximum, bMaximum);
	pParameter->asRange()->Get_Max_Parameter()->Set_Default(Default_Max);
	pParameter->asRange()->Set_Max(Default_Max);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::_Add_String(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool bInformation, const SG_Char *String, bool bLongText, bool bPassword)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(ParentID, ID, Name, Description, bLongText ? PARAMETER_TYPE_Text : PARAMETER_TYPE_String, bInformation ? PARAMETER_INFORMATION : 0);

	bool	bCallback	= Set_Callback(false);
	pParameter->Set_Value  (String);
	pParameter->Set_Default(String);
	Set_Callback(bCallback);

	((CSG_Parameter_String *)pParameter)->Set_Password(bPassword);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/debug.h>

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::_Add(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, int Constraint)
{
	wxASSERT_MSG(!ID.is_Empty(), "CSG_Parameter::Add: ID is empty");

	CSG_Parameter	*pParameter;

	switch( Type )
	{
	default:
		return( NULL );

	case PARAMETER_TYPE_Node             : pParameter	= new CSG_Parameter_Node              (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;

	case PARAMETER_TYPE_Bool             : pParameter	= new CSG_Parameter_Bool              (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Int              : pParameter	= new CSG_Parameter_Int               (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Double           : pParameter	= new CSG_Parameter_Double            (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Degree           : pParameter	= new CSG_Parameter_Degree            (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Date             : pParameter	= new CSG_Parameter_Date              (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Range            : pParameter	= new CSG_Parameter_Range             (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Choice           : pParameter	= new CSG_Parameter_Choice            (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Choices          : pParameter	= new CSG_Parameter_Choices           (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;

	case PARAMETER_TYPE_String           : pParameter	= new CSG_Parameter_String            (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Text             : pParameter	= new CSG_Parameter_Text              (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_FilePath         : pParameter	= new CSG_Parameter_File_Name         (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;

	case PARAMETER_TYPE_Font             : pParameter	= new CSG_Parameter_Font              (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Color            : pParameter	= new CSG_Parameter_Color             (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Colors           : pParameter	= new CSG_Parameter_Colors            (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_FixedTable       : pParameter	= new CSG_Parameter_Fixed_Table       (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Grid_System      : pParameter	= new CSG_Parameter_Grid_System       (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Table_Field      : pParameter	= new CSG_Parameter_Table_Field       (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Table_Fields     : pParameter	= new CSG_Parameter_Table_Fields      (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;

	case PARAMETER_TYPE_DataObject_Output: pParameter	= new CSG_Parameter_Data_Object_Output(this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Grid             : pParameter	= new CSG_Parameter_Grid              (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Grids            : pParameter	= new CSG_Parameter_Grids             (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Table            : pParameter	= new CSG_Parameter_Table             (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Shapes           : pParameter	= new CSG_Parameter_Shapes            (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_TIN              : pParameter	= new CSG_Parameter_TIN               (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_PointCloud       : pParameter	= new CSG_Parameter_PointCloud        (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;

	case PARAMETER_TYPE_Grid_List        : pParameter	= new CSG_Parameter_Grid_List         (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Grids_List       : pParameter	= new CSG_Parameter_Grids_List        (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Table_List       : pParameter	= new CSG_Parameter_Table_List        (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_Shapes_List      : pParameter	= new CSG_Parameter_Shapes_List       (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_TIN_List         : pParameter	= new CSG_Parameter_TIN_List          (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	case PARAMETER_TYPE_PointCloud_List  : pParameter	= new CSG_Parameter_PointCloud_List   (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;

	case PARAMETER_TYPE_Parameters       : pParameter	= new CSG_Parameter_Parameters        (this, Get_Parameter(ParentID), ID, Name, Description, Constraint);	break;
	}

	m_Parameters	= (CSG_Parameter **)SG_Realloc(m_Parameters, (m_nParameters + 1) * sizeof(CSG_Parameter *));
	m_Parameters[m_nParameters++]	= pParameter;

	pParameter->_Set_String();

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::_Add(CSG_Parameter *pSource)
{
	CSG_Parameter	*pParameter	= !pSource ? NULL : _Add(
		pSource->Get_Parent() ? pSource->Get_Parent()->Get_Identifier() : SG_T(""),
		pSource->Get_Identifier (),
		pSource->Get_Name       (),
		pSource->Get_Description(),
		pSource->Get_Type       (),
		pSource->m_Constraint
	);

	if( pParameter )
	{
		pParameter->Assign(pSource);
	}

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Get_Parameter(const CSG_String &ID)	const
{
	if( m_Parameters && !ID.is_Empty() )
	{
		for(int i=0; i<m_nParameters; i++)
		{
			if( m_Parameters[i]->Cmp_Identifier(ID) )
			{
				return( m_Parameters[i] );
			}
		}

		//-------------------------------------------------
		if( ID.Find('.') > 0 )	// id not found? check for sub-parameter ('id.id')!
		{
			CSG_Parameter	*pParameter	= Get_Parameter(ID.BeforeFirst('.'));

			if( pParameter )
			{
				switch( pParameter->Get_Type()  )
				{
				case PARAMETER_TYPE_Parameters:
					return( pParameter->asParameters()->Get_Parameter(ID.AfterFirst('.')) );

				case PARAMETER_TYPE_Range     :
					if( !ID.AfterFirst('.').CmpNoCase("min") || !ID.AfterFirst('.').CmpNoCase("minimum") )
					{
						return( pParameter->asRange()->Get_Min_Parameter() );
					}

					if( !ID.AfterFirst('.').CmpNoCase("max") || !ID.AfterFirst('.').CmpNoCase("maximum") )
					{
						return( pParameter->asRange()->Get_Max_Parameter() );
					}
					break;

				default:
					break;
				}
			}
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Parameters::Del_Parameter(int iParameter)
{
	if( m_Parameters && iParameter >= 0 && iParameter < m_nParameters )
	{
		CSG_Parameter	*pParameter	= m_Parameters[iParameter];

		for(m_nParameters--; iParameter<m_nParameters; iParameter++)
		{
			m_Parameters[iParameter]	= m_Parameters[iParameter + 1];
		}

		m_Parameters	= (CSG_Parameter **)SG_Realloc(m_Parameters, m_nParameters * sizeof(CSG_Parameter *));

		for(iParameter=pParameter->Get_Children_Count()-1; iParameter>=0; iParameter--)
		{
			Del_Parameter(pParameter->Get_Child(iParameter)->Get_Identifier());
		}

		CSG_Parameter	*pParent	= pParameter->Get_Parent();

		if( pParent )
		{
			for(iParameter=0; iParameter<pParent->m_nChildren; iParameter++)
			{
				if( pParent->m_Children[iParameter] == pParameter )
				{
					pParent->m_nChildren--;

					for( ; iParameter<pParent->m_nChildren; iParameter++)
					{
						pParent->m_Children[iParameter]	= pParent->m_Children[iParameter + 1];
					}
				}
			}

			pParent->m_Children	= (CSG_Parameter **)SG_Realloc(pParent->m_Children, pParent->m_nChildren * sizeof(CSG_Parameter *));
		}

		delete(pParameter);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameters::Del_Parameter(const CSG_String &Identifier)
{
	if( m_Parameters && Identifier.Length() )
	{
		for(int i=0; i<m_nParameters; i++)
		{
			if( !m_Parameters[i]->m_Identifier.Cmp(Identifier) )
			{
				return( Del_Parameter(i) );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameters::Del_Parameters(void)
{
	if( m_nParameters > 0 )
	{
		for(int i=0; i<m_nParameters; i++)
		{
			delete(m_Parameters[i]);
		}

		SG_Free(m_Parameters);

		m_Parameters	= NULL;
		m_nParameters	= 0;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Callback						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Callback function used to react on parameter changes.
// Return value is the previously set callback function.
TSG_PFNC_Parameter_Changed CSG_Parameters::Set_Callback_On_Parameter_Changed(TSG_PFNC_Parameter_Changed Callback)
{
	TSG_PFNC_Parameter_Changed	Previous	= m_Callback;

	m_Callback	= Callback;

	for(int i=0; i<m_nParameters; i++)
	{
		if( m_Parameters[i]->Get_Type() == PARAMETER_TYPE_Parameters)
		{
			m_Parameters[i]->asParameters()->Set_Callback_On_Parameter_Changed(Callback);
		}
	}

	return( Previous );
}

//---------------------------------------------------------
// If switched off parameter changes will not invoke a
// consecutive call to the On_Parameter_Changed function.
// Return value is the previous state.
bool CSG_Parameters::Set_Callback(bool bActive)
{
	bool	bPrevious	= m_bCallback;

	m_bCallback	= bActive;

	for(int i=0; i<m_nParameters; i++)
	{
		if( m_Parameters[i]->Get_Type() == PARAMETER_TYPE_Parameters)
		{
			m_Parameters[i]->asParameters()->Set_Callback(bActive);
		}
	}

	return( bPrevious );
}

//---------------------------------------------------------
bool CSG_Parameters::_On_Parameter_Changed(CSG_Parameter *pParameter, int Flags)
{
	if( m_Callback && m_bCallback )
	{
		bool	bCallback	= Set_Callback(false);

		m_Callback(pParameter, Flags);

		Set_Callback(bCallback);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const char       *ID, CSG_Parameter *pValue)	{	return( Set_Parameter(CSG_String(ID), pValue) );	}
bool CSG_Parameters::Set_Parameter(const wchar_t    *ID, CSG_Parameter *pValue)	{	return( Set_Parameter(CSG_String(ID), pValue) );	}
bool CSG_Parameters::Set_Parameter(const CSG_String &ID, CSG_Parameter *pValue)
{
	CSG_Parameter	*pTarget	= Get_Parameter(ID);

	return( pTarget && pValue && pTarget->Get_Type() == pValue->Get_Type() && pTarget->Assign(pValue) );
}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const char       *ID, void *Value, int Type)	{	return( Set_Parameter(CSG_String(ID), Value, Type) );	}
bool CSG_Parameters::Set_Parameter(const wchar_t    *ID, void *Value, int Type)	{	return( Set_Parameter(CSG_String(ID), Value, Type) );	}
bool CSG_Parameters::Set_Parameter(const CSG_String &ID, void *Value, int Type)
{
	CSG_Parameter	*pTarget	= Get_Parameter(ID);

	return( pTarget && (Type == PARAMETER_TYPE_Undefined || Type == pTarget->Get_Type()) && pTarget->Set_Value(Value) );
}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const char       *ID, CSG_Data_Object *Value, int Type)	{	return( Set_Parameter(CSG_String(ID), Value, Type) );	}
bool CSG_Parameters::Set_Parameter(const wchar_t    *ID, CSG_Data_Object *Value, int Type)	{	return( Set_Parameter(CSG_String(ID), Value, Type) );	}
bool CSG_Parameters::Set_Parameter(const CSG_String &ID, CSG_Data_Object *Value, int Type)
{
	CSG_Parameter	*pTarget	= Get_Parameter(ID);

	return( pTarget && (Type == PARAMETER_TYPE_Undefined || Type == pTarget->Get_Type()) && pTarget->Set_Value(Value) );
}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const char       *ID, int Value, int Type)	{	return( Set_Parameter(CSG_String(ID), Value, Type) );	}
bool CSG_Parameters::Set_Parameter(const wchar_t    *ID, int Value, int Type)	{	return( Set_Parameter(CSG_String(ID), Value, Type) );	}
bool CSG_Parameters::Set_Parameter(const CSG_String &ID, int Value, int Type)
{
	CSG_Parameter	*pTarget	= Get_Parameter(ID);

	return( pTarget && (Type == PARAMETER_TYPE_Undefined || Type == pTarget->Get_Type()) && pTarget->Set_Value(Value) );
}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const char       *ID, double Value, int Type)	{	return( Set_Parameter(CSG_String(ID), Value, Type) );	}
bool CSG_Parameters::Set_Parameter(const wchar_t    *ID, double Value, int Type)	{	return( Set_Parameter(CSG_String(ID), Value, Type) );	}
bool CSG_Parameters::Set_Parameter(const CSG_String &ID, double Value, int Type)
{
	CSG_Parameter	*pTarget	= Get_Parameter(ID);

	return( pTarget && (Type == PARAMETER_TYPE_Undefined || Type == pTarget->Get_Type()) && pTarget->Set_Value(Value) );
}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const char       *ID, const CSG_String &Value, int Type)	{	return( Set_Parameter(CSG_String(ID), Value, Type) );	}
bool CSG_Parameters::Set_Parameter(const wchar_t    *ID, const CSG_String &Value, int Type)	{	return( Set_Parameter(CSG_String(ID), Value, Type) );	}
bool CSG_Parameters::Set_Parameter(const CSG_String &ID, const CSG_String &Value, int Type)
{
	CSG_Parameter	*pTarget	= Get_Parameter(ID);

	return( pTarget && (Type == PARAMETER_TYPE_Undefined || Type == pTarget->Get_Type()) && pTarget->Set_Value(Value) );
}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const CSG_String &ID, const char    *Value, int Type)	{	return( Set_Parameter(ID, CSG_String(Value)) );	}
bool CSG_Parameters::Set_Parameter(const char       *ID, const char    *Value, int Type)	{	return( Set_Parameter(ID, CSG_String(Value)) );	}
bool CSG_Parameters::Set_Parameter(const wchar_t    *ID, const char    *Value, int Type)	{	return( Set_Parameter(ID, CSG_String(Value)) );	}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const CSG_String &ID, const wchar_t *Value, int Type)	{	return( Set_Parameter(ID, CSG_String(Value)) );	}
bool CSG_Parameters::Set_Parameter(const char       *ID, const wchar_t *Value, int Type)	{	return( Set_Parameter(ID, CSG_String(Value)) );	}
bool CSG_Parameters::Set_Parameter(const wchar_t    *ID, const wchar_t *Value, int Type)	{	return( Set_Parameter(ID, CSG_String(Value)) );	}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameters::Restore_Defaults(bool bClearData)
{
	Set_Callback(false);

	for(int i=0; i<Get_Count(); i++)
	{
		m_Parameters[i]->Restore_Default();

		if( bClearData )
		{
			if( m_Parameters[i]->is_DataObject() )
			{
				m_Parameters[i]->Set_Value(DATAOBJECT_NOTSET);
			}
			else if( m_Parameters[i]->is_DataObject_List() )
			{
				m_Parameters[i]->asList()->Del_Items();
			}
		}
	}

	Set_Callback(true);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameters::Assign(CSG_Parameters *pSource)
{
	return( pSource && pSource != this && Create(*pSource) );
}

//---------------------------------------------------------
bool CSG_Parameters::Assign_Values(CSG_Parameters *pSource)
{
	if( !pSource || pSource == this )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		i, n;

	for(i=0, n=0; i<pSource->Get_Count(); i++)
	{
		CSG_Parameter	*pParameter	= Get_Parameter(pSource->Get_Parameter(i)->Get_Identifier());

		if( pParameter && pParameter->Get_Type() == pSource->Get_Parameter(i)->Get_Type() )
		{
			pParameter->Assign(pSource->Get_Parameter(i));
			n++;
		}
	}

	return( n > 0 );
}

//---------------------------------------------------------
bool CSG_Parameters::Assign_Parameters(CSG_Parameters *pSource)
{
	if( !pSource || pSource == this )
	{
		return( false );
	}

	Del_Parameters();

	//-----------------------------------------------------
	for(int i=0; i<pSource->m_nParameters; i++)
	{
		CSG_Parameter	*pParameter	= pSource->m_Parameters[i];

		if( pParameter->Get_Type() == PARAMETER_TYPE_Parameters )
		{
			Add_Parameters("",
				pParameter->Get_Identifier (),
				pParameter->Get_Name       (),
				pParameter->Get_Description()
			)->asParameters()->Assign_Parameters(pParameter->asParameters());
		}
		else
		{
			_Add(pParameter);
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<pSource->m_nParameters; i++)
	{
		if( Get_Parameter(i) && pSource->m_Parameters[i]->m_pParent )
		{
			Get_Parameter(i)->m_pParent	= Get_Parameter(pSource->m_Parameters[i]->m_pParent->Get_Identifier());
		}
	}

	if( pSource->m_pGrid_System )
	{
		m_pGrid_System	= Get_Parameter(pSource->m_pGrid_System->Get_Identifier());
	}

	return( m_nParameters == pSource->m_nParameters );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameters::DataObjects_Check(bool bSilent)
{
	bool		bResult	= true;

	CSG_String	sError;

	//-----------------------------------------------------
	for(int i=0; i<Get_Count(); i++)
	{
		if( m_Parameters[i]->Check(bSilent) == false )
		{
			bResult	= false;

			sError.Append(CSG_String::Format("\n%s: %s", m_Parameters[i]->Get_Type_Name().c_str(), m_Parameters[i]->Get_Name()));
		}
	}

	//-----------------------------------------------------
	if( !bResult && !bSilent )
	{
		SG_UI_Dlg_Message(CSG_String::Format("%s\n%s", _TL("invalid input!"), sError.c_str()), Get_Name() );
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Parameters::DataObjects_Create(void)
{
	bool	bResult	= true;

	for(int i=0; i<Get_Count() && bResult; i++)
	{
		CSG_Parameter	*p	= m_Parameters[i];

		//-------------------------------------------------
		if( p->Get_Type() == PARAMETER_TYPE_Parameters )
		{
			bResult	= p->asParameters()->DataObjects_Create();
		}
		else if( p->Get_Type() == PARAMETER_TYPE_DataObject_Output )
		{
			if( m_pManager || p->asDataObject() == DATAOBJECT_CREATE )
			{
				p->Set_Value(DATAOBJECT_NOTSET);
			}
		}
		else if( p->is_Input() )
		{
			bResult	= p->Check(true);
		}

		//-------------------------------------------------
		else if( p->is_DataObject_List() )
		{
			for(int j=p->asList()->Get_Item_Count()-1; j>=0; j--)
			{
				if( m_pManager && !m_pManager->Exists(p->asList()->Get_Item(j)) )
				{
					p->asList()->Del_Item(j);
				}
			}
		}

		//-------------------------------------------------
		else if( p->is_DataObject() && p->is_Enabled() == false )
		{
			if( p->asDataObject() != DATAOBJECT_CREATE && (!m_pManager || !m_pManager->Exists(p->asDataObject())) )
			{
				p->Set_Value(DATAOBJECT_NOTSET);
			}
		}

		else if( p->is_DataObject() )
		{
			CSG_Data_Object	*pDataObject	= p->asDataObject();

			if(	(pDataObject == DATAOBJECT_CREATE)
			||	(pDataObject == DATAOBJECT_NOTSET && !p->is_Optional())
			||	(pDataObject != DATAOBJECT_NOTSET && m_pManager && !m_pManager->Exists(pDataObject)) )
			{
				pDataObject	= NULL;

				switch( p->Get_Type() )
				{
				case PARAMETER_TYPE_Table     :	pDataObject	= SG_Create_Table     ();	break;
				case PARAMETER_TYPE_TIN       :	pDataObject	= SG_Create_TIN       ();	break;
				case PARAMETER_TYPE_PointCloud:	pDataObject	= SG_Create_PointCloud();	break;
				case PARAMETER_TYPE_Shapes    :	pDataObject	= SG_Create_Shapes(
						((CSG_Parameter_Shapes *)p)->Get_Shape_Type()
					);
					break;

				case PARAMETER_TYPE_Grid      :
				case PARAMETER_TYPE_Grids     :
					if(	p->Get_Parent() && p->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System 
					&&	p->Get_Parent()->asGrid_System() && p->Get_Parent()->asGrid_System()->is_Valid() )
					{
						if( p->Get_Type() == PARAMETER_TYPE_Grid )
						{
							pDataObject	= SG_Create_Grid(*p->Get_Parent()->asGrid_System(), ((CSG_Parameter_Grid *)p)->Get_Preferred_Type());
						}
						else
						{
							pDataObject	= SG_Create_Grids(*p->Get_Parent()->asGrid_System(), 0, 0.0, ((CSG_Parameter_Grids *)p)->Get_Preferred_Type());
						}
					}
					break;

				default:
					break;
				}
			}
			else if( p->Get_Type() == PARAMETER_TYPE_Shapes && p->asShapes() )
			{
				if( ((CSG_Parameter_Shapes *)p)->Get_Shape_Type() != SHAPE_TYPE_Undefined
				&&	((CSG_Parameter_Shapes *)p)->Get_Shape_Type() != p->asShapes()->Get_Type() )
				{
					pDataObject	= SG_Create_Shapes(((CSG_Parameter_Shapes *)p)->Get_Shape_Type());
				}
			}

			if( pDataObject )
			{
				if( p->Set_Value(pDataObject) )
				{
					pDataObject->Set_Name(p->Get_Name());

					if( m_pManager )
					{
						m_pManager->Add(pDataObject);
					}
				}
				else
				{
					delete(pDataObject);

					bResult	= false;
				}
			}
			else
			{
				bResult	= p->is_Optional();
			}
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Parameters::DataObjects_Synchronize(void)
{
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Parameter	*p	= m_Parameters[i];

		if( p->Get_Type() == PARAMETER_TYPE_Parameters )
		{
			p->asParameters()->DataObjects_Synchronize();
		}

		//-------------------------------------------------
		else if( p->is_Output() )
		{
			if( p->is_DataObject() )
			{
				CSG_Data_Object	*pObject	= p->asDataObject();

				if( pObject == DATAOBJECT_CREATE )
				{
					p->Set_Value(DATAOBJECT_NOTSET);
				}
				else if( pObject != DATAOBJECT_NOTSET )
				{
					if( pObject->asShapes() && pObject->asShapes()->Get_Type() == SHAPE_TYPE_Undefined
					&&  (m_pManager == &SG_Get_Data_Manager() || !SG_Get_Data_Manager().Exists(pObject)) )
					{
						if( m_pManager && !m_pManager->Delete(pObject) )
						{
							delete(pObject);
						}

						p->Set_Value(DATAOBJECT_NOTSET);
					}
					else
					{
						if( m_pManager && !m_pManager->Exists(pObject) )
						{
							m_pManager->Add(pObject);
						}

						SG_UI_DataObject_Update(pObject, SG_UI_DATAOBJECT_UPDATE_ONLY, NULL);
					}
				}
			}

			//---------------------------------------------
			else if( p->is_DataObject_List() )
			{
				for(int j=0; j<p->asList()->Get_Item_Count(); j++)
				{
					CSG_Data_Object	*pObject	= p->asList()->Get_Item(j);

					if( m_pManager && !m_pManager->Exists(pObject) )
					{
						m_pManager->Add(pObject);
					}

					SG_UI_DataObject_Update(pObject, SG_UI_DATAOBJECT_UPDATE_ONLY, NULL);
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameters::DataObjects_Get_Projection(CSG_Projection &Projection)	const
{
	for(int i=0; i<Get_Count() && !Projection.is_Okay(); i++)
	{
		CSG_Parameter	*p	= m_Parameters[i];

		if( p->is_Enabled() && !p->ignore_Projection() )
		{
			if( p->Get_Type() == PARAMETER_TYPE_Parameters )
			{
				p->asParameters()->DataObjects_Get_Projection(Projection);
			}
			else if( p->is_Input() )
			{
				if( p->is_DataObject()
				&&  p->asDataObject() != DATAOBJECT_NOTSET
				&&  p->asDataObject() != DATAOBJECT_CREATE )
				{
					Projection	= p->asDataObject()->Get_Projection();
				}
				else if( p->is_DataObject_List() )
				{
					for(int j=0; j<p->asList()->Get_Item_Count() && !Projection.is_Okay(); j++)
					{
						Projection	= p->asList()->Get_Item(j)->Get_Projection();
					}
				}
			}
		}
	}

	return( Projection.is_Okay() );
}

//---------------------------------------------------------
bool CSG_Parameters::DataObjects_Set_Projection(const CSG_Projection &Projection)
{
	if( !Projection.is_Okay() )
	{
		return( false );
	}

	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Parameter	*p	= m_Parameters[i];

		if( !p->ignore_Projection() )
		{
			if( p->Get_Type() == PARAMETER_TYPE_Parameters )
			{
				p->asParameters()->DataObjects_Set_Projection(Projection);
			}
			else if( p->is_Output() )
			{
				if( p->is_DataObject()
				&&  p->asDataObject() != DATAOBJECT_NOTSET
				&&  p->asDataObject() != DATAOBJECT_CREATE )
				{
					p->asDataObject()->Get_Projection()	= Projection;
				}
				else if( p->is_DataObject_List() )
				{
					for(int j=0; j<p->asList()->Get_Item_Count(); j++)
					{
						p->asList()->Get_Item(j)->Get_Projection()	= Projection;
					}
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameters::Get_String(CSG_String &String, bool bOptionsOnly)
{
	bool	bResult	= false;

	if( Get_Count() > 0 )
	{
		if( m_pGrid_System )
		{
			m_pGrid_System->_Set_String();

			String	+= CSG_String::Format("%s: %s\n", m_pGrid_System->Get_Name(), m_pGrid_System->asString());
		}

		for(int i=0; i<Get_Count(); i++)
		{
			CSG_Parameter	*p	= m_Parameters[i];

			if( (!bOptionsOnly || p->is_Option()) && !p->asGrid_System() && p->is_Enabled() && !p->is_Information() && !(p->Get_Type() == PARAMETER_TYPE_String && ((CSG_Parameter_String *)p)->is_Password()) )
			{
				bResult	= true;

				p->_Set_String(); // forcing update (at scripting level some parameter types can be changed without the Set_Parameter() mechanism)

				String	+= CSG_String::Format("%s: %s\n", p->Get_Name(), p->asString());
			}
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Parameters::Msg_String(bool bOptionsOnly)
{
	CSG_String	Msg;

	if( Get_String(Msg, bOptionsOnly) )
	{
		SG_UI_Msg_Add_Execution(CSG_String::Format("\n__________\n[%s] %s:\n", m_Name.c_str(),
			bOptionsOnly ? _TL("Options") : _TL("Parameters")),
			false, SG_UI_MSG_STYLE_NORMAL
		);

		SG_UI_Msg_Add_Execution(Msg, false, SG_UI_MSG_STYLE_01);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameters::Set_History(CSG_MetaData &MetaData, bool bOptions, bool bDataObjects)
{
	CSG_MetaData	*pEntry;
	CSG_Data_Object	*pObject;

	//-----------------------------------------------------
	if( bOptions )
	{
		for(int i=0; i<Get_Count(); i++)	// get options...
		{
			CSG_Parameter	*p	= m_Parameters[i];

			if(	p->is_Option() && p->is_Enabled() && !p->is_Information()
			&&	!(p->Get_Type() == PARAMETER_TYPE_String && ((CSG_Parameter_String *)p)->is_Password()) )
			{
				p->Serialize(MetaData, true);
			}

			//---------------------------------------------
			else if( p->is_Parameters() )
			{
				p->asParameters()->Set_History(MetaData, true, false);
			}
		}
	}

	//-----------------------------------------------------
	if( bDataObjects )
	{
		for(int i=0; i<Get_Count(); i++)	// get input with history...
		{
			CSG_Parameter	*p	= m_Parameters[i];

			//---------------------------------------------
			if( p->is_Input() )
			{
				if( p->is_DataObject() && (pObject = p->asDataObject()) != NULL  )
				{
					pEntry	= MetaData.Add_Child("INPUT");

					pEntry->Add_Property("type" , p->Get_Type_Identifier());
					pEntry->Add_Property("id"   , p->Get_Identifier     ());
					pEntry->Add_Property("name" , p->Get_Name           ());
					pEntry->Add_Property("parms",    Get_Identifier     ());

					if( p->Get_Type() == PARAMETER_TYPE_Grid
					||  p->Get_Type() == PARAMETER_TYPE_Grids )
					{
						pEntry->Add_Property("system", p->Get_Parent()->Get_Identifier());
					}

					if( pObject->Get_History().Get_Children_Count() > 0 )
					{
						pEntry->Add_Children(pObject->Get_History());
					}
					else if( pObject->Get_File_Name() && *pObject->Get_File_Name() )
					{
						pEntry	= pEntry->Add_Child("FILE", pObject->Get_File_Name());
					}
				}

				else if( p->is_DataObject_List() && p->asList()->Get_Item_Count() > 0 )
				{
					CSG_MetaData	*pList	= MetaData.Add_Child("INPUT_LIST");

					pList->Add_Property("type" , p->Get_Type_Identifier());
					pList->Add_Property("id"   , p->Get_Identifier     ());
					pList->Add_Property("name" , p->Get_Name           ());
					pList->Add_Property("parms",    Get_Identifier     ());

					if( (p->Get_Type() == PARAMETER_TYPE_Grid_List || p->Get_Type() == PARAMETER_TYPE_Grids_List) && p->Get_Parent() && p->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
					{
						pList->Add_Property("system", p->Get_Parent()->Get_Identifier());
					}

					for(int j=0; j<p->asList()->Get_Item_Count(); j++)
					{
						pObject	= p->asList()->Get_Item(j);

						pEntry	= pList->Add_Child(*pList, false);

						pEntry->Set_Name("INPUT");
						
						if( !SG_Get_History_Ignore_Lists() && pObject->Get_History().Get_Children_Count() > 0 )
						{
							pEntry->Add_Children(pObject->Get_History());
						}
						else if( pObject->Get_File_Name() && *pObject->Get_File_Name() )
						{
							pEntry	= pEntry->Add_Child("FILE", pObject->Get_File_Name());
						}
					}
				}
			}

			//---------------------------------------------
			else if( p->is_Parameters() )
			{
				p->asParameters()->Set_History(MetaData, false, true);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid System						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* Sets the parameters' grid system if it has one. This is
* typically the case, if it represents the parameters list of a 
* CSG_Tool_Grid object.
*/
bool CSG_Parameters::Set_Grid_System(const CSG_Grid_System &System)
{
	return( m_pGrid_System && m_pGrid_System->asGrid_System() && m_pGrid_System->Set_Value((void *)&System) );
}

//---------------------------------------------------------
/**
* Resets the parameters' grid system if it has one. This is
* typically the case, if it represents the parameters list of a 
* CSG_Tool_Grid object.
*/
bool CSG_Parameters::Reset_Grid_System(void)
{
	CSG_Grid_System	System;

	return( Set_Grid_System(System) );
}


///////////////////////////////////////////////////////////
//														 //
//						Serialize						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Constant version of Serialize, only for saving to meta data file.
bool CSG_Parameters::Serialize(const CSG_String &File)	const
{
	CSG_MetaData	MetaData;

	return( Serialize(MetaData) && MetaData.Save(File) );
}
	
//---------------------------------------------------------
// Stores/loads parameter list settings to/from XML coded file.
bool CSG_Parameters::Serialize(const CSG_String &File, bool bSave)
{
	CSG_MetaData	MetaData;

	return( bSave
		? (Serialize(MetaData, bSave) && MetaData.Save(File))
		: (MetaData.Load(File) && Serialize(MetaData, bSave))
	);
}

//---------------------------------------------------------
// Constant version of Serialize, only for saving to meta data.
bool CSG_Parameters::Serialize(CSG_MetaData &Root)	const
{
	Root.Destroy();

	Root.Set_Name("parameters");
	Root.Set_Property("name", m_Name);

	for(int i=0; i<Get_Count(); i++)
	{
		m_Parameters[i]->Serialize(Root, true);
	}

	return( true );
}

//---------------------------------------------------------
// Stores/loads parameter list settings to/from CSG_MetaData object 'Root'.
bool CSG_Parameters::Serialize(CSG_MetaData &Root, bool bSave)
{
	if( bSave )
	{
		return( Serialize(Root) );
	}

	//-----------------------------------------------------
	if( Root.Cmp_Name("parameters") )
	{
		Root.Get_Property("name", m_Name);

		for(int i=0; i<Root.Get_Children_Count(); i++)
		{
			CSG_Parameter	*pParameter = Get_Parameter(Root(i)->Get_Property("id"));

			if(	pParameter && pParameter->Serialize(*Root(i), false) )
			{
				pParameter->has_Changed();
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
// SAGA 2.0 compatibility...
bool CSG_Parameters::Serialize_Compatibility(CSG_File &Stream)
{
	CSG_Parameter	*pParameter = NULL;
	CSG_String		sLine;

	if( !Stream.is_Open() )
	{
		return( false );
	}

	//-----------------------------------------------------
	while( Stream.Read_Line(sLine) && sLine.Cmp("[PARAMETER_ENTRIES_BEGIN]") );

	if( sLine.Cmp("[PARAMETER_ENTRIES_BEGIN]") )
	{
		return( false );
	}

	//-----------------------------------------------------
	while( Stream.Read_Line(sLine) && sLine.Cmp("[PARAMETER_ENTRIES_END]") )
	{
		if( !sLine.Cmp("[PARAMETER_ENTRY_BEGIN]")
		&&	Stream.Read_Line(sLine) && (pParameter = Get_Parameter(sLine)) != NULL
		&&	Stream.Read_Line(sLine) )
		{
			int			i;
			double		d;
			TSG_Rect	r;
			CSG_String	s;
			CSG_Table	t;

			switch( sLine.asInt() )
			{
			case  1: // PARAMETER_TYPE_Bool:
			case  2: // PARAMETER_TYPE_Int:
			case  6: // PARAMETER_TYPE_Choice:
			case 11: // PARAMETER_TYPE_Color:
			case 15: // PARAMETER_TYPE_Table_Field:
				pParameter->Set_Value(Stream.Scan_Int());
				break;

			case  3: // PARAMETER_TYPE_Double:
			case  4: // PARAMETER_TYPE_Degree:
				pParameter->Set_Value(Stream.Scan_Double());
				break;

			case  5: // PARAMETER_TYPE_Range:
				pParameter->asRange()->Set_Range(Stream.Scan_Double(), Stream.Scan_Double());
				break;

			case  7: // PARAMETER_TYPE_String:
			case  9: // PARAMETER_TYPE_FilePath:
				Stream.Read_Line(sLine);
				pParameter->Set_Value(sLine);
				break;

			case  8: // PARAMETER_TYPE_Text:
				s.Clear();
				while( Stream.Read_Line(sLine) && sLine.Cmp("[TEXT_ENTRY_END]") )
				{
					s	+= sLine + "\n";
				}
				pParameter->Set_Value(s);
				break;

			case 10: // PARAMETER_TYPE_Font:
				Stream.Read(&i, sizeof(i));
				pParameter->Set_Value(i);
				break;

			case 12: // PARAMETER_TYPE_Colors:
				pParameter->asColors()->Serialize(Stream, false, false);
				break;

			case 13: // PARAMETER_TYPE_FixedTable:
				if( t.Serialize(Stream, false) )
				{
					pParameter->asTable()->Assign_Values(&t);
				}
				break;

			case 14: // PARAMETER_TYPE_Grid_System:
				Stream.Read(&d, sizeof(d));
				Stream.Read(&r, sizeof(r));
				pParameter->asGrid_System()->Assign(d, r);
				break;

			case 16: // PARAMETER_TYPE_Grid:
			case 17: // PARAMETER_TYPE_Table:
			case 18: // PARAMETER_TYPE_Shapes:
			case 19: // PARAMETER_TYPE_TIN:
			case 24: // PARAMETER_TYPE_DataObject_Output:
				if( Stream.Read_Line(sLine) )
				{
					if( !sLine.Cmp("[ENTRY_DATAOBJECT_CREATE]") )
					{
						pParameter->Set_Value(DATAOBJECT_CREATE);
					}
					else
					{
						pParameter->Set_Value(m_pManager ? m_pManager->Find(sLine) : NULL);
					}
				}
				break;

			case 20: // PARAMETER_TYPE_Grid_List:
			case 21: // PARAMETER_TYPE_Table_List:
			case 22: // PARAMETER_TYPE_Shapes_List:
			case 23: // PARAMETER_TYPE_TIN_List:
				while( Stream.Read_Line(sLine) && sLine.Cmp("[ENTRY_DATAOBJECTLIST_END]") )
				{
					CSG_Data_Object	*pObject	= m_pManager ? m_pManager->Find(sLine) : NULL;

					if( pObject )
					{
						pParameter->asList()->Add_Item(pObject);
					}
				}
				break;

			case 25: // PARAMETER_TYPE_Parameters:
				pParameter->asParameters()->Serialize_Compatibility(Stream);
				break;
			}
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
