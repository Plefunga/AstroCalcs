/**
 * @file AstroCalcs.h
 * @brief A library for astronomical calculations
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

#ifndef ASTROCALCS_H

#define ASTROCALCS_H 1
#include "Arduino.h"
#include "Position.h"


/**
 * A class that allows J2000 right ascention and declination to be calculated into a position in the sky, correcting for refraction and precession.
 * 
 * Many of the functions in this class were assisted by Mel Bartel's calculators that were used to make an amateur telescope.
 * @see Mel Bartels's calculators at https://www.bbastrodesigns.com/tm.html#myCalculators
 */
class AstroCalcs
{
    public:
        //double _JD;
        /// the current position
        Position curr_pos;

        /**
         * Astrocalcs constructor
         * @param longitude the longitude of the telescope
         * @param latitude the latitude of the telescope
         */
        AstroCalcs(double longitude, double latitude);

        /**
         * updates the time in the library (also updating LST)
         * 
         * @param Y year
         * @param M month
         * @param D day 
         * @param h hour
         * @param m minute
         * @param s second
         */
        void updateTime(int Y, int M, int D, int h, int m, int s);


        /**
         * Calculates the JNOW right ascention and declination given a J2000 right ascention and declination, factoring for precession and refraction.
         * 
         * @param ra the J2000 right ascention
         * @param dec the J2000 declination
         * @returns acts in place on data in the class
         */
        void calcPosJ2000(double ra, double dec);
        
        /**
         * Returns the current LST
         * 
         * @returns the current LST as it is set in the library
         */
        double getLST();

        /**
         * Returns the current time-related variables as a string.
         * This can then be used to set up another instance of astrocalcs with these variables
         * to avoid recalculation of variables on boards with less power
         * 
         * @see lst()
         * @see bigt()
         * @see jdify()
         * @see updateTimeManual
         * 
         * @returns a string formatted as `Year|Month|Day|hour|minute|second|LST|diff|JD|t`
         */
        String timeVars();

        /**
         * Sets the time-related variables from a formatted string.
         * 
         * The string is formatted as:
         * `Year|Month|Day|hour|minute|second|LST|diff|JD|t`
         * 
         * This can be generated from `timeVars()`
         * @see timeVars()
         * 
         * @param s the formatted string to extract the time variables from
         * @returns acts in place on data in the class
         */
        void updateTimeManual(String s);
    

        /**
         * @param ra the right ascention
         * @param dec the declination
         * @result sets the right ascention and declination of the AstroCalcs library
         */
        void setRADEC(double ra, double dec);

        /**
         * returns the hour angle of the current target
         * 
         * @returns the hour angle of the current target
         */
        double getHA();

        /**
         * returns the right ascention
         * 
         * @returns the right ascention of the current target
         */
        double getRA();

        /**
         * returns the current declination
         * 
         * @returns the declination of the current target
         */
        double getDec();
        
        /**
         * Sets the altitude and the azimuth of the target, which is then converted to ra/dec
         * 
         * @param alt the altitude
         * @param az the azimuth
         * @returns sets the alt and az for the library, which then will convert into ra/dec internally
         */
        void setAltAz(double alt, double az);

        /**
         * Precess the current target position
         * 
         * @returns a new position that is precessed
         */
        Position precess_curr_pos();
    
    private:        
        /**
         * Calculates and sets the Julian date from the date provided in the constructor
         * 
         * @returns acts in place on variables in the class
         */
        //void jdify();

        /**
         * Calculates the value of T for use in calculating the local sidereal time.
         * 
         * @returns acts in place on data in the class
         */
        //void bigt();
        
        /**
         * Calculates the Greenwich Mean Sidereal Time using the Julian date and T
         * 
         * @returns the GMST
         */
        //double gmst();

        /**
         * Calculates the local sidereal time from the longitude and GMST
         * 
         * @returns acts in place on data in the class
         */
        void lst();

        /**
         * @see Alan Gilmore (University of Cantebury NZ) spreadsheet on precession
         * I believe it implements https://bbastrodesigns.com/lib/coordLib.js line 636
         * Corrects the J2000 coordinate for precession
         */
        void precess();
        

        /**
         * Corrects for refraction
         * 
         * @returns acts in place on data
         */
        void refract();

        /// @brief Year
        int _Y;

        /// @brief Month
        int _M;

        /// @brief Day
        int _D;

        /// @brief hour
        int _h;

        /// @brief minute
        int _m;

        /// @brief second
        int _s;

        /// @brief "T" calculated in `bigt()`
        //double _t;

        /// @brief diff between local sidereal time and grenwich mean sidereal time
        double _diff;

        /// @brief Local sidereal time
        double _LST;

        /// @brief Julian Date
        //double _JD;

        /// @brief Longitude
        double _longitude;
        
        /// @breif Latitude
        double _latitude;
};

#endif
