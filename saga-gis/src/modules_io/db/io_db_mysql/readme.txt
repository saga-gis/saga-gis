//////////////////////////////////////////////////////////
//                                                      //
//    This code is something I did for SAGA GIS to      //
//    import data from MySQL Database.			//
//							//
//	  INPUT : Connection information and SQL 	//
// 		  statement	 			//
//	  OUTPUT: Result data as Table			//
//							//
//		RELEASE : v0.1b (Lot more to do)	//
//							//
//	  The SAGA structure code is grapped from 	//
//    	  Olaf Conrad's ODBC example and database	//
//	  communication part is replaced with MySQL	//
//	  native library (libmysql.dll for win32)	//
//							//
//	  The code is compiled under			//
//	      Visual C++ 2008 Express Edition 	        //
//------------------------------------------------------//
//                                                      //
//    e-mail:     sagamysql@ferhatbingol.com            //
//                                                      //
//    contact:    Ferhat Bingöl                         //
//                                                      //
//////////////////////////////////////////////////////////

TO INSTALL on Win32:
-----------------------------------------------------
1) Copy RELEASE\io_table_mysql.dll to your <SAGA>\Modules directory
2) Load module in SAGA
3) Action is located under FILE -> IMPORT -> TABLE

TO COMPILE on Win32:
-----------------------------------------------------
1) This module is compiled with Visual C++ Express Edition nad project file is included (ONLY RELEASE)
2) You have to change the library link & link directory for "libmysql.lib", "mysql.h" etc. You can get these files from http://www.mysql.com
3) Compiled and load the module
4) Action is located under FILE -> IMPORT -> TABLE


TO INSTALL on Linux:
-----------------------------------------------------
1) You have to compile yourself, I did not have the type to try with my distro. 
2) Do not forget the change the include statements for config windows header