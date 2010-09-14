
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
//                     mat_tools.cpp                     //
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
#include "mat_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double			SG_Get_Square(double x)
{
	return( x * x );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Simple_Statistics::CSG_Simple_Statistics(void)
{
	Invalidate();
}

//---------------------------------------------------------
void CSG_Simple_Statistics::Invalidate(void)
{
	m_bEvaluated	= false;
	m_nValues		= 0;
	m_Weights		= 0.0;
	m_Sum			= 0.0;
	m_Sum2			= 0.0;

	m_Minimum		= 0.0;
	m_Maximum		= 0.0;
	m_Range			= 0.0;
	m_Mean			= 0.0;
	m_Variance		= 0.0;
	m_StdDev		= 0.0;
}

//---------------------------------------------------------
void CSG_Simple_Statistics::Add_Value(double Value, double Weight)
{
	if( m_nValues == 0 )
	{
		m_Minimum	= m_Maximum	= Value;
	}
	else if( m_Minimum > Value )
	{
		m_Minimum	= Value;
	}
	else if( m_Maximum < Value )
	{
		m_Maximum	= Value;
	}

	m_nValues++;

	if( Weight > 0.0 )
	{
		m_Weights		+= Weight;
		m_Sum			+= Weight * Value;
		m_Sum2			+= Weight * Value*Value;

		m_bEvaluated	= false;
	}
}

//---------------------------------------------------------
void CSG_Simple_Statistics::_Evaluate(void)
{
	if( m_Weights > 0.0 )
	{
		m_Range			= m_Maximum - m_Minimum;
		m_Mean			= m_Sum  / m_Weights;
		m_Variance		= m_Sum2 / m_Weights - m_Mean*m_Mean;
		m_StdDev		= m_Variance > 0.0 ? sqrt(m_Variance) : 0.0;

		m_bEvaluated	= true;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Class_Statistics::CSG_Class_Statistics(void)
{
	Create();
}

//---------------------------------------------------------
CSG_Class_Statistics::~CSG_Class_Statistics(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_Class_Statistics::Create(void)
{
	m_Array.Create(sizeof(TClass), 0, SG_ARRAY_GROWTH_1);

	m_Classes	= NULL;
}

//---------------------------------------------------------
void CSG_Class_Statistics::Destroy(void)
{
	m_Array.Set_Array(0, (void **)&m_Classes);
}

//---------------------------------------------------------
void CSG_Class_Statistics::Add_Value(double Value)
{
	for(size_t i=0; i<m_Array.Get_Size(); i++)
	{
		if( m_Classes[i].Value == Value )
		{
			m_Classes[i].Count++;

			return;
		}
	}

	if( m_Array.Inc_Array((void **)&m_Classes) )
	{
		m_Classes[Get_Count() - 1].Count	= 1;
		m_Classes[Get_Count() - 1].Value	= Value;
	}
}

//---------------------------------------------------------
int CSG_Class_Statistics::Get_Majority(void)
{
	int		Index	= 0;

	for(int i=1; i<Get_Count(); i++)
	{
		if( m_Classes[i].Count > m_Classes[Index].Count )
		{
			Index	= i;
		}
	}

	return( Index );
}

bool CSG_Class_Statistics::Get_Majority(double &Value)
{
	int		Count;

	return( Get_Class(Get_Majority(), Value, Count) );
}

bool CSG_Class_Statistics::Get_Majority(double &Value, int &Count)
{
	return( Get_Class(Get_Majority(), Value, Count) );
}

//---------------------------------------------------------
int CSG_Class_Statistics::Get_Minority(void)
{
	int		Index	= 0;

	for(int i=1; i<Get_Count(); i++)
	{
		if( m_Classes[i].Count > m_Classes[Index].Count )
		{
			Index	= i;
		}
	}

	return( Index );
}

bool CSG_Class_Statistics::Get_Minority(double &Value)
{
	int		Count;

	return( Get_Class(Get_Minority(), Value, Count) );
}

bool CSG_Class_Statistics::Get_Minority(double &Value, int &Count)
{
	return( Get_Class(Get_Minority(), Value, Count) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
