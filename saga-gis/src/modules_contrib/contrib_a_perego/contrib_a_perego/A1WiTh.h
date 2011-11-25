/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                        Module:                        //
//                        A1WiTh                         //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       A1WiTh.h                        //
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
#ifndef HEADER_INCLUDED__A1WiTh_H
#define HEADER_INCLUDED__A1WiTh_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CA1WiTh : public CSG_Module_Grid
{
public: ////// public members and functions: //////////////

	CA1WiTh(void);							// constructor
	virtual ~CA1WiTh(void);				// destructor

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("R:Filter for Grids") );	}


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute(void);		// always override this function


private: ///// private members and functions: /////////////


};


//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__A1WiTh_H
