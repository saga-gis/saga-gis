/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                imagery_photogrammetry                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      methods.cpp                      //
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
#include "methods.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
namespace methods {

//---------------------------------------------------------
CSG_Vector calcRotations(CSG_Vector c, CSG_Vector t)
{

	CSG_Vector exOri(3);

	exOri[0] = atan2((t[2]-c[2]) , (sqrt(pow((t[0]-c[0]),2.0) + pow((t[1]-c[1]),2))));
	exOri[1] = 0;
	exOri[2] = atan2((t[1]-c[1]) , (t[0]-c[0])) - M_PI * 0.5;
	
	return exOri;
}

//---------------------------------------------------------
CSG_Matrix calcRotnMatrix(CSG_Vector params)
{
	CSG_Matrix rMat(3,3);
	 /* 
		omega = params[0];
		kappa = params[1];
		alpha = params[2];
	*/
	
	double sw = sin(params[0]);
	double sk = sin(params[1]);
	double sa = sin(params[2]);

	double cw = cos(params[0]);
	double ck = cos(params[1]);
	double ca = cos(params[2]);
	
	rMat[0][0] = ck * ca;
	rMat[0][1] = ck * sa;
	rMat[0][2] = -sk;

	rMat[1][0] = sw * sk * ca - cw * sa;
	rMat[1][1] = sw * sk * sa + cw * ca;
	rMat[1][2] = sw * ck;

	rMat[2][0] = cw * sk * ca + sw * sa;
	rMat[2][1] = cw * sk * sa - sw * ca;
	rMat[2][2] = cw * ck;

	return rMat;

}

//---------------------------------------------------------
std::pair<double,double> minmax(CSG_Vector v)
{
	double min = v[0];
	double max = v[0];

	for (int i = 0; i < v.Get_Length(); i++) {
		if (v[i] > v[0]) {
			max = v[i];
		}
		else if (v[i] < v[0]) {
			min = v[i];
		}
	}

	return std::make_pair(min, max);
}

} // namespace methods

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------