
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//    System for an Automated Geo-Scientific Analysis    //
//                                                       //
//                    Module Library:                    //
//                    grid_analysis                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Soil_Texture.cpp                   //
//                                                       //
//                 Copyright (C) 2007 by                 //
//                    Gianluca Massei                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for an Automated  //
// Geo-Scientific Analysis'. SAGA is free software; you  //
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
//    e-mail:     g_massa@libero.it				     	 //
//                                                       //
//    contact:    Gianluca Massei                        //
//                Department of Economics and Appraisal  //
//                University of Perugia - Italy			 //
//                www.unipg.it                           //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Soil_Texture.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char	*TEXTURE_NAMES[12]	=
{
	_TL("Clay"),
	_TL("Silty Clay"),
	_TL("Silty Clay-Loam"),
	_TL("Sandy Clay"),
	_TL("Sandy Clay-Loam"),
	_TL("Clay-Loam"),
	_TL("Silt"),
	_TL("Silt-Loam"),
	_TL("Loam"),
	_TL("Sand"),
	_TL("Loamy Sand"),
	_TL("Sandy Loam")
};

//---------------------------------------------------------
long			TEXTURE_COLOR[12]	=
{
	SG_GET_RGB(000, 000, 255),	// clay
	SG_GET_RGB(000, 200, 255),	// silty clay
	SG_GET_RGB(000, 200, 200),	// silty clay-loam
	SG_GET_RGB(200, 000, 255),	// sandy clay
	SG_GET_RGB(200, 200, 200),	// sandy clay-loam
	SG_GET_RGB(127, 127, 200),	// clay-loam
	SG_GET_RGB(000, 255, 000),	// silt
	SG_GET_RGB(127, 255, 127),	// silt-loam
	SG_GET_RGB(127, 127, 127),	// loam
	SG_GET_RGB(255, 000, 000),	// sand
	SG_GET_RGB(255, 000, 127),	// loamy sand
	SG_GET_RGB(200, 127, 127)	// sandy loam
};


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSoil_Texture::CSoil_Texture(void)
{
	Set_Name		(_TL("Soil Texture Classification"));

	Set_Author		(_TL("Copyrights (c) 2007 by Gianluca Massei"));

	Set_Description	(_TW(
		"Define soil texture with USDA scheme from sand and clay contents (grids).\n\n"
		"  1 - Clay soils\n"
		"  2 - SiltyClay soils\n"
		"  3 - SiltyClayLoam soils\n"
		"  4 - SandyClay soils\n"
		"  5 - SandyClayLoam soils\n"
		"  6 - ClayLoam soils\n"
		"  7 - Silt soils\n"
		"  8 - SiltLoam soils\n"
		"  9 - Loam soils\n"
		" 10 - Sand soils\n"
		" 11 - LoamySand soils\n"
		" 12 - SandyLoam\n"
		"\nG. Massei (g_massa@libero.it)"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "SAND"	, _TL("Sand"),
		_TL("sand content"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "CLAY"	, _TL("Clay"),
		_TL("clay content"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "TEXTURE"	, _TL("Soil Texture"),
		_TL("Soil texture"),
		PARAMETER_OUTPUT, true, GRID_TYPE_Byte
	);
}

//---------------------------------------------------------
CSoil_Texture::~CSoil_Texture(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Texture::On_Execute(void)
{
	int			x,y,intSoilType, MaxX, MaxY;
	float		Sand_Input,Clay_Input;
	CSG_Grid	*pSandInput, *pClayInput, *pOutTexture;


	pSandInput	= Parameters("SAND")->asGrid();
	pClayInput	= Parameters("CLAY")->asGrid();
	pOutTexture	= Parameters("TEXTURE")->asGrid();



	if(pSandInput->Get_NX()==pClayInput->Get_NX() 
		&& pSandInput->Get_NY()==pSandInput->Get_NY() )
	{
		MaxX= pSandInput->Get_NX();
		MaxY= pSandInput->Get_NY();
	}
	else
	{
		MaxX= 0;
		MaxY= 0;

	}

	
	for(y=0;y<MaxY;y++)
	{
		for(x=0;x<MaxX;x++)
		{
			Sand_Input	= pSandInput->asFloat(x, y); 
			Clay_Input	= pClayInput->asFloat(x, y);
			intSoilType	= OutTexture(Sand_Input, Clay_Input);

			if (intSoilType<0 || intSoilType>12)//verify range validity
				pOutTexture->Set_NoData(x, y);
			else
				pOutTexture->Set_Value(x, y, intSoilType);
		
		}
	}


	//-------------------------------------------------
	if( 1 )
	{
		CSG_Parameters	Parms;

		if( DataObject_Get_Parameters(pOutTexture, Parms) && Parms("COLORS_TYPE") && Parms("LUT") )
		{
			CSG_Table	*pLUT	= Parms("LUT")->asTable();

			for(int iClass=0; iClass<12; iClass++)
			{
				CSG_Table_Record	*pClass;

				if( (pClass = pLUT->Get_Record(iClass)) == NULL )
				{
					pClass	= pLUT->Add_Record();
				}

				pClass->Set_Value(0, TEXTURE_COLOR[iClass]);
				pClass->Set_Value(1, TEXTURE_NAMES[iClass]);
				pClass->Set_Value(2, TEXTURE_NAMES[iClass]);
				pClass->Set_Value(3, iClass + 1);
				pClass->Set_Value(4, iClass + 1);
			}

			while( pLUT->Get_Record_Count() > 12 )
			{
				pLUT->Del_Record(pLUT->Get_Record_Count() - 1);
			}

			Parms("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

			DataObject_Set_Parameters(pOutTexture, Parms);
		}
	}


	//-------------------------------------------------
	Message_Add(_TL("Hello by G. Massei"));
	
	return( true );
}


/////////////////////////////////////////
//   Define specific texture function  //
/////////////////////////////////////////

int CSoil_Texture::OutTexture(float pSand, float pClay)
{    
  int i, SoilTexture;
  
  
  int texture[13]; //store the output of txture function
 
        texture[0]=Clay_Texture(pSand, pClay);                   // 1 define Clay soils function
        texture[1]=SiltyClay_Texture(pSand, pClay);              // 2 define SiltyClay soils function
        texture[2]=SiltyClayLoam_Texture(pSand, pClay);          // 3 define SiltyClayLoam soils function
        texture[3]=SandyClay_Texture(pSand, pClay);              // 4 define SandyClay soils function
        texture[4]=SandyClayLoam_Texture(pSand, pClay);          // 5 define SandyClayLoam soils function
        texture[5]=ClayLoam_Texture(pSand, pClay);               // 6 define ClayLoam soils function
        texture[6]=Silt_Texture(pSand, pClay);                   // 7 define Silt soils function
        texture[7]=SiltLoam_Texture(pSand, pClay);               // 8 define SiltLoam soils function
        texture[8]=Loam_Texture(pSand, pClay);                   // 9 define Loam soils function
        texture[9]=Sand_Texture(pSand, pClay);                   // 10 define Sand soils function
        texture[10]=LoamySand_Texture(pSand, pClay);             // 11 define LoamySand soils function
        texture[11]=SandyLoam_Texture(pSand, pClay); 

 
        for(i=0;i<12;i=i+1)
        {
                    if (texture[i]>0)
                    {
                    SoilTexture = texture[i];  
                    }
                    
	
         }

  return SoilTexture;
  
}

int CSoil_Texture::Clay_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;//variabile locale contenente la tessitura
    int xSand[6]={0,0,20,45,45,0}; //coord. X of polygon points
    int yClay[6]={100,60,40,40,55,100}; //coord Y of polygon points
    int nPoint = 6; //n° of polygon points
    // Value of determinant A
    //int texture=1;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture = !texture;
      }
      if (texture == 1)
         {
                 texture=1;
                 }
      return texture;
}


int CSoil_Texture::SiltyClay_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;
    int xSand[4]={0,0,20,0}; //coord. X of polygon points
    int yClay[4]={60,40,40,60}; //coord Y of polygon points
    int nPoint = 4; //n° of polygon points
     
    //int texture=2;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture =!texture;
      }
            if (texture == 1)
         {
                 texture=2;
                 }
      return texture;
}

int CSoil_Texture::SiltyClayLoam_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;
    int xSand[5]={0,0,20,20,0}; //coord. X of polygon points
    int yClay[5]={40,27,27,40,40}; //coord Y of polygon points
    int nPoint = 5; //n° of polygon points
     
    //int texture=3;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture = !texture;
      }
      if (texture == 1)
         {
                 texture=3;
                 }
      return texture;
}

int CSoil_Texture::SandyClay_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;
    int xSand[4]={45,45,65,45}; //coord. X of polygon points
    int yClay[4]={55,35,35,55}; //coord Y of polygon points
    int nPoint = 4; //n° of polygon points
     
    //int texture=4;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture =!texture;
      }
      if (texture == 1)
         {
                 texture=4;
                 }
      return texture;
}

int CSoil_Texture::SandyClayLoam_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;
    int xSand[6]={45,45,52,80,65,45}; //coord. X of polygon points
    int yClay[6]={35,27,20,20,35,35}; //coord Y of polygon points
    int nPoint = 6; //n° of polygon points
     
    //int texture=5;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture = !texture;
      }
      if (texture == 1)
         {
                 texture=5;
                 }
      return texture;
}

int CSoil_Texture::ClayLoam_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;
    int xSand[5]={20,20,45,45,20}; //coord. X of polygon points
    int yClay[5]={40,27,27,40,40}; //coord Y of polygon points
    int nPoint = 5; //n° of polygon points
     
    //int texture=6;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture = !texture;
      }
      if (texture == 1)
         {
                 texture=6;
                 }
      return texture;
}

int CSoil_Texture::Silt_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;
    int xSand[5]={0,0,20,8,0}; //coord. X of polygon points
    int yClay[5]={12,0,0,12,12}; //coord Y of polygon points
    int nPoint = 5; //n° of polygon points
     
    //int texture=7;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture =!texture;
      }
      if (texture == 1)
         {
                 texture=7;
                 }
      return texture;
}

int CSoil_Texture::SiltLoam_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;
    int xSand[7]={8,20,50,23,0,0,8}; //coord. X of polygon points
    int yClay[7]={12,0,0,27,27,12,12}; //coord Y of polygon points
    int nPoint = 7; //n° of polygon points
     
    //int texture=8;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture =!texture;
      }
      if (texture == 1)
         {
                 texture=8;
                 }
      return texture;
}

int CSoil_Texture::Loam_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;
    int xSand[6]={23,43,52,52,45,23}; //coord. X of polygon points
    int yClay[6]={27,7,7,20,27,27}; //coord Y of polygon points
    int nPoint = 6; //n° of polygon points
     
    //int texture=9;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture = !texture;
      }
      if (texture == 1)
         {
                 texture=9;
                 }
      return texture;
}

int CSoil_Texture::Sand_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;
    int xSand[4]={85,100,90,85}; //coord. X of polygon points
    int yClay[4]={0,0,10,0}; //coord Y of polygon points
    int nPoint = 4; //n° of polygon points
     
    //int texture=10;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture = !texture;
      }
      if (texture == 1)
         {
                 texture=10;
                 }
      return texture;
}

int CSoil_Texture::LoamySand_Texture (float pSand, float pClay)   
{
    int i, j, texture = 0;
    int xSand[5]={70,85,90,85,70}; //coord. X of polygon points
    int yClay[5]={0,0,10,15,0}; //coord Y of polygon points
    int nPoint = 5; //n° of polygon points
     
    //int texture=11;
    
      for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
        if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
             ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
            (pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
          texture =!texture;
      }
      if (texture == 1)
         {
                 texture=11;
                 }
      return texture;
}


int CSoil_Texture::SandyLoam_Texture(float pSand, float pClay)
{
	int i, j, texture = 0;
	int xSand[8]={43,50,70,85,80,52,52,43}; //coord. pSand of polygon points
	int yClay[8]={7,0,0,15,20,20,7,7}; //coord pClay of polygon points
	int nPoint = 8; //n° of polygon points  

	for (i = 0, j = nPoint-1; i < nPoint; j = i++) {
	if ((((yClay[i] <= pClay) && (pClay < yClay[j])) ||
		 ((yClay[j] <= pClay) && (pClay < yClay[i]))) &&
		(pSand < (xSand[j] - xSand[i]) * (pClay - yClay[i]) / (yClay[j] - yClay[i]) + xSand[i]))
	  texture = !texture;
	}
	if (texture == 1)
	 {
			 texture=12;
			 }
  return texture;
}
	
	
