/**********************************************************
 * Version $Id: destriping2.h 1514 2012-11-06 09:47:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                        Module:                        //
//                      destriping                       //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     destriping2.h                     //
//                                                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                                                       //
//    by Alessandro Perego (Italy)                       //
//                                                       //
//    e-mail:     alper78@alice.it                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
#ifndef HEADER_INCLUDED__destriping2_H
#define HEADER_INCLUDED__destriping2_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class Cdestriping2 : public CSG_Module_Grid
{
public: ////// public members and functions: //////////////

	Cdestriping2(void);					// constructor
	virtual ~Cdestriping2(void);				// destructor


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute(void);		// always override this function


private: ///// private members and functions: /////////////


};


//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__destriping2_H
