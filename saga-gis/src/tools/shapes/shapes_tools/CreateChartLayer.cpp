
/*******************************************************************************
    CCreateChartLayer.cpp
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "CreateChartLayer.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCreateChartLayer::CCreateChartLayer(void)
{
	Set_Name		(_TL("Create Chart Layer (Bars/Sectors)"));

	Set_Author		("V.Olaya (c) 2004");

	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. "
	));

	Parameters.Add_Shapes("", 
		"INPUT"  , _TL("Shapes"), 
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("", 
		"OUTPUT" , _TL("Chart"), 
		_TL(""), 
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field("INPUT",
		"SIZE"   , _TL("Size"),
		_TL("")
	);

	Parameters.Add_Table_Fields("INPUT",
		"FIELDS" , _TL("Attributes"),
		_TL("")
	);

	Parameters.Add_Double("", 
		"MAXSIZE", _TL("Maximum size"), 
		_TL(""),
		100., 0., true
	);

	Parameters.Add_Double("", 
		"MINSIZE", _TL("Minimum size"), 
		_TL(""),
		10., 0., true
	);
	
	Parameters.Add_Choice("", 
		"TYPE"   , _TL("Type"), 
		_TL(""), 
		CSG_String::Format("%s|%s",
			_TL("Sectors"),
			_TL("Bars")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCreateChartLayer::On_Execute(void)
{
	if( Parameters("FIELDS")->asTableFields()->Get_Count() < 1 )
	{
		Error_Set(_TL("no fields in selection"));

		return( false );
	}

	CSG_Shapes *pInput = Parameters("INPUT")->asShapes();

	m_fMaxSize = Parameters("MAXSIZE")->asDouble();
	m_fMinSize = Parameters("MINSIZE")->asDouble();

	if( m_fMinSize > m_fMaxSize )
	{
		m_fMinSize = m_fMaxSize;
	}

	m_fMaxValue = pInput->Get_Maximum(Parameters("SIZE")->asInt());
	m_fMinValue = pInput->Get_Minimum(Parameters("SIZE")->asInt());

	int Type = Parameters("TYPE")->asInt();

	m_pOutput = Parameters("OUTPUT")->asShapes();
	m_pOutput->Create(SHAPE_TYPE_Polygon);
	m_pOutput->Fmt_Name("%s (%s)", _TL("Chart"), Type == 1 ? _TL("Bars") : _TL("Sectors"));
	m_pOutput->Add_Field(_TL("ID"  ), SG_DATATYPE_Int   );
	m_pOutput->Add_Field(_TL("Name"), SG_DATATYPE_String);

	for(sLong i=0; i<pInput->Get_Count(); i++)
	{
		switch( Type )
		{
		default: AddPieChart(pInput->Get_Shape(i)); break;
		case  1: AddBarChart(pInput->Get_Shape(i)); break;
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CCreateChartLayer::AddPieChart(CSG_Shape *pShape)
{
	CSG_Parameter_Table_Fields &Fields = *Parameters("FIELDS")->asTableFields();

	double fSum = 0., fPartialSum = 0., fSize = pShape->asDouble(Parameters("SIZE")->asInt());

	fSize = m_fMinSize + (m_fMaxSize - m_fMinSize) / (m_fMaxValue - m_fMinValue) * (fSize - m_fMinValue);

	for(int i=0; i<Fields.Get_Count(); i++)
	{
		fSum += pShape->asDouble(Fields[i]);
	}

	if( !fSum )
	{
		return;
	}

	CSG_Point Center(pShape->Get_Centroid());

	for(int i=0; i<Fields.Get_Count(); i++)
	{
		CSG_Shape *pSector = m_pOutput->Add_Shape();

		pSector->Set_Value(0, i + 1);
		pSector->Set_Value(1, pShape->Get_Table()->Get_Field_Name(Fields[i]));

		pSector->Add_Point(Center);

		double fSectorSize = pShape->asDouble(Fields[i]) / fSum;
		int Steps = (int)(fSectorSize * 200.);
		for(int j=0; j<Steps; j++)
		{
			pSector->Add_Point(
				Center.x + fSize * sin((fPartialSum + (double)j / 200.) * M_PI_360),
				Center.y + fSize * cos((fPartialSum + (double)j / 200.) * M_PI_360)
			);
		}

		fPartialSum +=fSectorSize;
		pSector->Add_Point(
			Center.x + fSize * sin(fPartialSum * M_PI_360),
			Center.y + fSize * cos(fPartialSum * M_PI_360)
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CCreateChartLayer::AddBarChart(CSG_Shape* pShape)
{
	CSG_Parameter_Table_Fields &Fields = *Parameters("FIELDS")->asTableFields();

	double fMin = 0., fMax = 0., fSize = pShape->asDouble(Parameters("SIZE")->asInt());

	fSize = m_fMinSize + (m_fMaxSize - m_fMinSize) / (m_fMaxValue - m_fMinValue) * (fSize - m_fMinValue);

	for(int i=0; i<Fields.Get_Count(); i++)
	{
		double Value = pShape->asDouble(Fields[i]);

		if     ( i == 0       ) { fMin = fMax = Value; }
		else if( fMax < Value ) {        fMax = Value; }
		else if( fMin > Value ) { fMin        = Value; }
	}

	if( fMax > 0 && fMin > 0 ) { fMin = 0; }
	if( fMax < 0 && fMin < 0 ) { fMax = 0; }

	CSG_Point Point(pShape->Get_Centroid()); Point.x -= fSize / 2.;

	double fBarWidth = fSize / (double)Fields.Get_Count();

	for(int i=0; i<Fields.Get_Count(); i++)
	{
		CSG_Shape *pSector = m_pOutput->Add_Shape();

		pSector->Set_Value(0, i + 1);
		pSector->Set_Value(1, pShape->Get_Table()->Get_Field_Name(Fields[i]));

		double fBarHeight = fSize * pShape->asDouble(Fields[i]) / (fMax - fMin);

		pSector->Add_Point(Point.x + fBarWidth * (i    ), Point.y);
		pSector->Add_Point(Point.x + fBarWidth * (i + 1), Point.y);
		pSector->Add_Point(Point.x + fBarWidth * (i + 1), Point.y + fBarHeight);
		pSector->Add_Point(Point.x + fBarWidth * (i    ), Point.y + fBarHeight);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
