
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                imagery_classification                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  change_detection.cpp                 //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#include "change_detection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CChange_Detection::CChange_Detection(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Change Detection"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Change detection for classified grids."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INITIAL"		, _TL("Initial State"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "FINAL"		, _TL("Final State"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHANGE"		, _TL("Changes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	pNode	= Parameters.Add_Value(
		NULL	, "NOCHANGE"	, _TL("Report Unchanged Classes"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Table(
		NULL	, "STATS"		, _TL("Changes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	pNode	= Parameters.Add_Table(
		NULL	, "CLASSES"		, _TL("Look-up Table"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field(
		pNode	, "CLASSES_MIN"	, _TL("Value"),
		_TL(""),
		false
	);

	Parameters.Add_Table_Field(
		pNode	, "CLASSES_MAX"	, _TL("Value (Maximum)"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "CLASSES_NAM"	, _TL("Value (High)"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChange_Detection::On_Execute(void)
{
	bool		bNoChange;
	CSG_Grid	*pInitial, *pFinal, *pChange;

	//-----------------------------------------------------
	pInitial	= Parameters("INITIAL")	->asGrid();
	pFinal		= Parameters("FINAL")	->asGrid();
	pChange		= Parameters("CHANGE")	->asGrid();
	m_pChanges	= Parameters("STATS")	->asTable();
	bNoChange	= Parameters("NOCHANGE")->asBool();

	if( !Initialise() )
	{
		Error_Set(_TL("initialsation failed"));

		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int	iInitial	= Get_Class(pInitial->asDouble(x, y));
			int	iFinal		= Get_Class(pFinal  ->asDouble(x, y));

			if( bNoChange || iInitial != iFinal )
			{
				m_pChanges->Get_Record(iInitial)->Add_Value(1 + iFinal, 1);

				pChange->Set_Value(x, y, m_pChanges->Get_Count() * iInitial + iFinal);
			}
			else
			{
				pChange->Set_Value(x, y, -1);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pChange, P) && P("COLORS_TYPE") && P("LUT") )
	{
		CSG_Table	*pLUT	= P("LUT")->asTable();

		CSG_Colors	cRandom(m_pChanges->Get_Count());

		cRandom.Random();

		pLUT->Del_Records();

		for(int iInitial=0; iInitial<m_pChanges->Get_Count(); iInitial++)
		{
			CSG_Colors	cRamp(m_pChanges->Get_Count());

			cRamp.Set_Ramp(cRandom[iInitial], cRandom[iInitial]);
			cRamp.Set_Ramp_Brighness(225, 50);

			for(int iFinal=0; iFinal<m_pChanges->Get_Count(); iFinal++)
			{
				if( m_pChanges->Get_Record(iInitial)->asInt(1 + iFinal) > 0 )
				{
					CSG_Table_Record	*pClass	= pLUT->Add_Record();

					pClass->Set_Value(0, cRamp.Get_Color(iFinal));
					pClass->Set_Value(1, CSG_String::Format(SG_T("%s >> %s"), m_pChanges->Get_Record(iInitial)->asString(0), m_pChanges->Get_Record(iFinal)->asString(0)));
					pClass->Set_Value(3, m_pChanges->Get_Count() * iInitial + iFinal);
					pClass->Set_Value(4, m_pChanges->Get_Count() * iInitial + iFinal);
				}
			}
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pChange, P);
	}

	//-----------------------------------------------------
	m_pChanges	->Set_Name(CSG_String::Format(SG_T("%s [%s >> %s]"), _TL("Changes"), pInitial->Get_Name(), pFinal->Get_Name()));

	pChange		->Set_Name(CSG_String::Format(SG_T("%s [%s >> %s]"), _TL("Changes"), pInitial->Get_Name(), pFinal->Get_Name()));
	pChange		->Set_NoData_Value(-1);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	CLASS_ID	= 0,
	CLASS_NAME,
	CLASS_MIN,
	CLASS_MAX
};

//---------------------------------------------------------
bool CChange_Detection::Initialise(void)
{
	CSG_Table	*pClasses;

	m_Classes.Destroy();

	m_Classes.Add_Field(_TL("ID")	, SG_DATATYPE_Int);
	m_Classes.Add_Field(_TL("NAME")	, SG_DATATYPE_String);
	m_Classes.Add_Field(_TL("MIN")	, SG_DATATYPE_Double);
	m_Classes.Add_Field(_TL("MAX")	, SG_DATATYPE_Double);

	//-----------------------------------------------------
	if( (pClasses = Parameters("CLASSES")->asTable()) != NULL )
	{
		int	fNam	= Parameters("CLASSES_NAM")->asInt();
		int	fMin	= Parameters("CLASSES_MIN")->asInt();
		int	fMax	= Parameters("CLASSES_MAX")->asInt();

		if( fNam < 0 || fNam >= pClasses->Get_Field_Count() )	{	fNam	= fMin;	}
		if( fMax < 0 || fMax >= pClasses->Get_Field_Count() )	{	fMax	= fMin;	}

		for(int iClass=0; iClass<pClasses->Get_Count(); iClass++)
		{
			CSG_Table_Record	*pClass	= m_Classes.Add_Record();

			pClass->Set_Value(0, m_Classes.Get_Count());
			pClass->Set_Value(1, pClasses->Get_Record(iClass)->asString(fNam));
			pClass->Set_Value(2, pClasses->Get_Record(iClass)->asDouble(fMin));
			pClass->Set_Value(3, pClasses->Get_Record(iClass)->asDouble(fMax));
		}
	}

	//-----------------------------------------------------
	else if( DataObject_Get_Parameter(Parameters("INITIAL")->asGrid(), "LUT") )
	{
		pClasses	= DataObject_Get_Parameter(Parameters("INITIAL")->asGrid(), "LUT")->asTable();

		for(int iClass=0; iClass<pClasses->Get_Count(); iClass++)
		{
			CSG_Table_Record	*pClass	= m_Classes.Add_Record();

			pClass->Set_Value(0, m_Classes.Get_Count());
			pClass->Set_Value(1, pClasses->Get_Record(iClass)->asString(1));
			pClass->Set_Value(2, pClasses->Get_Record(iClass)->asDouble(3));
			pClass->Set_Value(3, pClasses->Get_Record(iClass)->asDouble(4));
		}
	}

	//-----------------------------------------------------
	else
	{
		double		z;
		CSG_Grid	*pGrid	= Parameters("INITIAL")->asGrid();

		for(int i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
		{
			double iz	= pGrid->asDouble(pGrid->Get_Sorted(i, false, false));

			if( i == 0 || iz != z )
			{
				CSG_Table_Record	*pClass	= m_Classes.Add_Record();

				pClass->Set_Value(0, m_Classes.Get_Count());
				pClass->Set_Value(1, z = iz);
				pClass->Set_Value(2, z);
				pClass->Set_Value(3, z);
			}
		}
	}

	//-----------------------------------------------------
	if( m_Classes.Get_Count() == 0 )
	{
		return( false );
	}

	m_pChanges->Destroy();

	m_pChanges->Add_Field(_TL("NAME"), SG_DATATYPE_String);

	for(int iFinal=0; iFinal<m_Classes.Get_Count(); iFinal++)
	{
		m_pChanges->Add_Field(m_Classes[iFinal].asString(1), SG_DATATYPE_Int);
	}

	m_pChanges->Add_Field(_TL("UNCLASSIFIED"), SG_DATATYPE_Int);

	for(int iInitial=0; iInitial<m_Classes.Get_Count(); iInitial++)
	{
		m_pChanges->Add_Record()->Set_Value(0, m_Classes[iInitial].asString(1));
	}

	m_pChanges->Add_Record()->Set_Value(0, _TL("UNCLASSIFIED"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CChange_Detection::Cmp_Class(double Value, int iClass)
{
	CSG_Table_Record	*pClass	= m_Classes.Get_Record_byIndex(iClass);

	double	min	= pClass->asDouble(CLASS_MIN);

	if( Value < min )
	{
		return( 1 );
	}

	double	max	= pClass->asDouble(CLASS_MAX);

	return( min < max
		?	(Value < max ?  0 : -1)
		:	(Value > min ? -1 :  0)
	);
}

//---------------------------------------------------------
int CChange_Detection::Get_Class(double Value)
{
	int		a, b, i, c;

	if( m_Classes.Get_Count() > 0 )
	{
		if( m_Classes.Get_Index_Field(0) != CLASS_MIN || m_Classes.Get_Index_Order(0) != TABLE_INDEX_Ascending )
		{
			m_Classes.Set_Index(CLASS_MIN, TABLE_INDEX_Ascending);
		}

		for(a=0, b=m_Classes.Get_Record_Count()-1; a < b; )
		{
			i	= a + (b - a) / 2;
			c	= Cmp_Class(Value, i);

			if( c > 0 )
			{
				b	= b > i ? i : b - 1;
			}
			else if( c < 0 )
			{
				a	= a < i ? i : a + 1;
			}
			else
			{
				return( m_Classes.Get_Record_byIndex(i)->Get_Index() );
			}
		}

		if( Cmp_Class(Value, a) == 0 )
		{
			return( m_Classes.Get_Record_byIndex(a)->Get_Index() );
		}

		if( a != b && Cmp_Class(Value, b) == 0 )
		{
			return( m_Classes.Get_Record_byIndex(b)->Get_Index() );
		}
	}

	return( m_Classes.Get_Count() );	// := unclassified
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
