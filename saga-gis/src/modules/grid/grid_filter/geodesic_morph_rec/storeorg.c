/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      storeorg.c                       //
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
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "storeorg.h"


#define SPMESSAGE  nixtrace

static int nixtrace (char *cp, ...)
{
   return 0;
}



static void *mem_anker [ANZ_MEMPOOL] = { NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL };


/*********************************************************************/
/* Die folgenden beiden defines muessen immer aufeinander abgestimmt */
/* werden !                                                          */
/*********************************************************************/

#define CAPS_MALLOC  check_malloc
#define CAPS_FREE    check_free





static void *check_malloc (long ext_size)
{
   /******************************************************************/
   /* Diese Funktion ist nur fuer Aufrufe innerhalb von chain_malloc */
   /* oder chain_realloc gedacht.                                    */
   /*                                                                */
   /* Belegung:                                                      */
   /*                                                                */
   /*      12 Byte Bereich zum Testen der Integritaet                */
   /*       4 Byte voriger Speicherbereich           |               */
   /*       4 Byte naechster Speicherbereich         |  ext_size =   */
   /*       4 Byte Groesse                           |  size + 12    */
   /*    size Byte Datenblock                        |               */
   /*      12 Byte Bereich zum Testen der Integritaet                */
   /*  -----------------------------------------------------         */
   /* size+36 Byte insgesamt                                         */
   /******************************************************************/

   char *p;


   SPMESSAGE ("Start check_malloc\n");
   SPMESSAGE ("size %ld\n", ext_size);

   p = malloc (ext_size + 24);

   if (p == NULL)
   {
      printf ("schrecklicher Fehler bei check_malloc:\n");
      printf ("Allokation gescheitert:\n");
      return NULL;
   }

   memset (p, 0x00, ext_size + 24);

   memcpy (p, "<0123456789>", 12);
   memcpy (p + 12 + ext_size, "<0123456789>", 12);
   p += 12;

   SPMESSAGE ("p  %p\n", p);
   SPMESSAGE ("Ende check_malloc\n");

   return p;
}





static void check_free (void *palt)

{
   char *p;
   long size_lfd;

   SPMESSAGE ("Start check_free\n");
   SPMESSAGE ("palt   %p \n", palt);

   if (palt == NULL)
   {
      printf ("schrecklicher Fehler in check_free\n");
      printf ("NULL-pointer erhalten\n");
      return;
   }

   p = palt;
   p -= 12;


   SPMESSAGE ("Pruefung der Integritaet\n");

   if (memcmp (p, "<0123456789>", 12) != 0)
   {
      printf ("check_free - schrecklicher Speicherfehler\n");
      printf ("Bereich vor Datenblock zerstoert\n");
      exit (20);
   }

   size_lfd = *((long *)(p + 20));

   if (memcmp (p + 24 + size_lfd, "<0123456789>", 12) != 0)
   {
      printf ("check_free - schrecklicher Speicherfehler\n");
      printf ("Bereich nach Datenblock zerstoert\n");
      exit (20);
   }

   free ((void *) p);

   SPMESSAGE ("Ende check_free\n");

   return;
}






void *chain_malloc (long size, short mempool)
{
   /*******************************************************************/
   /* Belegung:                                                       */
   /*                                                                 */
   /*       4 Byte voriger Speicherbereich                            */
   /*       4 Byte naechster Speicherbereich                          */
   /*       4 Byte Groesse                                            */
   /*    size Byte Datenblock                                         */
   /*  -----------------------------------------------------          */
   /* size+12 Byte insgesamt                                          */
   /*                                                                 */
   /* Der mem_anker ist jeweils das NEUESTE Element!                  */
   /*                                                                 */
   /* Ist allerdings CAPS_MALLOC als check_malloc definiert, also     */
   /* #define CAPS_MALLOC  check_malloc  ,                            */
   /* so wird zusaetzlich ein Prueffeld (Magic) vorne und hinten      */
   /* angehaengt, es gilt dann folgende Konvention fuer die Belegung  */
   /* des allokierten Arrays:                                         */
   /*                                                                 */
   /* [<0123456789> | prev | next | size |   DATEN    | <0123456789>] */
   /* [  12 Bytes   | 4 B  | 4 B  | 4 B  |  size      |   12 Bytes  ] */
   /*                ^                    ^                           */
   /*                |                    |                           */
   /*                |                    |                           */
   /* (hierher zeigt der Zeiger, den      |                           */
   /*  man von CAPS_MALLOC zurueck-       |                           */
   /*  bekommt, und DIESE Adresse         |                           */
   /*  steht in mem_anker, falls der      |                           */
   /*  Speicherbereich das neueste        |                           */
   /*  Element ist)                       |                           */
   /*                                     |                           */
   /*                             (hierher zeigt der Zeiger, den      */
   /*                              chain_malloc zurueckgibt)          */
   /*                                                                 */
   /*******************************************************************/

   char *p;
   char *ph;

   SPMESSAGE ("Start chain_malloc\n");
   SPMESSAGE ("size %ld\n", size);

   if (mempool < 0 || mempool >= ANZ_MEMPOOL)
      return NULL;

   p = CAPS_MALLOC (size + 12);

   if (p == NULL)
   {
      printf ("schrecklicher Fehler bei CAPS_MALLOC:\n");
      printf ("Allokation gescheitert:\n");
      return NULL;
   }

   memset (p, 0x00, size + 12);

   /*******************************************************************/
   /* In die ersten vier Bytes von p kommt die Speicheradresse des    */
   /* derzeitigen mem_ankers rein. Danach kommt umgekehrt in die      */
   /* Bytes 5 - 8 des mem_ankers, falls ungleich NULL, die Adresse    */
   /* von p rein.                                                     */
   /*******************************************************************/

   * ((void **) p) = mem_anker [mempool];

   if (mem_anker [mempool] != NULL)
   {
      ph = mem_anker [mempool];
      ph += 4;
      *((void **) ph) = p;
   }

   mem_anker [mempool] = p;

   *((long *)(p + 8)) = size;
   p += 12;


   SPMESSAGE ("p  %p\n", p);
   SPMESSAGE ("Ende chain_malloc\n");

   return p;
}





void chain_free (void *palt)

{
   char *p;
   char *p_prev;
   char *p_next;
   short i;
   short gefunden;

   SPMESSAGE ("Start chain_free\n");
   SPMESSAGE ("palt   %p \n", palt);

   if (palt == NULL)
   {
      printf ("schrecklicher Fehler in chain_free\n");
      printf ("NULL-pointer erhalten\n");
      return;
   }


   p = palt;
   p -= 12;

   /*******************************/
   /* Bereiche neu verpointern    */
   /*******************************/

   p_prev = *((void **) p);
   p_next = *((void **) (p + 4));

   if (p_prev != NULL)
      *((void **) (p_prev + 4)) = p_next;

   if (p_next != NULL)
      *((void **) (p_next)) = p_prev;
   else
   {
      gefunden = 0;
	  //#pragma omp parallel for
      for (i = 0; i < ANZ_MEMPOOL; i ++)
      {
         if ((void *) p == mem_anker [i])
         {
            gefunden = 1;
            break;
         }
      }

      if (gefunden > 0)
         mem_anker [i] = p_prev;
      else
      {
         printf ("schrecklicher Fehler: chain-Element ohne Anker\n");
         return;
      }
   }

   CAPS_FREE ((void *) p);

   SPMESSAGE ("Ende chain_free\n");

   return;
}





void *chain_realloc (void *palt,
                     long newsize,
                     short mempool)

{
   /*******************************************************************/
   /* Hier koennte man das Element auch unter der Hand in einen       */
   /* anderen mempool verschieben.                                    */
   /*******************************************************************/#

   void *p;
   long oldsize;
   char *ph;

   SPMESSAGE ("Start chain_realloc");

   p = chain_malloc (newsize, mempool);

   if (p != NULL)
   {
      if (palt != NULL)
      {
         ph = palt;
         oldsize = *((long *)(ph - 4));

         if (oldsize != 0)
            memcpy (p, palt, oldsize);

         chain_free (palt);
      }
   }
   else
      printf ("schrecklicher Fehler: chain_realloc gescheitert!\n");

   SPMESSAGE ("Ende chain_realloc");

   return p;
}





void chain_integrity (short mempool)
{
   /******************************************************************/
   /* Belegung:                                                      */
   /*                                                                */
   /*      12 Byte Bereich zum Testen der Integritaet                */
   /*       4 Byte voriger Speicherbereich                           */
   /*       4 Byte naechster Speicherbereich                         */
   /*       4 Byte Groesse                                           */
   /*    size Byte Datenblock                                        */
   /*      12 Byte Bereich zum Testen der Integritaet                */
   /*  -----------------------------------------------------         */
   /* size+36 Byte insgesamt                                         */
   /******************************************************************/

   char *p2;
   long size_lfd;

   SPMESSAGE ("Pruefung der Integritaet, mempool %hd\n", mempool);
   //#pragma omp parallel for
   for (p2 = mem_anker [mempool]; p2 != NULL; p2 = * ((void **) p2))
   {
      if (memcmp (p2 - 12, "<0123456789>", 12) != 0)
      {
         printf ("chain_integrity - "
                 "schrecklicher Speicherfehler\n");
         printf ("Bereich vor Datenblock zerstoert\n");
         exit (20);
      }

      size_lfd = *((long *)(p2 + 8));

      if (memcmp (p2 + 12 + size_lfd, "<0123456789>", 12) != 0)
      {
         printf ("chain_integrity - "
                 "schrecklicher Speicherfehler\n");
         printf ("Bereich nach Datenblock zerstoert\n");
         exit (20);
      }
   }

   printf ("Integritaet mempool %hd ok\n", mempool);

   return;
}





void chain_all_free (short mempool)

{
   void *cp;
   void *cp_prev;
   //#pragma omp parallel for
   for (cp = mem_anker [mempool]; cp != NULL; cp = cp_prev)
   {
      cp_prev = * ((void **) cp);
      CAPS_FREE (cp);
   }

   mem_anker [mempool] = NULL;

   return;
}


/*********************************************************************/
/*   eigenes (modifiziertes) basis_malloc usw.                       */
/*   realloc selbst, malloc und free durch                           */
/*   Standard-Speicherverwaltung realisiert;                         */
/*   Bereiche verpointern und jedesmal die Integritaet der           */
/*   Kette checken                                                   */
/*********************************************************************/

static void *stor_kette = NULL;


void *basis_malloc (long size)
{
   /******************************************************************/
   /* Belegung:                                                      */
   /*                                                                */
   /*       4 Byte voriger Speicherbereich                           */
   /*       4 Byte naechster Speicherbereich                         */
   /*       4 Byte Groesse                                           */
   /*      12 Byte Bereich zum Testen der Integritaet                */
   /*    size Byte Datenblock                                        */
   /*      12 Byte Bereich zum Testen der Integritaet                */
   /*  -----------------------------------------------------         */
   /* size+36 Byte insgesamt                                         */
   /******************************************************************/


   char *p;
   char *ph;

#if 0
   char *p2;
   long size_lfd;
#endif

   SPMESSAGE ("Start basis_malloc\n");
   SPMESSAGE ("size %ld\n", size);

   p = malloc (size + 36);

   if (p == NULL)
   {
      printf ("schrecklicher Fehler bei basis_malloc:\n");
      printf ("Allokation gescheitert:\n");
      return NULL;
   }

   memset (p, 0x00, size + 36);

   /*******************************************************************/
   /* In die ersten vier Bytes von p kommt die Speicheradresse von    */
   /* stor_kette rein. Danach kommt umgekehrt in die Bytes 5 - 8      */
   /* von stor_kette, falls ungleich NULL, die Adresse von p rein.    */
   /* (Ist das noetig, die Liste doppelt zu verketten?)               */
   /*******************************************************************/

   * ((void **) p) = stor_kette;

   if (stor_kette != NULL)
   {
      ph = stor_kette;
      ph += 4;
      *((void **) ph) = p;
   }

   stor_kette = p;

   *((long *)(p + 8)) = size;
   memcpy (p + 12, "<0123456789>", 12);
   memcpy (p + 24 + size, "<0123456789>", 12);
   p += 24;

#if 0
   SPMESSAGE ("Pruefung der Integritaet\n");

   for (p2 = stor_kette; p2 != NULL;  p2 = * ((void **) p2))
   {
      if (memcmp (p2 + 12, "<0123456789>", 12) != 0)
      {
         printf ("basis_malloc - schrecklicher Speicherfehler\n");
         printf ("Bereich vor Datenblock zerstoert\n");
         exit (20);
      }

      size_lfd = *((long *)(p2 + 8));

      if (memcmp (p2 + 24 + size_lfd, "<0123456789>", 12) != 0)
      {
         printf ("basis_malloc - schrecklicher Speicherfehler\n");
         printf ("Bereich nach Datenblock zerstoert\n");
         exit (20);
      }
   }
#endif

   SPMESSAGE ("p  %p\n", p);
   SPMESSAGE ("Ende basis_malloc\n");

   return p;
}




void basis_free (void *palt)

{
   char *p;
   char *p_prev;
   char *p_next;
   long size_lfd;

   SPMESSAGE ("Start basis_free\n");
   SPMESSAGE ("palt   %p \n", palt);

   if (palt == NULL)
   {
      printf ("schrecklicher Fehler in basis_free\n");
      printf ("NULL-pointer erhalten\n");
      return;
   }


   p = palt;
   p -= 24;


   SPMESSAGE ("Pruefung der Integritaet\n");

   if (memcmp (p + 12, "<0123456789>", 12) != 0)
   {
      printf ("basis_free - schrecklicher Speicherfehler\n");
      printf ("Bereich vor Datenblock zerstoert\n");
      exit (20);
   }

   size_lfd = *((long *)(p + 8));

   if (memcmp (p + 24 + size_lfd, "<0123456789>", 12) != 0)
   {
      printf ("basis_free - schrecklicher Speicherfehler\n");
      printf ("Bereich nach Datenblock zerstoert\n");
      exit (20);
   }

   /*******************************/
   /* Bereiche neu verpointern    */
   /*******************************/

   p_prev = *((void **) p);
   p_next = *((void **) (p + 4));

   if (p_prev != NULL)
      *((void **) (p_prev + 4)) = p_next;

   if (p_next != NULL)
      *((void **) (p_next)) = p_prev;
   else
      stor_kette = p_prev;

   free ((void *) p);

   SPMESSAGE ("Ende basis_free\n");

   return;
}





void integritaet_pruefen (void)
{
   /******************************************************************/
   /* Belegung:                                                      */
   /*                                                                */
   /*       4 Byte voriger Speicherbereich                           */
   /*       4 Byte naechster Speicherbereich                         */
   /*       4 Byte Groesse                                           */
   /*      12 Byte Bereich zum Testen der Integritaet                */
   /*    size Byte Datenblock                                        */
   /*      12 Byte Bereich zum Testen der Integritaet                */
   /*  -----------------------------------------------------         */
   /* size+36 Byte insgesamt                                         */
   /******************************************************************/

   char *p2;
   long size_lfd;

   SPMESSAGE ("Pruefung der Integritaet\n");

	//#pragma omp parallel for
   for (p2 = stor_kette; p2 != NULL; p2 = * ((void **) p2))
   {
      if (memcmp (p2 + 12, "<0123456789>", 12) != 0)
      {
         printf ("integritaet_pruefen - "
                 "schrecklicher Speicherfehler\n");
         printf ("Bereich vor Datenblock zerstoert\n");
         exit (20);
      }

      size_lfd = *((long *)(p2 + 8));

      if (memcmp (p2 + 24 + size_lfd, "<0123456789>", 12) != 0)
      {
         printf ("integritaet_pruefen - "
                 "schrecklicher Speicherfehler\n");
         printf ("Bereich nach Datenblock zerstoert\n");
         exit (20);
      }
   }

   printf ("Integritaet ok\n");

   return;
}




int adr_in_kette_finden (void *adr)
{
   /******************************************************************/
   /* Belegung:                                                      */
   /*                                                                */
   /*       4 Byte voriger Speicherbereich                           */
   /*       4 Byte naechster Speicherbereich                         */
   /*       4 Byte Groesse                                           */
   /*      12 Byte Bereich zum Testen der Integritaet                */
   /*    size Byte Datenblock                                        */
   /*      12 Byte Bereich zum Testen der Integritaet                */
   /*  -----------------------------------------------------         */
   /* size+36 Byte insgesamt                                         */
   /******************************************************************/

   char *p2;
   long size_lfd;
   int rc;

   SPMESSAGE ("Pruefung der Integritaet\n");

   rc = 0;
   //#pragma omp parallel for
   for (p2 = stor_kette; p2 != NULL; p2 = * ((void **) p2))
   {
      if (memcmp (p2 + 12, "<0123456789>", 12) != 0)
      {
         printf ("integritaet_pruefen - "
                 "schrecklicher Speicherfehler\n");
         printf ("Bereich vor Datenblock zerstoert\n");
         exit (20);
      }

      if (p2 + 24 == ((char *) adr))
         rc = 1;

      size_lfd = *((long *)(p2 + 8));

      if (memcmp (p2 + 24 + size_lfd, "<0123456789>", 12) != 0)
      {
         printf ("integritaet_pruefen - "
                 "schrecklicher Speicherfehler\n");
         printf ("Bereich nach Datenblock zerstoert\n");
         exit (20);
      }
   }

   return rc;
}




void *basis_calloc (long length, int obj_size)
{
   void *p;

   p = basis_malloc (length * obj_size);

   return p;
}




void integritaet_speziell (void *palt)

{
   char *p;
   long size_lfd;

   p = palt;
   p -= 24;


   SPMESSAGE ("Pruefung der Integritaet\n");

   if (memcmp (p + 12, "<0123456789>", 12) != 0)
   {
      printf ("integritaet_speziell - schrecklicher Speicherfehler\n");
      printf ("Bereich vor Datenblock zerstoert\n");
      exit (20);
   }

   size_lfd = *((long *)(p + 8));

   if (memcmp (p + 24 + size_lfd, "<0123456789>", 12) != 0)
   {
      printf ("integritaet_speziell - schrecklicher Speicherfehler\n");
      printf ("Bereich nach Datenblock zerstoert\n");
      exit (20);
   }

   return;
}






void *basis_realloc (void *palt,
                     long newsize)

{
   void *p;
   long oldsize;
   char *ph;

   SPMESSAGE ("Start basis_realloc");

   p = basis_malloc (newsize);

   if (p != NULL)
   {
      if (palt != NULL)
      {
         ph = palt;
         oldsize = *((long *)(ph - 16));

         if (oldsize != 0)
            memcpy (p, palt, oldsize);

         basis_free (palt);
      }
   }
   else
      printf ("schrecklicher Fehler: realloc gescheitert!\n");

   SPMESSAGE ("Ende basis_realloc");

   return p;
}




/**********************************************************************/
/* einen double-Vektor mit Indexbereich   0 ... n   allokieren und    */
/* initialisieren. Es wird also immer ein Element mehr allokiert als  */
/* im Prinzip noetig waere.                                           */
/**********************************************************************/

double *dvector_alloc (long n)
{
   double *v;

   v = (double *) BASIS_CALLOC ((size_t) (n + 1), sizeof (double));

   return v;
}





size_t element_length (char type)
{
   switch (type)
   {
      case 'C':
         return (sizeof (char));

      case 'U':
         return (sizeof (unsigned char));

      case 'H':
         return (sizeof (unsigned short));

      case 'S':
         return (sizeof (short));

      case 'I':
         return (sizeof (int));

      case 'L':
         return (sizeof (long));

      case 'F':
         return (sizeof (float));

      case 'D':
         return (sizeof (double));

      default:
         return 0;
   }
}



/**********************************************************************/
/* einen Vektor mit Indexbereich   0 ... n   allokieren und           */
/* initialisieren. Es wird also immer ein Element mehr allokiert als  */
/* im Prinzip noetig waere.                                           */
/**********************************************************************/

void *vector_alloc (long n, char type)
{
   size_t el;

   el = element_length (type);
   return BASIS_CALLOC ((size_t) (n + 1), el);
}




double *basis_dvector_alloc (long n)
{
   /***********************************************************/
   /* einen Vektor mit Indexbereich 1 ... n allokieren        */
   /* und initialisieren                                      */
   /***********************************************************/

   double *v;

   v = (double *) basis_calloc ((size_t) (n + 1), sizeof (double));

   return v;
}



#if 0
float *fvector_alloc (long n)
{
   /***********************************************************/
   /* einen Vektor mit Indexbereich 1 ... n allokieren        */
   /* und initialisieren                                      */
   /***********************************************************/

   float *v;

   v = (float *) calloc ((size_t) (n + 1), sizeof (float));

   return v;
}




float *fvector_alloc_1 (long n)
{
   /***********************************************************/
   /* einen Vektor mit Indexbereich 1 ... n allokieren        */
   /* und initialisieren                                      */
   /***********************************************************/

   float *v;

   v = (float *) calloc ((size_t) (n + 1), sizeof (float));

   return v;
}




int *ivector_alloc_1 (long n)
{
   /***********************************************************/
   /* einen Int-Vektor mit Indexbereich 1 ... n allokieren    */
   /* und initialisieren                                      */
   /***********************************************************/

   int *v;

   v = (int *) calloc ((size_t) (n + 1), sizeof (int));

   return v;
}
#endif


void *vector_realloc (void *v, long n_alt, long n_neu, char type)
{
   /*******************************************************************/
   /* einen Vektor reallokieren und den neuen Bereich initialisieren  */
   /*******************************************************************/

   void *u;
   size_t el;

   el = element_length (type);

   u = BASIS_REALLOC (v, (size_t) ((n_neu + 1) * el));

   if (n_neu > n_alt)
      memset (((char *) u) + (n_alt + 1) * el,
              0x00,
              (n_neu - n_alt) * el);

   return u;
}




double *dvector_realloc (double *v, long n_alt, long n_neu)
{
   /***********************************************************/
   /* einen double-Vektor auf den Indexbereich 1 ... n_neu    */
   /* reallokieren und den neuen Bereich initialisieren       */
   /***********************************************************/

   double *u;

   u = (double *) BASIS_REALLOC (v, ((size_t) (n_neu + 1)) *
                                                       sizeof (double));

   if (n_neu > n_alt)
      memset (u + n_alt + 1, 0x00, (n_neu - n_alt) * sizeof (double));

   return u;
}




double *dvector_realloc_1 (double *v, long n_alt, long n_neu)
{
   /***********************************************************/
   /* einen double-Vektor auf den Indexbereich 1 ... n_neu    */
   /* reallokieren und den neuen Bereich initialisieren       */
   /***********************************************************/

   double *u;

   u = (double *) realloc (v, ((size_t) (n_neu + 1)) * sizeof (double));

   if (n_neu > n_alt)
      memset (u + n_alt + 1, 0x00, (n_neu - n_alt) * sizeof (double));

   return u;
}




void ivector_free (int *v)

{
   /***************************************************************/
   /* einen int-Vektor freigeben                                  */
   /***************************************************************/

   BASIS_FREE (v);
   return;
}




void vector_free (void *v)

{
   /***************************************************************/
   /* einen Vektor freigeben                                      */
   /***************************************************************/

   BASIS_FREE (v);
   return;
}




void dvector_free (double *v)

{
   /***************************************************************/
   /* einen double-Vektor freigeben                               */
   /***************************************************************/

   BASIS_FREE (v);
   return;
}




void fvector_free (float *v)

{
   /***************************************************************/
   /* einen double-Vektor freigeben                               */
   /***************************************************************/

   BASIS_FREE (v);
   return;
}




/**********************************************************************/
/* Double-Matrix mit Indexbereich                                     */
/*                   indexing, ... num_rows + indexing - 1 (Zeilen),  */
/*                   indexing, ... num_cols + indexing - 1 (Spalten)  */
/* allokieren                                                         */
/* Hier wird nur der Vektor von Zeigern auf die Zeilen allokiert und  */
/* gesetzt. indexing kann 0 oder 1 sein.                              */
/* Der erste Zeiger m [indexing] zeigt auf den mitgegebenen Vektor a, */
/* so dass m [indexing][indexing] = a [indexing].                     */
/**********************************************************************/

double **dmatrix_pointer_alloc (double *a,
                                long num_rows,
                                long num_cols,
                                short indexing)
{
   double **m;
   long row_start;
   long end_row;
   long j;


   if (indexing < 0 || indexing > 1)
      return NULL;

   m = (double **) BASIS_MALLOC ((size_t) (num_rows + indexing) *
                                                    sizeof (double *));
   if (m == NULL)
      return NULL;

   m [0] = a;

   row_start = (1 - indexing) * num_cols;
   end_row = num_rows + indexing;
   //#pragma omp parallel for
   for (j = 1; j < end_row; j ++, row_start += num_cols)
      m [j] = a + row_start;

   return m;
}





/**********************************************************************/
/* Matrix mit Indexbereich                                            */
/*                   indexing, ... num_rows + indexing - 1 (Zeilen),  */
/*                   indexing, ... num_cols + indexing - 1 (Spalten)  */
/* allokieren                                                         */
/* Hier wird nur der Vektor von Zeigern auf die Zeilen der Matrix     */
/* allokiert und auf die entsprechenden Elemente des mitgegebenen     */
/* Datenvektors a gesetzt. indexing kann 0 oder 1 sein.               */
/* Der erste Zeiger m [indexing] zeigt auf den mitgegebenen Vektor a, */
/* so dass m [indexing][indexing] = a [indexing].                     */
/* Unabhaengig von indexing ist m [0] = a.                            */
/*                                                                    */
/* Folgende Typen sind moeglich:  D  -- double                        */
/*                                F  -- float                         */
/*                                I  -- int                           */
/*                                S  -- short                         */
/*                                L  -- long                          */
/*                                C  -- char                          */
/*                                U  -- unsigned char                 */
/*                                                                    */
/* Diese Funktion ist noch nicht genuegend getestet. Falls sie nicht  */
/* richtig funktioniert, kann man auf die kommentierte Variante       */
/* in speiorg.c zurueckgreifen !!!!                                   */
/**********************************************************************/

void **matrix_pointer_alloc (void *a,
                             long num_rows,
                             long num_cols,
                             char type,
                             short indexing)
{
   size_t el;
   long end_row;
   void **m;
   char *acurr;
   long j;

   el = element_length (type);

   if (indexing < 0 || indexing > 1)
      return NULL;

   end_row = num_rows + indexing;

   m = (void **) BASIS_MALLOC ((size_t) (num_rows + indexing) *
                                              sizeof (void *));
   if (m == NULL)
      return NULL;

   if (indexing == 1)
      m [0] = a;

   acurr = (char *) a;
   //#pragma omp parallel for
   for (j = indexing; j < end_row; j ++, acurr += (num_cols * el))
      m [j] = (void *) acurr;

   return m;
}




void ***array_3_pointer_alloc (void *a,
                               long num_rows,
                               long num_cols,
                               long num_levs,
                               char type,
                               short indexing)
{
   size_t el;
   long end_row;
   long num_last_dims;
   void ***p;
   char *acurr;
   long j;

   el = element_length (type);

   if (indexing < 0 || indexing > 1)
      return NULL;

   end_row = num_rows + indexing;
   num_last_dims = num_cols * num_levs;


   p = (void ***) BASIS_MALLOC ((size_t) end_row * sizeof (void **));

   if (p == NULL)
      return NULL;

   if (indexing == 1)
      p [0] = &a;

   acurr = (char *) a;

   for (j = indexing; j < end_row; j ++, acurr += (num_last_dims * el))
   {
      p [j] = matrix_pointer_alloc (acurr,
                                    num_cols,
                                    num_levs,
                                    type,
                                    indexing);
      if (p [j] == NULL)
         return NULL;
   }

   return p;
}




void ****array_4_pointer_alloc (void *a,
                                long num_rows,
                                long num_cols,
                                long num_levs,
                                long num_elems,
                                char type,
                                short indexing)
{
   size_t el;
   long end_row;
   long num_last_dims;
   void ****p;
   char *acurr;
   long j;

   el = element_length (type);

   if (indexing < 0 || indexing > 1)
      return NULL;

   end_row = num_rows + indexing;
   num_last_dims = num_cols * num_levs * num_elems;


   p = (void ****) BASIS_MALLOC ((size_t) end_row * sizeof (void ***));

   if (p == NULL)
      return NULL;

   if (indexing == 1)
      p [0][0] = &a;

   acurr = (char *) a;
   //#pragma omp parallel for
   for (j = indexing; j < end_row; j ++, acurr += (num_last_dims * el))
   {
      p [j] = array_3_pointer_alloc (acurr,
                                     num_cols,
                                     num_levs,
                                     num_elems,
                                     type,
                                     indexing);
      if (p [j] == NULL)
         return NULL;
   }

   return p;
}




void **matrix_all_alloc (long zn, long sn, char type, short indexing)
{
   void *a;
   long ges_laenge;

   ges_laenge = zn * sn;

   a = vector_alloc (ges_laenge, type);

   if (a == NULL)
      return NULL;

   return matrix_pointer_alloc (a, zn, sn, type, indexing);
}






void ***array_3_all_alloc (long zn,
                           long sn,
                           long hn,
                           char type,
                           short indexing)
{
   void *a;
   long ges_laenge;

   ges_laenge = zn * sn * hn;

   a = vector_alloc (ges_laenge, type);

   if (a == NULL)
      return NULL;

   return array_3_pointer_alloc (a, zn, sn, hn, type, indexing);
}






void **matrix_realloc_zeilen (void **m,
                              long zn_alt,
                              long zn_neu,
                              long sn,
                              char type,
                              short indexing)
{
   /***********************************************************/
   /* eine Matrix reallokieren.                               */
   /* Nur die Anzahl der Zeilen darf sich aendern!            */
   /***********************************************************/

   void **m_neu;
   void *a;
   long ges_laenge_alt;
   long ges_laenge_neu;


   a = m [0];

   ges_laenge_alt = zn_alt * sn;
   ges_laenge_neu = zn_neu * sn;

   a = vector_realloc (a, ges_laenge_alt, ges_laenge_neu, type);

   if (a == NULL)
      return NULL;

   BASIS_FREE (m);
   m_neu = matrix_pointer_alloc (a, zn_neu, sn, type, indexing);

   return m_neu;
}




double **dmatrix_zeiger_alloc_1 (double *a, long zn, long sn)
{
   /***********************************************************/
   /* eine Matrix mit Indexbereich 1, ... zn (Zeilen)         */
   /*                              1, ... sn (Spalten)        */
   /* allokieren                                              */
   /* Hier wird nur der Vektor von Zeigern allokiert und      */
   /* gesetzt. Der erste Zeiger m [1] zeigt auf den           */
   /* mitgegebenen Vektor a, so dass m [1][1] = a [1].        */
   /***********************************************************/

   double **m;
   long i;

   m = (double **) malloc ((size_t) (zn + 1) * sizeof (double *));

   if (m == NULL)
      return NULL;

   m [0] = a;
   #pragma omp parallel for
   for (i = 0; i < zn; i ++)
      m [i + 1] = a + i * sn;

   return m;
}








double **basis_dmatrix_zeiger_alloc (double *a, long zn, long sn)
{
   /***********************************************************/
   /* eine Matrix mit Indexbereich 1, ... zn (Zeilen)         */
   /*                              1, ... sn (Spalten)        */
   /* allokieren                                              */
   /* Hier wird nur der Vektor von Zeigern allokiert und      */
   /* gesetzt. Der erste Zeiger m [1] zeigt auf den           */
   /* mitgegebenen Vektor a, so dass m [1][1] = a [1].        */
   /***********************************************************/

   double **m;
   long i;

   m = (double **) basis_malloc ((size_t) (zn + 1) *
                                              sizeof (double *));

   if (m == NULL)
      return NULL;

   m [0] = a;
   #pragma omp parallel for
   for (i = 0; i < zn; i ++)
      m [i + 1] = a + i * sn;

   return m;
}




double **basis_dmatrix_zeiger_alloc_1 (double *a, long zn, long sn)
{
   /***********************************************************/
   /* eine Matrix mit Indexbereich 1, ... zn (Zeilen)         */
   /*                              1, ... sn (Spalten)        */
   /* allokieren                                              */
   /* Hier wird nur der Vektor von Zeigern allokiert und      */
   /* gesetzt. Der erste Zeiger m [1] zeigt auf den           */
   /* mitgegebenen Vektor a, so dass m [1][1] = a [1].        */
   /***********************************************************/

   double **m;
   long i;

   m = (double **) basis_malloc ((size_t) (zn + 1) *
                                              sizeof (double *));

   if (m == NULL)
      return NULL;

   m [0] = a;
   #pragma omp parallel for
   for (i = 0; i < zn; i ++)
      m [i + 1] = a + i * sn;

   return m;
}




double **basis_dmatrix_zeiger_alloc_0 (double *a, long zn, long sn)
{
   /***********************************************************/
   /* eine Matrix mit Indexbereich 0, ... zn - 1 (Zeilen)     */
   /*                              0, ... sn - 1 (Spalten)    */
   /* allokieren                                              */
   /* Hier wird nur der Vektor von Zeigern allokiert und      */
   /* gesetzt. Der erste Zeiger m [0] zeigt auf den           */
   /* mitgegebenen Vektor a, so dass m [0][0] = a [0].        */
   /***********************************************************/

   double **m;
   long i;

   m = (double **) basis_malloc ((size_t) zn * sizeof (double *));

   if (m == NULL)
      return NULL;
   #pragma omp parallel for
   for (i = 0; i < zn; i ++)
      m [i] = a + i * sn;

   return m;
}







double **doberes_dreieck_zeiger_alloc (double *a, long n)
{
   /***********************************************************/
   /* eine Matrix mit Indexbereich i = 1, ....... n (Zeilen)  */
   /*                              j =     i, ... n (Spalten) */
   /* allokieren                                              */
   /* Hier wird nur der Vektor von Zeigern allokiert und      */
   /* gesetzt. Der erste Zeiger m [1] zeigt auf den           */
   /* mitgegebenen Vektor a, so dass m [1][1] = a [1].        */
   /***********************************************************/

   double **m;
   long i;
   double *b;

   m = (double **) BASIS_MALLOC ((size_t) (n + 1) * sizeof (double *));

   if (m == NULL)
      return NULL;

   b = a;
   m [0] = a;

   #pragma omp parallel for
   for (i = 0; i < n; i ++)
   {
      m [i + 1] = b;
      b += (n - i - 1);
   }

   return m;
}



/**********************************************************************/
/* Matrix mit Indexbereich                                            */
/*               i = indexing, ... num_rows + indexing - 1 (Zeilen),  */
/*               j =        i, ... num_rows + indexing - 1 (Spalten)  */
/* allokieren                                                         */
/* Hier wird nur der Vektor von Zeigern auf die Zeilen allokiert und  */
/* gesetzt. indexing kann 0 oder 1 sein.                              */
/* Der erste Zeiger m [indexing] zeigt auf den mitgegebenen Vektor a, */
/* so dass m [indexing][indexing] = a [indexing].                     */
/*                                                                    */
/* Folgende Typen sind moeglich:  D  -- double                        */
/*                                F  -- float                         */
/*                                I  -- int                           */
/*                                S  -- short                         */
/*                                L  -- long                          */
/*                                C  -- char                          */
/*                                U  -- unsigned char                 */
/*                                                                    */
/* Diese Version ist experimentell und noch nicht genuegend           */
/* getestet. Falls sie nicht gut funktioniert, kann man auf die       */
/* kommentierte Version in speiorg.c zurueckgreifen.                  */
/**********************************************************************/

void **upper_triangle_pointer_alloc (void *a,
                                     long num_rows,
                                     char type,
                                     short indexing)
{
   size_t el;
   long j;
   long end_row;
   long inc_start;
   char *acurr;
   void **m;


   el = element_length (type);

   if (indexing < 0 || indexing > 1)
      return NULL;

   end_row = num_rows + indexing;


   /*******************************************************************/
   /* Das Inkrement von einem Zeiger auf eine Zeile zur naechsten ist */
   /* schon am Anfang eins kleiner als die Anzahl der Zeilen: Die     */
   /* Die erste Zeile hat num_rows Elemente, dann kommt aber im Falle */
   /* indexing = 1 schon m [2][2], es muss also m [2][0] dasselbe     */
   /* Element sein wie m [1][num_rows - 1].                           */
   /*******************************************************************/

   inc_start = (num_rows - 1) * (long)el;

   acurr = (char *) a;

   m = BASIS_MALLOC ((size_t) (num_rows + indexing) * sizeof (void *));

   if (m == NULL)
      return NULL;

   if (indexing == 1)
      m [0] = a;
   //#pragma omp parallel for
   for (j = indexing; j < end_row; j ++, inc_start -= (long)el)
   {
      m [j] = (void *) acurr;
      acurr += inc_start;
   }

   return m;
}




void **upper_triangle_all_alloc (long num_rows,
                                 char type,
                                 short indexing)
{

   void *a;
   long ges_laenge;

   ges_laenge = (num_rows + 1) * num_rows;
   ges_laenge = ges_laenge / 2 + indexing;

   a = vector_alloc (ges_laenge, type);

   if (a == NULL)
      return NULL;

   return upper_triangle_pointer_alloc (a, num_rows, type, indexing);
}




void dmatrix_zeiger_free (double **m)
{
   /***********************************************************/
   /* den Vektor mit Zeigern auf die Zeilen der Matrix        */
   /* freigeben (Gegenstueck zu dmatrix_zeiger_alloc und zu   */
   /* dmatrix_zeiger_alloc_0)                                 */
   /***********************************************************/

   BASIS_FREE (m);
   return;
}




void matrix_pointer_free (void **m)
{
   /***********************************************************/
   /* den Vektor mit Zeigern auf die Zeilen der Matrix        */
   /* freigeben (Gegenstueck zu matrix_pointer_alloc)         */
   /***********************************************************/

   free (m);
   return;
}




#if 0
float **fmatrix_all_alloc (long zn, long sn)
{
   /**************************************************************/
   /* eine Matrix mit Indexbereich 1, ... zn (Zeilen)            */
   /*                              1, ... sn (Spalten)           */
   /* allokieren                                                 */
   /* Hier werden der float-Vektor a fuer die Matrixelemente     */
   /* (gesamte Groesse der Matrix) und ein Vektor m von Zeigern  */
   /* allokiert. Der float Vektor a wird initialisiert, die      */
   /* Elemente des Zeiger-Vektors m werden auf die "Zeilen-      */
   /* anfaenge" (entsprechende Elemente von a) gesetzt, so dass  */
   /* m [1][1] = a [1].                                          */
   /**************************************************************/

   float **m;
   float *a;
   long ges_laenge;

   ges_laenge = zn * sn;

   a = fvector_alloc (ges_laenge);

   if (a == NULL)
      return NULL;

   m = fmatrix_zeiger_alloc (a, zn, sn);

   return m;
}




float **fmatrix_all_alloc_1 (long zn, long sn)
{
   /**************************************************************/
   /* eine Matrix mit Indexbereich 1, ... zn (Zeilen)            */
   /*                              1, ... sn (Spalten)           */
   /* allokieren                                                 */
   /* Hier werden der float-Vektor a fuer die Matrixelemente     */
   /* (gesamte Groesse der Matrix) und ein Vektor m von Zeigern  */
   /* allokiert. Der float Vektor a wird initialisiert, die      */
   /* Elemente des Zeiger-Vektors m werden auf die "Zeilen-      */
   /* anfaenge" (entsprechende Elemente von a) gesetzt, so dass  */
   /* m [1][1] = a [1].                                          */
   /**************************************************************/

   float **m;
   float *a;
   long ges_laenge;

   ges_laenge = zn * sn;

   a = fvector_alloc (ges_laenge);

   if (a == NULL)
      return NULL;

   m = fmatrix_zeiger_alloc (a, zn, sn);

   return m;
}
#endif



/**********************************************************************/
/* Double-Matrix mit Indexbereich                                     */
/*                   indexing, ... num_rows + indexing - 1 (Zeilen),  */
/*                   indexing, ... num_cols + indexing - 1 (Spalten)  */
/* allokieren                                                         */
/* Hier werden der double-Vektor a fuer die Matrixelemente (gesamte   */
/* Groesse der Matrix) und ein Vektor m von Zeigern auf die Zeilen    */
/* der Matrix allokiert. Der double-Vektor a wird initialisiert, die  */
/* Elemente des Zeiger-Vektors m werden auf die "Zeilenanfaenge"      */
/* (also die entsprechenden Elemente von a) gesetzt. indexing gibt    */
/* an, ob die Indizierung der Matrix bei 0 oder bei 1 beginnen soll.  */
/* indexing kann 0 oder 1 sein.                                       */
/* Der erste Zeiger m [indexing] zeigt auf den mitgegebenen Vektor a, */
/* so dass m [indexing][indexing] = a [indexing].                     */
/**********************************************************************/

double **dmatrix_all_alloc (long zn, long sn, short indexing)
{
   double **m;
   double *a;
   long ges_laenge;

   ges_laenge = zn * sn;

   a = dvector_alloc (ges_laenge);

   if (a == NULL)
      return NULL;

   m = dmatrix_pointer_alloc (a, zn, sn, indexing);

   return m;
}




double **basis_dmatrix_all_alloc (long zn, long sn)
{
   /***********************************************************/
   /* eine Matrix mit Indexbereich 1, ... zn (Zeilen)         */
   /*                              1, ... sn (Spalten)        */
   /* allokieren                                              */
   /* Hier werden der double-Vektor a fuer die Matrixelemente */
   /* (gesamte Groesse der Matrix) und ein Vektor m von       */
   /* Zeigern allokiert. Der double Vektor a wird             */
   /* initialisiert, die Elemente des zeiger-Vektors m werden */
   /* auf die "Zeilenanfaenge" (entsprechende Elemente von a) */
   /* gesetzt, so dass m [1][1] = a [1].                      */
   /***********************************************************/

   double **m;
   double *a;
   long ges_laenge;

   ges_laenge = zn * sn;

   a = basis_dvector_alloc (ges_laenge);

   if (a == NULL)
      return NULL;

   m = basis_dmatrix_zeiger_alloc (a, zn, sn);

   return m;
}




double **doberes_dreieck_all_alloc (long n)
{
   /******************************************************************/
   /* obere Dreiecks Matrix mit Indexbereich i = 1, ... n (Zeilen)   */
   /*                                        j = i, ... n (Spalten)  */
   /* allokieren                                                     */
   /* Eine eigene Funktion zum Freigeben braucht es nicht, man kann  */
   /* die Funktion dmatrix_all_free benutzen                         */
   /******************************************************************/

   double **m;
   double *a;
   long ges_laenge;

   ges_laenge = n * (n + 1) / 2;

   a = dvector_alloc (ges_laenge);

   if (a == NULL)
      return NULL;

   m = doberes_dreieck_zeiger_alloc (a, n);

   return m;
}




double **doberes_dreieck_all_alloc_1 (long n)
{
   /******************************************************************/
   /* obere Dreiecks Matrix mit Indexbereich i = 1, ... n (Zeilen)   */
   /*                                        j = i, ... n (Spalten)  */
   /* allokieren                                                     */
   /* Eine eigene Funktion zum Freigeben braucht es nicht, man kann  */
   /* die Funktion dmatrix_all_free benutzen                         */
   /******************************************************************/

   double **m;
   double *a;
   long ges_laenge;

   ges_laenge = n * (n + 1) / 2;

   a = dvector_alloc (ges_laenge);

   if (a == NULL)
      return NULL;

   m = doberes_dreieck_zeiger_alloc (a, n);

   return m;
}



#if 0
int **imatrix_all_alloc (long zn, long sn)
{
   /***********************************************************/
   /* eine Matrix mit Indexbereich 1, ... zn (Zeilen)         */
   /*                              1, ... sn (Spalten)        */
   /* allokieren                                              */
   /* Hier werden der Int-Vektor a fuer die Matrixelemente    */
   /* (gesamte Groesse der Matrix) und ein Vektor m von       */
   /* Zeigern allokiert. Der Int-Vektor a wird initialisiert, */
   /* die Elemente des Zeiger-Vektors m werden auf die        */
   /* "Zeilenanfaenge" (entsprechende Elemente von a)         */
   /* gesetzt, so dass m [1][1] = a [1].                      */
   /***********************************************************/

   int **m;
   int *a;
   long ges_laenge;

   ges_laenge = zn * sn;

   a = ivector_alloc (ges_laenge);

   if (a == NULL)
      return NULL;

   m = imatrix_zeiger_alloc (a, zn, sn);

   return m;
}




int **imatrix_all_alloc_1 (long zn, long sn)
{
   /***********************************************************/
   /* eine Matrix mit Indexbereich 1, ... zn (Zeilen)         */
   /*                              1, ... sn (Spalten)        */
   /* allokieren                                              */
   /* Hier werden der Int-Vektor a fuer die Matrixelemente    */
   /* (gesamte Groesse der Matrix) und ein Vektor m von       */
   /* Zeigern allokiert. Der Int-Vektor a wird initialisiert, */
   /* die Elemente des Zeiger-Vektors m werden auf die        */
   /* "Zeilenanfaenge" (entsprechende Elemente von a)         */
   /* gesetzt, so dass m [1][1] = a [1].                      */
   /***********************************************************/

   int **m;
   int *a;
   long ges_laenge;

   ges_laenge = zn * sn;

   a = ivector_alloc (ges_laenge);

   if (a == NULL)
      return NULL;

   m = imatrix_zeiger_alloc (a, zn, sn);

   return m;
}
#endif



void dmatrix_all_free (double **m)
{
   /***********************************************************/
   /* den Vektor mit Zeigern auf die Zeilen der Matrix        */
   /* sowie den Vektor der Matrixelemente freigeben           */
   /* (Gegenstueck zu dmatrix_all_alloc)                      */
   /***********************************************************/

   double *a;

   a = m [0];

   BASIS_FREE (m);
   BASIS_FREE (a);

   return;
}




void fmatrix_all_free (float **m)
{
   /***********************************************************/
   /* den Vektor mit Zeigern auf die Zeilen der Matrix        */
   /* sowie den Vektor der Matrixelemente freigeben           */
   /* (Gegenstück zufdmatrix_all_allc))                       */
   /***********************************************************/

   float *a;

   a = m [0];

   BASIS_FREE (m);
   BASIS_FREE (a);

   return;
}




void smatrix_all_free (short **m)
{
   /***********************************************************/
   /* den Vektor mit Zeigern auf die Zeilen der Matrix        */
   /* sowie den Vektor der Matrixelemente freigeben           */
   /* (Gegenstück zufdmatrix_all_allc))                       */
   /***********************************************************/

   short *a;

   a = m [0];

   BASIS_FREE (m);
   BASIS_FREE (a);

   return;
}




void matrix_all_free (void **m)
{
   /***********************************************************/
   /* den Vektor mit Zeigern auf die Zeilen der Matrix        */
   /* sowie den Vektor der Matrixelemente freigeben           */
   /* (Gegenstueck zu matrix_all_alloc)                       */
   /***********************************************************/

   void *a;

   a = m [0];

   BASIS_FREE (m);
   BASIS_FREE (a);

   return;
}




int legendre_dreieck_alloc (int maxdeg, double ***lf_anker)
{
   /**********************************************************/
   /* Speicherplatz fuer Legendre-Funktionen bis zum         */
   /* Entwicklungsgrad maxdeg allokieren und initialisieren  */
   /* Es wird sowohl der double-Vektor der Elemente          */
   /* allokiert und mit Nullen vorbesetzt als auch der       */
   /* Zeigervektor (Zeiger auf die Zeilen der Legendre-      */
   /* Matrix (Grade)) allokiert und gesetzt.                 */
   /**********************************************************/


   double *lf_kette;
   double *pd;
   double **lf_matrix;
   int grad;
   long anz_func;


   lf_kette = NULL;
   lf_matrix = NULL;

   anz_func = (maxdeg + 1) * (maxdeg + 2) / 2;

   lf_kette = (double *) calloc (anz_func, sizeof (double));

   if (lf_kette == NULL)
      return 8;

   lf_matrix = (double **) malloc (
                                  (maxdeg + 1) * sizeof (double *));

   if (lf_matrix == NULL)
   {
      free (lf_kette);
      return 12;
   }
   //#pragma omp parallel for
   for (grad = 0, pd = lf_kette; grad <= maxdeg;  grad ++, pd += grad)
   {
      lf_matrix [grad] = pd;
   }

   *lf_anker = lf_matrix;

   return 0;
}




double **legendre_dreieck_alloc_neu (int maxdeg)
{
   /**********************************************************/
   /* Speicherplatz fuer Legendre-Funktionen bis zum         */
   /* Entwicklungsgrad maxdeg allokieren und initialisieren  */
   /* Es wird sowohl der double-Vektor der Elemente          */
   /* allokiert und mit Nullen vorbesetzt als auch der       */
   /* Zeigervektor (Zeiger auf die Zeilen der Legendre-      */
   /* Matrix (Grade)) allokiert und gesetzt.                 */
   /**********************************************************/


   double *lf_kette;
   double *pd;
   double **lf_matrix;
   int grad;
   long anz_func;


   lf_kette = NULL;
   lf_matrix = NULL;

   anz_func = (maxdeg + 1) * (maxdeg + 2) / 2;

   lf_kette = (double *) calloc (anz_func, sizeof (double));

   if (lf_kette == NULL)
      return NULL;

   lf_matrix = (double **) malloc (
                               (maxdeg + 1) * sizeof (double *));

   if (lf_matrix == NULL)
   {
      free (lf_kette);
      return NULL;
   }
   //#pragma omp parallel for
   for (grad = 0, pd = lf_kette; grad <= maxdeg; grad ++, pd += grad)
   {
      lf_matrix [grad] = pd;
   }

   return lf_matrix;
}




int legendre_dreieck_free (double ***lf_anker)
{
   /***********************************************************/
   /* den Vektor mit Zeigern auf die Zeilen der Legendre-     */
   /* Matrix sowie den Vektor der Matrixelemente freigeben    */
   /* (Gegenstück zu legendre_dreieck_alloc)                  */
   /***********************************************************/


   double *pd;
   double **lf_matrix;


   lf_matrix = *lf_anker;
   pd = *lf_matrix;

   free (lf_matrix);
   free (pd);

   return 0;
}


