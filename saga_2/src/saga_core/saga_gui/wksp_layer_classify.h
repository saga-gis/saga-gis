
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
//                 WKSP_Layer_Classify.h                 //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_Classify_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_Classify_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/string.h>

#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	CLASSIFY_UNIQUE	= 0,
	CLASSIFY_LUT,
	CLASSIFY_METRIC,
	CLASSIFY_RGB,
	CLASSIFY_SHADE
};

//---------------------------------------------------------
enum
{
	LUT_COLOR		= 0,
	LUT_TITLE,
	LUT_DESCRIPTION,
	LUT_MIN,
	LUT_MAX
};

//---------------------------------------------------------
enum
{
	METRIC_MODE_NORMAL	= 0,
	METRIC_MODE_LOGUP,
	METRIC_MODE_LOGDOWN
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Layer_Classify
{
public: ///////////////////////////////////////////////////
	CWKSP_Layer_Classify(void);
	virtual ~CWKSP_Layer_Classify(void);

	bool						Initialise				(class CWKSP_Layer *pLayer, CTable *pLUT, CColors *pColors);

	void						Set_Mode				(int Mode);
	int							Get_Mode				(void)		{	return( m_Mode );		}

	void						Set_Unique_Color		(int Color);
	int							Get_Unique_Color		(void)		{	return( m_UNI_Color );	}

	void						Set_Metric				(int Mode, double LogFactor, double zMin, double zMax);
	int							Get_Metric_Mode			(void)		{	return( m_zMode );		}
	CColors *					Get_Metric_Colors		(void)		{	return( m_pColors );	}

	void						Metric2EqualElements	(void);


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	int							Get_Class_Count			(void)
	{
		switch( m_Mode )
		{
		case CLASSIFY_UNIQUE:	default:
			return( 1 );

		case CLASSIFY_LUT:
			return( m_pLUT->Get_Record_Count() );

		case CLASSIFY_METRIC:
		case CLASSIFY_SHADE:
			return( m_pColors->Get_Count() );
		}
	}

	//-----------------------------------------------------
	int							Get_Class				(double Value)
	{
		switch( m_Mode )
		{
		case CLASSIFY_UNIQUE:	default:
			return( 0 );

		case CLASSIFY_LUT:
			return( _LUT_Get_Class(Value) );

		case CLASSIFY_METRIC:
		case CLASSIFY_SHADE:
			return( _METRIC_Get_Class(Value) );
		}
	}

	//-----------------------------------------------------
	wxString					Get_Class_Name			(int iClass);
	wxString					Get_Class_Name_byValue	(double Value);


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	bool						Get_Class_Color			(int iClass, int &Color)
	{
		switch( m_Mode )
		{
		case CLASSIFY_UNIQUE:	default:
			Color	= m_UNI_Color;

			return( true );

		case CLASSIFY_LUT:
			if( iClass >= 0 && iClass < m_pLUT->Get_Record_Count() )
			{
				Color	= m_pLUT->Get_Record(iClass)->asInt(LUT_COLOR);

				return( true );
			}

			return( false );

		case CLASSIFY_METRIC:
			if( iClass < 0 )
			{
				iClass	= 0;
			}
			else if( iClass >= m_pColors->Get_Count() )
			{
				iClass	= m_pColors->Get_Count() - 1;
			}

			Color	= m_pColors->Get_Color(iClass);

			return( true );

		case CLASSIFY_SHADE:
			if( iClass < 0 )
			{
				iClass	= 0;
			}
			else if( iClass >= m_pColors->Get_Count() )
			{
				iClass	= m_pColors->Get_Count() - 1;
			}

			Color	= m_pColors->Get_Brightness(iClass);
			Color	= COLOR_GET_RGB(Color, Color, Color);

			return( true );
		}
	}

	//-----------------------------------------------------
	int							Get_Class_Color			(int iClass)
	{
		int		Color;

		return( Get_Class_Color(iClass, Color) ? Color : 0 );
	}

	//-----------------------------------------------------
	bool						Get_Class_Color_byValue	(double Value, int &Color)
	{
		int		iClass;

		switch( m_Mode )
		{
		case CLASSIFY_UNIQUE:	default:
			iClass	= 0;
			break;

		case CLASSIFY_LUT:
			iClass	= _LUT_Get_Class(Value);
			break;

		case CLASSIFY_METRIC:
		case CLASSIFY_SHADE:
			iClass	= _METRIC_Get_Class(Value);
			break;

		case CLASSIFY_RGB:
			Color	= (int)Value;
			return( true );
		}

		return( Get_Class_Color(iClass, Color) );
	}

	//-----------------------------------------------------
	int							Get_Class_Color_byValue	(double Value)
	{
		int		Color;

		return( Get_Class_Color_byValue(Value, Color) ? Color : 0 );
	}


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	double						Get_MetricToRelative	(double Value)
	{
		if( m_zRange >= 0.0 )
		{
			Value	= (Value - m_zMin) / m_zRange;

			switch( m_zMode )
			{
			case METRIC_MODE_LOGUP:
				Value	= log(1.0 + m_zLogRange * Value) / m_zLogMax;
				break;

			case METRIC_MODE_LOGDOWN:
				Value	= 1.0 - Value;
				Value	= log(1.0 + m_zLogRange * Value) / m_zLogMax;
				Value	= 1.0 - Value;
				break;
			}

			return( Value );
		}

		return( 0.0 );
	}

	//-----------------------------------------------------
	double						Get_RelativeToMetric	(double Value)
	{
		switch( m_zMode )
		{
		case METRIC_MODE_LOGUP:
			Value	= (exp(m_zLogMax * Value) - 1.0) / m_zLogRange;
			break;

		case METRIC_MODE_LOGDOWN:
			Value	= 1.0 - Value;
			Value	= (exp(m_zLogMax * Value) - 1.0) / m_zLogRange;
			Value	= 1.0 - Value;
			break;
		}

		return( m_zMin + (m_zRange * Value) );
	}


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	bool						Histogram_Update		(void);

	int							Histogram_Get_Maximum	(void)	{	return( m_HST_Maximum );	}
	int							Histogram_Get_Total		(void)	{	return( m_HST_Total );		}

	double						Histogram_Get_Count		(int iClass, bool bRelative = true)
	{
		return( bRelative && m_HST_Maximum > 0
			? (double)m_HST_Count[iClass] / m_HST_Maximum
			: m_HST_Count[iClass]
		);
	}

	double						Histogram_Get_Cumulative(int iClass, bool bRelative = true)
	{
		return( bRelative && m_HST_Total   > 0
			? (double)m_HST_Cumul[iClass] / m_HST_Total
			: m_HST_Cumul[iClass]
		);
	}


protected: ////////////////////////////////////////////////

	int							m_Mode, m_zMode,
								m_UNI_Color,
								*m_HST_Count, *m_HST_Cumul, m_HST_Maximum, m_HST_Total;

	double						m_zMin, m_zRange, m_zLogRange, m_zLogMax;

	CColors						*m_pColors;

	CTable						*m_pLUT;

	class CWKSP_Layer			*m_pLayer;


	//-----------------------------------------------------
	int							_LUT_Cmp_Class			(double Value, int iClass);
	int							_LUT_Get_Class			(double Value);

	int							_METRIC_Get_Class		(double Value)
	{
		if( Value < m_zMin )
		{
			return( -1 );
		}

		if( Value > m_zMin + m_zRange )
		{
			return( m_pColors->Get_Count() );
		}

		return( (int)(Get_MetricToRelative(Value) * m_pColors->Get_Count()) );
	}


	//-----------------------------------------------------
	bool						_Histogram_Update		(CGrid *pGrid);
	bool						_Histogram_Update		(CShapes *pShapes);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_Classify_H
