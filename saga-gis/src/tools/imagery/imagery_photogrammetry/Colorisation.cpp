/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                imagery_photogrammetry                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Colorisation.cpp                    //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     HfT Stuttgart                     //
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
//    e-mail:     avishek.dutta@hft-stuttgart.de         //
//                                                       //
//    contact:    Avishek Dutta                          //
//                Hochschule fuer Technik Stuttgart      //
//                Schellingstr. 24                       //
//                D-70174 Stuttgart                      //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Colorisation.h"
#include "methods.h"
#include <saga_api/api_core.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CColorisation::CColorisation(void)
{
	Set_Name		(_TL("Colorisation (PC)"));

	Set_Author		(SG_T("HfT Stuttgart (c) 2013"));

	Set_Description	(_TW(
					"This tool attaches the color information from a RGB image to Laser Points."
					"\n The RGB Image and the Point Cloud to be colorised are the inputs along "
					"with the interior and exterior orientation parameters of the camera. The "
					"necessary interior orientation parameters are Focal Length (mm), Pixel "
					"Size (um), Principal Point Offsets (pixels) and optionally the Radial "
					"Distortion Parameters. The distortion model being used is as follows: "
					"\n	 x_d = x_u (1 - dR)"
					"\n	 y_d = y_u (1 - dR) "
					"\n where, dR = K1 * r_u ^ 2 + K2 * r_u ^ 4 + K3 * r_u ^ 6,"
					"\n r_u ^ 2  = x_u ^ 2 + y_u ^ 2,"
					"\n x_u, y_u are the undistorted (corrected) image coordinates in mm,"
					"\n x_d, y_d are the distorted (observed) image coordinates in mm,"
					"\n K1 is in [mm ^ -2], K2 is in [mm ^ -4],  K3 is in [mm ^ -6]."
					"\nThe necessary exterior orientation parameters are the coordinates of the "
					"Projection Center and the Cardan angles Omega, Kappa and Alpha. The Cardan "
					"angles refer to the following transformation between image coordinates "
					"(x_image) and global coordinates (X_Global): "
					"\n x_image = R_1(omega) * R_2(kappa) * R_3(alpha) * X_Global "
					"\n\n [Optional] If the Time Stamp of the RGB Image is given, an acceptable time difference"
					" between the points and the image must provided in order to colorise only those points falling"
					" that range. If no time is given all points are colorised."
					"\n The result is a colorised point cloud."
					"\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL, "rgbImage", _TL("RGB Image"), _TL("RGB Image"), PARAMETER_INPUT);

	Parameters.Add_PointCloud(
		NULL	, "PC_IN"		, _TL("Points to be Colorised"),
		_TL("Points to be Colorised"),
		PARAMETER_INPUT
	);

	Parameters.Add_Value (NULL, "GIVE_TIME", _TL ("Provide Time stamp of Image?"), _TL ("Do you wish to provide the time stamp of Image?"), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value (NULL, "IMG_TIME", _TL ("Time stamp of Image"), _TL ("Time stamp of Image"), PARAMETER_TYPE_Double);
	Parameters.Add_Value (NULL, "TIME_DIFF", _TL ("Time Difference between Image & Points"), _TL ("Acceptable Time Difference between Image and Laser Points"), PARAMETER_TYPE_Double);
	
	Parameters.Add_Node(NULL, "INT ORI", _TL("Interior Orientation Parameters"), _TL("Internal Orientation Parameters"));
		Parameters.Add_Value (Parameters("INT ORI"), "F", _TL ("Focal Length (mm)"), _TL ("Focal Length in mm"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("INT ORI"), "W", _TL ("Pixel Width (um)"), _TL ("Pixel Width in micro meters"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("INT ORI"), "ppX", _TL ("Principal Point Offset in X (pixles)"), _TL ("Principal Point Offset in X"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("INT ORI"), "ppY", _TL ("Principal Point Offset in Y (pixels)"), _TL ("Principal Point Offset in Y"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("INT ORI"), "GIVE_DISTORTIONS", _TL ("Provide Radial Distortion Parameters?"), _TL ("Do you wish to provide the Radial Distortion Parameters?"), PARAMETER_TYPE_Bool, false);
		Parameters.Add_Value (Parameters("INT ORI"), "K1", _TL ("K1"), _TL ("K1"), PARAMETER_TYPE_Double, 0.0);
		Parameters.Add_Value (Parameters("INT ORI"), "K2", _TL ("K2"), _TL ("K2"), PARAMETER_TYPE_Double, 0.0);
		Parameters.Add_Value (Parameters("INT ORI"), "K3", _TL ("K3"), _TL ("K3"), PARAMETER_TYPE_Double, 0.0);

	Parameters.Add_Node(NULL, "EXT ORI", _TL("Exterior Orientation Parameters"), _TL("Exterior Orientation Parameters"));
		Parameters.Add_Value (Parameters("EXT ORI"), "Xc", _TL ("Projection Centre - X"), _TL ("Projection Centre"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("EXT ORI"), "Yc", _TL ("Projection Centre - Y"), _TL ("Projection Centre"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("EXT ORI"), "Zc", _TL ("Projection Centre - Z"), _TL ("Projection Centre"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("EXT ORI"), "omega", _TL ("Omega"), _TL ("Rotation Angle"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("EXT ORI"), "kappa", _TL ("Kappa"), _TL ("Rotation Angle"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("EXT ORI"), "alpha", _TL ("Alpha"), _TL ("Rotation Angle"), PARAMETER_TYPE_Double);

	
	Parameters.Add_PointCloud(
		NULL	, "PC_OUT"		, _TL("Colorised Point Cloud"),
		_TL("Colorised Point Cloud"),
		PARAMETER_OUTPUT
	);
	
}

//---------------------------------------------------------
bool CColorisation::On_Execute(void)
{
	
	CSG_Vector center(3), angles(3);

	CSG_Grid *rgbImage		= Parameters("rgbImage")	->asGrid();
	CSG_PointCloud *pPoints	= Parameters("PC_IN")		->asPointCloud();
	CSG_PointCloud *pResult	= Parameters("PC_OUT")		->asPointCloud();
	double c				= Parameters("F")			->asDouble();		// Focal Length (mm)
	double pixWmm			= Parameters("W")			->asDouble() / 1000;// Pixel Width (mm)
	double ppOffsetX		= Parameters("ppX")			->asDouble();		// Principal Point Offset X (pixels)
	double ppOffsetY		= Parameters("ppY")			->asDouble();		// Principal Point Offset Y (pixels)
	center[0]				= Parameters("Xc")			->asDouble();
	center[1]				= Parameters("Yc")			->asDouble();
	center[2]				= Parameters("Zc")			->asDouble();
	angles[0]				= Parameters("omega")		->asDouble();
	angles[1]				= Parameters("kappa")		->asDouble();
	angles[2]				= Parameters("alpha")		->asDouble();

	int pointCount = pPoints->Get_Point_Count();
	int attrCount = pPoints->Get_Attribute_Count();
	
	bool applyDistortions = false;
	CSG_Vector K(3);
	if ( Parameters("GIVE_DISTORTIONS")->asBool() ) {

		applyDistortions = true;
		K[0]			= Parameters("K1")			->asDouble();
		K[1]			= Parameters("K2")			->asDouble();
		K[2]			= Parameters("K3")			->asDouble();

	}

	double imgTimeStamp = 0;
	double timeThresh = 0;
	bool applyTimeStamp		= Parameters("GIVE_TIME")	->asBool();
	if (applyTimeStamp) {

		imgTimeStamp = Parameters("IMG_TIME")->asDouble();
		timeThresh	= Parameters("TIME_DIFF")->asDouble();
	}

	pResult->Create(pPoints);
	pResult->Set_Name("%s (%s)", pPoints->Get_Name(), _TL("colorised"));
//	pResult->Add_Field(SG_T("Red"  ), SG_DATATYPE_Int);
//	pResult->Add_Field(SG_T("Green"), SG_DATATYPE_Int);
//	pResult->Add_Field(SG_T("Blue" ), SG_DATATYPE_Int);
	pResult->Add_Field(SG_T("RGB"  ), SG_DATATYPE_Int);
	DataObject_Update(pResult);
		
	CSG_Matrix R = methods::calcRotnMatrix(angles);

	double xmin = rgbImage->Get_XMin();
	double xmax = rgbImage->Get_XMax();
	double ymin = rgbImage->Get_YMin();
	double ymax = rgbImage->Get_YMax();
	
	int iAccept = 0;
	
	for (int i = 0; i < pointCount && Set_Progress(i, pointCount); i++) {
		
		if ( applyTimeStamp && fabs( imgTimeStamp - pPoints->Get_Attribute(i,0) ) >= timeThresh ) {
			
			continue;

		}

		CSG_Vector pqs(3);										// Approx. pi, qi, si
		for (int j = 0; j < 3; j++) {
			pqs[j] = R[j][0] * (pPoints->Get_X(i) - center[0]) 
					+ R[j][1] * (pPoints->Get_Y(i) - center[1]) 
					+ R[j][2] * (pPoints->Get_Z(i) - center[2]);
		}

		double p_i = pqs[0];
		double q_i = pqs[1];
		double s_i = pqs[2];

		// Check for points behind image plane
		if ( q_i < 0) {
			continue;
		}

		// Image coordinates in mm
		CSG_Vector pixelPos(2);
		pixelPos[0] = c * p_i / q_i;
		pixelPos[1] = c * s_i / q_i;
			
		if ( applyDistortions ) {

			pixelPos = getDistortedPositions(pixelPos, K);

		}

		// Image coordinates in pixels
		pixelPos[0] = ( pixelPos[0] / pixWmm ) + ppOffsetX;
		pixelPos[1] = ( pixelPos[1] / pixWmm ) + ppOffsetY;

		int row = (int)floor(pixelPos[1] + 0.5);
		int col = (int)floor(pixelPos[0] + 0.5);

		if ( col >= xmin && col <= xmax && row >= ymin && row <= ymax ) {

			pResult->Add_Point(pPoints->Get_X(i), pPoints->Get_Y(i), pPoints->Get_Z(i));

			int iField;

			for(iField = 0; iField < attrCount; iField ++)
			{
				pResult->Set_Attribute(iAccept, iField, pPoints->Get_Attribute(i, iField));
			}

			pResult->Set_Attribute(iAccept, iField, rgbImage->asInt( col, row ));
				
			iAccept ++;

		}

		pqs.Destroy();
		pixelPos.Destroy();

	}

	DataObject_Update(pResult);

	R.Destroy();
	K.Destroy();
	center.Destroy();
	angles.Destroy();
	
	return true;
}

//---------------------------------------------------------
int CColorisation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{

	if( pParameter->Cmp_Identifier(SG_T("GIVE_DISTORTIONS")) )
	{
		pParameters->Get_Parameter("K1")->Set_Enabled( pParameter->asBool() );
		pParameters->Get_Parameter("K2")->Set_Enabled( pParameter->asBool() );
		pParameters->Get_Parameter("K3")->Set_Enabled( pParameter->asBool() );
	}

	if( pParameter->Cmp_Identifier(SG_T("GIVE_TIME")) )
	{
		pParameters->Get_Parameter("IMG_TIME")->Set_Enabled( pParameter->asBool() );
		pParameters->Get_Parameter("TIME_DIFF")->Set_Enabled( pParameter->asBool() );
	}

	return 1;
}

//---------------------------------------------------------
CSG_Vector CColorisation::getDistortedPositions(CSG_Vector pixPos, CSG_Vector K)
{

	CSG_Vector XYd(2);

	double r2 = pow(pixPos[0], 2.0) + pow(pixPos[1], 2.0);
	double dR = K[0] * r2 + K[1] * pow(r2, 2) + K[2] * pow(r2, 3);
	
	XYd[0] = pixPos[0] * (1 - dR);
	XYd[1] = pixPos[1] * (1 - dR);

	return XYd;
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------