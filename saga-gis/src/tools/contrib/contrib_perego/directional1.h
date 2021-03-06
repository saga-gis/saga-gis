/**********************************************************
 * Version $Id: directional1.h 1514 2012-11-06 09:47:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                        Tool:                        //
//                      directional1                     //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     directional1.h                    //
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
#ifndef HEADER_INCLUDED__directional1_H
#define HEADER_INCLUDED__directional1_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class Cdirectional1 : public CSG_Tool_Grid
{
public: ////// public members and functions: //////////////

	Cdirectional1(void);							// constructor
	virtual ~Cdirectional1(void);				// destructor

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Averages") );	}


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute(void);		// always override this function


private: ///// private members and functions: /////////////


};


//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__directional1_H
