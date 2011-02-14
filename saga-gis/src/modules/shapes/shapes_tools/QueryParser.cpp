/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    QueryParser.cpp
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

#include "QueryParser.h"

CQueryParser::CQueryParser(CSG_Shapes *pShapes, CSG_String sExpression){

	CSG_Formula Formula;
	int i,j;
	int iFields;
	CSG_Table *pTable;
	CSG_Table_Record *pRecord;
	double *pFieldValues;
	double dValue;

	m_bInit	= false;
	pTable	= pShapes;
	iFields = pTable->Get_Field_Count();

	//-----------------------------------------------------
	if( !Formula.Set_Formula(sExpression.c_str()) )
	{
		int			Position;
		CSG_String	Message, s;

		s	+= _TL("Error in formula");
		s	+= SG_T("\n") + Formula.Get_Formula();

		if( Formula.Get_Error(&Position, &Message) )
		{
			s	+= SG_T("\n") + Message;
			s	+= CSG_String::Format(SG_T("\n%s: %d"), _TL("Position") , Position);

			if( Position >= 0 && Position < (int)Formula.Get_Formula().Length() )
			{
				s	+= SG_T("\n")
					+  Formula.Get_Formula().Left(Position - 1) + SG_T("[")
					+  Formula.Get_Formula()[Position] + SG_T("]")
					+  Formula.Get_Formula().Right(Formula.Get_Formula().Length() - (Position + 1));
			}
		}

		SG_UI_Msg_Add_Error(s);
	}
	else
	{
		pFieldValues= new double[iFields];

		for (i = 0; i < pTable->Get_Record_Count(); i++){
			pRecord = pTable->Get_Record(i);
			for (j = 0; j < iFields; j++){
				pFieldValues[j] = pRecord->asDouble(j);
			}//for
			dValue = Formula.Get_Value(pFieldValues, iFields);
			if (dValue){
				m_pSelectedRecords.push_back(i);
			}//if
		}//for

		m_bInit = true;
	}
}//constructor

CQueryParser::~CQueryParser(){

	//delete [] m_pSelectedFields;

}//destructor

int& CQueryParser::GetSelectedRecords(){

	return m_pSelectedRecords[0];

}//method

int CQueryParser::GetSelectedRecordsCount(){

	return m_pSelectedRecords.size();
	//m_iSelectedFieldsCount;

}//method


bool CQueryParser::is_Initialized()
{
	return (m_bInit);
}