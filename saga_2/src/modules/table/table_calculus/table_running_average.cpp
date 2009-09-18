
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    Table_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               table_running_average.cpp               //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "table_running_average.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Running_Average::CTable_Running_Average(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Running Average"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		"\n"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Table(
		NULL	, "INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "COUNT"		, _TL("Number of Records"),
		_TL(""),
		PARAMETER_TYPE_Int, 10, 2, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Running_Average::On_Execute(void)
{
	int			iValue, nValues;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable	= Parameters("INPUT")	->asTable();
	iValue	= Parameters("FIELD")	->asInt();
	nValues	= Parameters("COUNT")	->asInt();

	if( Parameters("OUTPUT")->asTable() && Parameters("OUTPUT")->asTable() != pTable )
	{
		pTable	= Parameters("OUTPUT")	->asTable();

		pTable->Create(*Parameters("INPUT")->asTable());
	}

	//-----------------------------------------------------
	if( pTable->is_Valid() )
	{
		int		i, iLo, iHi, nRange, iAverage;
		double	sValues;

		iAverage	= pTable->Get_Field_Count();
		pTable->Add_Field(CSG_String::Format(SG_T("%s [%s]"), pTable->Get_Field_Name(iValue), _TL("Average")), SG_DATATYPE_Double);

		nRange	= nValues / 2;
		sValues	= 0.0;

		for(iLo=-nValues, i=-nRange, iHi=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count() + nRange); iLo++, i++, iHi++)
		{
			sValues	+= pTable->Get_Record(iHi < pTable->Get_Count() ? iHi : pTable->Get_Count() - 1)->asDouble(iValue);

			if( i < 0 )
			{
				sValues	+= pTable->Get_Record( 0 )->asDouble(iValue);
			}
			else
			{
				if( iLo < 0 )
				{
					sValues	-= pTable->Get_Record( 0 )->asDouble(iValue);
				}
				else if( iLo >= 0 )
				{
					sValues	-= pTable->Get_Record(iLo)->asDouble(iValue);
				}

				pTable->Get_Record(i)->Set_Value(iAverage, sValues / (double)nValues);
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
