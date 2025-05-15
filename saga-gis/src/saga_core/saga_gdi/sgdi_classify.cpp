
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: saga_gdi                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   sgdi_classify.cpp                   //
//                                                       //
//                 Copyright (C) 2025 by                 //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgdi_classify.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSGDI_Classify::CSGDI_Classify(void)
{
	// nop
}

//---------------------------------------------------------
CSGDI_Classify::CSGDI_Classify(CSG_Data_Object *pObject)
{
	Create(pObject);
}

//---------------------------------------------------------
CSGDI_Classify::CSGDI_Classify(CSG_Table *pTable, int Field, int Normalize)
{
	Create(pTable, Field, Normalize);
}

//---------------------------------------------------------
CSGDI_Classify::~CSGDI_Classify(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSGDI_Classify::Destroy(void)
{
	m_pObject = NULL; m_nValues = 0; m_Field = -1; m_Normalized.Destroy();

	m_Classes.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Classify::Create(CSG_Data_Object *pObject)
{
	Destroy();

	//-----------------------------------------------------
	if( pObject )
	{
		if( pObject->asGrid() )
		{
			m_pObject = pObject; m_nValues = pObject->asGrid()->Get_NCells();
		}
		else if( pObject->asGrids() )
		{
			m_pObject = pObject; m_nValues = pObject->asGrids()->Get_NCells();
		}
	}

	//-----------------------------------------------------
	if( is_Okay() )
	{
		return( true );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Classify::Create(CSG_Table *pTable, int Field, int Normalize)
{
	Destroy();

	//-----------------------------------------------------
	if( pTable && Field >= 0 && Field < pTable->Get_Field_Count() )
	{
		if( Normalize >= 0 && Normalize < pTable->Get_Field_Count() )
		{
			m_Normalized.Destroy(); m_Normalized.Add_Field("x", SG_DATATYPE_Double);

			for(sLong i=0; i<pTable->Get_Count(); i++)
			{
				double Value, Divisor;

				if( pTable->Get_Value(i, Normalize, Divisor) && Divisor && pTable->Get_Value(i, Field, Value) )
				{
					m_Normalized.Add_Record()->Set_Value(0, Value / Divisor);
				}
			}

			m_pObject = &m_Normalized; m_Field = 0; m_nValues = m_Normalized.Get_Count();
		}
		else
		{
			m_pObject = pTable; m_Field = Field; m_nValues = pTable->Get_Count();
		}
	}

	//-----------------------------------------------------
	if( is_Okay() )
	{
		return( true );
	}

	Destroy();

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const CSG_String & CSGDI_Classify::Get_Name(Classifier Classifier) const
{
	const CSG_String Names[] =
	{
		_TL("Unique Values"     ),
		_TL("Equal Interval"    ),
		_TL("Defined Interval"  ),
		_TL("Quantile"          ),
		_TL("Geometric Interval"),
		_TL("Natural Breaks"    ),
		_TL("Standard Deviation"),
		_TL("Manual Interval"   )
	};

	return( Names[(int)Classifier] );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Classify::is_Okay(void) const
{
	return( m_pObject && m_nValues > 0 );
}

//---------------------------------------------------------
bool CSGDI_Classify::Set_LUT(CSG_Table &Classes, CSG_Colors Colors) const
{
	if( !is_Okay() || m_Classes.Get_Count() < 1 )
	{
		return( false );
	}

	bool bNumeric = _is_Numeric(); Colors.Set_Count((int)m_Classes.Get_Count());

	Classes.Set_Field_Type((int)LUT_Fields::Minimum, bNumeric ? SG_DATATYPE_Double : SG_DATATYPE_String);
	Classes.Set_Field_Type((int)LUT_Fields::Maximum, bNumeric ? SG_DATATYPE_Double : SG_DATATYPE_String);

	for(int i=0; i<m_Classes.Get_Count(); i++)
	{
		CSG_Table_Record &Class = *Classes.Add_Record();

		Class.Set_Value((int)LUT_Fields::Color, Colors[i]);
		Class.Set_Value((int)LUT_Fields::Description, "");

		if( bNumeric )
		{
			double     Minimum = m_Classes[i].asDouble(0);
			double     Maximum = m_Classes[i].asDouble(1);

			Class.Set_Value((int)LUT_Fields::Name   , SG_Get_String(Minimum, -2) + " - " + SG_Get_String(Maximum, -2));
			Class.Set_Value((int)LUT_Fields::Minimum, Minimum);
			Class.Set_Value((int)LUT_Fields::Maximum, Maximum);
		}
		else
		{
			CSG_String Minimum = m_Classes[i].asString(0);
			CSG_String Maximum = m_Classes[i].asString(1);

			Class.Set_Value((int)LUT_Fields::Name   , Minimum.Cmp(Maximum) ? Minimum + " - " + Maximum : Minimum);
			Class.Set_Value((int)LUT_Fields::Minimum, Minimum);
			Class.Set_Value((int)LUT_Fields::Maximum, Maximum);
		}
	}

	if( false && bNumeric )
	{
		double Minimum = m_Classes[                        0].asDouble(0);
		double Maximum = m_Classes[m_Classes.Get_Count() - 1].asDouble(1);

		Classes[                      0].Set_Value((int)LUT_Fields::Minimum, Minimum - (Maximum - Minimum));
		Classes[Classes.Get_Count() - 1].Set_Value((int)LUT_Fields::Maximum, Maximum + (Maximum - Minimum));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Classify::_is_Numeric(void) const
{
	return( m_Field < 0 || SG_Data_Type_is_Numeric(m_pObject->asTable(true)->Get_Field_Type(m_Field)) );
}

//---------------------------------------------------------
bool CSGDI_Classify::_Create_Classes(void)
{
	m_Classes.Destroy();

	if( _is_Numeric() )
	{
		m_Classes.Add_Field("min", SG_DATATYPE_Double); m_Classes.Add_Field("max", SG_DATATYPE_Double);
	}
	else
	{
		m_Classes.Add_Field("min", SG_DATATYPE_String); m_Classes.Add_Field("max", SG_DATATYPE_String);
	}

	return( m_pObject && m_nValues > 0 );
}

//---------------------------------------------------------
inline bool CSGDI_Classify::_Set_Progress(sLong i) const
{
	if( m_Show_Progress )
	{
		return( SG_UI_Process_Set_Progress(i, m_nValues) );
	}

	return( true );
}

//---------------------------------------------------------
inline bool CSGDI_Classify::_Get_Value(sLong i, double &Value, bool bIndexed) const
{
	if( i >= 0 )
	{
		if( m_Field >= 0 )
		{
			if( i < m_pObject->asTable(true)->Get_Count() )
			{
				return( m_pObject->asTable(true)->Get_Value(bIndexed ? m_pObject->asTable(true)->Get_Index(i) : i, m_Field, Value) );
			}
		}

		if( m_pObject->asGrid() )
		{
			if( i < m_pObject->asGrid ()->Get_NCells() )
			{
				if( bIndexed ) { i = m_pObject->asGrid ()->Get_Sorted(i, false, false); }

				if( !m_pObject->asGrid ()->is_NoData(i) )
				{
					Value = m_pObject->asGrid ()->asDouble(i);

					return( true );
				}
			}
		}

		if( m_pObject->asGrids() )
		{
			if( i < m_pObject->asGrid ()->Get_NCells() )
			{
				if( bIndexed ) { i = m_pObject->asGrids()->Get_Sorted(i, false, false); }

				if( !m_pObject->asGrids()->is_NoData(i) )
				{
					Value = m_pObject->asGrids()->asDouble(i);

					return( true );
				}
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
inline bool CSGDI_Classify::_Get_Value(sLong i, CSG_String &Value, bool bIndexed) const
{
	return( m_Field >= 0 && m_pObject->asTable(true)->Get_Value(bIndexed ? m_pObject->asTable(true)->Get_Index(i) : i, m_Field, Value) );
}

//---------------------------------------------------------
bool CSGDI_Classify::_Get_Statistics(CSG_Simple_Statistics &Statistics) const
{
	if( m_Field >= 0 )
	{
		Statistics = m_pObject->asTable(true)->Get_Statistics(m_Field);
	}
	else if( m_pObject->asGrid () )
	{
		Statistics = m_pObject->asGrid ()->Get_Statistics();
	}
	else if( m_pObject->asGrids() )
	{
		Statistics = m_pObject->asGrids()->Get_Statistics();
	}

	return( Statistics.Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSGDI_Classify::_Get_Histogram(CSG_Histogram &Histogram) const
{
	if( m_Field >= 0 )
	{
		Histogram = m_pObject->asTable(true)->Get_Histogram(m_Field);
	}
	else if( m_pObject->asGrid () )
	{
		Histogram = m_pObject->asGrid ()->Get_Histogram();
	}
	else if( m_pObject->asGrids() )
	{
		Histogram = m_pObject->asGrids()->Get_Histogram();
	}

	return( Histogram.Get_Class_Count() > 0 );
}

//---------------------------------------------------------
bool CSGDI_Classify::_Set_Index(void)
{
	if( m_Field >= 0 )
	{
		return( m_pObject->asTable(true)->Set_Index(m_Field) );
	}
	else if( m_pObject->asGrid () )
	{
		return( m_pObject->asGrid ()->Set_Index() );
	}
	else if( m_pObject->asGrids() )
	{
		return( m_pObject->asGrids()->Set_Index() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Classify::Classify_Unique(int maxCount)
{
	if( !is_Okay() )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( _is_Numeric() )
	{
		CSG_Unique_Number_Statistics Statistics;

		for(sLong i=0; i<m_nValues && Statistics.Get_Count()<=maxCount && _Set_Progress(i); i++)
		{
			double Value; if( _Get_Value(i, Value) ) { Statistics += Value; }
		}

		_Create_Classes();

		for(int i=0; i<Statistics.Get_Count(); i++)
		{
			CSG_Table_Record &Class	= *m_Classes.Add_Record();

			Class.Set_Value(0, Statistics.Get_Value(i)); // Minimum
			Class.Set_Value(1, Statistics.Get_Value(i)); // Maximum
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Unique_String_Statistics Statistics;

		for(sLong i=0; i<m_nValues && Statistics.Get_Count()<=maxCount && _Set_Progress(i); i++)
		{
			CSG_String Value; if( _Get_Value(i, Value) ) { Statistics += Value; }
		}

		_Create_Classes();

		for(int i=0; i<Statistics.Get_Count(); i++)
		{
			CSG_Table_Record &Class	= *m_Classes.Add_Record();

			Class.Set_Value(0, Statistics.Get_Value(i)); // Minimum
			Class.Set_Value(1, Statistics.Get_Value(i)); // Maximum
		}
	}

	//-----------------------------------------------------
	return( m_Classes.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Classify::Classify_Equal(int Count)
{
	CSG_Simple_Statistics Statistics;

	if( Count < 1 || !_Get_Statistics(Statistics) )
	{
		return( false );
	}

	if( !Statistics.Get_Range() )
	{
		Count = 1;
	}

	_Create_Classes();

	double Maximum = Statistics.Get_Minimum(), Interval = Statistics.Get_Range() / Count;

	for(int i=0; i<Count; i++)
	{
		double Minimum = Maximum; Maximum += Interval;

		CSG_Table_Record &Class	= *m_Classes.Add_Record();

		Class.Set_Value(0, Minimum); // Minimum
		Class.Set_Value(1, Maximum); // Maximum
	}

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Classify::Classify_Defined(double Interval)
{
	CSG_Simple_Statistics Statistics;

	if( Interval <= 0. || !_Get_Statistics(Statistics) )
	{
		return( false );
	}

	_Create_Classes();

	double Minimum = Statistics.Get_Minimum();

	for(double Maximum=Minimum+Interval; Minimum<Statistics.Get_Maximum(); Minimum=Maximum, Maximum+=Interval)
	{
		CSG_Table_Record &Class	= *m_Classes.Add_Record();

		Class.Set_Value(0, Minimum); // Minimum
		Class.Set_Value(1, Maximum); // Maximum
	}

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Classify::Classify_Quantile(int Count, bool bHistogram)
{
	if( !is_Okay() || Count < 1 )
	{
		return( false );
	}

	if( m_nValues < Count )
	{
		Count = (int)m_nValues;
	}

	_Create_Classes();

	if( bHistogram )
	{
		CSG_Histogram Histogram;

		if( _Get_Histogram(Histogram) )
		{
			CSG_Simple_Statistics Statistics = Histogram.Get_Statistics();

			double Minimum, Maximum = Statistics.Get_Minimum();

			for(int i=0; i<Count; i++)
			{
				Minimum	= Maximum; double Quantile = (1. + i) / (double)Count;
				Maximum	= Histogram.Get_Quantile(Quantile);

				CSG_Table_Record &Class = *m_Classes.Add_Record();

				Class.Set_Value(0, Minimum);
				Class.Set_Value(1, Maximum);
			}
		}
	}
	else
	{
		CSG_Simple_Statistics Statistics;

		if( _Set_Index() && _Get_Statistics(Statistics) )
		{
			double Minimum, Maximum = Statistics.Get_Minimum();

			for(int i=0; i<Count; i++)
			{
				double Value, Quantile = (1. + i) / (double)Count;

				if( _Get_Value((sLong)(Quantile * (m_nValues - 1)), Value, true) )
				{
					CSG_Table_Record &Class = *m_Classes.Add_Record();

					Minimum = Maximum; Maximum = Value;

					Class.Set_Value(0, Minimum);
					Class.Set_Value(1, Maximum);
				}
			}
		}
	}

	return( m_Classes.Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSGDI_Classify::Classify_Geometric(int Count)
{
	CSG_Simple_Statistics Statistics;

	if( Count < 1 || !_Get_Statistics(Statistics) || !Statistics.Get_Range() )
	{
		return( false );
	}

	_Create_Classes();

	double k = log(Statistics.Get_Range()) / Count;

	double Minimum, Maximum = Statistics.Get_Minimum();

	for(int i=0; i<Count; i++)
	{
		Minimum = Maximum;
		Maximum = Statistics.Get_Minimum() + exp(k * (i + 1.));

		CSG_Table_Record &Class	= *m_Classes.Add_Record();

		Class.Set_Value(0, Minimum); // Minimum
		Class.Set_Value(1, Maximum); // Maximum
	}

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Classify::Classify_Natural(int Count)
{
	if( !is_Okay() || Count < 1 )
	{
		return( false );
	}

	CSG_Natural_Breaks Breaks;

	if( m_Field >= 0 )
	{
		Breaks.Create(m_pObject->asTable(true), m_Field, Count, m_nValues > 4096 ? 256 : 0);
	}
	else if( m_pObject->asGrid () )
	{
		Breaks.Create(m_pObject->asGrid (), Count, m_nValues > 4096 ? 256 : 0);
	}
	else if( m_pObject->asGrids() )
	{
		Breaks.Create(m_pObject->asGrids(), Count, m_nValues > 4096 ? 256 : 0);
	}
	else
	{
		return( false );
	}

	if( Breaks.Get_Count() <= Count )
	{
		return( false );
	}

	_Create_Classes();

	for(int i=0; i<Count; i++)
	{
		CSG_Table_Record &Class = *m_Classes.Add_Record();

		Class.Set_Value(0, Breaks[i    ]); // Minimum
		Class.Set_Value(1, Breaks[i + 1]); // Maximum
	}

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Classify::Classify_StdDev(double StdDev)
{
	CSG_Simple_Statistics Statistics;

	return( StdDev > 0. && _Get_Statistics(Statistics) && Classify_Defined(StdDev * Statistics.Get_StdDev()) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
