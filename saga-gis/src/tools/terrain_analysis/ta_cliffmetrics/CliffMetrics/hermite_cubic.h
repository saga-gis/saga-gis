/*!
 *
 * \brief Definitions of some routines from the hermite_cubic library
 * \details TODO This is a more detailed description of the hermite_cubic routines.
 * \author John Burkardt
 * \author Modified by David Favis-Mortlock, Andres Payo, Jim Hall
 * \date 2017
 * \copyright GNU Lesser General Public License
 *
 * \file hermitecubic.h
 * \brief Contains definitions of hermite-cubic routines
 *
 */

#ifndef HERMITE_H
  #define HERMITE_H
/*==============================================================================================================================

 This is hermite_cubic.h: part of a C++ library from http://people.sc.fsu.edu/~jburkardt/cpp_src/hermite_cubic/hermite_cubic.html. It demonstrates the use of cubic polynomials in the Hermite form

 r8vec_bracket3() finds the interval containing or nearest a given value
 hermite_cubic_spline_value() evaluates a Hermite cubic spline
 hermite_cubic_value() evaluates a Hermite cubic polynomial

===============================================================================================================================*/
void r8vec_bracket3(int const, double* const, double const, int*);

void hermite_cubic_value(double const, double const, double const, double const, double const, double const, int const, double* const, double* const, double* const, double* const, double* const);

void hermite_cubic_spline_value(int const, double* const, double* const, double* const, int const, double* const, double*, double*, double*, double*);

#endif // HERMITE_H
