/**********************************************************
 * Version $Id$
 *********************************************************/

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
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Parameters::_On_Construction(void)
{
	m_pOwner			= NULL;

	m_pManager			= &SG_Get_Data_Manager();

	m_Parameters		= NULL;
	m_nParameters		= 0;

	m_Callback			= NULL;
	m_bCallback			= true;

	m_pGrid_System		= NULL;
}

//---------------------------------------------------------
bool CSG_Parameters::Create(const CSG_Parameters &Parameters)
{
	Destroy();

	m_pOwner		= Parameters.m_pOwner;
	m_pManager		= Parameters.m_pManager;

	m_Callback		= Parameters.m_Callback;
	m_bCallback		= Parameters.m_bCallback;

	Set_Identifier	(Parameters.Get_Identifier());
	Set_Name		(Parameters.Get_Name());
	Set_Description	(Parameters.Get_Description());

	//-----------------------------------------------------
	int		i;

	for(i=0; i<Parameters.m_nParameters; i++)
	{
		_Add(Parameters.m_Parameters[i]);
	}

	for(i=0; i<Parameters.m_nParameters; i++)
	{
		if( Get_Parameter(i) && Parameters.m_Parameters[i]->m_pParent )
		{
			Get_Parameter(i)->m_pParent	= Get_Parameter(Parameters.m_Parameters[i]->m_pParent->Get_Identifier());
		}
	}

	if( Parameters.m_pGrid_System )
	{
		m_pGrid_System	= Get_Parameter(Parameters.m_pGrid_System->Get_Identifier());
	}

	return( m_nParameters == Parameters.m_nParameters );
}

//---------------------------------------------------------
bool CSG_Parameters::Create(void *pOwner, const SG_Char *Name, const SG_Char *Description, const SG_Char *Identifier, bool bGrid_System)
{
	Destroy();

	m_pOwner		= pOwner;

	Set_Identifier	(Identifier);
	Set_Name		(Name);
	Set_Description	(Description);

	if( bGrid_System )
	{
		m_pGrid_System	= Add_Grid_System(
			NULL, SG_T("PARAMETERS_GRID_SYSTEM"),
			_TL("Grid system"),
			_TL("Grid system")
		);
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Parameters::Destroy(void)
{
	m_pOwner		= NULL;
	m_pGrid_System	= NULL;

	Del_Parameters();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Set the responsible data manager for this parameter list.
  * If it is not NULL, the parameter list will perform checks
  * beforehand module executions, e.g. check if all data sets
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
  * Change the identifier of this parameter list after construction.
*/
//---------------------------------------------------------
void CSG_Parameters::Set_Identifier(const CSG_String &String)
{
	m_Identifier	= String;
}

//---------------------------------------------------------
/**
  * Change the name of this parameter list after construction.
*/
//---------------------------------------------------------
void CSG_Parameters::Set_Name(const CSG_String &String)
{
	m_Name			= String;
}

//---------------------------------------------------------
/**
  * Change the description for this parameter list after construction.
*/
//---------------------------------------------------------
void CSG_Parameters::Set_Description(const CSG_String &String)
{
	m_Description	= String;
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Parameter(CSG_Parameter *pParameter)
{
	return( _Add(pParameter) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Node(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	return( _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Node, PARAMETER_INFORMATION) );
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
CSG_Parameter * CSG_Parameters::Add_Value(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	return( _Add_Value(pParent, Identifier, Name, Description, false, Type, Value, Minimum, bMinimum, Maximum, bMaximum) );
}

CSG_Parameter * CSG_Parameters::Add_Info_Value(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, double Value)
{
	return( _Add_Value(pParent, Identifier, Name, Description,  true, Type, Value, 0.0, false, 0.0, false) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Bool  (CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, bool   Value)
{
	return( Add_Value(pParent, Identifier, Name, Description, PARAMETER_TYPE_Bool  , Value ? 1.0 : 0.0) );
}

CSG_Parameter * CSG_Parameters::Add_Int   (CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int    Value, int    Minimum, bool bMinimum, int    Maximum, bool bMaximum)
{
	return( Add_Value(pParent, Identifier, Name, Description, PARAMETER_TYPE_Int   , Value, Minimum, bMinimum, Maximum, bMaximum) );
}

CSG_Parameter * CSG_Parameters::Add_Double(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	return( Add_Value(pParent, Identifier, Name, Description, PARAMETER_TYPE_Double, Value, Minimum, bMinimum, Maximum, bMaximum) );
}

CSG_Parameter * CSG_Parameters::Add_Degree(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	return( Add_Value(pParent, Identifier, Name, Description, PARAMETER_TYPE_Degree, Value, Minimum, bMinimum, Maximum, bMaximum) );
}

CSG_Parameter * CSG_Parameters::Add_Date(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, double Value)	// Julian Day Number
{
	return( Add_Value(pParent, Identifier, Name, Description, PARAMETER_TYPE_Date  , Value) );
}

CSG_Parameter * CSG_Parameters::Add_Color (CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int    Value)
{
	return( Add_Value(pParent, Identifier, Name, Description, PARAMETER_TYPE_Color , Value) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Range(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, double Range_Min, double Range_Max, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	return( _Add_Range(pParent, Identifier, Name, Description, false, Range_Min, Range_Max, Minimum, bMinimum, Maximum, bMaximum) );
}

CSG_Parameter * CSG_Parameters::Add_Info_Range(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, double Range_Min, double Range_Max)
{
	return( _Add_Range(pParent, Identifier, Name, Description,  true, Range_Min, Range_Max, 0.0, false, 0.0, false) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Choice(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, const CSG_String &Items, int Default)
{
	CSG_Parameter			*pParameter;
	CSG_Parameter_Choice	*m_pData;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Choice, 0);

	m_pData		= (CSG_Parameter_Choice *)pParameter->m_pData;
	m_pData->Set_Items(Items);

	bool	bCallback	= Set_Callback(false);
	pParameter->Set_Value  (Default);
	pParameter->Set_Default(Default);
	Set_Callback(bCallback);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_String(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, const CSG_String &String, bool bLongText, bool bPassword)
{
	return( _Add_String(pParent, Identifier, Name, Description, false, String, bLongText, bPassword) );
}

CSG_Parameter * CSG_Parameters::Add_Info_String(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, const CSG_String &String, bool bLongText)
{
	return( _Add_String(pParent, Identifier, Name, Description,  true, String, bLongText, false) );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_FilePath(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, const SG_Char *Filter, const SG_Char *Default, bool bSave, bool bDirectory, bool bMultiple)
{
	CSG_Parameter			*pParameter;
	CSG_Parameter_File_Name	*m_pData;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_FilePath, 0);

	m_pData		= (CSG_Parameter_File_Name *)pParameter->m_pData;
	m_pData->Set_Filter			(Filter);
	m_pData->Set_Flag_Save		(bSave);
	m_pData->Set_Flag_Multiple	(bMultiple);
	m_pData->Set_Flag_Directory	(bDirectory);

	bool	bCallback	= Set_Callback(false);
	pParameter->Set_Value  (Default);
	pParameter->Set_Default(Default);
	Set_Callback(bCallback);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Font(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, const SG_Char *pInit)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Font, 0);

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
CSG_Parameter * CSG_Parameters::Add_Colors(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, CSG_Colors *pInit)
{
	CSG_Parameter			*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Colors, 0);

	pParameter->asColors()->Assign(pInit);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_FixedTable(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, CSG_Table *pTemplate)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_FixedTable, 0);

	pParameter->asTable()->Create(pTemplate);

	if( pTemplate )
	{
		for(int i=0; i<pTemplate->Get_Record_Count(); i++)
		{
			pParameter->asTable()->Add_Record(pTemplate->Get_Record(i));
		}
	}

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grid_System(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, CSG_Grid_System *pInit)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Grid_System, 0);

	if( pInit )
	{
		pParameter->asGrid_System()->Assign(*pInit);
	}

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grid(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent, TSG_Data_Type Preferred_Type)
{
	CSG_Parameter	*pParameter;

	if( !pParent || pParent->Get_Type() != PARAMETER_TYPE_Grid_System )
	{
		if( bSystem_Dependent && m_pGrid_System )
		{
			pParent	= m_pGrid_System;
		}
		else
		{
			pParent	= Add_Grid_System(pParent, CSG_String::Format(SG_T("%s_GRIDSYSTEM"), Identifier.c_str()), _TL("Grid system"), SG_T(""));
		}
	}

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Grid, Constraint);

	((CSG_Parameter_Grid *)pParameter->m_pData)->Set_Preferred_Type(Preferred_Type);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grid_or_Const(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum, bool bSystem_Dependent)
{
	CSG_Parameter	*pParameter	= Add_Grid(pParent, Identifier, Name, Description, PARAMETER_INPUT_OPTIONAL, bSystem_Dependent);

	((CSG_Parameter_Grid *)pParameter->m_pData)->Add_Default(Value, Minimum, bMinimum, Maximum, bMaximum);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grid_Output(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter->Get_Data())->Set_DataObject_Type(DATAOBJECT_TYPE_Grid);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Grid_List(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent)
{
	CSG_Parameter	*pParameter;

	if( (Constraint & PARAMETER_INPUT) && bSystem_Dependent && (!pParent || pParent->Get_Type() != PARAMETER_TYPE_Grid_System) )
	{
		pParent	= m_pGrid_System;
	}

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Grid_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table_Field(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, bool bAllowNone)
{
	if( pParent
	&&	(	pParent->Get_Type() == PARAMETER_TYPE_Table
		||	pParent->Get_Type() == PARAMETER_TYPE_Shapes
		||	pParent->Get_Type() == PARAMETER_TYPE_TIN
		||	pParent->Get_Type() == PARAMETER_TYPE_PointCloud	) )
	{
		return( _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Table_Field, bAllowNone ? PARAMETER_OPTIONAL : 0) );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table_Field_or_Const(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	CSG_Parameter	*pParameter	= Add_Table_Field(pParent, Identifier, Name, Description, true);

	if( pParameter )
	{
		((CSG_Parameter_Table_Field *)pParameter->m_pData)->Add_Default(Value, Minimum, bMinimum, Maximum, bMaximum);
	}

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table_Fields(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	if( pParent
	&&	(	pParent->Get_Type() == PARAMETER_TYPE_Table
		||	pParent->Get_Type() == PARAMETER_TYPE_Shapes
		||	pParent->Get_Type() == PARAMETER_TYPE_TIN
		||	pParent->Get_Type() == PARAMETER_TYPE_PointCloud	) )
	{
		return( _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Table_Fields, 0) );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Table, Constraint);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table_Output(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter->Get_Data())->Set_DataObject_Type(DATAOBJECT_TYPE_Table);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Table_List(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Table_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Shapes(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint, TSG_Shape_Type Shape_Type)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Shapes, Constraint);

	((CSG_Parameter_Shapes *)pParameter->m_pData)->Set_Shape_Type(Shape_Type);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Shapes_Output(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter->Get_Data())->Set_DataObject_Type(DATAOBJECT_TYPE_Shapes);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Shapes_List(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint, TSG_Shape_Type Type)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Shapes_List, Constraint);

	((CSG_Parameter_Shapes_List *)pParameter->m_pData)->Set_Shape_Type(Type);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_TIN(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_TIN, Constraint);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_TIN_Output(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter->Get_Data())->Set_DataObject_Type(DATAOBJECT_TYPE_TIN);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_TIN_List(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_TIN_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_PointCloud(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_PointCloud, Constraint);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_PointCloud_Output(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CSG_Parameter_Data_Object_Output *)pParameter->Get_Data())->Set_DataObject_Type(DATAOBJECT_TYPE_PointCloud);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_PointCloud_List(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_PointCloud_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Add_Parameters(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Parameters, 0);

	pParameter->asParameters()->m_Callback	= m_Callback;

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::_Add_Value(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, bool bInformation, TSG_Parameter_Type Type, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
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

	CSG_Parameter	*pParameter	= _Add(pParent, Identifier, Name, Description, Type, bInformation ? PARAMETER_INFORMATION : 0);

	if( !bInformation )
	{
		if( Type == PARAMETER_TYPE_Int
		||  Type == PARAMETER_TYPE_Double
		||  Type == PARAMETER_TYPE_Degree )
		{
			((CSG_Parameter_Value *)pParameter->m_pData)->Set_Minimum(Minimum, bMinimum);
			((CSG_Parameter_Value *)pParameter->m_pData)->Set_Maximum(Maximum, bMaximum);
		}
	}

	bool	bCallback	= Set_Callback(false);
	pParameter->Set_Value  (Value);
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
CSG_Parameter * CSG_Parameters::_Add_Range(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, bool bInformation, double Default_Min, double Default_Max, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	double				d;
	CSG_Parameter		*pParameter;
	CSG_Parameter_Range	*pData;

	//-----------------------------------------------------
	if( Default_Min > Default_Max )
	{
		d			= Default_Min;
		Default_Min	= Default_Max;
		Default_Max	= d;
	}

	//-----------------------------------------------------
	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Range, bInformation ? PARAMETER_INFORMATION : 0);

	pData		= pParameter->asRange();

	pData->Get_LoParm()->asValue()->Set_Minimum(Minimum, bMinimum);
	pData->Get_LoParm()->asValue()->Set_Maximum(Maximum, bMaximum);
	pData->Get_HiParm()->asValue()->Set_Minimum(Minimum, bMinimum);
	pData->Get_HiParm()->asValue()->Set_Maximum(Maximum, bMaximum);

	pData->Set_LoVal(Default_Min);
	pData->Set_HiVal(Default_Max);

	pData->Get_LoParm()->asValue()->Set_Default(Default_Min);
	pData->Get_HiParm()->asValue()->Set_Default(Default_Max);

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::_Add_String(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, bool bInformation, const SG_Char *String, bool bLongText, bool bPassword)
{
	CSG_Parameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, bLongText ? PARAMETER_TYPE_Text : PARAMETER_TYPE_String, bInformation ? PARAMETER_INFORMATION : 0);

	bool	bCallback	= Set_Callback(false);
	pParameter->Set_Value  (String);
	pParameter->Set_Default(String);
	Set_Callback(bCallback);

	((CSG_Parameter_String *)pParameter->Get_Data())->Set_Password(bPassword);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::_Add(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, int Constraint)
{
	CSG_Parameter	*pParameter;

	if( Identifier.Length() > 0 )
	{
		pParameter	= new CSG_Parameter(this, pParent, Identifier, Name, Description, Type, Constraint);
	}
	else
	{
		pParameter	= new CSG_Parameter(this, pParent, CSG_String::Format(SG_T("%d"), m_nParameters), Name, Description, Type, Constraint);
	}

	m_Parameters	= (CSG_Parameter **)SG_Realloc(m_Parameters, (m_nParameters + 1) * sizeof(CSG_Parameter *));
	m_Parameters[m_nParameters++]	= pParameter;

	return( pParameter );
}

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::_Add(CSG_Parameter *pSource)
{
	CSG_Parameter	*pParameter;

	if( pSource )
	{
		pParameter	= _Add(
			pSource->Get_Parent() ? Get_Parameter(pSource->Get_Parent()->Get_Identifier()) : NULL,
			pSource->Get_Identifier(),
			pSource->Get_Name(),
			pSource->Get_Description(),
			pSource->Get_Type(),
			pSource->m_pData->Get_Constraint()
		);

		pParameter->Assign(pSource);
	}
	else
	{
		pParameter	= NULL;
	}

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CSG_Parameters::Get_Parameter(const CSG_String &Identifier)
{
	if( m_Parameters && Identifier.Length() )
	{
		for(int i=0; i<m_nParameters; i++)
		{
			if( !m_Parameters[i]->m_Identifier.Cmp(Identifier) )
			{
				return( m_Parameters[i] );
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
		Set_Callback(false);
		m_Callback(pParameter, Flags);
		Set_Callback(true);

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
bool CSG_Parameters::Set_Parameter(const CSG_String &Identifier, CSG_Parameter *pSource)
{
	CSG_Parameter	*pTarget;

	if( pSource != NULL && (pTarget = Get_Parameter(Identifier)) != NULL && pSource->Get_Type() == pTarget->Get_Type() )
	{
		return( pTarget->Assign(pSource) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const CSG_String &Identifier, int Value, int Type)
{
	CSG_Parameter	*pTarget	= Get_Parameter(Identifier);

	if( pTarget && (Type == PARAMETER_TYPE_Undefined || Type == pTarget->Get_Type()) )
	{
		pTarget->Set_Value(Value);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const CSG_String &Identifier, double Value, int Type)
{
	CSG_Parameter	*pTarget	= Get_Parameter(Identifier);

	if( pTarget && (Type == PARAMETER_TYPE_Undefined || Type == pTarget->Get_Type()) )
	{
		pTarget->Set_Value(Value);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const CSG_String &Identifier, void *Value, int Type)
{
	CSG_Parameter	*pTarget	= Get_Parameter(Identifier);

	if( pTarget && (Type == PARAMETER_TYPE_Undefined || Type == pTarget->Get_Type()) )
	{
		pTarget->Set_Value(Value);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameters::Set_Parameter(const CSG_String &Identifier, const SG_Char *Value, int Type)
{
	CSG_Parameter	*pTarget	= Get_Parameter(Identifier);

	if( pTarget && (Type == PARAMETER_TYPE_Undefined || Type == pTarget->Get_Type()) )
	{
		pTarget->Set_Value(Value);

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
bool CSG_Parameters::Restore_Defaults(bool bClearData)
{
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

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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

	//-----------------------------------------------------
	int		i;

	Del_Parameters();

	for(i=0; i<pSource->m_nParameters; i++)
	{
		_Add(pSource->m_Parameters[i]);
	}

	for(i=0; i<pSource->m_nParameters; i++)
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
//														 //
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

			sError.Append(CSG_String::Format(SG_T("\n%s: %s"), m_Parameters[i]->Get_Type_Name().c_str(), m_Parameters[i]->Get_Name()));
		}
	}

	//-----------------------------------------------------
	if( !bResult && !bSilent )
	{
		SG_UI_Dlg_Message(CSG_String::Format(SG_T("%s\n%s"), _TL("invalid input!"), sError.c_str()), Get_Name() );
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
			p->Set_Value(DATAOBJECT_NOTSET);
		}
		else if( p->is_Input() )
		{
			bResult	= p->Check(true);
		}

		//-------------------------------------------------
		else if( p->is_DataObject_List() )
		{
			for(int j=p->asList()->Get_Count()-1; j>=0; j--)
			{
				if( m_pManager && !m_pManager->Exists(p->asList()->asDataObject(j)) )
				{
					p->asList()->Del_Item(j);
				}
			}
		}

		//-------------------------------------------------
		else if( p->is_DataObject() && p->is_Enabled() == false )
		{
			if( !m_pManager || !m_pManager->Exists(p->asDataObject()) )
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
				default:
					break;

				case PARAMETER_TYPE_Table:		pDataObject	= SG_Create_Table();		break;
				case PARAMETER_TYPE_TIN:		pDataObject	= SG_Create_TIN();			break;
				case PARAMETER_TYPE_PointCloud:	pDataObject	= SG_Create_PointCloud();	break;
				case PARAMETER_TYPE_Shapes:		pDataObject	= SG_Create_Shapes(
						((CSG_Parameter_Shapes *)p->Get_Data())->Get_Shape_Type()
					);
					break;

				case PARAMETER_TYPE_Grid:
					if(	p->Get_Parent() && p->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System 
					&&	p->Get_Parent()->asGrid_System() && p->Get_Parent()->asGrid_System()->is_Valid() )
					{
						pDataObject	= SG_Create_Grid(*p->Get_Parent()->asGrid_System(), ((CSG_Parameter_Grid *)p->Get_Data())->Get_Preferred_Type());
					}
					break;
				}
			}
			else if( p->Get_Type() == PARAMETER_TYPE_Shapes && p->asShapes() )
			{
				if( ((CSG_Parameter_Shapes *)p->Get_Data())->Get_Shape_Type() != SHAPE_TYPE_Undefined
				&&	((CSG_Parameter_Shapes *)p->Get_Data())->Get_Shape_Type() != p->asShapes()->Get_Type() )
				{
					pDataObject	= SG_Create_Shapes(((CSG_Parameter_Shapes *)p->Get_Data())->Get_Shape_Type());
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
		else
		{
			if( p->Get_Type() == PARAMETER_TYPE_Shapes
			&&  p->asShapes() != DATAOBJECT_NOTSET
			&&  p->asShapes() != DATAOBJECT_CREATE
			&&  p->asShapes()->Get_Type() == SHAPE_TYPE_Undefined )
			{
				if( m_pManager && !m_pManager->Delete(p->asShapes()) )
				{
					delete(p->asShapes());
				}

				p->Set_Value(DATAOBJECT_NOTSET);
			}

			//---------------------------------------------
			if( p->is_Output() )
			{
				if( p->is_DataObject()
				&&  p->asDataObject() != DATAOBJECT_NOTSET
				&&  p->asDataObject() != DATAOBJECT_CREATE )
				{
					CSG_Data_Object	*pObject	= p->asDataObject();

					if( m_pManager && !m_pManager->Exists(pObject) )
					{
						m_pManager->Add(pObject);
					}

					SG_UI_DataObject_Update(pObject, SG_UI_DATAOBJECT_UPDATE_ONLY, NULL);
				}
				else if( p->is_DataObject_List() )
				{
					for(int j=0; j<p->asList()->Get_Count(); j++)
					{
						CSG_Data_Object	*pObject	= p->asList()->asDataObject(j);

						if( m_pManager && !m_pManager->Exists(pObject) )
						{
							m_pManager->Add(pObject);
						}

						SG_UI_DataObject_Update(pObject, SG_UI_DATAOBJECT_UPDATE_ONLY, NULL);
					}
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameters::DataObjects_Get_Projection(CSG_Projection &Projection)	const
{
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Parameter	*p	= m_Parameters[i];

		if( !p->ignore_Projection() )
		{
			CSG_Projection	P;

			if( p->Get_Type() == PARAMETER_TYPE_Parameters )
			{
				if( !p->asParameters()->DataObjects_Get_Projection(P) )
				{
					return( false );
				}

				if( P.is_Okay() )
				{
					if( !Projection.is_Okay() )
					{
						Projection	= P;
					}
					else if( Projection != P )
					{
						return( false );
					}
				}
			}
			else if( p->is_Input() )
			{
				if( p->is_DataObject()
				&&  p->asDataObject() != DATAOBJECT_NOTSET
				&&  p->asDataObject() != DATAOBJECT_CREATE )
				{
					P	= p->asDataObject()->Get_Projection();

					if( P.is_Okay() )
					{
						if( !Projection.is_Okay() )
						{
							Projection	= P;
						}
						else if( Projection != P )
						{
							return( false );
						}
					}
				}
				else if( p->is_DataObject_List() )
				{
					for(int j=0; j<p->asList()->Get_Count(); j++)
					{
						P	= p->asList()->asDataObject(j)->Get_Projection();

						if( P.is_Okay() )
						{
							if( !Projection.is_Okay() )
							{
								Projection	= P;
							}
							else if( Projection != P )
							{
								return( false );
							}
						}
					}
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameters::DataObjects_Set_Projection(const CSG_Projection &Projection)
{
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
					for(int j=0; j<p->asList()->Get_Count(); j++)
					{
						p->asList()->asDataObject(j)->Get_Projection()	= Projection;
					}
				}
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
bool CSG_Parameters::Get_String(CSG_String &String, bool bOptionsOnly)
{
	bool	bResult	= false;

	if( Get_Count() > 0 )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			CSG_Parameter	*p	= m_Parameters[i];

			if( (!bOptionsOnly || p->is_Option()) && p->is_Enabled() && !p->is_Information() && !(p->Get_Type() == PARAMETER_TYPE_String && ((CSG_Parameter_String *)p->Get_Data())->is_Password()) )
			{
				bResult	= true;

			//	String	+= CSG_String::Format("[%s] %s: %s\n", p->Get_Type_Name(), p->Get_Name(), p->asString());
				String	+= CSG_String::Format("%s: %s\n", p->Get_Name(), p->asString());
			}
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Parameters::Msg_String(bool bOptionsOnly)
{
	CSG_String	s;

	if( Get_String(s, bOptionsOnly) )
	{
		SG_UI_Msg_Add_Execution(SG_T("\n"), false);
		SG_UI_Msg_Add_Execution(bOptionsOnly ? _TL("Options") : _TL("Parameters"), false);
		SG_UI_Msg_Add_Execution(SG_T("\n"), false);
		SG_UI_Msg_Add_Execution(s, false, SG_UI_MSG_STYLE_01);

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
			&&	!(p->Get_Type() == PARAMETER_TYPE_String && ((CSG_Parameter_String *)p->Get_Data())->is_Password()) )
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

					if( p->Get_Type() == PARAMETER_TYPE_Grid )
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

				else if( p->is_DataObject_List() && p->asList()->Get_Count() > 0 )
				{
					CSG_MetaData	*pList	= MetaData.Add_Child("INPUT_LIST");

					pList->Add_Property("type" , p->Get_Type_Identifier());
					pList->Add_Property("id"   , p->Get_Identifier     ());
					pList->Add_Property("name" , p->Get_Name           ());
					pList->Add_Property("parms",    Get_Identifier     ());

					if( p->Get_Type() == PARAMETER_TYPE_Grid_List && p->Get_Parent() && p->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
					{
						pList->Add_Property("system", p->Get_Parent()->Get_Identifier());
					}

					for(int j=0; j<p->asList()->Get_Count(); j++)
					{
						pObject	= p->asList()->asDataObject(j);

						pEntry	= pList->Add_Child(*pList, false); pEntry->Set_Name("INPUT");

						if( pObject->Get_History().Get_Children_Count() > 0 )
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
//						Serialize						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameters::Serialize(const CSG_String &File_Name, bool bSave)
{
	CSG_MetaData	MetaData;

	if( bSave )
	{
		return( Serialize(MetaData, true) && MetaData.Save(File_Name) );
	}
	else
	{
		return( MetaData.Load(File_Name) && Serialize(MetaData, false) );
	}
}

//---------------------------------------------------------
bool CSG_Parameters::Serialize(CSG_MetaData &MetaData, bool bSave)
{
	if( bSave )
	{
		MetaData.Destroy();

		MetaData.Set_Name    (SG_T("PARAMETERS"));
		MetaData.Set_Property(SG_T("name"), Get_Name());

		for(int i=0; i<Get_Count(); i++)
		{
			m_Parameters[i]->Serialize(MetaData, true);
		}
	}
	else
	{
		if( MetaData.Get_Name().Cmp(SG_T("PARAMETERS")) )
		{
			return( false );
		}

		MetaData.Get_Property(SG_T("name"), m_Name);

		for(int i=0; i<MetaData.Get_Children_Count(); i++)
		{
			CSG_String		Identifier;
			CSG_Parameter	*pParameter = NULL;

			if(	MetaData.Get_Child(i)->Get_Property(SG_T("id"), Identifier)
			&&	(pParameter	= Get_Parameter(Identifier)) != NULL )
			{
				if( pParameter->Serialize(*MetaData.Get_Child(i), false) )
				{
					pParameter->has_Changed();
				}
			}
		}
	}

	return( true );
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
	while( Stream.Read_Line(sLine) && sLine.Cmp(SG_T("[PARAMETER_ENTRIES_BEGIN]")) );

	if( sLine.Cmp(SG_T("[PARAMETER_ENTRIES_BEGIN]")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	while( Stream.Read_Line(sLine) && sLine.Cmp(SG_T("[PARAMETER_ENTRIES_END]")) )
	{
		if( !sLine.Cmp(SG_T("[PARAMETER_ENTRY_BEGIN]"))
		&&	Stream.Read_Line(sLine) && (pParameter = Get_Parameter(sLine)) != NULL
		&&	Stream.Read_Line(sLine) )
		{
			int			i;
			double		d, e;
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
				fscanf(Stream.Get_Stream(), "%d", &i);
				pParameter->Set_Value(i);
				break;

			case  3: // PARAMETER_TYPE_Double:
			case  4: // PARAMETER_TYPE_Degree:
				fscanf(Stream.Get_Stream(), "%lf", &d);
				pParameter->Set_Value(d);
				break;

			case  5: // PARAMETER_TYPE_Range:
				fscanf(Stream.Get_Stream(), "%lf %lf", &d, &e);
				pParameter->asRange()->Set_Range(d, e);
				break;

			case  7: // PARAMETER_TYPE_String:
			case  9: // PARAMETER_TYPE_FilePath:
				Stream.Read_Line(sLine);
				pParameter->Set_Value(sLine);
				break;

			case  8: // PARAMETER_TYPE_Text:
				s.Clear();
				while( Stream.Read_Line(sLine) && sLine.Cmp(SG_T("[TEXT_ENTRY_END]")) )
				{
					s	+= sLine + SG_T("\n");
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
					if( !sLine.Cmp(SG_T("[ENTRY_DATAOBJECT_CREATE]")) )
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
				while( Stream.Read_Line(sLine) && sLine.Cmp(SG_T("[ENTRY_DATAOBJECTLIST_END]")) )
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
