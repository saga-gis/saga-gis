/*!
 *
 * \file cliffmetrics.cpp
 * \brief The start-up routine for CliffMetrics
 * \details TODO A more detailed description of this routine
 * \author Andres Payo, David Favis-Mortlock, Martin Hurst, Monica Palaseanu-Lovejoy
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
// TODO Get the rest of CliffMetrics working :-)


#include "cliffmetrics.h"
#include "delineation.h"


/*===============================================================================================================================

 CliffMetrics's main function

===============================================================================================================================*/
int main (int argc, char* argv[])
{
   // TODO This is supposed to enable the use of UTF-8 symbols in CliffMetrics output, such as the \u0394 character. But does it work? If not, remove it?
   setlocale(LC_ALL, "en_GB.UTF-8");

   // OK, create a CDelineation objeect (duh!)
   CDelineation* pDelineation	 = new CDelineation;

   // Run the simulation and then check how it ends
   int nRtn = pDelineation->nDoDelineation(argc, argv);
   pDelineation->DoDelineationEnd(nRtn);

   // Then go back to the OS
   return (nRtn);
}
