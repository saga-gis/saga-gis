
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Shapes_IO                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Generate.cpp                      //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "generate.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGenerate_Export::CGenerate_Export(void)
{
	CSG_Parameter	*pNode;

	Set_Name(_TL("Export Shapes to Generate"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW("Export generate shapes format."));

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"	, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME", _TL("File"),
		_TL("")
	);
}

//---------------------------------------------------------
CGenerate_Export::~CGenerate_Export(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGenerate_Export::On_Execute(void)
{
	int			iShape, iPart, iPoint, iField;
	FILE		*aus;
	TSG_Point	Point;
	CSG_Shape	*pShape;
	CSG_Shapes	*pLayer;
	CSG_String	fName;

	fName	= Parameters("FILENAME")->asString();

	if(	(pLayer = Parameters("SHAPES")->asShapes()) != NULL )
	{
		if( (aus = fopen(fName.b_str(), "w")) != NULL )
		{
			if( pLayer->Get_Field_Count() > 0 )
			{
				iField	= Parameters("FIELD")->asInt();

				if( pLayer->Get_Field_Type(iField) == SG_DATATYPE_String )
				{
					iField	= -1;
				}

				fprintf(aus, "EXP %s\nARC ", pLayer->Get_Name());

				for(iShape=0; iShape<pLayer->Get_Count() && Set_Progress(iShape, pLayer->Get_Count()); iShape++)
				{
					pShape	= pLayer->Get_Shape(iShape);

					for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
					{
						// Value...
						if( iField < 0 )
						{
							fprintf(aus, "%d ", iShape + 1);
						}
						else
						{
							fprintf(aus, "%lf ", pShape->asDouble(iField));
						}

						// dummy_I dummy_I dummy_I dummy_I dummy_I...
						fprintf(aus, "1 2 3 4 5 ");
						// I_np...
						fprintf(aus, "%d ", pShape->Get_Point_Count(iPart));

						for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
						{
							Point	= pShape->Get_Point(iPoint, iPart);

							fprintf(aus, "%f %f ", Point.x, Point.y);
						}
					}
				}
			}

			fclose(aus);

			return( true );
		}
	}

	return( false );
}
