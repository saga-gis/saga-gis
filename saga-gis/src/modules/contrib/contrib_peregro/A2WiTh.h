/**********************************************************
 * Version $Id: A2WiTh.h 1514 2012-11-06 09:47:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                        Module:                        //
//                        A2WiTh                         //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       A2WiTh.h                        //
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
#ifndef HEADER_INCLUDED__A2WiTh_H
#define HEADER_INCLUDED__A2WiTh_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CA2WiTh : public CSG_Module_Grid
{
public: ////// public members and functions: //////////////

	CA2WiTh(void);							// constructor
	virtual ~CA2WiTh(void);				// destructor

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Averages") );	}


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute(void);		// always override this function


private: ///// private members and functions: /////////////


};


//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__A2WiTh_H
