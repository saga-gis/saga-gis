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
//                     Resection.cpp                     //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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
#include "Resection.h"
#include "methods.h"
#include <algorithm>

// Convergence Criteria (mm)
const double thresh = 1;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CResection::CResection(void)
{
	Set_Name		(_TL("Resection (Terrestrial)"));

	Set_Author		(SG_T("HfT Stuttgart (c) 2013"));

	Set_Description	(_TW(
		"Single Image Spatial Resection (Terrestrial): from at least 3 image points with known "
		"coordinates, the Cardan angles of the image orientation and the coordinates of the "
		"perspective center are calculated by a least-squares adjustment. The Cardan angles "
		"refer to the following transformation between image coordinates (x_image) and global "
		"coordinates (X_Global): \n\t x_image = R_1(omega) * R_2(kappa) * R_3(alpha) * X_Global "
		"\n Here R_1, R_2, R_3 denote rotation matrices of a right-handed (passive) coordinate "
		"transformation.\n\nThe inputs consist of a point cloud containing the identical points "
		"with their pixel indices as additional attributes. The origin of pixels is in the lower "
		"left corner of the image. The interior orientation parameters of the camera is to be "
		"provided. These include Focal Length (mm), Pixel Size (um), Principal Point Offsets (pixels) "
		"and optionally the Radial Distortion Parameters. The distortion model being used is as follows: "
		"\n	 x_d = x_u (1 - dR)"
		"\n	 y_d = y_u (1 - dR) "
		"\n where, dR = K1 * r_u ^ 2 + K2 * r_u ^ 4 + K3 * r_u ^ 6,"
		"\n r_u ^ 2  = x_u ^ 2 + y_u ^ 2,"
		"\n x_u, y_u are the undistorted (corrected) image coordinates in mm,"
		"\n x_d, y_d are the distorted (observed) image coordinates in mm,"
		"\n K1 is in [mm ^ -2], K2 is in [mm ^ -4],  K3 is in [mm ^ -6]."
		"\nApproximate coordinates in [m] for the Projection Center and the center of image are also "
		"to be provided.\n\nFollowing the adjustment the results are written to a text file. The main "
		"contents of the text file are: for each iteration the Sum of Squared Residuals, the A-Posteriori "
		"Standard Deviation (Sigma Naught) and the Condition of Normal Matrix, and the final estimated "
		"exterior orientation parameters (Xc, Yc, Zc, Omega, Kappa, Alpha).\n\n Optionally the Principal "
		"Point Offsets can be estimated. This requires at least 4 image points as input."
		"\n\n[Warning] The tool is dedicated to the terrestrial case which means the viewing direction "
		"must be sufficiently different from the vertical direction."
		"\nThe estimation of Principal Point Offsets is not recommended if the condition of the Normal "
		"Matrix is less than 10 ^ -7."
		"\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		NULL	, "POINTS"		, _TL("Measured Points (PC)"),
		_TL("List of Measured Points as PC"),
		PARAMETER_INPUT
	);

	Parameters.Add_Node(NULL, "INT ORI", _TL("Interior Orientation Parameters"), _TL("Internal Orientation Parameters"));
		Parameters.Add_Value (Parameters("INT ORI"), "F", _TL ("Focal Length (mm)"), _TL ("Focal Length in mm"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("INT ORI"), "W", _TL ("Pixel Width (um)"), _TL ("Pixel Width in micro meters"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("INT ORI"), "EST_OFFSETS", _TL ("Estimate Principal Point Offsets?"), _TL ("Do you wish to estimate Principal Point Offsets?"), PARAMETER_TYPE_Bool, false);
		Parameters.Add_Value (Parameters("INT ORI"), "ppX", _TL ("Principal Point Offset in X (pixels)"), _TL ("Principal Point Offset in X"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("INT ORI"), "ppY", _TL ("Principal Point Offset in Y (pixels)"), _TL ("Principal Point Offset in Y"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("INT ORI"), "GIVE_DISTORTIONS", _TL ("Provide Radial Distortion Parameters?"), _TL ("Do you wish to provide the Radial Distortion Parameters?"), PARAMETER_TYPE_Bool, false);
		Parameters.Add_Value (Parameters("INT ORI"), "K1", _TL ("K1"), _TL ("K1"), PARAMETER_TYPE_Double, 0.0);
		Parameters.Add_Value (Parameters("INT ORI"), "K2", _TL ("K2"), _TL ("K2"), PARAMETER_TYPE_Double, 0.0);
		Parameters.Add_Value (Parameters("INT ORI"), "K3", _TL ("K3"), _TL ("K3"), PARAMETER_TYPE_Double, 0.0);

	Parameters.Add_Node(NULL, "PROJ CENTER", _TL("Approximate Coordinates of Projection Center"), _TL("Approximate Coordinates of Projection Center"));
		Parameters.Add_Value (Parameters("PROJ CENTER"), "Xc", _TL ("X"), _TL ("Approximate Coordinate"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("PROJ CENTER"), "Yc", _TL ("Y"), _TL ("Approximate Coordinate"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("PROJ CENTER"), "Zc", _TL ("Z"), _TL ("Approximate Coordinate"), PARAMETER_TYPE_Double);

	Parameters.Add_Node(NULL, "TARGET", _TL("Approximate Coordinates of Target Point"), _TL("Approximate Coordinates of Target Point"));
		Parameters.Add_Value (Parameters("TARGET"), "Xt", _TL ("X"), _TL ("Approximate Coordinate"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("TARGET"), "Yt", _TL ("Y"), _TL ("Approximate Coordinate"), PARAMETER_TYPE_Double);
		Parameters.Add_Value (Parameters("TARGET"), "Zt", _TL ("Z"), _TL ("Approximate Coordinate"), PARAMETER_TYPE_Double);

	Parameters.Add_FilePath(
		NULL	, "OUTPUT FILE"	, _TL("Output Text File"),
		_TL("The file to write the Calculation Results to."),
		CSG_String::Format(SG_T("%s|%s"),
			_TL("Text Files (*.txt)")	, SG_T("*.txt")
		), NULL, true
	);
	
}

//---------------------------------------------------------
bool CResection::On_Execute(void)
{

	CSG_PointCloud			*pPoints;									// Input Point Cloud
	CSG_String				fileName;
	CSG_File				*pTabStream = NULL;
	int n					= 6;										// Number of unknowns
	CSG_Vector center(3);
	CSG_Vector target(3);

	double c			= Parameters("F")			->asDouble();		// Focal Length (mm)
	double pixWmm		= Parameters("W")			->asDouble() / 1000;// Pixel Width (mm)
	double ppOffsetX	= Parameters("ppX")			->asDouble();		// Principal Point Offset X (pixels)
	double ppOffsetY	= Parameters("ppY")			->asDouble();		// Principal Point Offset Y (pixels)
	pPoints				= Parameters("POINTS")		->asPointCloud();
	fileName			= Parameters("OUTPUT FILE")	->asString();
	center[0]			= Parameters("Xc")			->asDouble();
	center[1]			= Parameters("Yc")			->asDouble();
	center[2]			= Parameters("Zc")			->asDouble();
	target[0]			= Parameters("Xt")			->asDouble();
	target[1]			= Parameters("Yt")			->asDouble();
	target[2]			= Parameters("Zt")			->asDouble();

	int pointCount = pPoints->Get_Point_Count();

	bool estPPOffsets = false;

	if ( Parameters("EST_OFFSETS")->asBool() ) {

		estPPOffsets = true;
		n = 8;															// Increase number of unknowns by 2
	}

	bool applyDistortions = false;
	CSG_Vector K(3);

	if ( Parameters("GIVE_DISTORTIONS")->asBool() ) {

		applyDistortions = true;
		K[0]			= Parameters("K1")			->asDouble();
		K[1]			= Parameters("K2")			->asDouble();
		K[2]			= Parameters("K3")			->asDouble();

	}

	double dxapp = center [0] - target [0];
	double dyapp = center [1] - target [1];
	double dzapp = center [2] - target [2];
	double h_d	= sqrt (dxapp * dxapp + dyapp * dyapp + dzapp * dzapp);	// Distance between Proj. Center & Target (m)
	double h_dmm = h_d * 1000;											// Convert to mm

	if( fileName.Length() == 0 )
	{
		SG_UI_Msg_Add_Error(_TL("Please provide an output file name!"));
		return( false );
	}

	pTabStream = new CSG_File();

	if( !pTabStream->Open(fileName, SG_FILE_W, false) )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(_TL("Unable to open output file %s!"), fileName.c_str()));
		delete (pTabStream);
		return (false);
	}


	CSG_Vector rotns = methods::calcRotations(center,target);			// Approx. rotations omega, kappa, alpha

	CSG_String msg = "********* Initial Approximate Values *********";
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);

	msg = SG_T("Rotation Angles:");
	pTabStream->Write(SG_T("\n") + msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);

	msg = SG_T("Omega:\t") + SG_Get_String(rotns[0],6,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);
	msg = SG_T("Kappa:\t") + SG_Get_String(rotns[1],6,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);
	msg = SG_T("Alpha:\t") + SG_Get_String(rotns[2],6,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);

	msg = SG_T("Projection Center:");
	pTabStream->Write(SG_T("\n") + msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);

	msg = SG_T("Xc:\t") + SG_Get_String(center[0],4,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);
	msg = SG_T("Yc:\t") + SG_Get_String(center[1],4,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);
	msg = SG_T("Zc:\t") + SG_Get_String(center[2],4,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);
	
	if (estPPOffsets) {

		msg = SG_T("Principal Point Offsets:");
		pTabStream->Write(SG_T("\n") + msg + SG_T("\n"));
		SG_UI_Msg_Add(msg, true);

		msg = SG_T("ppX:\t") + SG_Get_String(ppOffsetX,5,false);
		pTabStream->Write(msg + SG_T("\n"));
		SG_UI_Msg_Add(msg, true);
		msg = SG_T("ppY:\t") + SG_Get_String(ppOffsetY,5,false);
		pTabStream->Write(msg + SG_T("\n"));
		SG_UI_Msg_Add(msg, true);

	}

	double itrNo = 0;
	CSG_Matrix invN;
	
	while (true) {													// Begin Iterations

		itrNo++;
		
		double omega = rotns[0];
		double kappa = rotns[1];
		double alpha = rotns[2];

		CSG_Matrix R = methods::calcRotnMatrix(rotns);				// Rotation Matrix from approximate values
		CSG_Matrix E(3,3);											// [w1;w2;w3] = E * [dw;dk;da]

		E[0][0] = -1;
		E[0][1] = E[1][0] = E[2][0] = 0;
		E[0][2] = sin(kappa);
		E[1][1] = -cos(omega);
		E[1][2] = -sin(omega) * cos(kappa);
		E[2][1] = sin(omega);
		E[2][2] = -cos(omega) * cos(kappa);

		CSG_Matrix N(n,n);											// Transpose(Design Matrix) * I * Design Matrix
		CSG_Vector ATL(n);											// Transpose(Design Matrix) * I * Shortened obs. vector

		double SS = 0;
		double sigma_naught = 0;
		
		for (int i = 0; i < pointCount; i++) {
			
			CSG_Vector pqs(3);										// Approx. pi, qi, si

			for (int j = 0; j < 3; j++) {
				pqs[j] = R[j][0] * (pPoints->Get_X(i) - center[0]) +
						 R[j][1] * (pPoints->Get_Y(i) - center[1]) +
						 R[j][2] * (pPoints->Get_Z(i) - center[2]);
			}

			double p_i = pqs[0];
			double q_i = pqs[1];
			double s_i = pqs[2];

			double dR =  0;
			
			// Undistorted
			double x_u = c * p_i / q_i;
			double y_u = c * s_i / q_i;
			
			double c_hat = c;
			
			if (applyDistortions) {
				double r2 = x_u * x_u + y_u * y_u;
				dR =  K[0] * r2 + K[1] * r2 * r2 + K[2] * r2 * r2 * r2;
				c_hat = c * (1 - dR);
			}

			// Approx. image coordinates (with distortions)
			double x_i = (1 - dR) * x_u + ppOffsetX * pixWmm;
			double z_i = (1 - dR) * y_u + ppOffsetY * pixWmm;

			// Shortened obervation vector: dxi & dzi
			double dx_i = pPoints->Get_Attribute(i,0) * pixWmm - x_i;
			double dz_i = pPoints->Get_Attribute(i,1) * pixWmm - z_i;
			SS += pow(dx_i,2) + pow(dz_i,2);

			/*
				x_i, z_i in [mm]
				p_i,q_i,s_i in [m]
				h_d in [m]
				c, c_hat in [mm]
				h_dmm in [mm]
			*/
			CSG_Matrix L(3,2);										// CSG_Matrix takes columns first and rows second
			CSG_Matrix V(3,3);
			CSG_Matrix LR(3,2);
			CSG_Matrix LVE(3,2);

			L[0][0] = L[1][2] = c_hat / (1000 * q_i);
			L[0][2] = L[1][0] = 0;
			L[0][1] = -x_u * (1 - dR) / (1000 * q_i);
			L[1][1] = -y_u * (1 - dR) / (1000 * q_i);

			V[0][0] = V[1][1] = V[2][2] = 0;
			V[0][1] =  s_i / h_d;
			V[0][2] = -q_i / h_d;
			V[1][0] = -s_i / h_d;
			V[1][2] =  p_i / h_d;
			V[2][0] =  q_i / h_d;
			V[2][1] = -p_i / h_d;

			LVE = ( L * V ) * E;
			LR = L * R;

			// Design Matrix (J)
			CSG_Matrix design(n,2);

			for(int j = 0; j < 2; j++) {
				for(int k = 0; k < 3; k++) {
					design[j][k] = LVE[j][k];
					design[j][k+3] = -LR[j][k];
				}
			}

			if ( estPPOffsets ) {
				design[0][6] = design[1][7] = 1.0;
			}

			// Build Normal Matrix
			for(int j = 0; j < n; j++) {
				for(int k = 0; k < n; k++) {
					N[j][k] += (design[0][j] * design[0][k] + design[1][j] * design[1][k]);
				}
			}

			// Build Tranpose (J) * I * (Shortened obs. vector)
			for (int m=0; m < n; m++) {
				ATL[m] += design[0][m] * dx_i + design[1][m] * dz_i;
			}

			L.Destroy();
			V.Destroy();
			LR.Destroy();
			LVE.Destroy();
			pqs.Destroy();
			design.Destroy();

		} // end looping over observations

		// Eigen values and Eigen Vectors
		CSG_Vector eigenVals(n);
		CSG_Matrix eigenVecs(n,n);
		SG_Matrix_Eigen_Reduction(N, eigenVecs, eigenVals, true);

		// One of the Eigen Values is 0
		// workaround the problem that the following snippet is C++11:
		// if (std::any_of(eigenVals.cbegin(), eigenVals.cend(), [] (double i) { return i == 0; }))
		bool	bValid = true;

		for (int i=0; i < eigenVals.Get_Size(); i++)
		{
			if (eigenVals.Get_Data(i) == 0.0)
			{
				bValid = false;
				break;
			}
		}

		if (!bValid)
		{
			msg = "The Normal Matrix has a rank defect. Please measure more points.";
			pTabStream->Write(msg + SG_T("\n"));
			SG_UI_Msg_Add(msg, true);
			break;
		}

		double mx = *std::max_element(eigenVals.cbegin(), eigenVals.cend());
		double mn = *std::min_element(eigenVals.cbegin(), eigenVals.cend());

		// Ratio of Smallest to the Biggest Eigen value is too small
		if ((mn / mx) < pow(10,-12.0)) {
			msg = SG_T("Condition of the Matrix of Normal Equations:\t") + CSG_String::Format(SG_T("  %13.5e"), mn/mx);
			pTabStream->Write(msg + SG_T("\n"));
			SG_UI_Msg_Add(msg, true);
			msg = "The Normal Matrix is weakly conditioned. Please measure more points.";
			pTabStream->Write(msg + SG_T("\n"));
			SG_UI_Msg_Add(msg, true);
			break;
		}

		// Calculate the adjustments
		double absMax = 0;
		invN = N.Get_Inverse();
		CSG_Vector est_param_incs = invN * ATL;

		for (int i = 0; i < n; i++) {
			if (abs(est_param_incs[i]) > absMax) {
				absMax = abs(est_param_incs[i]);
			}
		}

		if (absMax < thresh) {
			msg = "Solution has converged.";
			pTabStream->Write(SG_T("\n") + msg + SG_T("\n"));
			SG_UI_Msg_Add(msg, true);
			break;
		}

		for (int a = 0; a < 3; a++) {
			rotns[a] += est_param_incs[a] / h_dmm;								// New Approx. rotations omega, kappa, alpha
			center[a] += est_param_incs[a+3] / 1000;							// New Approx. Projection Center
		}

		if ( estPPOffsets ) {
			ppOffsetX += (est_param_incs[6] / pixWmm);							// New Approx. Principal Point
			ppOffsetY += (est_param_incs[7] / pixWmm);
		}

		sigma_naught = sqrt(SS / (2 * pointCount - n));

		// Writing To Output File & SAGA Console
		msg = "********* Iteration: " + SG_Get_String(itrNo,0,false) + " *********";
		pTabStream->Write(SG_T("\n") + msg + SG_T("\n"));
		SG_UI_Msg_Add(msg, true);

		msg = "Sum of Squared Residuals:\t" + SG_Get_String(SS,5,false);
		pTabStream->Write(SG_T("\n") + msg + SG_T("\n"));
		SG_UI_Msg_Add(msg, true);
		
		msg = "Sigma Naught:\t" + SG_Get_String(sigma_naught,5,false);
		pTabStream->Write(msg + SG_T("\n"));
		SG_UI_Msg_Add(msg, true);
		
		msg = SG_T("Condition of the Matrix of Normal Equations:\t") + CSG_String::Format(SG_T("  %13.5e"), mn/mx);
		pTabStream->Write(msg + SG_T("\n"));
		SG_UI_Msg_Add(msg, true);
		
		R.Destroy();
		E.Destroy();
		N.Destroy();
		ATL.Destroy();
		invN.Destroy();
		eigenVals.Destroy();
		eigenVecs.Destroy();
		est_param_incs.Destroy();

	} // end of iterations

	msg = "********* Final Estimated Parameters *********";
	pTabStream->Write(SG_T("\n") + msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);

	msg = SG_T("Rotation Angles:");
	pTabStream->Write(SG_T("\n") + msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);

	msg = SG_T("Omega:\t") + SG_Get_String(rotns[0],6,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);
	msg = SG_T("Kappa:\t") + SG_Get_String(rotns[1],6,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);
	msg = SG_T("Alpha:\t") + SG_Get_String(rotns[2],6,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);

	msg = SG_T("Projection Center:");
	pTabStream->Write(SG_T("\n") + msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);

	msg = SG_T("Xc:\t") + SG_Get_String(center[0],4,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);
	msg = SG_T("Yc:\t") + SG_Get_String(center[1],4,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);
	msg = SG_T("Zc:\t") + SG_Get_String(center[2],4,false);
	pTabStream->Write(msg + SG_T("\n"));
	SG_UI_Msg_Add(msg, true);

	if (estPPOffsets) {

		msg = SG_T("Principal Point Offsets:");
		pTabStream->Write(SG_T("\n") + msg + SG_T("\n"));
		SG_UI_Msg_Add(msg, true);

		msg = SG_T("ppX:\t") + SG_Get_String(ppOffsetX,5,false);
		pTabStream->Write(msg + SG_T("\n"));
		SG_UI_Msg_Add(msg, true);
		msg = SG_T("ppY:\t") + SG_Get_String(ppOffsetY,5,false);
		pTabStream->Write(msg + SG_T("\n"));
		SG_UI_Msg_Add(msg, true);

	}


	K.Destroy();
	rotns.Destroy();
	center.Destroy();
	target.Destroy();

	pTabStream->Close();
	
	return true;
}

//---------------------------------------------------------
int CResection::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{

	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("GIVE_DISTORTIONS")) )
	{
		pParameters->Get_Parameter("K1")->Set_Enabled( pParameter->asBool() );
		pParameters->Get_Parameter("K2")->Set_Enabled( pParameter->asBool() );
		pParameters->Get_Parameter("K3")->Set_Enabled( pParameter->asBool() );
	}

	return( 1 );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------