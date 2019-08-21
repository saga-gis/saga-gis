/*!
 *
 * \class CCellLandform
 * \brief Class used to represent cell landform objects
 * \details TODO This is a more detailed description of the CCCellLandform class.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file cell_landform.h
 * \brief Contains CCellLandform definitions
 *
 */

#ifndef CELL_LANDFORM_H
#define CELL_LANDFORM_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
class CCellLandform
{
private:
   int
      m_nCategory,
      m_nSubCategory,
      m_nCoast,
      m_nPointOnCoast;

   double
      m_dAccumWaveEnergy;

      union
      {
         struct
         {
            int m_nDummy;
         }
         m_sBeachData;

         struct
         {
            double
               m_dNotchBaseElev,
               m_dNotchOverhang,
               m_dRemaining;
         }
         m_sCliffData;

      }
      m_uLFData;

public:
   CCellLandform();
   ~CCellLandform(void);

   void SetLFCategory(int const);
   int nGetLFCategory(void) const;
   void SetLFSubCategory(int const);
   int nGetLFSubCategory(void) const;
   void SetCoast(int const);
   int nGetCoast(void) const;
   void SetPointOnCoast(int const);
   int nGetPointOnCoast(void) const;
   void SetAccumWaveEnergy(double const);
   double dGetAccumWaveEnergy(void) const;
   void SetCliffNotchBaseElev(double const);
   double dGetCliffNotchBaseElev(void) const;
   void SetCliffNotchOverhang(double const);
   double dGetCliffNotchOverhang(void) const;
   void SetCliffRemaining(double const);
   double dGetCliffRemaining(void) const;
};
#endif // CELL_LANDFORM_H
