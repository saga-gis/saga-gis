
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  grid_collection.cpp                  //
//                                                       //
//                 Copyright (C) 2017 by                 //
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
#include "grid_collection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_ID_NAME(i)	CSG_String::Format("NAME%d", i)
#define GET_ID_TYPE(i)	CSG_String::Format("TYPE%d", i)


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Create::CGrids_Create(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Create a Grid Collection"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"Create a new grid collection from existing grids."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"LIST"		, _TL("Single Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grids("",
		"GRIDS"		, _TL("Grid Collection"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_String("",
		"NAME"		, _TL("Name"),
		_TL(""),
		""
	);

	Parameters.Add_Bool("LIST",
		"DELETE"	, _TL("Delete"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"ATTRIBUTES", _TL("Attribute Definition"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("index and name"),
			_TL("user defined structure"),
			_TL("table with values"),
			_TL("copy from other grid collection")
		), 0
	);

	Parameters.Add_Table("ATTRIBUTES",
		"TABLE"		, _TL("Attributes"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field("TABLE",
		"TABLE_Z"	, _TL("Z Attribute"),
		_TL("")
	);

	Parameters.Add_Int("ATTRIBUTES",
		"NFIELDS"	, _TL("Number of Attributes"),
		_TL(""),
		2, 0, true
	);

	Parameters.Add_Int("NFIELDS",
		"ZFIELD"	, _TL("Z Attribute"),
		_TL(""),
		1, 1, true
	);

	Parameters.Add_Parameters("NFIELDS",
		"FIELDS"	, _TL("Attributes"),
		_TL("")
	);

	Set_Field_Count(Parameters("FIELDS")->asParameters(), Parameters("NFIELDS")->asInt());

	Parameters("FIELDS")->asParameters()->Get_Parameter(GET_ID_NAME(0))->Set_Value("ID");
	Parameters("FIELDS")->asParameters()->Get_Parameter(GET_ID_TYPE(0))->Set_Value( 8  );	// int

	Parameters.Add_Grids("ATTRIBUTES",
		"COPY"		, _TL("Copy from Grid Collection"),
		_TL(""),
		PARAMETER_INPUT, false
	)->ignore_Projection(true);

	Parameters.Add_Bool("COPY",
		"COPY_SET"	, _TL("Copy Settings"),
		_TL(""),
		true
	)->Set_UseInCMD(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrids_Create::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "LIST") )
	{
		if( pParameter->asList()->Get_Item_Count() > 0 )
		{
			pParameters->Get("NAME")->Set_Value(pParameter->asList()->Get_Item(0)->Get_Name());
		}
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NFIELDS") && pParameter->asInt() > 0 )
	{
		Set_Field_Count(pParameters->Get("FIELDS")->asParameters(), pParameter->asInt());

		return( true );
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrids_Create::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "ATTRIBUTES") )
	{
		pParameters->Set_Enabled("NFIELDS", pParameter->asInt() == 1);
		pParameters->Set_Enabled("TABLE"  , pParameter->asInt() == 2);

		pParameters->Set_Enabled(pParameters->Get("COPY")->Get_Parent()->Get_Identifier(), pParameter->asInt() == 3);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NFIELDS") )
	{
		pParameters->Set_Enabled("ZFIELD" , pParameter->asInt() > 0);
		pParameters->Set_Enabled( "FIELDS", pParameter->asInt() > 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrids_Create::Set_Field_Count(CSG_Parameters *pFields, int nFields)
{
	static const CSG_String	Types(
		SG_Data_Type_Get_Name(SG_DATATYPE_String) + "|" + //  0
		SG_Data_Type_Get_Name(SG_DATATYPE_Date  ) + "|" + //  1
		SG_Data_Type_Get_Name(SG_DATATYPE_Color ) + "|" + //  2
		SG_Data_Type_Get_Name(SG_DATATYPE_Byte  ) + "|" + //  3
		SG_Data_Type_Get_Name(SG_DATATYPE_Char  ) + "|" + //  4
		SG_Data_Type_Get_Name(SG_DATATYPE_Word  ) + "|" + //  5
		SG_Data_Type_Get_Name(SG_DATATYPE_Short ) + "|" + //  6
		SG_Data_Type_Get_Name(SG_DATATYPE_DWord ) + "|" + //  7
		SG_Data_Type_Get_Name(SG_DATATYPE_Int   ) + "|" + //  8
		SG_Data_Type_Get_Name(SG_DATATYPE_ULong ) + "|" + //  9
		SG_Data_Type_Get_Name(SG_DATATYPE_Long  ) + "|" + // 10
		SG_Data_Type_Get_Name(SG_DATATYPE_Float ) + "|" + // 11
		SG_Data_Type_Get_Name(SG_DATATYPE_Double) + "|" + // 12
		SG_Data_Type_Get_Name(SG_DATATYPE_Binary) + "|"   // 13
	);

	if( pFields && nFields >= 0 )
	{
		int		nCurrent	= pFields->Get_Count() / 2;

		if( nCurrent < nFields )
		{
			for(int i=nCurrent; i<nFields; i++)
			{
				pFields->Add_String(""            , GET_ID_NAME(i), _TL("Name"), _TL(""), _TL("Value"));
				pFields->Add_Choice(GET_ID_NAME(i), GET_ID_TYPE(i), _TL("Type"), _TL(""), Types, 11);	// float
			}
		}
		else if( nCurrent > nFields )
		{
			for(int i=nCurrent-1; i>=nFields; i--)
			{
				pFields->Del_Parameter(i);
			}
		}
	}
}

//---------------------------------------------------------
CSG_String CGrids_Create::Get_Field_Name(int iField)
{
	return( Parameters("FIELDS")->asParameters()->Get_Parameter(GET_ID_NAME(iField))->asString() );
}

//---------------------------------------------------------
TSG_Data_Type CGrids_Create::Get_Field_Type(int iField)
{
	switch( Parameters("FIELDS")->asParameters()->Get_Parameter(GET_ID_TYPE(iField))->asInt() )
	{
	default: return( SG_DATATYPE_String );
	case  1: return( SG_DATATYPE_Date   );
	case  2: return( SG_DATATYPE_Color  );
	case  3: return( SG_DATATYPE_Byte   );
	case  4: return( SG_DATATYPE_Char   );
	case  5: return( SG_DATATYPE_Word   );
	case  6: return( SG_DATATYPE_Short  );
	case  7: return( SG_DATATYPE_DWord  );
	case  8: return( SG_DATATYPE_Int    );
	case  9: return( SG_DATATYPE_ULong  );
	case 10: return( SG_DATATYPE_Long   );
	case 11: return( SG_DATATYPE_Float  );
	case 12: return( SG_DATATYPE_Double );
	case 13: return( SG_DATATYPE_Binary );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Create::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pList	= Parameters("LIST")->asGridList();

	if( pList->Get_Grid_Count() <= 0 )
	{
		Error_Set(_TL("empty input grid list"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable, Table;

	int	zField;

	switch( Parameters("ATTRIBUTES")->asInt() )
	{
	default:	// index and name
		{
			pTable	= &Table;
			zField	= 0;

			Table.Add_Field("ID"  , SG_DATATYPE_Int   );
			Table.Add_Field("NAME", SG_DATATYPE_String);

			for(int i=0; i<pList->Get_Grid_Count(); i++)
			{
				CSG_Table_Record	*pRecord	= Table.Add_Record();

				pRecord->Set_Value(0, i + 1);
				pRecord->Set_Value(1, pList->Get_Grid(i)->Get_Name());
			}
		}
		break;

	case  1:	// user defined structure
		{
			pTable	= &Table;
			zField	= Parameters("ZFIELD")->asInt();

			for(int i=0; i<Parameters("NFIELDS")->asInt(); i++)
			{
				Table.Add_Field(Get_Field_Name(i), Get_Field_Type(i));
			}
		}
		break;

	case  2:	// table with values
		{
			pTable	= Parameters("TABLE")->asTable();
		}

	case  3:	// copy from other grid collection
		{
			pTable	= Parameters("COPY")->asGrids()->Get_Attributes_Ptr();
			zField	= Parameters("COPY")->asGrids()->Get_Z_Attribute();
		}
		break;
	}

	//-----------------------------------------------------
	CSG_Grids	*pGrids	= Parameters("GRIDS")->asGrids();

	if( pTable->Get_Count() == pList->Get_Grid_Count() )
	{
		pGrids->Create(pList->Get_Grid(0)->Get_System(), *pTable, zField, pList->Get_Grid(0)->Get_Type());
	}
	else
	{
		pGrids->Create(pList->Get_Grid(0)->Get_System());
	}

	if( !pGrids->is_Valid() )
	{
		Error_Set(_TL("failed to create grid collection"));

		return( false );
	}

	pGrids->Set_Name(Parameters("NAME")->asString());

	//-----------------------------------------------------
	bool	bDelete	= Parameters("DELETE")->asBool();

	if( bDelete )
	{
		for(int i=0; i<pList->Get_Item_Count(); i++)
		{
			if( Parameters.Get_Manager() )
			{
				Parameters.Get_Manager()->Delete(pList->Get_Item(i), true);

				DataObject_Update(pList->Get_Item(i));
			}

			if( pList->Get_Item(i)->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
			{
				((CSG_Grids *)pList->Get_Item(i))->Del_Grids(true);

				delete(pList->Get_Item(i));
			}
		}
	}

	for(int i=0; i<pList->Get_Grid_Count() && Set_Progress(i, pList->Get_Grid_Count()); i++)
	{
		if( pTable && i < pTable->Get_Count() )
		{
			pGrids->Add_Grid(*pTable->Get_Record_byIndex(i), pList->Get_Grid(i), bDelete);
		}
		else
		{
			pGrids->Add_Grid(i, pList->Get_Grid(i), bDelete);
		}
	}

	if( bDelete )
	{
		pList->Del_Items();
	}

	//-----------------------------------------------------
	if( Parameters("ATTRIBUTES")->asInt() == 3 && Parameters("COPY_SET")->asBool() )
	{
		DataObject_Set_Parameters(pGrids, Parameters("COPY")->asGrids());
	}
	else
	{
		DataObject_Set_Parameter(pGrids, "BAND_R", 0);
		DataObject_Set_Parameter(pGrids, "BAND_G", 1);
		DataObject_Set_Parameter(pGrids, "BAND_B", 2);
	}

	pGrids->Set_Name(Parameters("NAME")->asString());

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Extract::CGrids_Extract(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Extract Grids from a Grid Collection"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"LIST"		, _TL("Single Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grids("",
		"GRIDS"		, _TL("Grid Collection"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choices("",
		"SELECTION"	, _TL("Selection"),
		_TL(""),
		""
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrids_Extract::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "GRIDS") )
	{
		CSG_Parameter_Choices	*pChoices	= pParameters->Get("SELECTION")->asChoices();

		pChoices->Del_Items();

		for(int i=0; pParameter->asGrids() && i<pParameter->asGrids()->Get_Grid_Count(); i++)
		{
			pChoices->Add_Item(pParameter->asGrids()->Get_Grid_Name(i));
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrids_Extract::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Extract::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pList	= Parameters("LIST")->asGridList();

	pList->Del_Items();

	CSG_Grids	*pGrids	= Parameters("GRIDS")->asGrids();

	CSG_Parameter_Choices	*pSelection	= Parameters("SELECTION")->asChoices();

	if( pSelection->Get_Selection_Count() > 0 )
	{
		for(int i=0; i<pSelection->Get_Selection_Count() && Set_Progress(i, pSelection->Get_Selection_Count()); i++)
		{
			CSG_Grid	*pGrid	= SG_Create_Grid(pGrids->Get_Grid(pSelection->Get_Selection_Index(i)));

			pGrid->Set_Name(pGrids->Get_Grid_Name(pSelection->Get_Selection_Index(i)));

			pList->Add_Item(pGrid);
		}
	}
	else
	{
		for(int i=0; i<pGrids->Get_Grid_Count() && Set_Progress(i, pGrids->Get_Grid_Count()); i++)
		{
			CSG_Grid	*pGrid	= SG_Create_Grid(pGrids->Get_Grid(i));

			pGrid->Set_Name(pGrids->Get_Grid_Name(i));

			pList->Add_Item(pGrid);
		}
	}

	//-----------------------------------------------------
	return( pList->Get_Grid_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Delete::CGrids_Delete(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Delete Grids from a Grid Collection"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grids("",
		"GRIDS"		, _TL("Grid Collection"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choices("",
		"SELECTION"	, _TL("Selection"),
		_TL(""),
		""
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrids_Delete::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "GRIDS") )
	{
		CSG_Parameter_Choices	*pChoices	= pParameters->Get("SELECTION")->asChoices();

		pChoices->Del_Items();

		for(int i=0; pParameter->asGrids() && i<pParameter->asGrids()->Get_Grid_Count(); i++)
		{
			pChoices->Add_Item(pParameter->asGrids()->Get_Grid_Name(i));
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrids_Delete::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Delete::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Choices	*pSelection	= Parameters("SELECTION")->asChoices();

	if( pSelection->Get_Selection_Count() <= 0 )
	{
		Error_Set(_TL("No grids in selection"));

		return( false );
	}

	if( pSelection->Get_Selection_Count() >= pSelection->Get_Item_Count() )
	{
		Error_Set(_TL("It is not allowed to remove all grids from a grid collection."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grids	*pGrids	= Parameters("GRIDS")->asGrids();

	for(int i=pGrids->Get_Grid_Count()-1; i>=0 && Process_Get_Okay(); i--)
	{
		if( pSelection->is_Selected(i) )
		{
			pGrids->Del_Grid(i);
		}
	}

	On_Parameter_Changed(&Parameters, Parameters("GRIDS"));

	DataObject_Update(pGrids);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
