/**********************************************************
 * Version $Id: A3WiTh.h 1514 2012-11-06 09:47:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                        Module:                        //
//                        A3WiTh                         //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       A3WiTh.h                        //
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
#ifndef HEADER_INCLUDED__A3WiTh_H
#define HEADER_INCLUDED__A3WiTh_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CA3WiTh : public CSG_Module_Grid
{
public: ////// public members and functions: //////////////

	CA3WiTh(void);							// constructor
	virtual ~CA3WiTh(void);				// destructor

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Averages") );	}


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute(void);		// always override this function


private: ///// private members and functions: /////////////


};


//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__A3WiTh_H
