
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameters::CParameters(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CParameters::CParameters(void *pOwner, const char *Name, const char *Description, const char *Identifier, bool bGrid_System)
{
	_On_Construction();

	Create(pOwner, Name, Description, Identifier, bGrid_System);
}

//---------------------------------------------------------
CParameters::~CParameters(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CParameters::_On_Construction(void)
{
	m_pOwner		= NULL;

	m_Parameters	= NULL;
	m_nParameters	= 0;

	m_Callback		= NULL;
	m_bCallback		= false;

	m_pGrid_System	= NULL;

	m_bManaged		= true;
}

//---------------------------------------------------------
void CParameters::Create(void *pOwner, const char *Name, const char *Description, const char *Identifier, bool bGrid_System)
{
	Destroy();

	m_pOwner		= pOwner;

	Set_Identifier	(Identifier);
	Set_Name		(Name);
	Set_Description	(Description);

	if( bGrid_System )
	{
		m_pGrid_System	= Add_Grid_System(
			NULL, "PARAMETERS_GRID_SYSTEM",
			LNG("[PRM] Grid system"),
			LNG("[PRM] Grid system")
		);
	}
}

//---------------------------------------------------------
void CParameters::Destroy(void)
{
	m_pOwner		= NULL;
	m_pGrid_System	= NULL;

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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CParameters::Set_Identifier(const char *String)
{
	if( String )
	{
		m_Identifier.Printf(String);
	}
	else
	{
		m_Identifier.Clear();
	}
}

const char * CParameters::Get_Identifier(void)
{
	return( m_Identifier );
}

//---------------------------------------------------------
void CParameters::Set_Name(const char *String)
{
	if( String )
	{
		m_Name.Printf(String);
	}
	else
	{
		m_Name.Clear();
	}
}

const char * CParameters::Get_Name(void)
{
	return( m_Name );
}

//---------------------------------------------------------
void CParameters::Set_Description(const char *String)
{
	if( String )
	{
		m_Description.Printf(String);
	}
	else
	{
		m_Description.Clear();
	}
}

const char * CParameters::Get_Description(void)
{
	return( m_Description );
}

//---------------------------------------------------------
void CParameters::Set_Translation(CSG_Translator &Translator)
{
	m_Name			= Translator.Get_Translation(m_Name);
	m_Description	= Translator.Get_Translation(m_Description);

	for(int i=0; i<m_nParameters; i++)
	{
		m_Parameters[i]->m_Name			= Translator.Get_Translation(m_Parameters[i]->m_Name);
		m_Parameters[i]->m_Description	= Translator.Get_Translation(m_Parameters[i]->m_Description);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter * CParameters::Add_Node(CParameter *pParent, const char *Identifier, const char *Name, const char *Description)
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
CParameter * CParameters::Add_Value(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, TParameter_Type Type, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	return( _Add_Value(pParent, Identifier, Name, Description, false, Type, Value, Minimum, bMinimum, Maximum, bMaximum) );
}

CParameter * CParameters::Add_Info_Value(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, TParameter_Type Type, double Value)
{
	return( _Add_Value(pParent, Identifier, Name, Description,  true, Type, Value, 0.0, false, 0.0, false) );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Range(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, double Range_Min, double Range_Max, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	return( _Add_Range(pParent, Identifier, Name, Description, false, Range_Min, Range_Max, Minimum, bMinimum, Maximum, bMaximum) );
}

CParameter * CParameters::Add_Info_Range(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, double Range_Min, double Range_Max)
{
	return( _Add_Range(pParent, Identifier, Name, Description,  true, Range_Min, Range_Max, 0.0, false, 0.0, false) );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Choice(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, const char *Items, int Default)
{
	CParameter			*pParameter;
	CParameter_Choice	*m_pData;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Choice, 0);

	m_pData		= (CParameter_Choice *)pParameter->m_pData;
	m_pData->Set_Items(Items);

	pParameter->Set_Value(Default);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_String(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, const char *String, bool bLongText, bool bPassword)
{
	return( _Add_String(pParent, Identifier, Name, Description, false, String, bLongText, bPassword) );
}

CParameter * CParameters::Add_Info_String(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, const char *String, bool bLongText)
{
	return( _Add_String(pParent, Identifier, Name, Description,  true, String, bLongText, false) );
}

//---------------------------------------------------------
CParameter * CParameters::Add_FilePath(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, const char *Filter, const char *Default, bool bSave, bool bDirectory, bool bMultiple)
{
	CParameter			*pParameter;
	CParameter_FilePath	*m_pData;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_FilePath, 0);

	m_pData		= (CParameter_FilePath *)pParameter->m_pData;
	m_pData->Set_Filter			(Filter);
	m_pData->Set_Flag_Save		(bSave);
	m_pData->Set_Flag_Multiple	(bMultiple);
	m_pData->Set_Flag_Directory	(bDirectory);

	pParameter->Set_Value((void *)Default);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Font(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, wxFont *pInit)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Font, 0);

	if( pInit )
	{
		pParameter->Set_Value(pInit);
	}

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Colors(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, CSG_Colors *pInit)
{
	CParameter			*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Colors, 0);

	pParameter->asColors()->Assign(pInit);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_FixedTable(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, CTable *pTemplate)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_FixedTable, 0);

	pParameter->asTable()->Create(pTemplate);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter * CParameters::Add_Grid_System(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, CGrid_System *pInit)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Grid_System, 0);

	if( pInit )
	{
		pParameter->asGrid_System()->Assign(*pInit);
	}

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Grid(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint, bool bSystem_Dependent, TGrid_Type Preferred_Type)
{
	CParameter	*pParameter;

	if( !pParent || pParent->Get_Type() != PARAMETER_TYPE_Grid_System )
	{
		if( bSystem_Dependent && m_pGrid_System )
		{
			pParent	= m_pGrid_System;
		}
		else
		{
			pParent	= Add_Grid_System(pParent, CSG_String::Format("%s_GRIDSYSTEM", Identifier), LNG("[PRM] Grid system"), "");
		}
	}

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Grid, Constraint);

	((CParameter_Grid *)pParameter->m_pData)->Set_Preferred_Type(Preferred_Type);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Grid_Output(CParameter *pParent, const char *Identifier, const char *Name, const char *Description)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CParameter_DataObject_Output *)pParameter->Get_Data())->Set_DataObject_Type(DATAOBJECT_TYPE_Grid);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Grid_List(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint, bool bSystem_Dependent)
{
	CParameter	*pParameter;

	if( bSystem_Dependent && (!pParent || pParent->Get_Type() != PARAMETER_TYPE_Grid_System) )
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
CParameter * CParameters::Add_Table_Field(CParameter *pParent, const char *Identifier, const char *Name, const char *Description)
{
	if( pParent
	&&	(	pParent->Get_Type() == PARAMETER_TYPE_Table
		||	pParent->Get_Type() == PARAMETER_TYPE_Shapes
		||	pParent->Get_Type() == PARAMETER_TYPE_TIN	) )
	{
		return( _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Table_Field, 0) );
	}

	return( NULL );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Table(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Table, Constraint);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Table_Output(CParameter *pParent, const char *Identifier, const char *Name, const char *Description)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CParameter_DataObject_Output *)pParameter->Get_Data())->Set_DataObject_Type(DATAOBJECT_TYPE_Table);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Table_List(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Table_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter * CParameters::Add_Shapes(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint, TShape_Type Shape_Type)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Shapes, Constraint);

	((CParameter_Shapes *)pParameter->m_pData)->Set_Shape_Type(Shape_Type);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Shapes_Output(CParameter *pParent, const char *Identifier, const char *Name, const char *Description)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CParameter_DataObject_Output *)pParameter->Get_Data())->Set_DataObject_Type(DATAOBJECT_TYPE_Shapes);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_Shapes_List(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint, TShape_Type Type)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Shapes_List, Constraint);

	((CParameter_Shapes_List *)pParameter->m_pData)->Set_Shape_Type(Type);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter * CParameters::Add_TIN(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_TIN, Constraint);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_TIN_Output(CParameter *pParent, const char *Identifier, const char *Name, const char *Description)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_DataObject_Output, PARAMETER_OUTPUT_OPTIONAL);

	((CParameter_DataObject_Output *)pParameter->Get_Data())->Set_DataObject_Type(DATAOBJECT_TYPE_TIN);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::Add_TIN_List(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_TIN_List, Constraint);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter * CParameters::Add_Parameters(CParameter *pParent, const char *Identifier, const char *Name, const char *Description)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Parameters, 0);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter * CParameters::_Add_Value(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, bool bInformation, TParameter_Type Type, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	CParameter			*pParameter;
	CParameter_Value	*m_pData;

	switch( Type )	// Check if Type is valid...
	{
	case PARAMETER_TYPE_Bool:
	case PARAMETER_TYPE_Int:
	case PARAMETER_TYPE_Double:
	case PARAMETER_TYPE_Degree:
	case PARAMETER_TYPE_Color:
		break;

	default:
		Type	= PARAMETER_TYPE_Double;	// if not valid set Type to [double]...
		break;
	}

	pParameter	= _Add(pParent, Identifier, Name, Description, Type, bInformation ? PARAMETER_INFORMATION : 0);

	if( !bInformation )
	{
		switch( Type )
		{
		default:
			break;

		case PARAMETER_TYPE_Int:
		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
			m_pData		= (CParameter_Value *)pParameter->m_pData;
			m_pData->Set_Minimum(Minimum, bMinimum);
			m_pData->Set_Maximum(Maximum, bMaximum);
			break;
		}
	}

	pParameter->Set_Value(Value);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::_Add_Range(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, bool bInformation, double Default_Min, double Default_Max, double Minimum, bool bMinimum, double Maximum, bool bMaximum)
{
	double				d;
	CParameter			*pParameter;
	CParameter_Range	*m_pData;

	//-----------------------------------------------------
	if( Default_Min > Default_Max )
	{
		d			= Default_Min;
		Default_Min	= Default_Max;
		Default_Max	= d;
	}

	//-----------------------------------------------------
	pParameter	= _Add(pParent, Identifier, Name, Description, PARAMETER_TYPE_Range, bInformation ? PARAMETER_INFORMATION : 0);

	m_pData		= pParameter->asRange();

	m_pData->Get_LoParm()->asValue()->Set_Minimum(Minimum, bMinimum);
	m_pData->Get_LoParm()->asValue()->Set_Maximum(Maximum, bMaximum);
	m_pData->Get_HiParm()->asValue()->Set_Minimum(Minimum, bMinimum);
	m_pData->Get_HiParm()->asValue()->Set_Maximum(Maximum, bMaximum);

	m_pData->Set_LoVal(Default_Min);
	m_pData->Set_HiVal(Default_Max);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::_Add_String(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, bool bInformation, const char *String, bool bLongText, bool bPassword)
{
	CParameter	*pParameter;

	pParameter	= _Add(pParent, Identifier, Name, Description, bLongText ? PARAMETER_TYPE_Text : PARAMETER_TYPE_String, bInformation ? PARAMETER_INFORMATION : 0);

	pParameter->Set_Value((void *)String);

	((CParameter_String *)pParameter)->Set_Password(bPassword);

	return( pParameter );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter * CParameters::_Add(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, TParameter_Type Type, int Constraint)
{
	CParameter	*pParameter;

	m_Parameters	= (CParameter **)SG_Realloc(m_Parameters, (m_nParameters + 1) * sizeof(CParameter *));

	pParameter		= m_Parameters[m_nParameters++]	= new CParameter(this, pParent, Identifier, Name, Description, Type, Constraint);

	return( pParameter );
}

//---------------------------------------------------------
CParameter * CParameters::_Add(CParameter *pSource)
{
	CParameter	*pParameter;

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
CParameter * CParameters::Get_Parameter(const char *Identifier)
{
	int		i;

	if( m_Parameters && Identifier )
	{
		for(i=0; i<m_nParameters; i++)
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
CParameter * CParameters::Get_Parameter(int iParameter)
{
	if( m_Parameters && iParameter >= 0 && iParameter < m_nParameters )
	{
		return( m_Parameters[iParameter] );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//						Callback						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CParameters::Set_Callback_On_Parameter_Changed(PFNC_Parameter_Changed Callback)
{
	m_Callback	= Callback;
}

//---------------------------------------------------------
void CParameters::Set_Callback(bool bActive)
{
	m_bCallback	= bActive;
}

//---------------------------------------------------------
bool CParameters::_On_Parameter_Changed(CParameter *pSender)
{
	if( m_Callback && m_bCallback )
	{
		m_bCallback	= false;
		m_Callback(pSender);
		m_bCallback	= true;

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
bool CParameters::Set_Parameter(const char *Identifier, CParameter *pSource)
{
	CParameter	*pTarget;

	if( pSource != NULL && (pTarget = Get_Parameter(Identifier)) != NULL && pSource->Get_Type() == pTarget->Get_Type() )
	{
		switch( pTarget->Get_Type() )
		{
		default:
			return( pTarget->Assign(pSource) );

		case PARAMETER_TYPE_DataObject_Output:
		case PARAMETER_TYPE_Grid:
		case PARAMETER_TYPE_Table:
		case PARAMETER_TYPE_Shapes:
		case PARAMETER_TYPE_TIN:
			return( pTarget->Set_Value(pSource->asDataObject()) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters::Set_Parameter(const char *Identifier, int Type, int Value)
{
	CParameter	*pTarget;

	if( (pTarget = Get_Parameter(Identifier)) != NULL && Type == pTarget->Get_Type() )
	{
		pTarget->Set_Value(Value);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters::Set_Parameter(const char *Identifier, int Type, double Value)
{
	CParameter	*pTarget;

	if( (pTarget = Get_Parameter(Identifier)) != NULL && Type == pTarget->Get_Type() )
	{
		pTarget->Set_Value(Value);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters::Set_Parameter(const char *Identifier, int Type, void *Value)
{
	CParameter	*pTarget;

	if( (pTarget = Get_Parameter(Identifier)) != NULL && Type == pTarget->Get_Type() )
	{
		pTarget->Set_Value(Value);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters::Set_Parameter(const char *Identifier, int Type, const char *Value)
{
	CParameter	*pTarget;

	if( (pTarget = Get_Parameter(Identifier)) != NULL && Type == pTarget->Get_Type() )
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
int CParameters::Assign(CParameters *pSource)
{
	int			i;
	CParameter	*pParameter;

	if( pSource )
	{
		Destroy();

		m_pOwner	= pSource->Get_Owner();

		Set_Identifier	(pSource->Get_Identifier());
		Set_Name		(pSource->Get_Name());
		Set_Description	(pSource->Get_Description());

		m_Callback	= pSource->m_Callback;
		m_bCallback	= pSource->m_bCallback;

		if( pSource->Get_Count() > 0 )
		{
			for(i=0; i<pSource->Get_Count(); i++)
			{
				_Add(pSource->Get_Parameter(i));
			}

			for(i=0; i<pSource->Get_Count(); i++)
			{
				if( Get_Parameter(i) && (pParameter = pSource->Get_Parameter(i)->Get_Parent()) != NULL )
				{
					Get_Parameter(i)->m_pParent	= Get_Parameter(pParameter->Get_Identifier());
				}
			}

			if( pSource->m_pGrid_System )
			{
				m_pGrid_System	= Get_Parameter(pSource->m_pGrid_System->Get_Identifier());
			}
		}

		return( m_nParameters );
	}

	return( -1 );
}

//---------------------------------------------------------
int CParameters::Assign_Values(CParameters *pSource)
{
	int			i, n;
	CParameter	*pParameter;

	if( pSource )
	{
		for(i=0, n=0; i<pSource->Get_Count(); i++)
		{
			pParameter	= Get_Parameter(pSource->Get_Parameter(i)->Get_Identifier());

			if( pParameter && pParameter->Get_Type() == pSource->Get_Parameter(i)->Get_Type() )
			{
				pParameter->Assign(pSource->Get_Parameter(i));
				n++;
			}
		}

		return( n );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//						Serialize						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PARAMETER_ENTRIES_BEGIN		"[PARAMETER_ENTRIES_BEGIN]"
#define PARAMETER_ENTRIES_END		"[PARAMETER_ENTRIES_END]"
#define PARAMETER_ENTRY_BEGIN		"[PARAMETER_ENTRY_BEGIN]"
#define PARAMETER_ENTRY_END			"[PARAMETER_ENTRY_END]"

//---------------------------------------------------------
bool CParameters::Serialize(const char *File_Name, bool bSave)
{
	bool	bResult	= false;
	FILE	*Stream;

	if( File_Name && (Stream = fopen(File_Name, bSave ? "wb" : "rb")) != NULL )
	{
		bResult	= Serialize(Stream, bSave);

		fclose(Stream);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CParameters::Serialize(FILE *Stream, bool bSave)
{
	int			i;
	CParameter	*pParameter;
	CSG_String	sLine;

	if( Stream )
	{
		//-------------------------------------------------
		if( bSave )
		{
			fprintf(Stream, "\n%s\n", PARAMETER_ENTRIES_BEGIN);

			for(i=0; i<m_nParameters; i++)
			{
				if(	m_Parameters[i]->is_Serializable() )
				{
					fprintf(Stream, "%s\n", PARAMETER_ENTRY_BEGIN);
					fprintf(Stream, "%s\n", m_Parameters[i]->m_Identifier.c_str());

					m_Parameters[i]->Serialize(Stream, true);

					fprintf(Stream, "%s\n", PARAMETER_ENTRY_END);
				}
			}

			fprintf(Stream, "%s\n", PARAMETER_ENTRIES_END);

			return( true );
		}

		//-------------------------------------------------
		else
		{
			while( SG_Read_Line(Stream, sLine) && sLine.Cmp(PARAMETER_ENTRIES_BEGIN) );

			if( !sLine.Cmp(PARAMETER_ENTRIES_BEGIN) )
			{
				while( SG_Read_Line(Stream, sLine) && sLine.Cmp(PARAMETER_ENTRIES_END) )
				{
					if( !sLine.Cmp(PARAMETER_ENTRY_BEGIN) )
					{
						if( SG_Read_Line(Stream, sLine) && (pParameter = Get_Parameter(sLine)) != NULL )
						{
							pParameter->Serialize(Stream, false);
						}
					}
				}
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CParameters::DataObjects_Check(bool bSilent)
{
	bool	bInvalid, bResult	= true;
	CSG_String	s;

	//-----------------------------------------------------
	for(int i=0; i<Get_Count(); i++)
	{
		switch( m_Parameters[i]->Get_Type() )
		{
		default:
			bInvalid	= false;
			break;

		case PARAMETER_TYPE_Parameters:
			bInvalid	= m_Parameters[i]->asParameters()->DataObjects_Check(bSilent);
			break;

		case PARAMETER_TYPE_Grid:
		case PARAMETER_TYPE_Table:
		case PARAMETER_TYPE_Shapes:
		case PARAMETER_TYPE_TIN:
			bInvalid	=  m_Parameters[i]->is_Input()				== true
						&& m_Parameters[i]->is_Optional()			== false
						&& m_Parameters[i]->asDataObject()			== NULL;
			break;

		case PARAMETER_TYPE_Grid_List:
		case PARAMETER_TYPE_Table_List:
		case PARAMETER_TYPE_Shapes_List:
		case PARAMETER_TYPE_TIN_List:
			bInvalid	=  m_Parameters[i]->is_Input()				== true
						&& m_Parameters[i]->is_Optional()			== false
						&& m_Parameters[i]->asList()->Get_Count()	== 0;
			break;
		}

		if( bInvalid )
		{
			bResult	= false;
			s.Append(CSG_String::Format("\n%s: %s", m_Parameters[i]->Get_Type_Name(), m_Parameters[i]->Get_Name()));
		}
	}

	//-----------------------------------------------------
	if( !bResult && !bSilent )
	{
		SG_UI_Dlg_Message(CSG_String::Format("%s\n%s", LNG("[DLG] Invalid parameters!"), s.c_str()), Get_Name());
	}

	return( bResult );
}

//---------------------------------------------------------
bool CParameters::DataObjects_Create(void)
{
	if( m_bManaged )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			CDataObject		*pDataObject;
			CGrid_System	*pGrid_System;
			CParameter		*p	= m_Parameters[i];

			if( p->Get_Type() == PARAMETER_TYPE_Parameters )
			{
				p->asParameters()->DataObjects_Create();
			}
			else if( p->Get_Type() == PARAMETER_TYPE_DataObject_Output )
			{
				p->Set_Value(DATAOBJECT_NOTSET);
			}
			else if( p->is_DataObject() && p->is_Output()
			&&	(	(p->asDataObject() == DATAOBJECT_CREATE)
				||	(p->asDataObject() == NULL && !p->is_Optional())	)	)
			{
				pDataObject	= NULL;

				switch( p->Get_Type() )
				{
				default:
					break;

				case PARAMETER_TYPE_Grid:
					if(	p->Get_Parent() && p->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System
					&&	(pGrid_System = p->Get_Parent()->asGrid_System()) != NULL && pGrid_System->is_Valid() )
					{
						pDataObject	= SG_Create_Grid(*pGrid_System, ((CParameter_Grid *)p->Get_Data())->Get_Preferred_Type());
					}
					break;

				case PARAMETER_TYPE_Table:
					pDataObject	= SG_Create_Table();
					break;

				case PARAMETER_TYPE_Shapes:
					pDataObject	= SG_Create_Shapes(((CParameter_Shapes *)p->Get_Data())->Get_Shape_Type());
					break;

				case PARAMETER_TYPE_TIN:
					pDataObject	= SG_Create_TIN();
					break;
				}

				p->Set_Value(pDataObject);

				if( pDataObject )
				{
					pDataObject->Set_Name(p->Get_Name());
					SG_UI_DataObject_Add(pDataObject, false);
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CParameters::DataObjects_Synchronize(void)
{
	if( m_bManaged )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			CParameter	*p	= m_Parameters[i];

			if( p->Get_Type() == PARAMETER_TYPE_Parameters )
			{
				p->asParameters()->DataObjects_Synchronize();
			}
			else
			{
				if( p->Get_Type() == PARAMETER_TYPE_Shapes && p->asShapes() && p->asShapes()->Get_Type() == SHAPE_TYPE_Undefined )
				{
					delete(p->asShapes());
					p->Set_Value(DATAOBJECT_NOTSET);
				}

				if( p->is_Output() )
				{
					if( p->is_DataObject() )
					{
						if( p->asDataObject() )
						{
							SG_UI_DataObject_Add		(p->asDataObject(), false);
							SG_UI_DataObject_Update	(p->asDataObject(), false, NULL);
						}
					}
					else if( p->is_DataObject_List() )
					{
						for(int j=0; j<p->asList()->Get_Count(); j++)
						{
							SG_UI_DataObject_Add		(p->asList()->asDataObject(j), false);
							SG_UI_DataObject_Update	(p->asList()->asDataObject(j), false, NULL);
						}
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
bool CParameters::Get_String(CSG_String &String, bool bOptionsOnly)
{
	bool	bResult	= false;

	if( Get_Count() > 0 )
	{
		String.Printf("%s:", bOptionsOnly ? LNG("[CAP] Options") : LNG("[CAP] Parameters"));

		for(int i=0; i<Get_Count(); i++)
		{
			if( (!bOptionsOnly || m_Parameters[i]->is_Option()) && !m_Parameters[i]->is_Information() )
			{
				bResult	= true;

				String.Append(CSG_String::Format("\n[%s] %s: %s",
					m_Parameters[i]->Get_Type_Name(),
					m_Parameters[i]->Get_Name(),
					m_Parameters[i]->asString())
				);
			}
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CParameters::Msg_String(bool bOptionsOnly)
{
	CSG_String	s;

	if( Get_String(s, bOptionsOnly) )
	{
		SG_UI_Msg_Add_Execution(s, true);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters::Set_History(CSG_History &History, bool bOptions, bool bDataObjects)
{
	int			i, j;
	CDataObject	*pObject;
	CParameter	*p;

	//-----------------------------------------------------
	if( bOptions )
	{
		for(i=0; i<Get_Count(); i++)	// get options...
		{
			p	= m_Parameters[i];

			if(	p->is_Option() && !p->is_Information() )
			{
				History.Add_Entry(p->Get_Name(), p->asString());
			}

			if( p->is_Parameters() )
			{
				p->asParameters()->Set_History(History, true, false);
			}
		}
	}

	//-----------------------------------------------------
	if( bDataObjects )
	{
		for(i=0; i<Get_Count(); i++)	// get input with history...
		{
			p	= m_Parameters[i];

			if( p->is_Input() && p->is_DataObject() && (pObject = p->asDataObject()) != NULL )
			{
				History.Add_Entry(p->Get_Name(), pObject->Get_Name(), &pObject->Get_History());
			}

			if( p->is_Input() && p->is_DataObject_List() )
			{
				History.Add_Entry(p->Get_Name(), p->asString());

				for(j=0; j<p->asList()->Get_Count(); j++)
				{
					pObject	= p->asList()->asDataObject(j);
					History.Add_Entry(p->Get_Name(), pObject->Get_Name(), &pObject->Get_History());
				}
			}

			if( p->is_Parameters() )
			{
				p->asParameters()->Set_History(History, false, true);
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
