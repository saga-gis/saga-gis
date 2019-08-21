/*!
 *
 * \file random_numbers.cpp
 * \brief Random number routines
 * \details TODO A more detailed description of these routines.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 */

/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include <cmath>

#include "cliffmetrics.h"
#include "delineation.h"


/*===============================================================================================================================

 CliffMetrics's two random number generators are maximally equidistributed combined Tausworthe generators. The code is modified from taus.c in gsl-1.9, the GNU Scientific Library. The sequence is

 x_n = (s1_n ^ s2_n ^ s3_n)

 s1_{n+1} = (((s1_n & 4294967294) <<12) ^ (((s1_n <<13) ^ s1_n) >> 19))
 s2_{n+1} = (((s2_n & 4294967288) << 4) ^ (((s2_n << 2) ^ s2_n) >> 25))
 s3_{n+1} = (((s3_n & 4294967280) <<17) ^ (((s3_n << 3) ^ s3_n) >> 11))

 computed modulo 2^32. In the three formulae above '^' means exclusive-or (C-notation), not exponentiation. Note that the algorithm relies on the properties of 32-bit integers (it is formally defined on bit-vectors of length 32). GNU have added a bitmask to make it also work on 64 bit machines.

 Each generator is initialized with

 s1_1 .. s3_1 = s_n MOD m

 where s_n = (69069 * s_{n-1}) mod 2^32, and s_0 = s is the user-supplied seed.

 The theoretical value of x_{10007} is 2733957125. The subscript 10007 means (1) seed the generator with s=1 (2) do six warm-up timesteps, (3) then do 10000 actual timesteps. The period of this generator is about 2^88 i.e. 309485009821345068724781056

 From: P. L'Ecuyer, "Maximally equidistributed combined Tausworthe generators", Mathematics of Computation 65, 213 (1996), 203-213. This is available on the net from L'Ecuyer's home page:

 http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme.ps
 ftp://ftp.iro.umontreal.ca/pub/simulation/lecuyer/papers/tausme.ps

 Update: April 2002

 There is an erratum in the paper "Tables of Maximally Equidistributed Combined LFSR  Generators", Mathematics of Computation, 68, 225 (1999), 261-269. See http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme2.ps

 "... the k_j most significant bits of z_j must be non-zero, for each j. (Note: this  restriction also applies to the computer code given in [4], but was mistakenly not mentioned in that paper.)"

 This affects the seeding procedure by imposing the requirement s1 > 1, s2 > 7, s3 > 15.

 Update: November 2002

 There was a bug in the correction to the seeding procedure for s2. It affected the following seeds 254679140 1264751179 1519430319 2274823218 2529502358 3284895257 3539574397 (s2 < 8).

===============================================================================================================================*/
unsigned long CDelineation::ulGetRand0(void)
{
   // ulGetRand0() generates random numbers
   m_ulRState[0].s1 = ulGetTausworthe(m_ulRState[0].s1, 13, 19, 4294967294ul, 12);
   m_ulRState[0].s2 = ulGetTausworthe(m_ulRState[0].s2, 2, 25, 4294967288ul, 4);
   m_ulRState[0].s3 = ulGetTausworthe(m_ulRState[0].s3, 3, 11, 4294967280ul, 17);

   return (m_ulRState[0].s1 ^ m_ulRState[0].s2 ^ m_ulRState[0].s3);
}


unsigned long CDelineation::ulGetRand1(void)
{
   // ulGetRand1() generates random numbers
   m_ulRState[1].s1 = ulGetTausworthe(m_ulRState[1].s1, 13, 19, 4294967294ul, 12);
   m_ulRState[1].s2 = ulGetTausworthe(m_ulRState[1].s2, 2, 25, 4294967288ul, 4);
   m_ulRState[1].s3 = ulGetTausworthe(m_ulRState[1].s3, 3, 11, 4294967280ul, 17);

   return (m_ulRState[1].s1 ^ m_ulRState[1].s2 ^ m_ulRState[1].s3);
}


/*===============================================================================================================================

 Each of these initializes one of the Tausworthe generators

===============================================================================================================================*/
void CDelineation::InitRand0(unsigned long ulSeed)
{
   if (0 == ulSeed)
      ulSeed = 1;                                        // default seed is 1

   m_ulRState[0].s1 = ulGetLCG(ulSeed);
   if (m_ulRState[0].s1 < 2)
      m_ulRState[0].s1 += 2UL;

   m_ulRState[0].s2 = ulGetLCG(m_ulRState[0].s1);
   if (m_ulRState[0].s2 < 8)
      m_ulRState[0].s2 += 8UL;

   m_ulRState[0].s3 = ulGetLCG(m_ulRState[0].s2);
   if (m_ulRState[0].s3 < 8)
      m_ulRState[0].s3 += 16UL;

   // Warm it up
   ulGetRand0();
   ulGetRand0();
   ulGetRand0();
   ulGetRand0();
   ulGetRand0();
   ulGetRand0();

   return;
}


void CDelineation::InitRand1(unsigned long ulSeed)
{
   if (0 == ulSeed)
      ulSeed = 1;                                        // default seed is 1

   m_ulRState[1].s1 = ulGetLCG(ulSeed);
   if (m_ulRState[1].s1 < 2)
      m_ulRState[1].s1 += 2UL;

   m_ulRState[1].s2 = ulGetLCG(m_ulRState[1].s1);
   if (m_ulRState[1].s2 < 8)
      m_ulRState[1].s2 += 8UL;

   m_ulRState[1].s3 = ulGetLCG(m_ulRState[1].s2);
   if (m_ulRState[1].s3 < 8)
      m_ulRState[1].s3 += 16UL;

   // Warm it up
   ulGetRand1();
   ulGetRand1();
   ulGetRand1();
   ulGetRand1();
   ulGetRand1();
   ulGetRand1();

   return;
}


/*===============================================================================================================================

 Called by the RandSet routines

===============================================================================================================================*/
unsigned long CDelineation::ulGetLCG(unsigned long const ulN)
{
   return ((69069 * ulN) & MASK);
}


/*===============================================================================================================================

 Using ulGetRand0(), randomly samples from a unit Gaussian (normal) distribution, is routine gasdev.c in:

 Press, W.H., Teukolsky, S.A., Vetterling, W.T. and Flannery, B.P. (1992). Numerical Recipes in C (Second Edition), Cambridge University Press, Cambridge. 994 pp.

===============================================================================================================================*/
double CDelineation::dGetRand0Gaussian(void)
{
   static int snSet = 0;
   static double sdGset;
   double dRet;

   if (0 == snSet)                              // we don't have an extra deviate handy, so
   {
      double dFac, dRsq, dV1, dV2;

      do
      {
         // Pick two uniform numbers in the square extending from -1 to +1 in each direction, see if they are in the unit circle
         dV1 = 2 * dGetRand0d1()-1;             // uses ulGetRand0()
         dV2 = 2 * dGetRand0d1()-1;             // ditto
         dRsq = dV1 * dV1 + dV2 * dV2;
      }
      while (dRsq >= 1 || 0 == dRsq);           // if they are not, try again

      dFac = sqrt(-2 * log(dRsq)/dRsq);

      // Now make the Box-Muller transformation to get two normal deviates, return one and save the other for next time
      sdGset = dV1 * dFac;
      snSet = 1;                                // set flag
      dRet = dV2 * dFac;
   }
   else
   {
      snSet = 0;                                // we have an extra deviate handy so unset the flag and return it
      dRet = sdGset;
   }

   return (dRet);
}


/*=========================================================================================================================================

 Randomly swaps the elements of an integer array, so that on output nArray is a random permutation of its value on input. From p145 of:

 Knuth, D.E. (1997). The Art of Computer Programming. Volume 2. Seminumerical Algorithms (Third Edition), Addison-Wesley Longman, Reading MA. 762 pp.

=========================================================================================================================================*/
void CDelineation::Rand1Shuffle(int* nArray, int nLen)
{
   nLen--;
   while (nLen > 0)
   {
      unsigned int n1 = nGetRand1To(nLen);      // uses Rand1()
      unsigned int nTmp = nArray[n1];
      nArray[n1] = nArray[nLen];
      nArray[nLen--] = nTmp;
   }
}

#ifdef RANDCHECK
/*===============================================================================================================================

 Outputs random numbers for checking

===============================================================================================================================*/
void CDelineation::CheckRand(void) const
{
/*
   // Have already done 6 warm-up timesteps, plus one to write to .out file
   for (unsigned long k = 6; k < 10005; k++)
   {
      unsigned long n = ulGetRand0();
      if (k > 9990)
         LogStream << k << "\t" << n << endl;
   }
*/

/*
   for (unsigned long k = 0; k < 100000; k++)
   {
      // This is for tests using ENT, must also change LogStream to open in binary mode (see run.cpp)
      LogStream << static_cast<unsigned char>(nGetRand0To(256));        // uses ulGetRand0()
   }
   return RTN_OK;
*/

   /*
   for (unsigned long k = 0; k < 65536; k++)          // max number that Excel will read
   {
      static unsigned long sulr = 0;
      LogStream << sulr << "\t";
      sulr = ulGetRand0();
      LogStream << sulr << endl;

//      LogStream << dGetRand0d1() << endl;           // uses GetRand0()
//      int x = nGetRand0To(65535);          // ditto
//      int y = nGetRand0To(65535);          // ditto
//      int x = nGetRand0To(150);                     // ditto
//      int y = nGetRand0To(100);                     // ditto
//      LogStream << x << '\t' << y << endl;
   }
*/
}
#endif


/*==============================================================================================================================

 Deterministically (i.e. using a known Z value) returns a probability from a cumulative unit Gaussian (normal) distribution. A numerical approximation to the normal distribution is used, this is from Abramowitz and Stegun's "Handbook of Mathematical Functions", Dover Publications, 1965 (originally published by the US National Bureau of Standards, 1964)

==============================================================================================================================*/
// double CDelineation::dGetCGaussianPDF(double const dZ)
// {
//    double const b1 =  0.31938153;
//    double const b2 = -0.356563782;
//    double const b3 =  1.781477937;
//    double const b4 = -1.821255978;
//    double const b5 =  1.330274429;
//    double const p  =  0.2316419;
//    double const c2 =  0.3989423;
//
//    if (dZ > 6)
//       return (1);             // more than 6 SDs into the rh tail, just return 1
//
//    if (dZ < -6)
//       return (0);             // more than 6 SDs into the lh tail, just return 0
//
//    double a = tAbs(dZ);
//    double t = 1 / (1 + a * p);
//    double b = c2 * exp((-dZ) * (dZ/2));
//    double dn = ((((b5 * t + b4) * t + b3) * t + b2) * t + b1) * t;
//    dn = 1 - b * dn;
//
//    return ((dZ < 0) ? 1 - dn : dn);
// }
