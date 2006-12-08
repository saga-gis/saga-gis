/*******************************************************************************
    SeparateShapes.cpp
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

#include "SeparateShapes.h"

#define NAMING_NUMERIC 0
#define NAMING_FIELD 1

CSeparateShapes::CSeparateShapes(void){

	CSG_Parameter *pNode;
	
	Parameters.Set_Name(_TL("Separate Shapes"));
	Parameters.Set_Description(_TW("(c) 2005 by Victor Olaya."));

	pNode = Parameters.Add_Shapes(NULL, 
						"INPUT", 
						_TL("Shapes"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Table_Field(pNode,
								"FIELD",
								_TL("Field"),
								_TL(""));
	
	Parameters.Add_FilePath(NULL,
							"OUTPUTPATH",
							_TL("shp files folder"),
							_TL("shp files folder"),
							_TL(""),
							_TL(""),
							true,
							true);

	Parameters.Add_Choice(NULL, 
						"NAMING",
						_TL("File Naming"),
						_TL(""),
						_TW("By number|" 
						"Use field content|"), 
						0);

}//constructor


CSeparateShapes::~CSeparateShapes(void)
{}


bool CSeparateShapes::On_Execute(void){

	int i,j;
	int iNaming = Parameters("NAMING")->asInt();
	int iField = Parameters("FIELD")->asInt();
	CSG_Shapes *pInput = Parameters("INPUT")->asShapes();
	CSG_Shapes *pOutput; 
	CSG_String sPath = Parameters("OUTPUTPATH")->asString();;
	CSG_String sFilename;
	CSG_String sName;
	CSG_Table *pTable;

	pTable	= &pInput->Get_Table();

	if( !SG_Dir_Create(sPath) )
	{
		return( false );
	}
 
	for (i = 0; i < pInput->Get_Count(); i++){		
		if (iNaming == NAMING_NUMERIC){
			sName.Printf(SG_T("%s_%s"), pInput->Get_Name(), SG_Get_String(i).c_str());
		}//if
		else{
			sName.Printf(SG_T("%s"), pTable->Get_Record(i)->asString(iField));
		}//else

		pOutput = SG_Create_Shapes(pInput->Get_Type());
		pOutput->Set_Name(sName.c_str());

		sFilename	= SG_File_Make_Path(sPath, sName, SG_T("shp"));
		
		for (j=0; j<pTable->Get_Field_Count(); j++){
			pOutput->Get_Table().Add_Field(pTable->Get_Field_Name(j), pTable->Get_Field_Type(j));	
		}//for

		pOutput->Add_Shape()->Assign(pInput->Get_Shape(j));
		pOutput->Save(sFilename.c_str());

		delete pOutput;
	}//for
	
	return true;

}//method
