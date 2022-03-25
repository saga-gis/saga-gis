///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    pc_transform.cpp                   //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
//                                                       //
//    Based on TransformShapes.cpp, (C) Victor Olaya     //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pc_transform.h"


///////////////////////////////////////////////////////////
//                                                       //
//              Construction/Destruction                 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_Transform::CPC_Transform(void)
{
    //-----------------------------------------------------
    Set_Name(_TL("Transform Point Cloud"));

    Set_Author(_TL("V. Wichmann (c) 2010"));

    Set_Description (_TW(
        "The tool allows one to move, rotate and/or scale a point cloud.\n\n")
    );

    //-----------------------------------------------------
    Parameters.Add_PointCloud("",
        "IN"    , _TL("Input"),
        _TL("The input point cloud."),
        PARAMETER_INPUT
    );

    Parameters.Add_PointCloud("",
        "OUT"   , _TL("Output"), 
        _TL("The transformed output point cloud."),
        PARAMETER_OUTPUT
    );

    //-----------------------------------------------------
    Parameters.Add_Node("", "MOVE"      , _TL("Translation")    , _TL("The amount in map units that vertices will be moved."));

    Parameters.Add_Double("MOVE"    , "DX"      , _TL("dX")         , _TL("The shift along the x-axis [map units].")        , 0.0);
    Parameters.Add_Double("MOVE"    , "DY"      , _TL("dY")         , _TL("The shift along the y-axis [map units].")        , 0.0);
    Parameters.Add_Double("MOVE"    , "DZ"      , _TL("dZ")         , _TL("The shift along the z-axis [map units].")        , 0.0);

    //-----------------------------------------------------
    Parameters.Add_Node("", "ROTATE"    , _TL("Rotation")       , _TL("The rotation angles around coordinate axes in degree counting clockwise."));
    
    Parameters.Add_Double("ROTATE"  , "ANGLEX"  , _TL("Angle X")    , _TL("Angle in degrees, clockwise around x-axis.")     , 0.0);
    Parameters.Add_Double("ROTATE"  , "ANGLEY"  , _TL("Angle Y")    , _TL("Angle in degrees, clockwise around y-axis.")     , 0.0);
    Parameters.Add_Double("ROTATE"  , "ANGLEZ"  , _TL("Angle Z")    , _TL("Angle in degrees, clockwise around z-axis.")     , 0.0);

    //-----------------------------------------------------
    Parameters.Add_Node("", "SCALE"     , _TL("Scaling")        , _TL("The scale factors to apply."));

    Parameters.Add_Double("SCALE"   , "SCALEX"  , _TL("Scale Factor X") , _TL("The scale factor in x-direction.")           , 1.0);
    Parameters.Add_Double("SCALE"   , "SCALEY"  , _TL("Scale Factor Y") , _TL("The scale factor in y-direction.")           , 1.0);
    Parameters.Add_Double("SCALE"   , "SCALEZ"  , _TL("Scale Factor Z") , _TL("The scale factor in z-direction.")           , 1.0);

    //-----------------------------------------------------
    Parameters.Add_Node("", "ANCHOR"    , _TL("Anchor Point")   , _TL("The anchor point for scaling and rotation."));

    Parameters.Add_Double("ANCHOR"  , "ANCHORX" , _TL("X")              , _TL("The x-coordinate of the anchor point.")      , 0.0);
    Parameters.Add_Double("ANCHOR"  , "ANCHORY" , _TL("Y")              , _TL("The y-coordinate of the anchor point.")      , 0.0);
    Parameters.Add_Double("ANCHOR"  , "ANCHORZ" , _TL("Z")              , _TL("The z-coordinate of the anchor point.")      , 0.0);
}

//---------------------------------------------------------
CPC_Transform::~CPC_Transform(void)
{}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPC_Transform::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
    if( pParameter->Cmp_Identifier("IN") && pParameter->asPointCloud() != NULL )
    {
        pParameters->Set_Parameter("ANCHORX",  pParameter->asPointCloud()->Get_Extent().Get_Center().Get_X());
        pParameters->Set_Parameter("ANCHORY",  pParameter->asPointCloud()->Get_Extent().Get_Center().Get_Y());
        pParameters->Set_Parameter("ANCHORZ", (pParameter->asPointCloud()->Get_ZMin() + pParameter->asPointCloud()->Get_ZMax()) / 2.0);
    }

    return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Transform::On_Execute(void)
{
    bool            bCopy;
    double          angleX, angleY, angleZ;
    TSG_Point_Z     P, Q, Move, Scale, Anchor;
    CSG_PointCloud  *pIn, *pOut;
    double          a11, a12, a13, a21, a22, a23, a31, a32, a33;

    //-----------------------------------------------------
    pIn         = Parameters("IN")      ->asPointCloud();
    pOut        = Parameters("OUT")     ->asPointCloud();
    Scale.x     = Parameters("SCALEX")  ->asDouble();
    Scale.y     = Parameters("SCALEY")  ->asDouble();
    Scale.z     = Parameters("SCALEZ")  ->asDouble();
    Move.x      = Parameters("DX")      ->asDouble();
    Move.y      = Parameters("DY")      ->asDouble();
    Move.z      = Parameters("DZ")      ->asDouble();
    Anchor.x    = Parameters("ANCHORX") ->asDouble();
    Anchor.y    = Parameters("ANCHORY") ->asDouble();
    Anchor.z    = Parameters("ANCHORZ") ->asDouble();

    angleX      = Parameters("ANGLEX")  ->asDouble() * -M_DEG_TO_RAD;
    angleY      = Parameters("ANGLEY")  ->asDouble() * -M_DEG_TO_RAD;
    angleZ      = Parameters("ANGLEZ")  ->asDouble() * -M_DEG_TO_RAD;

    if( pIn == pOut )
    {
        bCopy   = true;
        pOut    = SG_Create_PointCloud();
    }
    else
        bCopy   = false;

    pOut->Create(pIn);

    pOut->Fmt_Name("%s [%s]", pIn->Get_Name(), _TL("Transformed"));


    //-----------------------------------------------------
    for (int iPoint=0; iPoint<pIn->Get_Point_Count(); iPoint++)
    {
        P   = pIn->Get_Point(iPoint);

        // anchor shift
        P.x -= Anchor.x;
        P.y -= Anchor.y;
        P.z -= Anchor.z;

        // create rotation matrix
        a11 = cos(angleY) * cos(angleZ);
        a12 = -cos(angleX) * sin(angleZ) + sin(angleX) * sin(angleY) * cos(angleZ);
        a13 = sin(angleX) * sin(angleZ) + cos(angleX) * sin(angleY) * cos(angleZ);

        a21 = cos(angleY) * sin(angleZ);
        a22 = cos(angleX) * cos(angleZ) + sin(angleX) * sin(angleY) * sin(angleZ);
        a23 = -sin(angleX) * cos(angleZ) + cos(angleX) * sin(angleY) * sin(angleZ);

        a31 = -sin(angleY);
        a32 =  sin(angleX) * cos(angleY);
        a33 = cos(angleX) * cos(angleY);


        // transform
        Q.x = (P.x * a11 + P.y * a12 + P.z * a13) * Scale.x;
        Q.y = (P.x * a21 + P.y * a22 + P.z * a23) * Scale.y;
        Q.z = (P.x * a31 + P.y * a32 + P.z * a33) * Scale.z;

        // undo anchor shift and apply move
        Q.x += Anchor.x + Move.x;
        Q.y += Anchor.y + Move.y;
        Q.z += Anchor.z + Move.z;

        pOut->Add_Point(Q.x, Q.y, Q.z);

        for (int j=0; j<pIn->Get_Attribute_Count(); j++)
        {
            switch (pIn->Get_Attribute_Type(j))
            {
            default:                    pOut->Set_Attribute(iPoint, j, pIn->Get_Attribute(iPoint, j));      break;
            case SG_DATATYPE_Date:
            case SG_DATATYPE_String:    CSG_String sAttr; pIn->Get_Attribute(iPoint, j, sAttr); pOut->Set_Attribute(iPoint, j, sAttr);      break;
            }
        }
    }

    //-----------------------------------------------------
    if( bCopy )
    {
        pIn->Assign(pOut);
        delete(pOut);
    }

    //-----------------------------------------------------
    return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
