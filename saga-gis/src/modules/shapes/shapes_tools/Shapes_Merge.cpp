/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Shapes_Merge.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Shapes_Merge.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Merge::CShapes_Merge(void) : CTables_Merge()
{
	Parameters.Del_Parameters();

	Set_Name		(_TL("Merge Layers"));

	Set_Author		("V.Olaya (c) 2004");

	Set_Description	(_TW(
		"Merge vector layers."
	));

	Parameters.Add_Shapes_List(
		NULL	, "INPUT"	, _TL("Layers"),
		_TL("Output will inherit shape type and table structure from the first layer in this list."),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "MERGED"	, _TL("Merged Layer"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "SRCINFO"	, _TL("Add Source Information"),
		_TL("Adds a field with the name of the original input data set."),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "MATCH"	, _TL("Match Fields by Name"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTables_Merge::CTables_Merge(void)
{
	Set_Name		(_TL("Merge Tables"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Merge tables."
	));

	Parameters.Add_Table_List(
		NULL	, "INPUT"	, _TL("Tables"),
		_TL("The resulting table inherits its field structure from the first table in this list."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "MERGED"	, _TL("Merged Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "SRCINFO"	, _TL("Add Source Information"),
		_TL("Adds a field with the name of the original input data set."),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "MATCH"	, _TL("Match Fields by Name"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTables_Merge::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_List	*pList	= Parameters("INPUT")->asList();

	if( pList->Get_Type() == PARAMETER_TYPE_Shapes_List )
	{
		int	n	= pList->Get_Count();

		for(int i=n-1; i>0; i--)
		{
			if( ((CSG_Shapes *)pList->asDataObject(0))->Get_Type() != ((CSG_Shapes *)pList->asDataObject(i))->Get_Type() )
			{
				pList->Del_Item(i);
			}
		}

		if( n > pList->Get_Count() )
		{
			Message_Add(CSG_String::Format(SG_T("%s [%d]"), _TL("incompatible items have been removed from input list"), n - pList->Get_Count()));
		}
	}

	if( pList->Get_Count() < 2 )
	{
		Error_Set(_TL("Nothing to do! Merging needs more than one input data set."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pMerged	= Parameters("MERGED")->asTable();

	if( pList->Get_Type() == PARAMETER_TYPE_Shapes_List )
	{
		((CSG_Shapes *)pMerged)->Create(*((CSG_Shapes *)pList->asDataObject(0)));
	}
	else // if( pList->Get_Type() == PARAMETER_TYPE_Table_List )
	{
		pMerged->Create(*((CSG_Table *)pList->asDataObject(0)));
	}

	pMerged->Set_Name(_TL("Merged Layers"));

	//-----------------------------------------------------
	bool	bInfo	= Parameters("SRCINFO")->asBool();

	if( bInfo )
	{
		pMerged->Add_Field(_TL("Source"), SG_DATATYPE_String, 0);

		for(int i=0; i<pMerged->Get_Count(); i++)
		{
			pMerged->Set_Value(i, 0, pList->asDataObject(0)->Get_Name());
		}
	}

	int		*Index	= NULL;

	//-----------------------------------------------------
	for(int iTable=1; iTable<pList->Get_Count() && Process_Get_Okay(); iTable++)
	{
		CSG_Table	*pTable	= (CSG_Table *)pList->asDataObject(iTable);

		//-------------------------------------------------
		if( Parameters("MATCH")->asBool() )	// see which fields are in both attributes tables
		{
			Index	= (int *)SG_Realloc(Index, pTable->Get_Field_Count() * sizeof(int));

			for(int i=0; i<pTable->Get_Field_Count(); i++)
			{
				CSG_String	Name(pTable->Get_Field_Name(i));

				Index[i]	= -1;

				for(int j=bInfo?1:0; Index[i]<0 && j<pMerged->Get_Field_Count(); j++)
				{
					if( !Name.CmpNoCase(pMerged->Get_Field_Name(j)) )
					{
						Index[i]	= j;
					}
				}
			}
		}

		//-------------------------------------------------
		for(int iRecord=0; iRecord<pTable->Get_Count(); iRecord++)
		{
			CSG_Table_Record	*pOutput, *pInput 	= pTable->Get_Record(iRecord);

			if( pMerged->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
			{
				pOutput	= ((CSG_Shapes *)pMerged)->Add_Shape(pInput, SHAPE_COPY_GEOM);
			}
			else // if( pMerged->Get_ObjectType() == DATAOBJECT_TYPE_Table )
			{
				pOutput	= pMerged->Add_Record();
			}

			if( bInfo )
			{
				pOutput->Set_Value(0, pTable->Get_Name());
			}

			if( Index )
			{
				for(int i=bInfo?1:0; i<pTable->Get_Field_Count(); i++)
				{
					if( Index[i] >= 0 )
					{
						*pOutput->Get_Value(Index[i])	= *pInput->Get_Value(i);
					}
				}
			}
			else // if( !Index )
			{
				for(int i=0, j=bInfo?1:0; i<pTable->Get_Field_Count() && j<pMerged->Get_Field_Count(); i++, j++)
				{
					*pOutput->Get_Value(j)	= *pInput->Get_Value(i);
				}
			}
		}
	}

	//-----------------------------------------------------
	SG_FREE_SAFE(Index);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
