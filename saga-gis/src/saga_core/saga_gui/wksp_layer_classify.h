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
	CLASSIFY_GRADUATED,
	CLASSIFY_SHADE,
	CLASSIFY_OVERLAY,
	CLASSIFY_RGB
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

//---------------------------------------------------------
enum
{
	SHADE_MODE_DSC_GREY	= 0,
	SHADE_MODE_ASC_GREY,
	SHADE_MODE_DSC_CYAN,
	SHADE_MODE_ASC_CYAN,
	SHADE_MODE_DSC_MAGENTA,
	SHADE_MODE_ASC_MAGENTA,
	SHADE_MODE_DSC_YELLOW,
	SHADE_MODE_ASC_YELLOW
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

	bool						Initialise				(class CWKSP_Layer *pLayer, CSG_Table *pLUT, CSG_Colors *pColors);

	void						Set_Mode				(int Mode)	{	m_Mode			= Mode;	}
	int							Get_Mode				(void)		{	return( m_Mode );		}

	void						Set_Shade_Mode			(int Mode)	{	m_Shade_Mode	= Mode;	}
	int							Get_Shade_Mode			(void)		{	return( m_Shade_Mode );	}

	void						Set_Unique_Color		(int Color);
	int							Get_Unique_Color		(void)		{	return( m_UNI_Color );	}


	///////////////////////////////////////////////////////

	void						Set_Class_Count			(int Count)	{	if( Count > 0 )	m_Count	= Count;	}

	//-----------------------------------------------------
	int							Get_Class_Count			(void)
	{
		switch( m_Mode )
		{
		case CLASSIFY_UNIQUE   : default:
			return( 1 );

		case CLASSIFY_LUT      :
			return( m_pLUT->Get_Record_Count() );

		case CLASSIFY_GRADUATED:
		case CLASSIFY_SHADE    :
		case CLASSIFY_OVERLAY  :
			return( m_Count );

		case CLASSIFY_METRIC   :
			return( m_pColors->Get_Count() );
		}
	}

	//-----------------------------------------------------
	int							Get_Class				(double Value)
	{
		switch( m_Mode )
		{
		case CLASSIFY_UNIQUE   : default:
			return( 0 );

		case CLASSIFY_LUT      :
			return( _LUT_Get_Class(Value) );

		case CLASSIFY_GRADUATED:
		case CLASSIFY_METRIC   :
		case CLASSIFY_SHADE    :
		case CLASSIFY_OVERLAY  :
			return( _METRIC_Get_Class(Value) );
		}
	}

	int							Get_Class				(const CSG_String &Value)
	{
		if( m_Mode == CLASSIFY_LUT )
		{
			return( _LUT_Get_Class(Value) );
		}

		return( Get_Class(Value.asDouble()) );
	}

	//-----------------------------------------------------
	double						Get_Class_Value_Minimum	(int iClass);
	double						Get_Class_Value_Center	(int iClass);
	double						Get_Class_Value_Maximum	(int iClass);
	wxString					Get_Class_Name			(int iClass);
	wxString					Get_Class_Name_byValue	(double          Value);
	wxString					Get_Class_Name_byValue	(const wxString &Value);


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	bool						Get_Class_Color			(int iClass, int &Color)
	{
		switch( m_Mode )
		{
		case CLASSIFY_UNIQUE   : default:
			Color	= m_UNI_Color;
			break;

		case CLASSIFY_LUT      :
			if( iClass < 0 || iClass >= m_pLUT->Get_Record_Count() )
			{
				Color	= m_UNI_Color;

				return( false );
			}

			Color	= m_pLUT->Get_Record(iClass)->asInt(LUT_COLOR);
			break;


		case CLASSIFY_METRIC   :
			Color	= m_pColors->Get_Color(iClass < 0 ? 0 : iClass >= m_pColors->Get_Count() ? m_pColors->Get_Count() - 1 : iClass);
			break;

		case CLASSIFY_GRADUATED:
		case CLASSIFY_SHADE    :
		case CLASSIFY_OVERLAY  :
			Get_Class_Color_byValue(Get_RelativeToMetric(iClass / (double)m_Count), Color);
			break;
		}

		return( true );
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
		switch( m_Mode )
		{
		case CLASSIFY_UNIQUE   : default:
			{
				return( Get_Class_Color(0, Color) );
			}

		case CLASSIFY_LUT      :
			{
				return( Get_Class_Color(_LUT_Get_Class(Value), Color) );
			}

		case CLASSIFY_METRIC   :
			{
				return( Get_Class_Color(_METRIC_Get_Class(Value), Color) );
			}

		case CLASSIFY_GRADUATED:
			{
				double	iClass	= Get_MetricToRelative(Value) * (m_pColors->Get_Count() - 1);

				if( iClass < 0 )
				{
					Color	= m_pColors->Get_Color(0);
				}
				else if( iClass >= m_pColors->Get_Count() - 1 )
				{
					Color	= m_pColors->Get_Color(m_pColors->Get_Count() - 1);
				}
				else
				{
					int		a	= m_pColors->Get_Color(    (int)iClass);
					int		b	= m_pColors->Get_Color(1 + (int)iClass);
					double	d	= iClass - (int)iClass;

					Color	= SG_GET_RGB(
						SG_GET_R(a) + d * (SG_GET_R(b) - SG_GET_R(a)),
						SG_GET_G(a) + d * (SG_GET_G(b) - SG_GET_G(a)),
						SG_GET_B(a) + d * (SG_GET_B(b) - SG_GET_B(a))
					);
				}

				return( true );
			}

		case CLASSIFY_SHADE    :
			{
				int	iClass	= (int)(255.0 * Get_MetricToRelative(Value));

				if( iClass < 0 ) iClass = 0; else if( iClass > 255 ) iClass = 255;

				switch( m_Shade_Mode )
				{
				default:
				case SHADE_MODE_DSC_GREY   : Color = SG_GET_RGB(255 - iClass, 255 - iClass, 255 - iClass);	break;
				case SHADE_MODE_DSC_CYAN   : Color = SG_GET_RGB(255 - iClass, 255         , 255         );	break;
				case SHADE_MODE_DSC_MAGENTA: Color = SG_GET_RGB(255         , 255 - iClass, 255         );	break;
				case SHADE_MODE_DSC_YELLOW : Color = SG_GET_RGB(255         , 255         , 255 - iClass);	break;
				case SHADE_MODE_ASC_GREY   : Color = SG_GET_RGB(      iClass,       iClass,       iClass);	break;
				case SHADE_MODE_ASC_CYAN   : Color = SG_GET_RGB(      iClass, 255         , 255         );	break;
				case SHADE_MODE_ASC_MAGENTA: Color = SG_GET_RGB(255         ,       iClass, 255         );	break;
				case SHADE_MODE_ASC_YELLOW : Color = SG_GET_RGB(255         , 255         ,       iClass);	break;
				}

				return( true );
			}

		case CLASSIFY_OVERLAY  :
			{
				int	iClass	= (int)(255.0 * Get_MetricToRelative(Value));
			
				if( iClass < 0 ) iClass = 0; else if( iClass > 255 ) iClass = 255;

				Color	= SG_GET_RGB(iClass, iClass, iClass);

				return( true );
			}

		case CLASSIFY_RGB      :
			{
				Color	= (int)Value;
			
				return( true );
			}
		}
	}

	bool						Get_Class_Color_byValue	(const CSG_String &Value, int &Color)
	{
		if( m_Mode == CLASSIFY_LUT )
		{
			return( Get_Class_Color(_LUT_Get_Class(Value), Color) );
		}
		
		return( Get_Class_Color(Value.asDouble(), Color) );
	}

	//-----------------------------------------------------
	int							Get_Class_Color_byValue	(double Value)
	{
		int		Color;

		return( Get_Class_Color_byValue(Value, Color) ? Color : 0 );
	}

	int							Get_Class_Color_byValue	(const CSG_String &Value)
	{
		int		Color;

		return( Get_Class_Color_byValue(Value, Color) ? Color : 0 );
	}


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	void						Set_Metric				(int Mode, double LogFactor, double zMin, double zMax);
	int							Get_Metric_Mode			(void)		{	return( m_zMode );		}
	CSG_Colors *				Get_Metric_Colors		(void)		{	return( m_pColors );	}
	double						Get_Metric_Minimum		(void)	const	{	return( m_zMin            );	}
	double						Get_Metric_Maximum		(void)	const	{	return( m_zMin + m_zRange );	}
	double						Get_Metric_Range		(void)	const	{	return(          m_zRange );	}
	double						Get_Metric_LogFactor	(void)	const	{	return( m_zLogRange );	}

	void						Metric2EqualElements	(void);

	//-----------------------------------------------------
	double						Get_MetricToRelative	(double Value)
	{
		if( m_zRange > 0.0 )
		{
			Value	= (Value - m_zMin) / m_zRange;

			switch( m_zMode )
			{
			default:
				return( Value );

			case METRIC_MODE_LOGUP:
				return( Value > 0.0 ?       (log(1.0 + m_zLogRange * (      Value)) / m_zLogMax) : 0.0 );

			case METRIC_MODE_LOGDOWN:
				return( Value < 1.0 ? 1.0 - (log(1.0 + m_zLogRange * (1.0 - Value)) / m_zLogMax) : 1.0 );
			}
		}

		return( 0.0 );
	}

	//-----------------------------------------------------
	double						Get_RelativeToMetric	(double Value)
	{
		switch( m_zMode )
		{
		case METRIC_MODE_LOGUP:
			Value	=       ((exp(m_zLogMax * (      Value)) - 1.0) / m_zLogRange);
			break;

		case METRIC_MODE_LOGDOWN:
			Value	= 1.0 - ((exp(m_zLogMax * (1.0 - Value)) - 1.0) / m_zLogRange);
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

	int							m_Mode, m_zMode, m_Shade_Mode, m_Count, m_UNI_Color;

	sLong						*m_HST_Count, *m_HST_Cumul, m_HST_Maximum, m_HST_Total;

	double						m_zMin, m_zRange, m_zLogRange, m_zLogMax;

	CSG_Colors					*m_pColors;

	CSG_Table					*m_pLUT;

	class CWKSP_Layer			*m_pLayer;


	//-----------------------------------------------------
	int							_LUT_Cmp_Class			(double Value, int iClass);
	int							_LUT_Get_Class			(double Value);

	int							_LUT_Cmp_Class			(const CSG_String &Value, int iClass);
	int							_LUT_Get_Class			(const CSG_String &Value);

	int							_METRIC_Get_Class		(double Value)
	{
		if( Value < m_zMin )
		{
			return( -1 );
		}

		if( Value > m_zMin + m_zRange )
		{
			return( Get_Class_Count() );
		}

		int	Class	= (int)(Get_MetricToRelative(Value) * Get_Class_Count());

		return( Class < 0 ? 0 : Class < Get_Class_Count() ? Class : Get_Class_Count() - 1 );
	}


	//-----------------------------------------------------
	bool						_Histogram_Update		(CSG_Grid  *pGrid );
	bool						_Histogram_Update		(CSG_Grids *pGrids);
	bool						_Histogram_Update		(CSG_Shapes *pShapes, int Attribute, int Normalize = -1);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_Classify_H
