/**
 * @file Position.h
 * @brief A class for position data
 * @author Nathan Carter
 */


/*
    Copyright (C) 2024 Nathan Carter

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    To read the full terms and conditions, see https://www.gnu.org/licenses/.
*/

#ifndef POSITION_H

#define POSITION_H 1
#include "Arduino.h"

/// a macro for converting a value in seconds to the increment in LST, instead of recalculating it from the ground up.
#define SECONDS_TO_LST(x) ((x)*0.00423611)

/**
 * Position Class
 * 
 * Useful for storing data about a position.
 * 
 * This class has functions that are used in converting from right ascention and declination to altitude and azimuth.
 * It also contains functions for parsing ra/dec and alt/az decimals to degrees/minutes/seconds or hour/minutes/seconds for display.
 */
class Position
{
    public:
        /// @brief hour angle
        double ha;

        /// @brief right ascention
        double ra;

        /// @brief declination
        double dec;

        /// @brief local sidereal time
        double LST;

        /// @brief latitude
        double latitude;

        /// @brief altitude
        double alt;

        /// @brief azimuth
        double az;

        /**
         * Constructor
         * 
         * sets all values to zero
         */
        Position()
        {
            this->ra = 0.0;
            this->dec = 0.0;
            this->ha = 0.0;
            this->latitude = 0.0;
            this->LST = 0.0;
            this->alt = 0.0;
            this->az = 0.0;
        }

        /**
         * Constructor
         * 
         * Calculates the hour angle, altitude and azimuth of the target
         * 
         * @param right_ascention sets the R.A. of the position
         * @param declination sets the declination of the position
         * @param offset an amount of seconds to offset the LST by instead of recalculating it
         * @param latitude the observers latitude
         * @param LST the observers LST
         */
        Position(double right_ascention, double declination, int offset, double latitude, double LST)
        {
            this->ra = (limit(right_ascention));
            this->dec = (declination);

            LST = limit(LST + SECONDS_TO_LST(offset));

            this->ha = (limit(LST - right_ascention));

            this->LST = (LST);
            this->latitude = (latitude);

            this->altAz();
        }

        /**
         * Constructor
         * 
         * Calculates the hour angle, altitude and azimuth of the target
         * 
         * @param r the right ascention
         * @param d the declination
         * @param latitude the observer's latitude
         * @param LST the local sidereal time
         */
        Position(double r, double d, double latitude, double LST)
        {
            this->ra = (limit(r));
            this->dec = (d);

            this->ha = (limit(LST - r));
            this->LST = limit(LST);
            this->latitude = (latitude);

            //this->az = 0.0;
            //this->alt = 0.0;
            this->altAz();
        }

        /**
         * calculates the hours:minutes:seconds of the current position
         * 
         * @see decDMS does the same, but with degrees:minutes:seconds for declination
         * 
         * @param h an integer pointer where the hours will be set
         * @param m an integer pointer where the minutes will be set
         * @param s a double pointer to where the seconds will be set
         * @returns acts in place on data
         */
        void raHMS(int* h, int* m, double* s)
        {
            double hours = (this->ra / 360.0) * 24.0;
            int rah = floor((this->ra / 360.0) * 24.0);
            double minutes = (hours - (double)rah) * 60.0;
            int ram = floor(minutes);
            double ras = (minutes - (double)ram) * 60.0;

            *h = rah;
            *m = ram;
            *s = ras;
        }

        /**
         * Calculates the degrees:minutes:seconds of the current declination
         * 
         * @see raHMS
         * 
         * @param d an integer pointer to where the degrees will be set
         * @param m an integer pointer to where the minutes will be set
         * @param s a double pointer to where the seconds will be set
         * @returns acts in place on data
         */
        void decDMS(int* d, int* m, double* s)
        {
            if(this->dec >=0.0)
            {
                int decd = floor(this->dec);
                double minutes = (this->dec - (double)decd) * 60.0;
                int decm = floor(minutes);
                double decs = (minutes - (double)decm) * 60.0;
                *d = decd;
                *m = decm;
                *s = decs;
            }
            else
            {
                int decd = ceil(this->dec);
                double minutes = (this->dec - (double)decd) * 60.0;
                int decm = ceil(minutes);
                double decs = (minutes - (double)decm) * 60.0;
                *d = decd;
                *m = decm;
                *s = decs;
            }
        }

        /**
         * Computes the degrees:minutes:seconds for the current altitude.
         * 
         * Run the `altAz()` function before doing this.
         * 
         * @param d an integer pointer where the degrees will be set
         * @param m an integer pointer where the minutes will be set
         * @param s a double pointer to where the seconds will be set
         * @returns acts in place on data
         */
        void altDMS(int* d, int* m, double* s)
        {
            if(this->alt >=0.0)
            {
                int altd = floor(this->alt);
                double minutes = (this->alt - (double)altd) * 60.0;
                int altm = floor(minutes);
                double alts = (minutes - (double)altm) * 60.0;
                *d = altd;
                *m = altm;
                *s = alts;
            }
            else
            {
                int altd = ceil(this->alt);
                double minutes = (this->alt - (double)altd) * 60.0;
                int altm = ceil(minutes);
                double alts = (minutes - (double)altm) * 60.0;
                *d = altd;
                *m = altm;
                *s = alts;
            }
        }

        /**
         * Computes the degrees:minutes:seconds for the current azimuth.
         * 
         * Run the `altAz()` function before doing this.
         * 
         * @param d an integer pointer where the degrees will be set
         * @param m an integer pointer where the minutes will be set
         * @param s a double pointer to where the seconds will be set
         * @returns acts in place on data
         */
        void azDMS(int* d, int* m, double* s)
        {
            int azd = floor(this->az);
            double minutes = (this->az - (double)azd) * 60.0;
            int azm = floor(minutes);
            double azs = (minutes - (double)azm) * 60.0;
            *d = azd;
            *m = azm;
            *s = azs;
        }

        /**
         * Calculates the alt/az for the current hour angle / declination of the target.
         * Call this as soon as possible after the LST is set.
         * 
         * To retrieve the result, access position.alt and position.az.
         * 
         * @returns acts in place on the data in the class
         */
        void altAz()
        {
            double h = radians(this->ha);
            double d = radians(this->dec);
            double l = radians(this->latitude);
            double azimuth = PI + atan2(sin(h), cos(h) * sin(l) - tan(d) * cos(l));
            double altitude = asin(sin(l) * sin(d) + cos(h) * cos(d) * cos(l));
            this->az = limit(degrees(azimuth));
            this->alt = degrees(altitude);
        }

        /**
         * Increments the local sidereal time by some amount of seconds `t`.
         * @param t the amount of seconds to increment the local sidereal time by
         * 
         * @returns acts in place on data in the class.
         */
        void increment(double t)
        {
            this->LST = limit(this->LST + SECONDS_TO_LST(t));
            this->ha = (limit(LST - this->ra));
            this->altAz();
        }

        /**
         * Updates the local sidereal time in the position. Useful in calculating the alt/az.
         * @param LST the local sidereal time.
         * @returns acts in place on data in class.
         */
        void updateLST(double LST)
        {
            this->LST = limit(LST);
            this->ha = (limit(this->LST - this->ra));
            this->altAz();
        }

    private:
        /**
         * Restricts a value into the interval [0, 360)
         * @param x a double of degrees.
         * @returns x, but in the interval [0,360)
         */
        double limit(double x)
        {
            int diff = floor(x/360);
            
            for(int i = 0; i < diff; i++)
            {
                x -= 360.0;
            }

            diff = ceil(-x/360);

            for(int i = 0; i < diff; i++)
            {
                x += 360.0;
            }
            return x;
        }
};

#endif