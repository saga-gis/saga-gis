/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                WKSP_Layer_Classify.cpp                //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
#include "helper.h"

#include "wksp_layer.h"
#include "wksp_layer_classify.h"
#include "wksp_grid.h"
#include "wksp_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Layer_Classify::CWKSP_Layer_Classify(void)
{
	m_Mode			= CLASSIFY_UNIQUE;
	m_Shade_Mode	= SHADE_MODE_DSC_GREY;

	m_pLayer		= NULL;
	m_pColors		= NULL;
	m_pLUT			= NULL;

	m_HST_Count		= NULL;
}

//---------------------------------------------------------
CWKSP_Layer_Classify::~CWKSP_Layer_Classify(void)
{
	if( m_HST_Count )
	{
		SG_Free(m_HST_Count);
		m_HST_Count	= NULL;

		SG_Free(m_HST_Cumul);
		m_HST_Cumul	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer_Classify::Initialise(CWKSP_Layer *pLayer, CSG_Table *pLUT, CSG_Colors *pColors)
{
	m_pLayer	= pLayer;
	m_pLUT		= pLUT;
	m_pColors	= pColors;

	//-----------------------------------------------------
	switch( m_pLayer->Get_Type() )
	{
	default:
		m_pColors->Set_Count(10);
		break;

	case WKSP_ITEM_TIN:
	case WKSP_ITEM_PointCloud:
	case WKSP_ITEM_Grid:
		m_pColors->Set_Count(100);
		break;
	}

	//-----------------------------------------------------
	if( m_pLUT && m_pLUT->Get_Record_Count() == 0 )
	{
		CSG_Table_Record	*pRecord;

		pRecord	= m_pLUT->Add_Record();
		pRecord->Set_Value(LUT_COLOR		, SG_GET_RGB(1, 1, 1));
		pRecord->Set_Value(LUT_TITLE		, LNG("Class 1"));
		pRecord->Set_Value(LUT_DESCRIPTION	, LNG("First Class"));
		pRecord->Set_Value(LUT_MIN			, 0.0);
		pRecord->Set_Value(LUT_MAX			, 1.0);

		pRecord	= m_pLUT->Add_Record();
		pRecord->Set_Value(LUT_COLOR		, SG_GET_RGB(255, 0, 0));
		pRecord->Set_Value(LUT_TITLE		, LNG("Class 2"));
		pRecord->Set_Value(LUT_DESCRIPTION	, LNG("Second Class"));
		pRecord->Set_Value(LUT_MIN			, 1.0);
		pRecord->Set_Value(LUT_MAX			, 2.0);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CWKSP_Layer_Classify::Get_Class_Value_Minimum(int iClass)
{
	switch( m_Mode )
	{
	default:
		break;

	case CLASSIFY_LUT:
		if( iClass >= 0 && iClass < m_pLUT->Get_Record_Count() )
		{
			return( m_pLUT->Get_Record(iClass)->asDouble(LUT_MIN) );
		}
		break;

	case CLASSIFY_METRIC:
	case CLASSIFY_SHADE:
	case CLASSIFY_OVERLAY:
		if( m_zRange > 0.0 )
		{
			return( Get_RelativeToMetric(iClass / (double)Get_Class_Count()) );
		}
		break;
	}

	return( m_zMin );
}

double CWKSP_Layer_Classify::Get_Class_Value_Maximum(int iClass)
{
	switch( m_Mode )
	{
	default:
		break;

	case CLASSIFY_LUT:
		if( iClass >= 0 && iClass < m_pLUT->Get_Record_Count() )
		{
			return( m_pLUT->Get_Record(iClass)->asDouble(LUT_MAX) );
		}
		break;

	case CLASSIFY_METRIC:
	case CLASSIFY_SHADE:
	case CLASSIFY_OVERLAY:
		if( m_zRange > 0.0 )
		{
			return( Get_RelativeToMetric((1.0 + iClass) / (double)Get_Class_Count()) );
		}
		break;
	}

	return( m_zMin + m_zRange );
}

double CWKSP_Layer_Classify::Get_Class_Value_Center(int iClass)
{
	switch( m_Mode )
	{
	default:
		break;

	case CLASSIFY_LUT:
		if( iClass >= 0 && iClass < m_pLUT->Get_Record_Count() )
		{
			return( 0.5 * (m_pLUT->Get_Record(iClass)->asDouble(LUT_MIN) + m_pLUT->Get_Record(iClass)->asDouble(LUT_MAX)) );
		}
		break;

	case CLASSIFY_METRIC:
	case CLASSIFY_SHADE:
	case CLASSIFY_OVERLAY:
		if( m_zRange > 0.0 )
		{
			return( Get_RelativeToMetric((0.5 + iClass) / (double)Get_Class_Count()) );
		}
		break;
	}

	return( m_zMin + 0.5 * m_zRange );
}

//---------------------------------------------------------
wxString CWKSP_Layer_Classify::Get_Class_Name(int iClass)
{
	CSG_String	s;

	switch( m_Mode )
	{
	default:
		break;

	case CLASSIFY_LUT:
		if( iClass >= 0 && iClass < m_pLUT->Get_Record_Count() )
		{
			s.Printf(SG_T("%s"), m_pLUT->Get_Record(iClass)->asString(LUT_TITLE));
		}
		break;

	case CLASSIFY_METRIC:
	case CLASSIFY_SHADE:
	case CLASSIFY_OVERLAY:
		s	= SG_Get_String(Get_Class_Value_Minimum(iClass), -2) + SG_T(" < ")
			+ SG_Get_String(Get_Class_Value_Maximum(iClass), -2);
		break;
	}

	return( s.c_str() );
}

//---------------------------------------------------------
wxString CWKSP_Layer_Classify::Get_Class_Name_byValue(double Value)
{
	return( Get_Class_Name(Get_Class(Value)) );
}


///////////////////////////////////////////////////////////
//														 //
//						Unique							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer_Classify::Set_Unique_Color(int Color)
{
	m_UNI_Color	= Color;
}


///////////////////////////////////////////////////////////
//														 //
//						Metric							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer_Classify::Set_Metric(int Mode, double LogFactor, double zMin, double zMax)
{
	m_zMode		= Mode;
	m_zMin		= zMin;
	m_zRange	= zMax - zMin;
	m_zLogRange	= LogFactor;
	m_zLogMax	= log(1.0 + m_zLogRange);

	if( m_zRange <= 0.0 || (m_zMode != 0 && m_zLogRange <= 0.0) )
	{
		m_zRange	= 0.0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Lookup Table					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CWKSP_Layer_Classify::_LUT_Cmp_Class(double Value, int iClass)
{
	CSG_Table_Record	*pClass	= m_pLUT->Get_Record_byIndex(iClass);

	double	min	= pClass->asDouble(LUT_MIN);

	if( Value < min )
	{
		return( 1 );
	}

	double	max	= pClass->asDouble(LUT_MAX);

	return( min < max
		?	(Value < max ?  0 : -1)
		:	(Value > min ? -1 :  0)
	);
}

//---------------------------------------------------------
int CWKSP_Layer_Classify::_LUT_Get_Class(double Value)
{
	int		a, b, i, c;

	if( m_pLUT->Get_Record_Count() > 0 )
	{
		if( m_pLUT->Get_Index_Field(0) != LUT_MIN || m_pLUT->Get_Index_Order(0) != TABLE_INDEX_Ascending )
		{
			m_pLUT->Set_Index(LUT_MIN, TABLE_INDEX_Ascending);
		}

		for(a=0, b=m_pLUT->Get_Record_Count()-1; a < b; )
		{
			i	= a + (b - a) / 2;
			c	= _LUT_Cmp_Class(Value, i);

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
				return( m_pLUT->Get_Record_byIndex(i)->Get_Index() );
			}
		}

		if( _LUT_Cmp_Class(Value, a) == 0 )
		{
			return( m_pLUT->Get_Record_byIndex(a)->Get_Index() );
		}

		if( a != b && _LUT_Cmp_Class(Value, b) == 0 )
		{
			return( m_pLUT->Get_Record_byIndex(b)->Get_Index() );
		}
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer_Classify::Metric2EqualElements(void)
{
	int				x, y, iClass, nClasses;
	double			dClass, zA, zB;
	CSG_Table_Record	*pRecord;
	CSG_Grid			*pGrid;

	if( m_pLayer->Get_Type() == WKSP_ITEM_Grid && (nClasses = m_pColors->Get_Count()) > 1 )
	{
		m_pLUT->Del_Records();

		pGrid	= ((CWKSP_Grid *)m_pLayer)->Get_Grid();
		dClass	= (double)pGrid->Get_NCells() / (double)(nClasses);

		pGrid->Get_Sorted(0, x, y, false, false);
		zA		= pGrid->asDouble(x, y);

		for(iClass=0; iClass<nClasses-1; iClass++)
		{
			pGrid->Get_Sorted((int)(dClass * (iClass + 1.0)), x, y, false, false);
			zB		= zA;
			zA		= pGrid->asDouble(x, y);

			pRecord	= m_pLUT->Add_Record();
			pRecord->Set_Value(LUT_COLOR		, m_pColors->Get_Color(iClass));
			pRecord->Set_Value(LUT_TITLE		, wxString::Format(wxT(">=%f"), zB));
			pRecord->Set_Value(LUT_DESCRIPTION	, wxString::Format(wxT("%f <-> %f"), zB, zA));
			pRecord->Set_Value(LUT_MIN			, zB);
			pRecord->Set_Value(LUT_MAX			, zA);
		}

		pGrid->Get_Sorted(pGrid->Get_NCells() - 1, x, y, false, false);
		zB		= zA;
		zA		= pGrid->asDouble(x, y);
		pRecord	= m_pLUT->Add_Record();
		pRecord->Set_Value(LUT_COLOR		, m_pColors->Get_Color(iClass));
		pRecord->Set_Value(LUT_TITLE		, wxString::Format(wxT(">=%f"), zB));
		pRecord->Set_Value(LUT_DESCRIPTION	, wxString::Format(wxT("%f <-> %f"), zB, zA));
		pRecord->Set_Value(LUT_MIN			, zB);
		pRecord->Set_Value(LUT_MAX			, zA);

		Set_Mode(CLASSIFY_LUT);
		m_pLayer->Update_Views(false);
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Histogram						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer_Classify::Histogram_Update(void)
{
	int		i;

	//-----------------------------------------------------
	if( m_HST_Count )
	{
		SG_Free(m_HST_Count);
		SG_Free(m_HST_Cumul);
		m_HST_Count	= NULL;
		m_HST_Cumul	= NULL;
	}

	//-----------------------------------------------------
	if( Get_Class_Count() > 0 )
	{
		STATUSBAR_Set_Text(LNG("[MSG] Build Histogram..."));

		m_HST_Count	= (int *)SG_Calloc(Get_Class_Count(), sizeof(int));
		m_HST_Cumul	= (int *)SG_Calloc(Get_Class_Count(), sizeof(int));

		switch( m_pLayer->Get_Type() )
		{
		default:
			break;

		case WKSP_ITEM_Grid:
			_Histogram_Update(((CWKSP_Grid   *)m_pLayer)->Get_Grid());
			break;

		case WKSP_ITEM_Shapes:
			_Histogram_Update(((CWKSP_Shapes *)m_pLayer)->Get_Shapes(), m_pLayer->Get_Parameters()->Get_Parameter("COLORS_ATTRIB")->asInt());
			break;
		}

		PROCESS_Set_Okay();

		//-------------------------------------------------
		for(i=0, m_HST_Maximum=0, m_HST_Total=0; i<Get_Class_Count(); i++)
		{
			m_HST_Cumul[i]	= (m_HST_Total += m_HST_Count[i]);

			if( m_HST_Count[i] > m_HST_Maximum )
			{
				m_HST_Maximum	= m_HST_Count[i];
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CWKSP_Layer_Classify::_Histogram_Update(CSG_Grid *pGrid)
{
	for(int y=0; y<pGrid->Get_NY() && PROGRESSBAR_Set_Position(y, pGrid->Get_NY()); y++)
	{
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				int		Class	= Get_Class(pGrid->asDouble(x, y));
				
				if( Class >= 0 && Class < Get_Class_Count() )
				{
					m_HST_Count[Class]++;
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Layer_Classify::_Histogram_Update(CSG_Shapes *pShapes, int Attribute)
{
	if( Attribute >= 0 && Attribute < pShapes->Get_Field_Count() )
	{
		for(int i=0; i<pShapes->Get_Count() && PROGRESSBAR_Set_Position(i, pShapes->Get_Count()); i++)
		{
			int		Class	= Get_Class(pShapes->Get_Record(i)->asDouble(Attribute));
			
			if( Class >= 0 && Class < Get_Class_Count() )
			{
				m_HST_Count[Class]++;
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
