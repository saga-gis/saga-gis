/**********************************************************
 * Version $Id$
 *********************************************************/
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

#define NAMING_NUMERIC	0
#define NAMING_FIELD	1

CSeparateShapes::CSeparateShapes(void)
{
	CSG_Parameter *pNode;

	Set_Name		(_TL("Separate Shapes"));
	Set_Author		(SG_T("(c) 2005 by Victor Olaya"));
	Set_Description	(_TW("Separate Shapes."));

	pNode = Parameters.Add_Shapes(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);
	
	Parameters.Add_FilePath(
		NULL	, "PATH"	, _TL("shp files folder"),
		_TL("shp files folder"),
		_TL(""), _TL(""), true, true
	);

	Parameters.Add_Choice(
		NULL	, "NAMING"	, _TL("File Naming"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("By number"),
			_TL("Use field content")
		), 0
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"	, _TL("Field for File Naming"),
		_TL("")
	);
}

CSeparateShapes::~CSeparateShapes(void)
{}

bool CSeparateShapes::On_Execute(void)
{
	int			iNaming, iField;
	CSG_String	sPath, sName, sFile;
	CSG_Shapes	*pShapes, *pShape;

	pShapes	= Parameters("SHAPES")	->asShapes();
	iNaming	= Parameters("NAMING")	->asInt();
	iField	= Parameters("FIELD")	->asInt();
	sPath	= Parameters("PATH")	->asString();

	if( SG_Dir_Create(sPath) )
	{
		for(int iShape=0; iShape<pShapes->Get_Count(); iShape++)
		{
			switch( iNaming )
			{
			case NAMING_NUMERIC:	default:
				sName.Printf(SG_T("%s_%04d"), SG_File_Get_Name(pShapes->Get_Name(), false).c_str(), iShape + 1);
				break;

			case NAMING_FIELD:
				sName.Printf(SG_T("%s_%s")	, SG_File_Get_Name(pShapes->Get_Name(), false).c_str(), pShapes->Get_Record(iShape)->asString(iField));
				break;
			}

			sFile	= SG_File_Make_Path(sPath, sName, SG_T("shp"));
			pShape	= SG_Create_Shapes(pShapes->Get_Type(), sName, pShapes);
			pShape	->Add_Shape(pShapes->Get_Shape(iShape));
			pShape	->Save(sFile);

			delete(pShape);
		}

		return( true );
	}

	return( false );
}
