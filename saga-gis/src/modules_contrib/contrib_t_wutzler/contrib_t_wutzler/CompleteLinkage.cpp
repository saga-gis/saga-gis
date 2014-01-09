/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                   contrib_t_wutzler                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 CompleteLinkage.cpp                   //
//                                                       //
//            Copyright (C) 2003 Your Name               //
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
//    e-mail:     your@e-mail.abc                        //
//                                                       //
//    contact:    Your Name                              //
//                And Address                            //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


#include "CompleteLinkage.h"
#include <math.h>
#include<limits> 
#include<list>
#include<set>
#include<iostream>
#include"DebugStream.h"
#include <stdexcept>
#include <sstream>
#include "NoDataValueError.h"

using std::runtime_error;
using std::deque;

#define MIN(X,Y) (((X)<(Y))?(X):(Y))

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

CCompleteLinkage::CCompleteLinkage(void)
{
	//-----------------------------------------------------
	// Place information about your module here...

	Set_Name		(_TL("CompleteLinkage"));

	Set_Author		(_TL("Copyrights (c) 2004 by Thomas Wutzler"));

	Set_Description	(_TW(
		"CompleteLinkage\n"
	));


	//-----------------------------------------------------
	//Parameters.Add_List(NULL, SG_T("InputGrids")	, SG_T("Input Grids")	, SG_T("Grids to input"), PARAMETER_INPUT, PARAMETER_TYPE_Grid);
	//Parameters.Add_List(NULL, SG_T("InputWeights")	, SG_T("Input Weights")	, SG_T("Weights for each input Grid"), PARAMETER_INPUT, PARAMETER_TYPE_Int); does not work

	//Parameters.Add_Grid(	NULL, SG_T("Curvature")	, SG_T("Curvature")	, SG_T("Input Curvate grid.")	, PARAMETER_INPUT);
	CSG_Parameter *parentPar;
	Parameters.Add_Grid(	NULL, SG_T("InputGrid1")	, SG_T("Input Grid1")	, SG_T("Input Grid1 (e.g. profile curvatue)")	, PARAMETER_INPUT);
	parentPar = Parameters.Add_Grid(	NULL, SG_T("InputGrid2")	, SG_T("Input Grid2")	, SG_T("Input Grid2 (e.g. curvatue)")	, PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Value( parentPar, SG_T("InputWeight2"), SG_T("weighting factor"), SG_T("weighting factor of this grid compared to InputGrid1=1"),  PARAMETER_TYPE_Double, 1.0 );
	parentPar = Parameters.Add_Grid(	NULL, SG_T("InputGrid3")	, SG_T("Input Grid3")	, SG_T("Input Grid3 (e.g. slope)")	, PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Value( parentPar, SG_T("InputWeight3"), SG_T("weighting factor"), SG_T("weighting factor of this grid compared to InputGrid1=1"),  PARAMETER_TYPE_Double, 1.0 );
	parentPar = Parameters.Add_Grid(	NULL, SG_T("InputGrid4")	, SG_T("Input Grid4")	, SG_T("Input Grid4")	, PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Value( parentPar, SG_T("InputWeight4"), SG_T("weighting factor"), SG_T("weighting factor of this grid compared to InputGrid1=1"),  PARAMETER_TYPE_Double, 1.0 );
	parentPar = Parameters.Add_Grid(	NULL, SG_T("InputGrid5")	, SG_T("Input Grid5")	, SG_T("Input Grid5")	, PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Value( parentPar, SG_T("InputWeight5"), SG_T("weighting factor"), SG_T("weighting factor of this grid compared to InputGrid1=1"),  PARAMETER_TYPE_Double, 1.0 );
	Parameters.Add_Grid(	NULL, SG_T("ClassesGrid")	, SG_T("Classes")		, SG_T("Output grid of aggregated classes")	, PARAMETER_OUTPUT);
	Parameters.Add_Grid(	NULL, SG_T("OrphantsGrid"), SG_T("Orphants")	, SG_T("Output grid of removed orphants")	, PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(	NULL, SG_T("MinDistGrid"), SG_T("MinDist")	, SG_T("Output grid of minimum distances to neighbor pixels class in parameter space")	, PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(	NULL, SG_T("MinDistDirGrid"), SG_T("MinDistDir")	, SG_T("Output grid of direction of minimum distances to neighbor pixels class in parameter space")	, PARAMETER_OUTPUT_OPTIONAL);

	//Parameters.Add_Value(	NULL, SG_T("BOOLEAN")	, SG_T("Boolean")	, SG_T("A value of type boolean.")		, PARAMETER_TYPE_Bool	, true);
	//Parameters.Add_Value(	NULL, SG_T("INTEGER")	, SG_T("Integer")	, SG_T("A value of type integer.")		, PARAMETER_TYPE_Int	, 200);
	//Parameters.Add_Value(	NULL, SG_T("DOUBLE")	, SG_T("Double")	, SG_T("A floating point value.")			, PARAMETER_TYPE_Double	, 3.145);
	//Parameters.Add_Value(	NULL, SG_T("NoDataValue")	, SG_T("NoDataValue")	, SG_T("Value for 'no data'")			, PARAMETER_TYPE_Double	, -9999);
	Parameters.Add_Value(	NULL, SG_T("MaxOrphantsSize")	, SG_T("MaxOrphantsSize")	, SG_T("class of size up to this will be joined to next neighbouring class")	, PARAMETER_TYPE_Int, 5);
	Parameters.Add_Value(	NULL, SG_T("GenFactor")	, SG_T("GenFactor")	, SG_T("Factor of Generalisation (range 0-1)")	, PARAMETER_TYPE_Double	, 0.99);
	Parameters.Add_Value(	NULL, SG_T("RegionExtent"), SG_T("RegionExtent"), SG_T("Extend of one dimension of algorithms subgrid")	, PARAMETER_TYPE_Int	, 30);

	parentPar = Parameters.Add_Grid(	NULL, SG_T("ClassesGrid1")	, SG_T("Classes add1")	, SG_T("additional output of classes at specified generalisation")	, PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Value(	parentPar, SG_T("GenFactor1")	, SG_T("GenFactor add1")	, SG_T("Factor of Generalisation (range 0-genFactor)")	, PARAMETER_TYPE_Double	, 0.75);
	parentPar = Parameters.Add_Grid(	NULL, SG_T("ClassesGrid2")	, SG_T("Classes add2")	, SG_T("additional output of classes at specified generalisation")	, PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Value(	parentPar, SG_T("GenFactor2")	, SG_T("GenFactor add2")	, SG_T("Factor of Generalisation (range 0-genFactor)")	, PARAMETER_TYPE_Double	, 0.9);
	parentPar = Parameters.Add_Grid(	NULL, SG_T("ClassesGrid3")	, SG_T("Classes add3")	, SG_T("additional output of classes at specified generalisation")	, PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Value(	parentPar, SG_T("GenFactor3")	, SG_T("GenFactor add3")	, SG_T("Factor of Generalisation (range 0-genFactor)")	, PARAMETER_TYPE_Double	, 0.95);
	parentPar = Parameters.Add_Grid(	NULL, SG_T("ClassesGrid4")	, SG_T("Classes add4")	, SG_T("additional output of classes at specified generalisation")	, PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Value(	parentPar, SG_T("GenFactor4")	, SG_T("GenFactor add4")	, SG_T("Factor of Generalisation (range 0-genFactor)")	, PARAMETER_TYPE_Double	, 0.975);
	parentPar = Parameters.Add_Grid(	NULL, SG_T("ClassesGrid5")	, SG_T("Classes add5")	, SG_T("additional output of classes at specified generalisation")	, PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Value(	parentPar, SG_T("GenFactor5")	, SG_T("GenFactor add5")	, SG_T("Factor of Generalisation (range 0-genFactor)")	, PARAMETER_TYPE_Double	, 0.987);

	/*
	Parameters.Add_Choice(	NULL, SG_T("METHOD")	, SG_T("Method")	, SG_T("Choose a method from this select option.",
		"First Method|"
		"Second Method|",
		0
	);
	*/

	//----------------------------------------
	// Other init
	for( int i = 0; i < MAX_SINGLE_PIXEL_ARR; i++ ){
		//singlePixelSetArr[i].insert( -1 );
		singlePixelSetArr[i].push_back( -1 );
	}

}

//---------------------------------------------------------
CCompleteLinkage::~CCompleteLinkage(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// The only thing left to do is to realize your ideas whithin
// the On_Execute() function (which will be called automatically
// by the framework). But that's really your job :-)

template <class T>
std::string ToString ( const T& t ){
    std::ostringstream oss;
    oss << t;
    return oss.str();
}


bool CCompleteLinkage::On_Execute(void){

	// ----------------------------- declarations ---------------------
	CSG_Grid *pOrphantsGrid;
	long minPixelCnt, ndvCnt, nPi;
	double genFactor;

	double genFactorRegion = 0.8; // ratio of overall steps performed only in subgrid without calculating grids minimum
	long i,nAll, ireg, nRegion; //aggStep, maximum Step for Region and overall
	long nRegionStd, nRegionNX, nRegionNY, nRegionNXY; // number of steps for edge regions
	int minx, miny;	// pixel to be aggretate in one step
	double minDist; // minimum pixels distance
	distMapT classDistMap;	// mapping class -> ptr(map class -> distance)
	RegionSetT affectedRegionSet; // set of region pixels, thats minimum value has been changed
	pixelSetT neighbourPixels;	// set of pixels pointing to changed class

	DebugStream dout;

	//------------------------------- user input ----------------------
	pInputGrid[0]	= Parameters("InputGrid1")->asGrid();
	pInputGrid[1]	= Parameters("InputGrid2")->asGrid();
	pInputGrid[2]	= Parameters("InputGrid3")->asGrid();
	pInputGrid[3]	= Parameters("InputGrid4")->asGrid();
	pInputGrid[4]	= Parameters("InputGrid5")->asGrid();
	inputWeight[0]	= 1;
	inputWeight[1]	= Parameters("InputWeight2")->asDouble();
	inputWeight[2]	= Parameters("InputWeight3")->asDouble();
	inputWeight[3]	= Parameters("InputWeight4")->asDouble();
	inputWeight[4]	= Parameters("InputWeight5")->asDouble();

	
	pOutputGridAdd[0]	= Parameters("ClassesGrid1")->asGrid();
	pOutputGridAdd[1]	= Parameters("ClassesGrid2")->asGrid();
	pOutputGridAdd[2]	= Parameters("ClassesGrid3")->asGrid();
	pOutputGridAdd[3]	= Parameters("ClassesGrid4")->asGrid();
	pOutputGridAdd[4]	= Parameters("ClassesGrid5")->asGrid();

	genFactor	= Parameters("GenFactor")->asDouble();
	regionExtent= Parameters("RegionExtent")->asInt();
	minPixelCnt = Parameters("MaxOrphantsSize")->asInt() +1;

	pClassGrid  	= Parameters("ClassesGrid")->asGrid();
	pOrphantsGrid	= Parameters("OrphantsGrid")->asGrid();
	pMinDist		= Parameters("MinDistGrid")->asGrid();
	pMinDistDir		= Parameters("MinDistDirGrid")->asGrid();

	if( pMinDist == NULL ){
		pMinDist = SG_Create_Grid(SG_DATATYPE_Double, Get_NX(), Get_NY(), Get_Cellsize());
		pMinDist->Set_Name(_TL("MinDist"));
	}
	if( pMinDistDir == NULL ){
		pMinDistDir = SG_Create_Grid( SG_DATATYPE_Double, Get_NX(), Get_NY(), Get_Cellsize());
		pMinDistDir->Set_Name(_TL("MinDistDir"));
	}
 
	//-------------  Region Grids ---------------	
	{int rnx = ceil( Get_NX() / (double) regionExtent ); // without (double) its an integer division
	int rny = ceil( Get_NY() / (double) regionExtent );
	pRegionMinDistGrid = SG_Create_Grid(SG_DATATYPE_Double, rnx, rny, Get_Cellsize()*regionExtent);
	pRegionMinDistGrid->Set_Name(_TL("MinDist Region"));
	pRegionMinPixelGrid = SG_Create_Grid(SG_DATATYPE_Int, rnx, rny, Get_Cellsize()*regionExtent);
	pRegionMinPixelGrid->Set_Name(_TL("Regions MinPixel"));
	pRegionNdvCntGrid = SG_Create_Grid(SG_DATATYPE_Int, rnx, rny, Get_Cellsize()*regionExtent);
	pRegionNdvCntGrid->Set_Name(_TL("Regions Count of No-DataValue-Pixels"));
	}

	try{
		prof.resetAll();

		// ---------------- initalize 
		// and calculate pixels and grids minimal distance
		Message_Add(_TL("calculating initial distances\n"));
		prof.start("init");
		ndvCnt = initClassesGrid();
		prof.stop("init");

		// number of steps until output
		nPi = Get_NCells()-ndvCnt;
		nAll = (long) ( nPi * genFactor); // number of originial classes to remove
		nAll = min(nAll,(long)30000);
		outputStep[0] = Parameters("GenFactor1")->asDouble() * nPi;
		//pOutputGridAdd[0]->Set_Name( string("Classes ").assign( ToString( Parameters("GenFactor1")->asDouble())).c_str() );
		outputStep[1] = Parameters("GenFactor2")->asDouble() * nPi;
		//pOutputGridAdd[1]->Set_Name( string("Classes ").assign( ToString( Parameters("GenFactor2")->asDouble())).c_str() );
		outputStep[2] = Parameters("GenFactor3")->asDouble() * nPi;
		//pOutputGridAdd[2]->Set_Name( string("Classes ").assign( ToString( Parameters("GenFactor3")->asDouble())).c_str() );
		outputStep[3] = Parameters("GenFactor4")->asDouble() * nPi;
		//pOutputGridAdd[3]->Set_Name( string("Classes ").assign( ToString( Parameters("GenFactor4")->asDouble())).c_str() );
		outputStep[4] = Parameters("GenFactor5")->asDouble() * nPi;
		//pOutputGridAdd[4]->Set_Name( string("Classes ").assign( ToString( Parameters("GenFactor5")->asDouble())).c_str() );

		nRegionStd = (long) regionExtent*regionExtent * genFactor * genFactorRegion;
		//edge-Regions only can afford fewer runs
		nRegionNX = (long) (Get_NX() % regionExtent) * regionExtent * genFactor * genFactorRegion;
		nRegionNY = (long) regionExtent * (Get_NY() % regionExtent) * genFactor * genFactorRegion;
		nRegionNXY = (long) (Get_NX() % regionExtent) * (Get_NY() % regionExtent) * genFactor * genFactorRegion;

		// ------------------- region loops -------------------------
		Message_Add(_TL("aggregating inside regions\n"));
		i = 0;
		for( int regy = 0; regy < pRegionMinDistGrid->Get_NY() && Set_Progress(regy); regy++ ){
			for( int regx = 0; regx < pRegionMinDistGrid->Get_NX() ; regx++ ){
				nRegion = nRegionStd;
				if( regy == pRegionMinDistGrid->Get_NY()-1 ) 
					nRegion = nRegionNY;
				if( regx == pRegionMinDistGrid->Get_NX()-1 ){
					if( regy == pRegionMinDistGrid->Get_NY()-1 ) 
						nRegion = nRegionNXY;
					else
						nRegion = nRegionNX;
				}
				// decrease number of steps within region if there are no-data-values
				ndvCnt = pRegionNdvCntGrid->asDouble(regx,regy);
				nRegion -= (ndvCnt / genFactorRegion);
				for( ireg = 0; ireg < nRegion; ireg++ ){
					prof.start("regionLoop");
					calcRegionsMinPixel(regx,regy, /*out*/ minx, miny, minDist );
					if( minDist == DBL_MAX ){
						prof.stop("regionLoop");
						break;
					}
					if( minx == -1 )
						throw new runtime_error("could not determind minPixel");
					aggregatePixel(i++, minx,miny, classDistMap, affectedRegionSet);
					prof.stop("regionLoop");
				}
				int regpi = REGXY2REGPI(regx,regy);
				affectedRegionSet.insert( regpi );
			}
		}

		Message_Add(_TL("Recalculating all Regions Minimum Distances\n"));
		prof.start("recalcRegionMins_1");
		recalcRegionMins(affectedRegionSet);
		prof.stop("recalcRegionMins_1");


		// ----------------- grids loop --------------------------------
		// needs recalculation of several Regions 
		// and minimum loop over all regions
		Message_Add(_TL("aggregating over all the grid\n"));
		try{
			for( i = i; (i<nAll) && Set_Progress_NCells(i-Get_NCells()); i++ ){
				prof.start("mainLoop");

				prof.start("checkOutput");
				checkOutput(i);
				prof.stop("checkOutput");
				
				prof.start("calcMinDist");
				recalcRegionMins(affectedRegionSet);
				prof.stop("calcMinDist");

				prof.start("getGridsMinPixel");
				long minpi = getGridsMinPixel();
				minx = PI2X(minpi);
				miny = PI2Y(minpi);
				prof.stop("getGridsMinPixel");

				prof.start("aggregatePixel");
				aggregatePixel(i, minx,miny, classDistMap, affectedRegionSet);
				prof.stop("aggregatePixel");

				prof.stop("mainLoop");
			}
		}catch( runtime_error e){ 
			// no minpixel found
			dout << e.what(); 
		}

		// -------------- remove Orphants ---------------------
		//(classes with only few cells)
		if( minPixelCnt > 1){
			Message_Add(_TL("removing orphants\n"));
			prof.start("removeOrphantClasses");
			removeOrphantClasses(minPixelCnt, pOrphantsGrid, i, classDistMap, affectedRegionSet);
			prof.stop("removeOrphantClasses");
		}

		// ----------------- rename classes --------------------
		//prof.start("renameClasses");
		Message_Add(_TL("renaming classes\n"));
		renameClasses();
		//prof.stop("renameClasses");


	}catch( std::runtime_error e){ 
		string str = e.what();
	}

	// -------------------- cleanup --------------
	Message_Add(_TL("clean up temporary data structures\n"));
	prof.start("cleanup");
	{for( distMapT::iterator it = classDistMap.begin(); it != classDistMap.end(); ++it){
		delete (*it).second;
	}}
	classDistMap.clear();

	{for( classPixelSetMapT::iterator it = classPixelMap.begin(); it != classPixelMap.end(); ++it){
		delete (*it).second;
	}}
	classPixelMap.clear();

	//grids
	delete pRegionMinDistGrid;
	delete pRegionMinPixelGrid;
	delete pRegionNdvCntGrid; 
	
	if( Parameters("MinDistGrid")->asGrid() == NULL) 
		delete pMinDist;
	if( Parameters("MinDistDirGrid")->asGrid() == NULL) 
		delete pMinDistDir;

	//DataObject_Add(pRegionMinDistGrid, true);
	prof.stop("cleanup");


	//-----------------------------------------------------
#ifdef _DEBUG
	prof.output( dout );
#endif

	return( true );
}

// for given pixel it returns the 
// - the minimum distance to next distant cell (0), 
// - the direction to the next distant cell (1)
// - the class of the next distant cell (2)
// returns 0, if there are no other neighboring classes, direction then is undefined 
// do not forget to clear up the allocated double array by delete[] 
double *CCompleteLinkage::calcPixelsClassDistance(int x, int y, distMapT& cache, long remClass1, long remClass2)
{
	int xmin = (x > 0) ? x-1 : 0;
	int xmax = (x < (Get_NX()-1) ) ? x+1 : Get_NX()-1;
	int ymin = (y > 0) ? y-1 : 0;
	int ymax = (y < (Get_NY()-1) ) ? y+1 : Get_NY()-1;
	double dist, minDist = DBL_MAX, minClass;
	int dir; 
	long class1, class2;
	class1 = pClassGrid->asInt(x,y);
	if( !pClassGrid->is_NoData_Value(class1) ){
		for( int lx = xmin; lx <= xmax; lx++ ){
			for( int ly = ymin; ly <= ymax; ly++ ){
				class2 = pClassGrid->asInt(lx,ly);
				if( (class2 != class1) && !pClassGrid->is_NoData_Value(class2) ){
					//dist = minDist-1;
					try{
						dist = getClassesDist( class1, class2, cache, remClass1, remClass2 );
						if( dist < minDist ){
							minDist = dist;
							dir = (lx+1-x) +3*(ly+1-y);
							minClass = class2;
						}
					}catch( CNoDataValueError ){ 
						minDist = minDist;
						/* just do not account direction */ 
					}
				}// class1 != class2
			}//x
		}//y
	}// no data value
	if( minDist == DBL_MAX ){
		minDist = 0;
		dir = 4;
		minClass = class1;
	}
	double *ret = new double[3];
	ret[0] = minDist;
	ret[1] = dir;
	ret[2] = minClass;
	return ret;
}


// returns the distance of both classes
// uses and fills a cache (classDistMap), if cache fails it calls calcClassesDist
// see clearClassDistMap, for clearing cahce
// remClass1, remClass2 : Classes to remove from Distance-Cache
double CCompleteLinkage::getClassesDist( long class1, long class2, distMapT& cache, long remClass1, long remClass2 ){
	// same class has no distance 
	if(  class1 == class2 ) return 0;
	if( pClassGrid->is_NoData_Value(class1) || pClassGrid->is_NoData_Value(class2) ) 
		throw CNoDataValueError("getClassesDist with one class as NoData");
	// distances are symetric, so evt switch classes: class2 > class1
	if( class1 > class2 ){
		long tmp = class1;
		class1 = class2;
		class2 = tmp;
	}
	if( class2 < Get_NCells() ){
		prof.start("getClassesDist_singlePixel");
		// both classes are just single pixels
		double x1,y1,x2,y2;
		x1 = class1 % Get_NX(); y1 = class1 / Get_NX();
		x2 = class2 % Get_NX(); y2 = class2 / Get_NX();
		// NoDataValueError must be caught at higher level, and insert NoDataValue in ClassesGrid
		return calculateEucledianDistance(x1,y1,x2,y2);
		prof.stop("getClassesDist_singlePixel");
	}
	// cash of calculated class differences
	// mapping class2(long) -> class1(long) -> distance(double)
	double dist;
	distPtrT pDistMap;
	distMapT::iterator it = cache.find( class2 );
	distT::iterator it2;
	if( it == cache.end() ){
		pDistMap = new distT();
		cache.insert( distMapT::value_type( class2, pDistMap) );
	}else{
		pDistMap = (*it).second;
		// remove former classes from distance cache
		if( remClass1 >= Get_NCells() ){
			it2 = pDistMap->find( remClass1 );
			if( it2 != pDistMap->end() ) 
				pDistMap->erase( it2 );
		}
		if( remClass2 >= Get_NCells() ){
			it2 = pDistMap->find( remClass2 );
			if( it2 != pDistMap->end() ) 
				pDistMap->erase( it2 );
		}
	}
	it2 = pDistMap->find( class1 );
	if( it2 == pDistMap->end() ){
		prof.start("getClassesDist_calc");
		try{
			dist = calcClassesDist(class1, class2);
			pDistMap->insert( distT::value_type( class1, dist ) );
		}catch( CNoDataValueError ){
			// should not occure
			throw runtime_error("cauth noDataError of calcClassesDist of valid classes");
		}
		prof.stop("getClassesDist_calc");
	}else{
		prof.start("getClassesDist_cache");
		dist = (*it2).second;
		prof.stop("getClassesDist_cache");
	}
	return dist;
}

// calulates distance betwenn two classes
// this is the maximum the distances between each pixel of class1 
// related to each pixel of class2
double CCompleteLinkage::calcClassesDist(long class1, long class2){
	int x1,y1,x2,y2, pi;
	double dist, maxDist = -1;

	pixelSetPtrT pC1pix = getPixelSetItForClass(class1,1).first;
	pixelSetPtrT pC2pix = getPixelSetItForClass(class2,2).first;

	for( pixelSetT::iterator it1 = pC1pix->begin(); it1 != pC1pix->end(); ++it1 ){
		pi = (*it1);
		x1 = (*it1) % Get_NX();
		y1 = (*it1) / Get_NX();
		for( pixelSetT::iterator it2 = pC2pix->begin(); it2 != pC2pix->end(); ++it2 ){
			pi = (*it2);
			x2 = (*it2) % Get_NX();
			y2 = (*it2) / Get_NX();
			try{			
				dist = calculateEucledianDistance(x1,y1,x2,y2);
				if( dist > maxDist ) maxDist = dist;
			}catch( CNoDataValueError ){} // do not account for distances to missing pixels
		}
	}

	/*
	// outer loop: all pixels of class1
	for(y1=0; y1<Get_NY(); y1++)	{
		for(x1=0; x1<Get_NX(); x1++)		{
			if( pClassGrid->asInt(x1,y1) == class1 ){

				// inner loop: all pixels of class2
				for(y2=0; y2<Get_NY(); y2++)			{
					for(x2=0; x2<Get_NX(); x2++)				{
						if( pClassGrid->asInt(x2,y2) == class2 ){
							dist = calculateEucledianDistance(x1,y1,x2,y2);
							if( dist > maxDist ) maxDist = dist;
						}// if class2 inner loop
					}// x inner loop
				}// y inner loop

			}// if class1 outer loop
		}// x outer loop
	}//y outer loop
	*/

	return maxDist;
}


// returns the Distance in parameter-space for given two pixel
double CCompleteLinkage::calculateEucledianDistance(int x1, int y1, int x2, int y2){
	double c1,c2,dist, sumDist = 0;
	double w;
	for( int i = 0; i < MAX_INPUT_GRIDS; i++ ){
		if( pInputGrid[i] != NULL){
			w = inputWeight[i];
			c1 = pInputGrid[i]->asDouble(x1,y1);
			c2 = pInputGrid[i]->asDouble(x2,y2);
			if( pInputGrid[i]->is_NoData_Value(c1) ) throw CNoDataValueError("calculateEucledianDistance");
			if( pInputGrid[i]->is_NoData_Value(c2) ) throw CNoDataValueError("calculateEucledianDistance");
			dist =  c1 - c2;
			dist = w * dist * dist; //weight and square
			sumDist += dist;
		}
	}
	//sqare root not needed, because its monotone
	return sumDist; 
}

// copies on grid to another grid, targetGrid is allocated
CSG_Grid *CCompleteLinkage::copyGrid(CSG_Grid *fromGrid, TSG_Data_Type gridType){
	int x,y;
	CSG_Grid *targetGrid;
	targetGrid = SG_Create_Grid(gridType, Get_NX(), Get_NY(), Get_Cellsize());
	for(y=0; y<Get_NY();  y++){
		for(x=0; x<Get_NX(); x++){
			switch( gridType ) {
				case SG_DATATYPE_Byte:
					targetGrid->Set_Value(x,y, fromGrid->asByte(x,y) );
					break;
				case SG_DATATYPE_Char:
					targetGrid->Set_Value(x,y, fromGrid->asChar(x,y)  );
					break;
				case SG_DATATYPE_Short:
					targetGrid->Set_Value(x,y, fromGrid->asShort(x,y)  );
					break;
				case SG_DATATYPE_Int:
				case SG_DATATYPE_Word:
				case SG_DATATYPE_DWord:
					targetGrid->Set_Value(x,y, fromGrid->asInt(x,y)  );
					break;
				case SG_DATATYPE_Float:
					targetGrid->Set_Value(x,y, fromGrid->asFloat(x,y)  );
				/*
					*/
				default:
					targetGrid->Set_Value(x,y, fromGrid->asDouble(x,y) );
					break;
			}// switch
 		}//x
	}//y
	return targetGrid;
}


// returns the class of the pixel with minimal distance
// pMinDistDir must have been calculated
long CCompleteLinkage::getDestClass(int x, int y){
	int dir = (int) pMinDistDir->asDouble(x,y);
	if( dir == pMinDistDir->Get_NoData_Value() ) 
		return pClassGrid->Get_NoData_Value();
	int dx = DIRX(dir);
	int dy = DIRY(dir);
	return pClassGrid->asInt( x+DIRX(dir), y+DIRY(dir) );
}

/*
void CCompleteLinkage::removeOrphantClasses(long minClassCnt, CSG_Grid *pOrphantsGrid){
	// calulate number of pixels per class and minimum distance
	classPixelSetMapT classPixelMap;	//class -> List of Pixel
	classPixelSetMapT::iterator it1;

	classPixelSetMapT::value_type pixelVec;
	classPixelSetMapT::value_type::iterator it2;

	long class1, class2, cnt, pi;
	double edist;
	int x,y;
	for(y=0; y<Get_NY() && Set_Progress(y); y++){
		for(x=0; x<Get_NX(); x++){
			class1 = pClassGrid->asDouble(x,y); 
			it1 = classPixelMap.find(class1);
			if( it1 == classPixelMap.end() ){
				pixelVec.clear();
				pixelVec.insert( pixelVec.end(), (y*Get_NX()+x) );
				//inserts copy, so we do not need to create new pointer
				classPixelMap.insert( classPixelSetMapT::value_type( class1, pixelVec ) );
			}else{
				//(*it1).second  is pixelVec
				(*it1).second.insert( (*it1).second.end(), (y*Get_NX()+x) );
			}
		}
	}

	// reclass orphant pixels and remember in output
	double avgDist;
	double *dist_array;
	for( it1 = classPixelMap.begin(); it1 != classPixelMap.end(); ++it1 ){
		class1 = (*it1).first;
		cnt = (*it1).second.size();
		if( cnt < minClassCnt ){
			// calculate class distances to each neighboring class
			clearClassDistMap();
			for( it2 = (*it1).second.begin(); it2 != (*it1).second.end(); ++it2 ){
				pi = (*it2);
				//x = pi % Get_NX();
				//y = pi / Get_NX();
				dist_array = calcPixelsClassDistance(pi);
				// class distances anyway
			}

		}
	}
}
*/


void CCompleteLinkage::searchSetsMinPixel(pixelSetPtrT pPix, int &minx, int &miny){
	double edist, minDist = DBL_MAX;
	long pi;
	for( pixelSetT::iterator it = pPix->begin(); it != pPix->end(); it++ ){
		pi = (*it);
		edist = pMinDist->asDouble(pi);
		if( (edist > 0) && (edist < minDist)){
			minDist = edist;
			minx = PI2X(pi);
			miny = PI2Y(pi);
		}
	}
}


// remove orphant classes
// all classes, consisting of fewer than minClassCnt pixels are attached to 
// classes with minimal distance
// the distance of these pixels is put to pOrphantsGrid
// pClassGrid, pMinDist, pMinDistDir must have been calculated
void CCompleteLinkage::removeOrphantClasses(
		const long minClassCnt, CSG_Grid *pOrphantsGrid,
		long& i, 
		/* inout */ distMapT& classDistMap,	// mapping class -> ptr(map class -> distance)
		/* out */ RegionSetT& affectedRegionSet // set of region pixels, thats minimum value has been changed
		){
	long class1;
	double edist;
	pixelSetPtrT pPix, pPixN;
	vector<long> littleSets;
	//vector<pixelSetPtrT>::iterator itp;
	int minx, miny;
	classPixelSetMapT::iterator itclass;

	for( int y = 0; y < Get_NY(); y++ ){
		for( int x = 0; x < Get_NX(); x++ ){
			class1 = pClassGrid->asInt(x,y);
			if( !pClassGrid->is_NoData_Value(class1) && class1 < Get_NCells() ){
				edist = pMinDist->asDouble(x,y);
				aggregatePixel(i++, x, y, classDistMap, affectedRegionSet );
				if( pOrphantsGrid != NULL ) 
					pOrphantsGrid->Set_Value(x,y,edist);
			}else if(pOrphantsGrid != NULL ) {
				pOrphantsGrid->Set_NoData(x,y);
			}	
		}
	}

	if( minClassCnt > 1 ){
		//search all Sets and remember litte sets
		for( classPixelSetMapT::iterator it = classPixelMap.begin(); it != classPixelMap.end(); ++it ){
			pPix = (*it).second;
			if( pPix->size() < minClassCnt )
				littleSets.push_back( (*it).first );
		}
		// continue to merge pixelSets until merged Sets are large enough
		while( littleSets.size() > 0 ){
			class1 = littleSets[ littleSets.size()-1 ];
			littleSets.pop_back();
			itclass = classPixelMap.find(class1);
			// do search for class, because it could have been aggregated away in meantime
			if( itclass != classPixelMap.end() ){
				pPix = (*itclass).second;
				searchSetsMinPixel( pPix, minx, miny );
				if( pOrphantsGrid != NULL ){
					edist = pMinDist->asDouble(minx,miny);
					for( pixelSetT::iterator it = pPix->begin(); it != pPix->end(); it++ )
						pOrphantsGrid->Set_Value( (*it), edist );
				}
				pPixN = aggregatePixel(i++, minx,miny, classDistMap, affectedRegionSet ); 
				// if new Class has still to few pixels, remember its class
				if( pPixN->size() < minClassCnt )
					littleSets.push_back( Get_NCells()+i ); 
			}// class yet exists
		}// loop littleSets
	}// minClassCnt > 0

	/* XXXX unite smaller classes */
	/*
	// calulate number of pixels per class and minimum distance
	map<long,long> classCntMap;	//class -> cnt
	map<long,long>::iterator it1;
	map<long,long>::iterator it3;
	
	map<long,long> classMinPixelMap; // class -> pixel, that has minimal distance
	map<long,long>::iterator it2;

	map<long,long> classesToChange; // mapping of class->class to change
	map<long,long>::iterator it4;

	set<long> remainingOrphants; // classes that remain orphants after uniting with neighbor
	set<long> destClasses; // classes that are destination of aggregation
							// this list is needed because, pixels of target Classes must be recalculated too
	set<long>::iterator itS;
							
	
	vector<long> changedPixels; // pixels that have been united
	vector<long>::iterator itV;

	double *dist_dir;

	long class1, class2, cnt, pi;
	double edist;
	int x,y;
	
	// clear oprhants Grid
	//pOrphantsGrid = SG_Create_Grid(SG_DATATYPE_Double, Get_NX(), Get_NY(), Get_Cellsize()); input parameter
	if( pOrphantsGrid != NULL ){
		double ndv = pOrphantsGrid->Get_NoData_Value();
		for(y=0; y<Get_NY(); y++){
			for(x=0; x<Get_NX(); x++){
				pOrphantsGrid->Set_Value(x,y,ndv);
			}
		}
	}

	//unite classes, until size of united classes reaches minimum pixel size
	do{

		remainingOrphants.clear();
		classesToChange.clear();
		destClasses.clear();
		classCntMap.clear();
		classMinPixelMap.clear();

		// construct histogram of classes and minimum points
		for(y=0; y<Get_NY() && Set_Progress(y); y++){
			for(x=0; x<Get_NX(); x++){
				class1 = pClassGrid->asInt(x,y); 
				edist = pMinDist->asDouble(x,y);
				it1 = classCntMap.find(class1);
				if( it1 == classCntMap.end() ){
					classCntMap.insert( map<long,long>::value_type( class1, 1 ) );
				}else{
					(*it1).second++;
				}
				it2 = classMinPixelMap.find(class1);
				if( it2 == classMinPixelMap.end() ){
					classMinPixelMap.insert( map<long,long>::value_type( class1, (y*Get_NY()+x) ) );
				}else{
					if( (edist > 0) && (edist < pMinDist->asDouble((*it2).second) )) 
						(*it2).second = (y*Get_NY()+x);
				}
			}
		}


		// construct request of class mappings
		for( it1 = classCntMap.begin(); it1 != classCntMap.end(); ++it1 ){
			class1 = (*it1).first;
			cnt =  (*it1).second;
			if( cnt < minClassCnt ){
				it2 = classMinPixelMap.find( class1 );
				pi = (*it2).second;
				class2 = getDestClass(pi);
				if( class2 == class1 ){
					// need to recalculate minPixel
					class2 = class2;
				}
				// check, if destination class is going to change
				it4 = classesToChange.find( class2 );
				if( it4 != classesToChange.end() ){
					class2 = (*it4).second;
				}
				// do nothing on cyclic reference (dest class changed to this class)
				it3 = classCntMap.find( class2 );
				if( class1 != class2 ){ 
					// add new lass change request
					classesToChange.insert( map<long,long>::value_type( class1, class2 ));
					destClasses.insert( class2 );
					// increment PixelCount of destination class
					(*it3).second += cnt;
				} // class1 != class2
				// check if target classes pixel count is still smaller than mininum 
				// occurs only on cyclic references: class1 -> class2 -> class1
				if( (*it3).second < minClassCnt ){
					remainingOrphants.insert( remainingOrphants.end(), class2 );
				}
			}// cnt < minClassCnt
		}//Iterator over all classes

		// perform class change
		changedPixels.clear();
		for(y=0; y<Get_NY() && Set_Progress(y); y++){
			for(x=0; x<Get_NX(); x++){
				class1 = pClassGrid->asInt(x,y); 
				it1 = classesToChange.find( class1 );
				if( it1 != classesToChange.end() ){
					class2 = (*it1).second;
					pClassGrid->Set_Value(x,y, class2 );
					// get minimal distance of class before aggregation and put to outputGrid
					if( (pOrphantsGrid != NULL) && pOrphantsGrid->asDouble(x,y) == pOrphantsGrid->Get_NoData_Value() ){
						it1 = classMinPixelMap.find( class1); 
						pi = (*it1).second;
						edist = pMinDist->asDouble(pi);
						pOrphantsGrid->Set_Value(x,y, edist );
					}
					// rember pixels for recalculating distances
					changedPixels.push_back( (y*Get_NY()+x) );
				}// found in request
				itS = destClasses.find( class1 );
				if( itS != destClasses.end() )
					changedPixels.push_back( (y*Get_NY()+x) );
			}//for x
		}//for y

		//recalculate distances and direction of updated pixels
		distMapT classDistMap;
		for( itV = changedPixels.begin(); itV!=changedPixels.end(); ++itV ){
			pi = (*itV);
			x = pi % Get_NX();
			y = pi / Get_NX();
			dist_dir = calcPixelsClassDistance(x,y,classDistMap);
			pMinDist->Set_Value(x,y, dist_dir[0] );
			pMinDistDir->Set_Value(x,y, dist_dir[1] );
			delete[] dist_dir;
		}//itV

	}while( remainingOrphants.size()>0 );
	//XXX loop if remaining oprhants
	*/

}

// renames classes, so that classes are numbered from 0 to max-1
// classPixelMap is not longer valid afterwards
void CCompleteLinkage::renameClasses(){
	long nr = 0, pi;
	pixelSetPtrT pPixSet;
	for( classPixelSetMapT::iterator it = classPixelMap.begin(); it!=classPixelMap.end(); ++it ){
		pPixSet = (*it).second;
		// constant (*it).first = nr;
		for( pixelSetT::iterator itp = pPixSet->begin(); itp != pPixSet->end(); ++itp ){
			pi = (*itp);
			pClassGrid->Set_Value(pi,nr);
		}
		nr++;
	}
	/*
	long maxC = (long) pClassGrid->Get_ZMax(); // maximum class number
	long class1, class2, cnum;
	map<long,long> cmap;
	map<long,long>::iterator it;
	int x,y;

	// calculate new numbers above maximum
	cnum = 0;
	for(y=0; y<Get_NY() && Set_Progress(y); y++){
		for(x=0; x<Get_NX(); x++){
			class1 = pClassGrid->asInt(x,y); 
			it = cmap.find( class1 );
			if( it == cmap.end() ){
				cnum++;
				class2 = cnum + maxC;
				cmap.insert( map<long,long>::value_type( class1, class2 ) );
			}else{
				class2 = (*it).second;
			}
			pClassGrid->Set_Value(x,y, class2 );
		} // x
	} // y

	// substract maximum
	for(y=0; y<Get_NY() && Set_Progress(y); y++){
		for(x=0; x<Get_NX(); x++){
			pClassGrid->Set_Value(x,y, pClassGrid->asInt(x,y) - maxC );
		} // x
	} // y
	*/
}


// initialized ClassesGrid and calculates first Distances and Directions
// returns pixel, that has minimum distance
long CCompleteLinkage::initClassesGrid(){
	prof.start("initClassesGrid");
	int x,y;
	double minDist, dir, edist;
	long ndvCnt = 0;

	typedef double double3[3];
	double3 *nextRowDist; // [ Get_NX() ][3] distances of next row to directions 0,1,2 of next rows pixel
	double prevDist; // previous pixels distance to direction 5
	double nextRowDist0; // temporary store for value nextRowDist[x+1][0]

	// no new for dynamic 2dimensional arrays, Get_NX() not constant
	nextRowDist = new double3[Get_NX()];
	// first row
	y = 0;{
		x = 0;{
			dir = 5;
			try{
				minDist = calculateEucledianDistance( x,y, x+1, y );
				prevDist = minDist;
			}catch( CNoDataValueError ){prevDist = DBL_MAX;}
			try{
				edist = calculateEucledianDistance( x,y, x, y+1 );
				nextRowDist[x][1] =  edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 7;
				}
			}catch( CNoDataValueError ){nextRowDist[x][1] =  DBL_MAX;}
			try{
				edist = calculateEucledianDistance( x,y, x+1, y+1 );
				nextRowDist[x+1][0] =  edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 8;
				}
			}catch( CNoDataValueError ){ nextRowDist[x+1][0] = DBL_MAX; }
			setInitDist(x,y,minDist,dir, ndvCnt);
		} // x = 0
		for(x=1; x<Get_NX()-1; x++){
			dir = 3;
			minDist = prevDist; //calculateEucledianDistance( x,y, x-1, y );
			try{
				edist = calculateEucledianDistance( x,y, x+1, y );
				prevDist = edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 5;
				}
			}catch( CNoDataValueError ){ prevDist = DBL_MAX; }
			try{
				edist = calculateEucledianDistance( x,y, x-1, y+1 );
				nextRowDist[x-1][2] =  edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 6;
				}
			}catch( CNoDataValueError ){ nextRowDist[x-1][2] = DBL_MAX; }
			try{
				edist = calculateEucledianDistance( x,y, x, y+1 );
				nextRowDist[x][1] =  edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 7;
				}
			}catch( CNoDataValueError ){ nextRowDist[x][1] = DBL_MAX; }
			try{
				edist = calculateEucledianDistance( x,y, x+1, y+1 );
				nextRowDist[x+1][0] =  edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 8;
				}
			}catch( CNoDataValueError ){ nextRowDist[x+1][0] = DBL_MAX; }
			setInitDist(x,y,minDist,dir, ndvCnt);
		} // x [1..n-1]
		x = Get_NX()-1; {
			dir = 3;
			minDist = prevDist; //calculateEucledianDistance( x,y, x-1, y );
			try{
				edist = calculateEucledianDistance( x,y, x-1, y+1 );
				nextRowDist[x-1][2] =  edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 6;
				}
			}catch( CNoDataValueError ){ nextRowDist[x-1][2] = DBL_MAX; }
			try{
				edist = calculateEucledianDistance( x,y, x, y+1 );
				nextRowDist[x][1] =  edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 7;
				}
			}catch( CNoDataValueError ){ nextRowDist[x][1] = DBL_MAX; }
			setInitDist(x,y,minDist,dir, ndvCnt);
		} // x = Get_NX)() -1
	}// y = 0

	//------------------ center rows ------------------
	for(y=1; y<Get_NY()-1 && Set_Progress(y); y++)	{
		x = 0;{
			dir = 1;
			minDist = nextRowDist[x][1];
			edist = nextRowDist[x][2];
			if( edist < minDist ){
				dir = 2;
				minDist = edist;
			}
			try{
				edist = calculateEucledianDistance( x,y, x+1, y );
				prevDist = edist;
				if( edist < minDist ){
					dir = 5;
					minDist = edist;
				}
			}catch( CNoDataValueError ){ prevDist = DBL_MAX; }
			try{
				edist = calculateEucledianDistance( x,y, x, y+1 );
				nextRowDist[x][1] =  edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 7;
				}
			}catch( CNoDataValueError ){nextRowDist[x][1] = DBL_MAX; }
			try{
				edist = calculateEucledianDistance( x,y, x+1, y+1 );
				nextRowDist0 =  edist; //nextRowDist[x+1][0];
				if( edist < minDist ){
					minDist = edist;
					dir = 8;
				}
			}catch( CNoDataValueError ){ nextRowDist0 = DBL_MAX; }
			setInitDist(x,y,minDist,dir, ndvCnt);
		}//x = 0
		for(x=1; x<Get_NX()-1; x++)		{
			dir = 0;
			minDist = nextRowDist[x][0]; nextRowDist[x][0] = nextRowDist0;
			edist = nextRowDist[x][1];
			if( edist < minDist ){
				dir = 1;
				minDist = edist;
			}
			edist = nextRowDist[x][2];
			if( edist < minDist ){
				dir = 2;
				minDist = edist;
			}
			edist = prevDist;
			if( edist < minDist ){
				dir = 3;
				minDist = edist;
			}
			try{
				edist = calculateEucledianDistance( x,y, x+1, y );
				prevDist = edist;
				if( edist < minDist ){
					dir = 5;
					minDist = edist;
				}
			}catch( CNoDataValueError ){ prevDist = DBL_MAX; }
			try{
				edist = calculateEucledianDistance( x,y, x-1, y+1 );
				nextRowDist[x-1][2] = edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 6;
				}
			}catch( CNoDataValueError ){ nextRowDist[x-1][2] = DBL_MAX; }
			try{
				edist = calculateEucledianDistance( x,y, x, y+1 );
				nextRowDist[x][1] = edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 7;
				}
			}catch( CNoDataValueError ){ nextRowDist[x][1] = DBL_MAX; }
			try{
				edist = calculateEucledianDistance( x,y, x+1, y+1 );
				//nextRowDist[x+1][0]; do not overwrite before use
				nextRowDist0 = edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 8;
				}
			}catch( CNoDataValueError ){ nextRowDist0 = DBL_MAX; }
			setInitDist(x,y,minDist,dir, ndvCnt);
		}//x [1..n-1]
		x = Get_NX()-1; {
			dir = 0;
			minDist = nextRowDist[x][0]; 
			edist = nextRowDist[x][1];
			if( edist < minDist ){
				dir = 1;
				minDist = edist;
			}
			edist = prevDist;
			if( edist < minDist ){
				dir = 3;
				minDist = edist;
			}
			try{
				edist = calculateEucledianDistance( x,y, x-1, y+1 );
				nextRowDist[x-1][2] =  edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 6;
				}
			}catch( CNoDataValueError ){ nextRowDist[x-1][2] = DBL_MAX; }
			try{
				edist = calculateEucledianDistance( x,y, x, y+1 );
				nextRowDist[x][1] =  edist;
				if( edist < minDist ){
					minDist = edist;
					dir = 7;
				}
			}catch( CNoDataValueError ){ nextRowDist[x][1] = DBL_MAX; }
			setInitDist(x,y,minDist,dir, ndvCnt);
		} // x = Get_NX)() -1
	}// y [1..n-1]

	//---------------- last row ----------------------------
	y = Get_NY() -1;{
		x = 0;{
			dir = 1;
			minDist = nextRowDist[x][1];
			edist = nextRowDist[x][2];
			if( edist < minDist ){
				dir = 2;
				minDist = edist;
			}
			try{
				edist = calculateEucledianDistance( x,y, x+1, y );
				prevDist = edist;
				if( edist < minDist ){
					dir = 5;
					minDist = edist;
				}
			}catch( CNoDataValueError ){ prevDist = DBL_MAX; }
			setInitDist(x,y,minDist,dir, ndvCnt);
		}//x = 0
		for(x=1; x<Get_NX()-1; x++)		{
			dir = 0;
			minDist = nextRowDist[x][0]; 
			edist = nextRowDist[x][1];
			if( edist < minDist ){
				dir = 1;
				minDist = edist;
			}
			edist = nextRowDist[x][2];
			if( edist < minDist ){
				dir = 2;
				minDist = edist;
			}
			edist = prevDist;
			if( edist < minDist ){
				dir = 3;
				minDist = edist;
			}
			try{
				edist = calculateEucledianDistance( x,y, x+1, y );
				prevDist = edist;
				if( edist < minDist ){
					dir = 5;
					minDist = edist;
				}
			}catch( CNoDataValueError ){ prevDist = DBL_MAX; }
			setInitDist(x,y,minDist,dir, ndvCnt);
		}//x [1..n-1]
		x = Get_NX()-1; {
			dir = 0;
			minDist = nextRowDist[x][0]; 
			edist = nextRowDist[x][1];
			if( edist < minDist ){
				dir = 1;
				minDist = edist;
			}
			edist = prevDist;
			if( edist < minDist ){
				dir = 3;
				minDist = edist;
			}
			setInitDist(x,y,minDist,dir, ndvCnt);
		} // x = Get_NX)() -1
	}
	delete[] nextRowDist;

	prof.stop("initClassesGrid");
 	return ndvCnt;
}

void CCompleteLinkage::setInitDist(int x, int y, double minDist, int dir, long& ndvCnt){
	long pi = XY2PI(x,y);
	if( minDist == DBL_MAX ){
		// could not calculate Class-Distance to any direction, may be itself No_Data in one of the grids
		pClassGrid->Set_NoData(x,y);
		pMinDist->Set_NoData(x,y);
		pMinDistDir->Set_NoData(x,y);
		ndvCnt++;
		incrementRegionsNdvCnt(x,y);
	}else{
		pClassGrid->Set_Value(x,y,pi);
		pMinDist->Set_Value(x,y,minDist);
		pMinDistDir->Set_Value(x,y,dir);
	}
}

// returns the pixelSet for given class
// and the Iterator to class in classPixelMap or end() if constructed from single pixel 
pair<pixelSetPtrT,classPixelSetMapT::iterator> CCompleteLinkage::getPixelSetItForClass(long cl, BYTE tmpStorageNumber){
	classPixelSetMapT::iterator it;
	pixelSetPtrT pPixSet;
	if( cl < Get_NCells() ){
		it = classPixelMap.end();
		if( !(tmpStorageNumber <= MAX_SINGLE_PIXEL_ARR) || (tmpStorageNumber==0)) 
			throw runtime_error( ToString(tmpStorageNumber).append(" to large tmpStorageNumber") );
		pPixSet = &(singlePixelSetArr[tmpStorageNumber-1]);
		//pixelSetT::iterator itp = pPixSet->begin();
        //(*itp) = cl;
		(*pPixSet->begin()) = cl;
	}else{
		it = classPixelMap.find(cl);
		if( it != classPixelMap.end() ){
			pPixSet = (*it).second;
		}else{
			string s = "no pixel found for class ";
			s.append( ToString(cl) );
			throw runtime_error(s);
		}
	}
	return pair<pixelSetPtrT,classPixelSetMapT::iterator>( pPixSet, it );
}


// recalculates Minimum of all regions, found in set affectedRegionSet
// stores Minimum in 
void CCompleteLinkage::recalcRegionMins(RegionSetT& affectedRegionSet){
	for( RegionSetT::iterator it = affectedRegionSet.begin(); it != affectedRegionSet.end(); /* empty increment within loop */ ){
		long regpi = (*it);
		int regx = REGPI2REGX(regpi);
		int regy = REGPI2REGY(regpi);
		int minx, miny;
		double minDist;
		long minPixel;
		calcRegionsMinPixel(regx,regy,minx,miny,minDist);
		if( minDist == DBL_MAX ){ 
			minDist = pRegionMinDistGrid->Get_NoData_Value(); // whole region without a valid distance
			minPixel = pRegionMinPixelGrid->Get_NoData_Value();
			if( regy > pRegionMinDistGrid->Get_NY() )
				throw runtime_error( "to large index for grid pRegionMinDistGrid" );
		}else{
			minPixel = XY2PI(minx,miny);
		}
		pRegionMinDistGrid->Set_Value(regx,regy,minDist);
		pRegionMinPixelGrid->Set_Value(regx,regy,minPixel);
		// erase and increment iterator -> see http://groups.google.de/groups?hl=de&lr=&threadm=bxCa7.1518%24h94.176381%40paloalto-snr1.gtei.net&rnum=1&prev=/groups%3Fq%3Dstl%2Biterator%2Berase%26hl%3Dde%26lr%3D%26selm%3DbxCa7.1518%2524h94.176381%2540paloalto-snr1.gtei.net%26rnum%3D1
		affectedRegionSet.erase(it++);
	}//changedRegionSet_it
}

long CCompleteLinkage::getGridsMinPixel(){
	double gridMinDist = DBL_MAX, regMinDist;
	int regMinX = -1, regMinY = -1;
	for( int regx = 0; regx < pRegionMinDistGrid->Get_NX(); regx++ ){
		for( int regy = 0; regy < pRegionMinDistGrid->Get_NY(); regy++ ){
			regMinDist = pRegionMinDistGrid->asDouble(regx, regy);
			if( (regMinDist > 0) && (regMinDist < gridMinDist) ){
				gridMinDist = regMinDist;
				regMinX = regx;
				regMinY = regy;
			}
		}
	}
	if( gridMinDist == DBL_MAX ) 
		throw runtime_error("no min-pixel found");
	return getRegionsMinPixel( regMinX, regMinY );
}

long CCompleteLinkage::getRegionsMinPixel(int regx, int regy){
	return pRegionMinPixelGrid->asInt(regx, regy);
}


void CCompleteLinkage::searchNeighbourPixels( const long nClass, RegionSetT& affectedRegionSet, pixelSetT& neighbourPixels ){
		//neighbourPixels.reserve( neighbourPixels.size() + 2*cNpix.size() ); // allocate memory in range of pixels of referenced class
		neighbourPixels.reserve( neighbourPixels.size() + 64 );
		{for( RegionSetT::iterator it = affectedRegionSet.begin(); it != affectedRegionSet.end(); ++it ){
			long regpi = (*it);
			int regx = REGPI2REGX(regpi);
			int regy = REGPI2REGY(regpi);
			int mx = min( Get_NX(), (regx+1)*regionExtent );
			int my = min( Get_NY(), (regy+1)*regionExtent );
			long fclass;
			for( int y = (regy*regionExtent); y < my; y++ ){
				for( int x = (regx*regionExtent); x < mx; x++ ){
					// get pixels, that point to one of these classes
					fclass = getDestClass(x,y);
					if(fclass == nClass){ 
						long pi = XY2PI(x,y);
						neighbourPixels.push_back( pi );
					}
				}//x
			}//y
		}}
}


/* returns the pixel-Sets for given classes
pCNpix may be the same as pC1Pix of pC2pix
the Iterators point to the entry of pC1pix in given classPixelMap
or they point to end(), if the pixelSets are temporary single pixels
*/
void CCompleteLinkage::getPixelSets(
			classPixelSetMapT& classPixelMap, 
			const long class1, const long class2, 
			pixelSetPtrT& pC1pix, classPixelSetMapT::iterator& itc1pix, 
			pixelSetPtrT& pC2pix, classPixelSetMapT::iterator& itc2pix, 
			pixelSetPtrT& pCNpix
			){
	pair<pixelSetPtrT,classPixelSetMapT::iterator> res;
	res = getPixelSetItForClass( class1, 1 );
	pC1pix = res.first; // pixel set
	itc1pix = res.second; // iterator to entry in pixelMap
	/*
	if( class1 >= Get_NCells()) classPixelMap.erase( itc1pix ); // iterator
	if( class2 >= Get_NCells()) classPixelMap.erase( itc2pix ); // iterator
	*/
	res = getPixelSetItForClass( class2, 2 );
	pC2pix = res.first;
	itc2pix = res.second;

	// classes are symetric, so unite smaller to bigger class, 
	// assign new class the number of the larger class
	// create new class if former class consists of just single pixel
	// let pCNpix point to larger vector or create new one
	if( pC1pix->size() >= pC2pix->size() ){
		if( class1 < Get_NCells() ){
			pCNpix = new pixelSetT;
		}else{
			pCNpix = pC1pix;
		}
	}else{
		if( class2 < Get_NCells() ){
			pCNpix = new pixelSetT;
		}else{
			pCNpix = pC2pix;
		}
	}
}

// update map class->pixelSet, append smaller set 2 to larger set
// copy pixels from smaller vector to new Vector, delete other ones
// but other vectors are not erased from classPixelMap
void CCompleteLinkage::updatePixelSet(const long class1, const long class2, pixelSetPtrT pC1pix, pixelSetPtrT pC2pix, pixelSetPtrT pCNpix){
	long pi;
	if( pCNpix != pC1pix ){
		pCNpix->reserve( pCNpix->size() + pC1pix->size() ); // allocate enough memory
		for( pixelSetT::iterator itn1 = pC1pix->begin(); itn1 != pC1pix->end(); ++itn1){
			pi = (*itn1);
			pCNpix->push_back( pi ); // used in conjunction with reserve

		}
		if( class1 >= Get_NCells()){
			delete pC1pix; // original Classes Pixel Vector are not constructed
		}
	}
	if( pCNpix != pC2pix ){
		pCNpix->reserve( pCNpix->size() + pC2pix->size() );
		for( pixelSetT::iterator itn = pC2pix->begin(); itn != pC2pix->end(); ++itn){
			pi = (*itn);
			pCNpix->push_back( pi );
		}
		if( class2 >= Get_NCells()){
			delete pC2pix;
		}
	}
}

void CCompleteLinkage::aggregateClasses(const long nClass, pixelSetPtrT pCNpix, RegionSetT &affectedRegionSet){
	//determine Regions, in which to search f, or pixels to change
	//look at environment
	long pi, regpi;
	int x,y,regx,regy,modx,mody;
	for( pixelSetT::iterator it = pCNpix->begin(); it != pCNpix->end(); ++it ){
		pi = (*it);
		x = PI2X(pi);
		y = PI2Y(pi);
		regx = X2REGX(x);
		regy = Y2REGY(y);
		// do change class of given pixel
		pClassGrid->Set_Value(x,y, nClass );
		regpi = REGXY2REGPI( regx, regy );
		affectedRegionSet.insert( regpi );
		modx = x % regionExtent;
		mody = y % regionExtent;
		if((modx == 0) && (x>0)){ // left edge of region
			regpi = REGXY2REGPI( regx-1, regy );
			affectedRegionSet.insert( regpi );
			if((mody == 0) && (y>0)){ // top edge of region left corner
				regpi = REGXY2REGPI( regx-1, regy-1 );
				affectedRegionSet.insert( regpi );
			}else if((mody == regionExtent-1) && (y < Get_NY()-1)){ // bottom edge of region left corner
				regpi = REGXY2REGPI( regx-1, regy+1 );
				affectedRegionSet.insert( regpi );
			}
		}else if((modx == regionExtent-1) && (x < Get_NX()-1)){ // right edge of region
			regpi = REGXY2REGPI( regx+1, regy );
			affectedRegionSet.insert( regpi );
			if((mody == 0) && (y>0)){
				regpi = REGXY2REGPI( regx+1, regy-1 ); // top edge of region right corner
				affectedRegionSet.insert( regpi );
			}else if((mody == regionExtent-1) && (y < Get_NY()-1)){ // bottom edge of region right corner
				regpi = REGXY2REGPI( regx+1, regy+1 );
				affectedRegionSet.insert( regpi );
			}
		}
		if((mody == 0) && (y>0)){ // top edge of region
			regpi = REGXY2REGPI( regx, regy-1 );
			affectedRegionSet.insert( regpi );
		}else if((mody == regionExtent-1) && (y < Get_NY()-1)){ // bottom edge of region
			regpi = REGXY2REGPI( regx, regy+1 );
			affectedRegionSet.insert( regpi );
		}
	}
}

void CCompleteLinkage::recalcClassDistances(pixelSetPtrT pCNpix, pixelSetT &neighbourPixels, const long remClass1, const long remClass2, distMapT &classDistMap){
	// delete old classes from first mapping
	distMapT::iterator it;
	it = classDistMap.find( remClass1 );
	if( it != classDistMap.end() ){
		classDistMap.erase( it );
	}
	it = classDistMap.find( remClass2 );
	if( it != classDistMap.end() ){
		classDistMap.erase( it );
	}

	// recalculate minimal distances of cells,
	// that originate or point to classes of aggregated cells (stored in neighbourPixels)
	// clearClassDistMap();
	// pixel of new Class
	{for( pixelSetT::iterator it = pCNpix->begin(); it != pCNpix->end(); ++it ){
		long pi = (*it);
		int x = PI2X(pi);
		int y = PI2Y(pi);
		double *dist_dir = calcPixelsClassDistance(x,y, classDistMap, remClass1, remClass2);
		pMinDist->Set_Value(x,y, dist_dir[0] );
		pMinDistDir->Set_Value(x,y, dist_dir[1] );
		delete[] dist_dir;
	}}
	// all pixel that point to pixels of new class
	{for( vector<long>::iterator it = neighbourPixels.begin(); it != neighbourPixels.end(); ++it ){
		long pi = (*it);
		int x = PI2X(pi);
		int y = PI2Y(pi);
		double *dist_dir = calcPixelsClassDistance(x,y, classDistMap, remClass1, remClass2);
		pMinDist->Set_Value(x,y, dist_dir[0] );
		pMinDistDir->Set_Value(x,y, dist_dir[1] );
		delete[] dist_dir;
	}}
}

void CCompleteLinkage::calcRegionsMinPixel(int regx, int regy, int &minx, int &miny, double &minDist){
	int mx = min( Get_NX(), (regx+1)*regionExtent );
	int my = min( Get_NY(), (regy+1)*regionExtent );
	minDist = DBL_MAX;
	minx = -1; miny = -1;
	for( int y = (regy*regionExtent); y < my; y++ ){
		for( int x = (regx*regionExtent); x < mx; x++ ){
			double edist = pMinDist->asDouble(x,y);
			if(  (edist > 0) && (edist < minDist) ){
				minDist = edist;
				minx = x;
				miny = y;
			}//minDist
		}//x
	}//y
}

// basic step of the algorithm, aggregates specified pixel to its 
// closest neighbor and recalculates all class distances and caches
pixelSetPtrT CCompleteLinkage::aggregatePixel(int i, int minx, int miny,
		/* inout */ distMapT& classDistMap,	// mapping class -> ptr(map class -> distance)
		/* out */ RegionSetT& affectedRegionSet // set of region pixels, thats minimum value has been changed
		){
	long nClass, class1, class2; // new and former class-numbers
	pixelSetT neighbourPixels;	// set of pixels pointing to changed class

	class1 = pClassGrid->asInt(minx,miny);
	class2 = getDestClass(minx,miny);
	nClass = Get_NCells() + i;

	// ------ get PixelSets --------
	//prof.start("searchPixelSets");
	pixelSetPtrT pC1pix, pC2pix; // vector of pixels of old classes 
	pixelSetPtrT pCNpix; // vector of pixels of new class
	classPixelSetMapT::iterator itc1pix, itc2pix;
	getPixelSets( classPixelMap, class1, class2, pC1pix, itc1pix, pC2pix, itc2pix, pCNpix );
	//prof.stop("searchPixelSets");

	// ----- copy pixels -----
	//into larger vector, delete other one and update classPixelMap
	//prof.start("updatePixelSets");
	updatePixelSet( class1, class2, pC1pix, pC2pix, pCNpix );
	// erase old pixelSets from classPixelMap
	if( itc1pix != classPixelMap.end() ) classPixelMap.erase( itc1pix );
	if( itc2pix != classPixelMap.end() ) classPixelMap.erase( itc2pix );
	// insert new pixel-Set
	pair<classPixelSetMapT::iterator, bool> ret =
		classPixelMap.insert( classPixelSetMapT::value_type( nClass, pCNpix ) );
	if( !ret.second )
		throw runtime_error( ToString(nClass).append(" Class: could not insert in classPixelMap") );
	//prof.stop("updatePixelSets");

	// ------ aggregate -----------
	// all pixels of two classes to new class
	// remember regions, that are affected
	prof.start("aggregatePixels");
	affectedRegionSet.clear(); // will be output from next method
	aggregateClasses( nClass, pCNpix, affectedRegionSet ); 
	prof.stop("aggregatePixels");

	//------- search neighbours: --------
	//iterate over all pixels in found regions, remember pixels, that point to new class for recalculating distances
	prof.start("searchNeighbours");
	searchNeighbourPixels( nClass, affectedRegionSet, neighbourPixels );
	prof.stop("searchNeighbours");

	// -------- recalculate class distances ----
	//, update class distance cache		
	prof.start("recalcDistanceAll");
	recalcClassDistances( pCNpix, neighbourPixels, class1, class2, classDistMap );
	prof.stop("recalcDistanceAll");

	return pCNpix;
}


void CCompleteLinkage::checkOutput(long step){
	for( int i = 0; i < MAX_OUTPUT_GRIDS; i++ ){
		if( (pOutputGridAdd[i] != NULL) && (step == outputStep[i])){
			pOutputGridAdd[i]->Assign( pClassGrid );
			CSG_Colors colors;
			DataObject_Get_Colors( pOutputGridAdd[i], colors );
			colors.Random();
			DataObject_Set_Colors( pOutputGridAdd[i], colors );
		}
	}
}


long CCompleteLinkage::incrementRegionsNdvCnt(int x, int y){
	int regx, regy;
	regx = X2REGX(x);
	regy = Y2REGY(y);
	long cnt = pRegionNdvCntGrid->asInt(regx,regy);
	pRegionNdvCntGrid->Set_Value(regx,regy, ++cnt );
	return cnt;
}



