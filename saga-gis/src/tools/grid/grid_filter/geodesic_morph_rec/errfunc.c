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
//                       errfunc.c                       //
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "errfunc.h"






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
             void *info7)

/************************************************/
/*   Ausgabe von Laufzeitfehlern,               */
/*   ggf. Programmabbruch                       */
/************************************************/

{
   int j;
   int gefunden;
   int sl;

   char fmessage [255];
   char *cp1;
   char *cp2;
   char *cp3;
   char *cp4;
   fehlerinfo *fil;

   void *info [8];


   info [1] = info1;
   info [2] = info2;
   info [3] = info3;
   info [4] = info4;
   info [5] = info5;
   info [6] = info6;
   info [7] = info7;

   fprintf (protfile, "\n\n");

   gefunden = 0;

   for (fil = finf; fil != NULL && fil -> fnr != 0; fil ++)
   {
      if (fnr == fil -> fnr)
      {
         gefunden = 1;
         break;
      }
   }

   if (gefunden == 0)
   {
      fprintf (protfile, "+++ Unbekannter Fehler\n");
      fflush (protfile);
      fprintf (protfile, "    Abbruch des Programms.\n\n");
      exit (20);
   }

   if (fil -> art == 'W')
      fprintf (protfile, 
               "+++ Warnung in Funktion %s,\n"
               "               Datei    %s,\n"
               "               Zeile    %d:\n",
               funk_name,
               filename, 
               zeile);
   else if (fil -> art == 'F' || fil -> art == 'E')
      fprintf (protfile, 
               "+++ Fehler in Funktion %s,\n"
               "              Datei    %s,\n"
               "              Zeile    %d:\n",
               funk_name,
               filename, 
               zeile);

   for (cp1 = fil -> message, j = 1, cp4 = cp1;
        j <= 7 && info [j] != NULL; j ++)
   {
      cp2 = strchr (cp4, '%');
   
      if (cp2 == NULL)
         break;

      if (*(cp2 + 1) == '%')
      {
         cp4 = cp2 + 2;
         j --;
         continue;
      }


      gefunden = 0;

      cp3 = strpbrk (cp2 + 1, "csdlhfe");

      if (cp3 == NULL)
      {
         fprintf (protfile, "    Fehler in errfunc.c: ");
         fprintf (protfile, "unzulaessige Formatangabe\n");
         break;
      }

      if (*(cp3 + 1) != 0x00 && *(cp3 + 1) != '%')
         cp2 = cp3 + 2;
      else
         cp2 = cp3 + 1;

      sl = cp2 - cp1;

      memcpy (fmessage, cp1, sl);
      fmessage [sl] = 0x00;

      switch (*cp3)
      {
         case 'd':
            fprintf (protfile, fmessage, *((int *)(info [j])));
            break;
                
         case 'l':
            fprintf (protfile, fmessage, *((long *)(info [j])));
            break;

         case 'h':
            fprintf (protfile, fmessage, *((short *)(info [j])));
            break;

         case 'e':
         case 'f':
            fprintf (protfile, fmessage, *((double *)(info [j])));
            break;

         case 'c':
            fprintf (protfile, fmessage, *((char *)(info [j])));
            break;

         case 's':
            fprintf (protfile, fmessage, (char *)(info [j]));
            break;

         default:
            break;
      }

      cp1 = cp2;
      cp4 = cp2;
   }

   fprintf (protfile, "%s", cp1);

   if (fil -> art == 'E')
   {
      fprintf (protfile, "    Abbruch des Programms.\n\n");
      exit (20);
   }

   fflush (protfile);

   return;
}






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
                    void *info7)

/************************************************/
/*   Ausgabe von Laufzeitfehlern,               */
/*   ggf. Programmabbruch                       */
/************************************************/

{
   int j;
   int gefunden;
   int sl;
   int sl_head;
   int sl_all;
   int sl_alt;
   fehlerinfo *fil;

   char fmessage [512];
   char zw [255];
   char *cp1;
   char *cp2;
   char *cp3;
   char *cp4;

   void *info [8];


   info [1] = info1;
   info [2] = info2;
   info [3] = info3;
   info [4] = info4;
   info [5] = info5;
   info [6] = info6;
   info [7] = info7;

   gefunden = 0;

   for (fil = finf; fil != NULL && fil -> fnr != 0; fil ++)
   {
      if (fnr == fil -> fnr)
      {
         gefunden = 1;
         break;
      }
   }

   if (gefunden == 0)
   {
      sprintf (fmessage, "\n"
                         "+++ Unbekannter Fehler\n"
                         "    Abbruch des Programms.\n\n");
      sl_all = strlen (fmessage);
      
      sl_alt = strlen (*error_list);
      *error_list = realloc (*error_list, 
                             (sl_alt + sl_all + 1) * sizeof (char));
      memcpy (*error_list + sl_alt, fmessage, sl_all);
      *(*error_list + sl_alt + sl_all) = 0x00;
   
      return;
   }


   if (fil -> art == 'W')
      sprintf (fmessage, "\n"
               "+++ Warnung in Funktion %s,\n"
               "               Datei    %s,\n"
               "               Zeile    %d:\n",
               funk_name,
               filename, 
               zeile);
   else if (fil -> art == 'F' || fil -> art == 'E')
      sprintf (fmessage, "\n"
               "+++ Fehler in Funktion %s,\n"
               "              Datei    %s,\n"
               "              Zeile    %d:\n",
               funk_name,
               filename, 
               zeile);


   sl_head = strlen (fmessage);
   memset (fmessage + sl_head, ' ', 4);
   sl_head += 4;
   sl_all = sl_head;

   for (cp1 = fil -> message, j = 1, cp4 = cp1;
        j <= 7 && info [j] != NULL; 
        j ++)
   {
      cp2 = strchr (cp4, '%');
   
      if (cp2 == NULL)
         break;

      if (*(cp2 + 1) == '%')
      {
         cp4 = cp2 + 2;
         j --;
         continue;
      }


      gefunden = 0;

      cp3 = strpbrk (cp2 + 1, "csdlhfe");

      if (cp3 == NULL)
      {
         sprintf (fmessage + sl_all, "    Fehler in errfunc.c: "
                                     "unzulaessige Formatangabe\n");
         break;
      }

      if (*(cp3 + 1) != 0x00 && *(cp3 + 1) != '%')
         cp2 = cp3 + 2;
      else
         cp2 = cp3 + 1;

      sl = cp2 - cp1;

      memcpy (zw, cp1, sl);
      zw [sl] = 0x00;

      switch (*cp3)
      {
         case 'd':
            sprintf (fmessage + sl_all, zw, *((int *)(info [j])));
            break;
                
         case 'l':
            sprintf (fmessage + sl_all, zw, *((long *)(info [j])));
            break;

         case 'h':
            sprintf (fmessage + sl_all, zw, *((short *)(info [j])));
            break;

         case 'e':
         case 'f':
            sprintf (fmessage + sl_all, zw, *((double *)(info [j])));
            break;

         case 'c':
            sprintf (fmessage + sl_all, zw, *((char *)(info [j])));
            break;

         case 's':
            sprintf (fmessage + sl_all, zw, (char *)(info [j]));
            break;

         default:
            break;
      }
      
      sl_all = strlen (fmessage);

      cp1 = cp2;
      cp4 = cp2;
   }

   sprintf (fmessage + sl_all, "%s", cp1);
   sl_all = strlen (fmessage);
   
   sl_alt = strlen (*error_list);
   *error_list = realloc (*error_list, 
                          (sl_alt + sl_all + 1) * sizeof (char));
   memcpy (*error_list + sl_alt, fmessage, sl_all);
   *(*error_list + sl_alt + sl_all) = 0x00;

   return;
}





void fehler2 (int zeile,
              int fnr,
              char *filename,
              char *funk_name,
              FILE *protfile,
              fehlerinfo *finf,
              ...)

/************************************************/
/*   Ausgabe von Laufzeitfehlern,               */
/*   ggf. Programmabbruch                       */
/************************************************/

{
   int j;
   int gefunden;
   int sl;

   char fmessage [255];
   char *cp1;
   char *cp2;
   char *cp3;
   char *cp4;
   fehlerinfo *fil;
   va_list ap;

   int vint;
   double vdouble;
   long vlong;
   char *pchar;

   fprintf (protfile, "\n\n");

   gefunden = 0;

   for (fil = finf; fil != NULL && fil -> fnr != 0; fil ++)
   {
      if (fnr == fil -> fnr)
      {
         gefunden = 1;
         break;
      }
   }

   if (gefunden == 0)
   {
      fprintf (protfile, "+++ Unbekannter Fehler\n");
      fprintf (protfile, "    Abbruch des Programms.\n\n");
      exit (20);
   }

   if (fil -> art == 'W')
      fprintf (protfile, 
               "+++ Warnung in Funktion %s,\n"
               "               Datei    %s,\n"
               "               Zeile    %d:\n",
               funk_name,
               filename, 
               zeile);
   else if (fil -> art == 'F' || fil -> art == 'E')
      fprintf (protfile, 
               "+++ Fehler in Funktion %s,\n"
               "              Datei    %s,\n"
               "              Zeile    %d:\n",
               funk_name,
               filename, 
               zeile);


   va_start(ap,finf);

   for (cp1 = fil -> message, j = 1, cp4 = cp1;
        j <= 7; j ++)
   {
      cp2 = strchr (cp4, '%');
   
      if (cp2 == NULL)
         break;

      if (*(cp2 + 1) == '%')
      {
         cp4 = cp2 + 2;
         j --;
         continue;
      }


      gefunden = 0;

      cp3 = strpbrk (cp2 + 1, "sdlfe");

      if (cp3 == NULL)
      {
         fprintf (protfile, "    Fehler in errfunc.c: ");
         fprintf (protfile, "unzulaessige Formatangabe\n");
         break;
      }

      if (*(cp3 + 1) != 0x00 && *(cp3 + 1) != '%')
         cp2 = cp3 + 2;
      else
         cp2 = cp3 + 1;

      sl = cp2 - cp1;

      memcpy (fmessage, cp1, sl);
      fmessage [sl] = 0x00;

      switch (*cp3)
      {
         /*****************************************************/
         /* shorts und chars darf es hier aus Syntaxgründen   */
         /* nicht geben (!?!)                                 */
         /*****************************************************/

         case 'd':
            vint = va_arg(ap,int);
            fprintf (protfile, fmessage, vint);
            break;
                
         case 'l':
            vlong = va_arg(ap,long);
            fprintf (protfile, fmessage, vlong);
            break;

         case 'e':
         case 'f':
            vdouble = va_arg(ap,double);
            fprintf (protfile, fmessage, vdouble);
            break;

         case 's':
            pchar = va_arg(ap,char *);
            fprintf (protfile, fmessage, pchar);
            break;

         default:
            break;
      }

      cp1 = cp2;
      cp4 = cp2;
   }

   va_end (ap);

   fprintf (protfile, "%s", cp1);

   if (fil -> art == 'E')
   {
      fprintf (protfile, "    Abbruch des Programms.\n\n");
      exit (20);
   }

   return;
}




void fehler3 (int zeile,
              int fnr,
              char *filename,
              char *funk_name,
              char **error_list,
              fehlerinfo *finf,
              ...)

/************************************************/
/*   Ausgabe von Laufzeitfehlern,               */
/*   ggf. Programmabbruch                       */
/************************************************/

{
   int j;
   int gefunden;
   int sl;
   int sl_all;
   int sl_alt;
   int sl_head;

   char fmessage [512];
   char zw [255];
   char *cp1;
   char *cp2;
   char *cp3;
   char *cp4;
   fehlerinfo *fil;
   va_list ap;

   int vint;
   double vdouble;
   long vlong;
   char *pchar;

   gefunden = 0;

   for (fil = finf; fil != NULL && fil -> fnr != 0; fil ++)
   {
      if (fnr == fil -> fnr)
      {
         gefunden = 1;
         break;
      }
   }


   if (gefunden == 0)
   {
      sprintf (fmessage, "\n+++ Unbekannter Fehler\n"
                           "    Abbruch des Programms.\n\n");
      sl_all = strlen (fmessage);
      
      sl_alt = strlen (*error_list);
      *error_list = realloc (*error_list, 
                             (sl_alt + sl_all + 1) * sizeof (char));
      memcpy (*error_list + sl_alt, fmessage, sl_all);
      *(*error_list + sl_alt + sl_all) = 0x00;
   
      return;
   }


   if (fil -> art == 'W')
      sprintf (fmessage, "\n"
               "+++ Warnung in Funktion %s,\n"
               "               Datei    %s,\n"
               "               Zeile    %d:\n",
               funk_name,
               filename, 
               zeile);
   else if (fil -> art == 'F' || fil -> art == 'E')
      sprintf (fmessage, "\n"
               "+++ Fehler in Funktion %s,\n"
               "              Datei    %s,\n"
               "              Zeile    %d:\n",
               funk_name,
               filename, 
               zeile);


   sl_head = strlen (fmessage);
   memset (fmessage + sl_head, ' ', 4);
   sl_head += 4;
   sl_all = sl_head;

   va_start (ap, finf);

   for (cp1 = fil -> message, j = 1, cp4 = cp1;
        j <= 7; j ++)
   {
      cp2 = strchr (cp4, '%');
   
      if (cp2 == NULL)
         break;

      if (*(cp2 + 1) == '%')
      {
         cp4 = cp2 + 2;
         j --;
         continue;
      }


      gefunden = 0;

      cp3 = strpbrk (cp2 + 1, "sdlfe");

      if (cp3 == NULL)
      {
         sprintf (fmessage + sl_all, "    Fehler in errfunc.c: "
                                     "unzulaessige Formatangabe\n");
         break;
      }

      if (*(cp3 + 1) != 0x00 && *(cp3 + 1) != '%')
         cp2 = cp3 + 2;
      else
         cp2 = cp3 + 1;

      sl = cp2 - cp1;

      memcpy (zw, cp1, sl);
      zw [sl] = 0x00;

      switch (*cp3)
      {
         /*****************************************************/
         /* shorts und chars darf es hier aus Syntaxgruenden  */
         /* nicht geben (!?!)                                 */
         /*****************************************************/

         case 'd':
            vint = va_arg (ap, int);
            sprintf (fmessage + sl_all, zw, vint);
            break;
                
         case 'l':
            vlong = va_arg (ap, long);
            sprintf (fmessage + sl_all, zw, vlong);
            break;

         case 'e':
         case 'f':
            vdouble = va_arg (ap, double);
            sprintf (fmessage + sl_all, zw, vdouble);
            break;

         case 's':
            pchar = va_arg (ap, char *);
            sprintf (fmessage + sl_all, zw, pchar);
            break;

         default:
            break;
      }

      sl_all = strlen (fmessage);

      cp1 = cp2;
      cp4 = cp2;
   }

   va_end (ap);

   sprintf (fmessage + sl_all, "%s", cp1);
   sl_all = strlen (fmessage);
   
   sl_alt = strlen (*error_list);
   *error_list = realloc (*error_list, 
                          (sl_alt + sl_all + 1) * sizeof (char));
   memcpy (*error_list + sl_alt, fmessage, sl_all);
   *(*error_list + sl_alt + sl_all) = 0x00;

   return;
}




