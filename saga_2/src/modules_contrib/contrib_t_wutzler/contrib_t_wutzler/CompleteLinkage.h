
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                      $$modul$$                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      CompleteLinkage.h                       //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#if !defined(SAGA_CompleteLinkage_H__DD97C872_D413_11D4_B653_525400DF71C8__INCLUDED_)
#define SAGA_CompleteLinkage_H__DD97C872_D413_11D4_B653_525400DF71C8__INCLUDED_

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//avoid compile complaining for long names in stl
#pragma warning(disable:4786)

#include<map>
#include<vector>
#include<set>
#include<deque>
#include"Profiler.h"
#include<limits>
//const int DBL_MAX= numeric_limits<double>::max();

//#include <boost/shared_ptr.hpp>

using std::map;
using std::vector;
using std::set;
//using boost::shared_ptr;

#define PI2X(PI) ( (long)(PI) % Get_NX() )
#define PI2Y(PI) ( (long)(PI) / Get_NX() )	// ensure pi and Get_NX() provide integers
#define XY2PI(X,Y) ( (Y)*Get_NX()+(X) )

#define X2REGX(X) ( (int)(X)/(regionExtent) )	// ensure x and regionExtent are integers
#define Y2REGY(Y) ( (int)(Y)/(regionExtent) )	// ensure x and regionExtent are integers
#define REGPI2REGX(PI) ( (long)(PI) % (pRegionMinDistGrid->Get_NX()) ) 
#define REGPI2REGY(PI) ( (long)(PI) / (pRegionMinDistGrid->Get_NY()) )
#define REGXY2REGPI(X,Y) ( (Y)*(pRegionMinDistGrid->Get_NX())+(X) )

#define DIRX(DIR) ((DIR)%3-1)
#define DIRY(DIR) ((DIR)/3-1)	// DIR must be integer
#define XY2DIR(X,Y) ((X)+1+(3*((Y)+1)))

#define MAX_INPUT_GRIDS 5
#define MAX_OUTPUT_GRIDS 5


	// class -> set of pixels
	//typedef std::set<long> pixelSetT;
	//typedef std::deque<long> pixelSetT;
	typedef std::vector<long> pixelSetT; // performs better than set or deque using resize
	//typedef boost::shared_ptr<pixelSetT> pixelSetPtrT;
	typedef pixelSetT* pixelSetPtrT;
	typedef std::map<long,pixelSetPtrT> classPixelSetMapT;

	// class -> class -> distance
	typedef std::map<int,double> distT;
	//typedef boost::shared_ptr<distT> distPtrT;
	typedef distT* distPtrT;
	typedef std::map<int,distPtrT> distMapT;

	typedef std::set<long> RegionSetT;

	const BYTE MAX_SINGLE_PIXEL_ARR = 2;


class CCompleteLinkage : public CSG_Module_Grid
{
public:
	CCompleteLinkage(void);
	virtual ~CCompleteLinkage(void);


protected:
	double calcClassesDist(long class1, long class2);
	CSG_Grid * copyGrid(CSG_Grid *fromGrid, TSG_Data_Type gridType);

	virtual bool		On_Execute(void);



private:
	long incrementRegionsNdvCnt(int x,int y);
	//void removeSinglePixelOrphants( CSG_Grid *pClassGrid, CSG_Grid *pOrphantsGrid );
	void checkOutput(long i);
	void searchSetsMinPixel( pixelSetPtrT pPix, int& minx, int& miny );
	pixelSetPtrT aggregatePixel(
		int i, int x, int y,
		/* inout */ distMapT& classDistMap,	// mapping class -> ptr(map class -> distance)
		/* out */ RegionSetT& affectedRegionSet // set of region pixels, thats minimum value has been changed
		);
	void calcRegionsMinPixel(
		int regx,int regy, 
		int& minx, int& miny, double& minDist 
		);
	void recalcClassDistances( 
		pixelSetPtrT pCNpix, pixelSetT& neighbourPixels, 
		const long remClass1, const long remClass2, 
		/*out*/ distMapT& classDistMap 
		);
	void aggregateClasses( 
		const long nClass, pixelSetPtrT pCNpix, 
		/*out*/ RegionSetT& affectedRegionSet 
		);
	void updatePixelSet( 
		const long class1, const long class2, 
		/*p inout*/ pixelSetPtrT pC1pix, 
		/*p inout*/ pixelSetPtrT pC2pix, 
		/*p inout*/ pixelSetPtrT pCNpix 
		);
	void getPixelSets(
		classPixelSetMapT& classPixelMap, 
		const long class1, const long class2, 
			/*out*/ pixelSetPtrT& pC1pix, /*out*/ classPixelSetMapT::iterator& itc1pix, 
			/*out*/ pixelSetPtrT& pC2pix, /*out*/ classPixelSetMapT::iterator& itc2pix, 
			/*out*/ pixelSetPtrT& pCNpix
		);

	/*
	searches for pixels, that point to nClass, only in Regions of affectedRegionSet
	pixels are inserted into PixelSet nClassPixel
	*/
	void searchNeighbourPixels( 
		const long nClass, 
		/*const*/ RegionSetT& affectedRegionSet, 
		pixelSetT& neighbourPixels 
		);

	long getRegionsMinPixel(int regx, int regy);
	long getGridsMinPixel();
	void recalcRegionMins(RegionSetT& affectedRegionSet);

	void setInitDist( int x, int y, double minDist, int dir, long& ndvCnt );
	pair<pixelSetPtrT,classPixelSetMapT::iterator> getPixelSetItForClass(long cl, BYTE tmpStorageNumber);
	long initClassesGrid();
	void renameClasses();
	void removeOrphantClasses(
		const long minClassCnt, CSG_Grid *pOrphantsGrid,
		long& i, 
		/* inout */ distMapT& classDistMap,	// mapping class -> ptr(map class -> distance)
		/* out */ RegionSetT& affectedRegionSet // set of region pixels, thats minimum value has been changed
		);
	long getDestClass(int x, int y);
	long getDestClass(long pi){ return getDestClass( PI2X(pi), PI2Y(pi) ); }

	double calculateEucledianDistance( int x1,int y1,int x2,int y2 );

	double *calcPixelsClassDistance( int x, int y, distMapT& cache, long remClass1, long remClass2 );
	double *calcPixelsClassDistance( int x, int y, distMapT& cache ){ return calcPixelsClassDistance(x,y,cache,-1,-1); }
	double *calcPixelsClassDistance(long pi, distMapT& cache, long remClass1, long remClass2){ return calcPixelsClassDistance( PI2X(pi), PI2Y(pi), cache, remClass1, remClass2 ); }
	
	double getClassesDist( long class1, long class2, distMapT& cache, long remClass1, long remClass2 );

	// input / output grids and parameters
	CSG_Grid    *pInputGrid[MAX_INPUT_GRIDS];
	double  inputWeight[MAX_INPUT_GRIDS];	
	CSG_Grid    *pOutputGridAdd[MAX_OUTPUT_GRIDS];
	long	outputStep[MAX_OUTPUT_GRIDS];	

	int regionExtent;
	CSG_Grid *pClassGrid;
	
	// temporary grids accessible by all functions
	CSG_Grid	*pMinDist;		// grid of Distances to min distance neighbouring pixel
	CSG_Grid	*pMinDistDir;	// grid of direction to min diestance neighbouring cell
	CSG_Grid	*pRegionMinDistGrid;	// grid of minimum distances for a region
	CSG_Grid	*pRegionMinPixelGrid;	// grid of (original extent)pixels, having minimum distances for a region
	CSG_Grid	*pRegionNdvCntGrid;		// grid of counts of no data values for each region

	classPixelSetMapT classPixelMap; // mapping of class -> Ptr to set of pixels
	pixelSetT singlePixelSetArr[ MAX_SINGLE_PIXEL_ARR ]; // temporary storage for vectors consisting of single pixels

	CProfiler prof;
};

#endif // !defined(SAGA_CompleteLinkage_H__DD97C872_D413_11D4_B653_525400DF71C8__INCLUDED_)
