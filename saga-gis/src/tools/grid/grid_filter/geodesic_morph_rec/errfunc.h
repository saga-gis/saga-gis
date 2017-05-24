/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//              Grid_Filter / Grid_Calculus              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       errfunc.h                       //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     HfT Stuttgart                     //
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
//    e-mail:     johannes.engels@hft-stuttgart.de       //
//                                                       //
//    contact:    Johannes Engels                        //
//                Hochschule fuer Technik Stuttgart      //
//                Schellingstr. 24                       //
//                70174 Stuttgart                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct fehlerinfo
{
   int fnr;
   char art;
   char *message;
}
fehlerinfo;

void fehler (int zeile,
             int fnr,
             char *filename,
             char *funk_name,
             FILE *protfile,
             fehlerinfo *finf,
             void *info1,
             void *info2,
             void *info3,
             void *info4,
             void *info5,
             void *info6,
             void *info7);


void error_message (int zeile,
                    int fnr,
                    char *filename,
                    char *funk_name,
                    char **error_list,
                    fehlerinfo *finf,
                    void *info1,
                    void *info2,
                    void *info3,
                    void *info4,
                    void *info5,
                    void *info6,
                    void *info7);
       
void fehler2 (int zeile,
              int fnr,
              char *filename,
              char *funk_name,
              FILE *protfile,
              fehlerinfo *finf,
              ...);

void fehler3 (int zeile,
              int fnr,
              char *filename,
              char *funk_name,
              char **error_list,
              fehlerinfo *finf,
              ...);


#define ERR_MESS_0(fnr)   error_message (__LINE__, (fnr), __FILE__, \
                                         funk_name, &error_liste,   \
                                         finfo, NULL, NULL, NULL,   \
                                         NULL, NULL, NULL,          \
                                         NULL)

#define ERR_MESS_1(fnr,y)   error_message (__LINE__, (fnr), __FILE__, \
                                           funk_name, &error_liste,   \
                                           finfo, (y), NULL, NULL,    \
                                           NULL, NULL, NULL,          \
                                           NULL)


#define ERRCHECK2_RC(fnr,finfo,x1,x2)                            \
                                                                 \
if (rc != 0)                                                     \
{                                                                \
   error_message (__LINE__,(fnr),__FILE__,funk_name,error_liste, \
                  finfo,x1,x2,NULL,NULL,NULL,NULL,NULL);         \
   return 8;                                                     \
}

#define ERRCHECK2_NULL(y,fnr,finfo,x1,x2)                        \
                                                                 \
if (y == NULL)                                                   \
{                                                                \
   error_message (__LINE__,(fnr),__FILE__,funk_name,error_liste, \
                  finfo,x1,x2,NULL,NULL,NULL,NULL,NULL);         \
}

#define ERRCHECK2_RC_R(fnr,finfo,x1,x2)                          \
                                                                 \
if (rc != 0)                                                     \
{                                                                \
   error_message (__LINE__,(fnr),__FILE__,funk_name,error_liste, \
                  finfo,x1,x2,NULL,NULL,NULL,NULL,NULL);         \
   return rc;                                                    \
}


#define ERRCHECK2_NULL_R(y,fnr,finfo,x1,x2)                      \
                                                                 \
if (y == NULL)                                                   \
{                                                                \
   error_message (__LINE__,(fnr),__FILE__,funk_name,error_liste, \
                  finfo,x1,x2,NULL,NULL,NULL,NULL,NULL);         \
   return;                                                       \
}


#if 0

#define ERRCHECK7_RC(fnr,finfo,x1,x2,x3,x4,x5,x6,x7)             \
                                                                 \
if (rc != 0)                                                     \
{                                                                \
   fehler2 (__LINE__,(fnr),__FILE__,funk_name,stdout,finfo,      \
            x1,x2,x3,x4,x5,x6,x7);                               \
}



#define ERRCHECK7_NULL(y,fnr,finfo,x1,x2,x3,x4,x5,x6,x7)         \
                                                                 \
if (y == NULL)                                                   \
{                                                                \
   fehler2 (__LINE__,(fnr),__FILE__,funk_name,stdout,finfo,      \
            x1,x2,x3,x4,x5,x6,x7);                               \
}


#define ERRCHECK7_RC_R(fnr,finfo,x1,x2,x3,x4,x5,x6,x7)           \
                                                                 \
if (rc != 0)                                                     \
{                                                                \
   fehler2 (__LINE__,(fnr),__FILE__,funk_name,stdout,finfo,      \
            x1,x2,x3,x4,x5,x6,x7);                               \
   return rc;                                                    \
}


#define ERRCHECK7_NULL_R(y,fnr,finfo,x1,x2,x3,x4,x5,x6,x7)       \
                                                                 \
if (y == NULL)                                                   \
{                                                                \
   fehler2 (__LINE__,(fnr),__FILE__,funk_name,stdout,finfo,      \
            x1,x2,x3,x4,x5,x6,x7);                               \
   return;                                                       \
}


#endif



