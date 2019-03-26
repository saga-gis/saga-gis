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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
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
		"", "INPUT"		, _TL("Layers"),
		_TL("Output will inherit shape type and table structure from the first layer in this list."),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		"", "MERGED"	, _TL("Merged Layer"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool(
		"", "SRCINFO"	, _TL("Add Source Information"),
		_TL("Adds a field with the name of the original input data set."),
		true
	);

	Parameters.Add_Bool(
		"", "MATCH"		, _TL("Match Fields by Name"),
		_TL(""),
		true
	);

	Parameters.Add_Bool(
		"", "DELETE"	, _TL("Delete"),
		_TL("Deletes each input data set immediately after it has been merged, thus saving memory resources."),
		false
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
		"", "INPUT"		, _TL("Tables"),
		_TL("The resulting table inherits its field structure from the first table in this list."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		"", "MERGED"	, _TL("Merged Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool(
		"", "SRCINFO"	, _TL("Add Source Information"),
		_TL("Adds a field with the name of the original input data set."),
		true
	);

	Parameters.Add_Bool(
		"", "MATCH"		, _TL("Match Fields by Name"),
		_TL(""),
		true
	);

	Parameters.Add_Bool(
		"", "DELETE"	, _TL("Delete"),
		_TL("Deletes each input data set immediately after it has been merged, thus saving memory resources."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTables_Merge::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_List	*pList	= Parameters("INPUT")->asList();

	if( pList->Get_Type() == PARAMETER_TYPE_Shapes_List )
	{
		int	n	= pList->Get_Item_Count();

		for(int i=n-1; i>0; i--)
		{
			if( ((CSG_Shapes *)pList->Get_Item(0))->Get_Type() != ((CSG_Shapes *)pList->Get_Item(i))->Get_Type() )
			{
				pList->Del_Item(i);
			}
		}

		if( n > pList->Get_Item_Count() )
		{
			Message_Fmt("\n%s [%d]", _TL("incompatible items have been removed from input list"), n - pList->Get_Item_Count());
		}
	}

	if( pList->Get_Item_Count() < 2 )
	{
		Error_Set(_TL("Nothing to do! Merging needs more than one input data set."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pMerged	= Parameters("MERGED")->asTable();

	if( pMerged->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
	{
		pMerged->asShapes()->Create(*((CSG_Shapes *)pList->Get_Item(0)));
	}
	else // if( pMerged->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table )
	{
		pMerged->asTable ()->Create(*((CSG_Table  *)pList->Get_Item(0)));
	}

	pMerged->Set_Name(_TL("Merged Layers"));

	bool	bInfo	= Parameters("SRCINFO")->asBool();

	if( bInfo )
	{
		pMerged->Add_Field(_TL("Source"), SG_DATATYPE_String, 0);

		for(int i=0; i<pMerged->Get_Count(); i++)
		{
			pMerged->Set_Value(i, 0, pList->Get_Item(0)->Get_Name());
		}
	}

	//-----------------------------------------------------
	bool	bDelete	= Parameters.Get_Manager() && Parameters("DELETE")->asBool();

	if( bDelete )
	{
		((CSG_Table *)pList->Get_Item(0))->Del_Records();
	}

	//-----------------------------------------------------
	for(int iTable=1; iTable<pList->Get_Item_Count() && Set_Progress(iTable, pList->Get_Item_Count()); iTable++)
	{
		CSG_Table	*pTable	= (CSG_Table *)pList->Get_Item(iTable);

		//-------------------------------------------------
		CSG_Array_Int	Index;

		if( Parameters("MATCH")->asBool() )	// see which fields are in both attributes tables
		{
			Index.Create(pTable->Get_Field_Count());

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

			if( pMerged->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
			{
				pOutput	= ((CSG_Shapes *)pMerged)->Add_Shape(pInput, SHAPE_COPY_GEOM);
			}
			else // if( pMerged->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table )
			{
				pOutput	= pMerged->Add_Record();
			}

			if( bInfo )
			{
				pOutput->Set_Value(0, pTable->Get_Name());
			}

			if( Index.Get_Size() > 0 )
			{
				for(int i=0; i<pTable->Get_Field_Count(); i++)
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

		//-------------------------------------------------
		if( bDelete )
		{
			pTable->Del_Records();
		}
	}

	//-----------------------------------------------------
	if( bDelete )
	{
		for(int i=0; i<pList->Get_Item_Count(); i++)
		{
			CSG_Data_Object	*pObject	= pList->Get_Item(i);

			Parameters.Get_Manager()->Delete(pObject, true);

			DataObject_Update(pObject);

			switch( pObject->Get_ObjectType() )
			{
			case SG_DATAOBJECT_TYPE_Table     : delete(pObject->asTable     ()); break;
			case SG_DATAOBJECT_TYPE_Shapes    : delete(pObject->asShapes    ()); break;
			case SG_DATAOBJECT_TYPE_PointCloud: delete(pObject->asPointCloud()); break;
			case SG_DATAOBJECT_TYPE_TIN       : delete(pObject->asTIN       ()); break;
			default:	break;
			}
		}

		pList->Del_Items();

		DataObject_Add(pMerged);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
