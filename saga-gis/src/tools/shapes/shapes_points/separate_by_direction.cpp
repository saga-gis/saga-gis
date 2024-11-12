
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     Table_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                separate_by_direction.cpp              //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "separate_by_direction.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSeparate_by_Direction::CSeparate_by_Direction(void)
{
	Set_Name		(_TL("Separate points by direction"));

	Set_Author		("O. Conrad (c) 2008");

	Set_Description	(_TW(
		"Separates points by direction. Direction is determined as average direction of three consecutive points A, B, C. "
		"If the angle between the directions of A-B and B-C is higher than given tolerance angle the point is dropped. "
		"This tool has been designed to separate GPS tracks recorded by tractors while preparing a field. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes_List("",
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Int("",
		"DIRECTIONS", _TL("Number of Directions"),
		_TL(""),
		4, 2, true
	);

	Parameters.Add_Double("",
		"TOLERANCE"	, _TL("Tolerance (Degree)"),
		_TL(""),
		5., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSeparate_by_Direction::On_Execute(void)
{
	CSG_Shapes *pPoints = Parameters("POINTS")->asShapes();

	if( !pPoints || !pPoints->is_Valid() || pPoints->Get_Count() < 3 )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Tolerance = Parameters("TOLERANCE")->asDouble() * M_DEG_TO_RAD;
	m_nSectors  = Parameters("DIRECTIONS")->asInt();
	m_dSector   = M_PI_360 / m_nSectors;

	//-----------------------------------------------------
	CSG_Parameter_Shapes_List *pOutput = Parameters("OUTPUT")->asShapesList(); pOutput->Del_Items();

	int dir_Field = pPoints->Get_Field_Count();

	for(int iSector=0; iSector<m_nSectors; iSector++)
	{
		pOutput->Add_Item(SG_Create_Shapes(SHAPE_TYPE_Point, CSG_String::Format("Direction %.2f", iSector * m_dSector * M_RAD_TO_DEG), pPoints));
		pOutput->Get_Shapes(iSector)->Add_Field(_TL("Direction"), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	CSG_Shape *pt_B = pPoints->Get_Shape(pPoints->Get_Count() - 2);
	CSG_Shape *pt_A = pPoints->Get_Shape(pPoints->Get_Count() - 1);

	double dir_A = SG_Get_Angle_Of_Direction(pt_B->Get_Point(), pt_A->Get_Point());

	for(sLong iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		pt_B = pt_A;
		pt_A = pPoints->Get_Shape(iPoint);

		double dir_B = dir_A; dir_A = SG_Get_Angle_Of_Direction(pt_B->Get_Point(), pt_A->Get_Point());
		double dif = fmod(dir_A - dir_B, M_PI_360);

		if( dif > M_PI_180 )
		{
			dif	-= M_PI_360;
		}
		else if( dif < -M_PI_180 )
		{
			dif	+= M_PI_360;
		}

		if( fabs(dif) <= m_Tolerance )
		{
			double dir = dir_B + 0.5 * dif;

			int iSector	= (int)(fmod(M_PI_360 + 0.5 * m_dSector + dir, M_PI_360) / m_dSector);

			if( iSector >= 0 && iSector < m_nSectors )
			{
				pOutput->Get_Shapes(iSector)->Add_Shape(pt_B)->Set_Value(dir_Field, dir * M_RAD_TO_DEG);
			}
		}
	}

	//-----------------------------------------------------
	for(int iSector=pOutput->Get_Item_Count()-1; iSector>=0; iSector--)
	{
		if( pOutput->Get_Shapes(iSector)->Get_Count() == 0 )
		{
			delete(pOutput->Get_Shapes(iSector));

			pOutput->Del_Item(iSector);
		}
	}

	//-----------------------------------------------------
	return( pOutput->Get_Item_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
