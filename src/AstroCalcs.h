/*
    Copyright (C) 2023 Nathan Carter

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    To read the full terms and conditions, see https://www.gnu.org/licenses/.
*/

#ifndef ASTROCALCS_H

#define ASTROCALCS_H 1
#include "Arduino.h"

#define SECONDS_TO_LST(x) ((x)*0.00423611)

class Position{
    public:
        Position(double r, double d, int offset, double latitude, double LST);

        double ha;

        double ra;
        int rah;
        int ram;
        double ras;

        double dec;
        int decd;
        int decm;
        int decs;

        double alt;
        int altd;
        int altm;
        double alts;

        double az;
        int azd;
        int azm;
        double azs;
    private:
        double limit(double x);
};

class AstroCalcs{
    public:
        AstroCalcs(double longitude, double latitude);
        void updateTime(int Y, int M, int D, int h, int m, int s);
        void calcPosJ2000(double ra, double dec);
        void radecify(int rah, int ram, double ras, int decd, int decm, double decs);
        double getLST();
        String timeVars();
        void updateTimeManual(String s);
        double _ra;
        double _dec;
        int _rah;
        int _ram;
        double _ras;
        int _decd;
        int _decm;
        double _decs;
        double _ha;
        double _LST;
        double _JD;
        double _alt;
        double _az;
        void setRADEC(double ra, double dec);
        double getHA();
        void refract();
        double getDec();
        void setAltAz(double alt, double az);
        double getRA();
        double calcRA(double altitude, double azimuth);
        double calcDec(double altitude, double azimuth);
        double calcAlt(double right_ascention, double declination);
        double calcAz(double right_ascention, double declination);
        Position getPosition(int offset);
        double _longitude;
        double _latitude;

    private:
        void jdify();
        void bigt();
        double gmst();
        void lst();
        void hmsify();
        void dmsify();
        void altaz();
        void precess();
        void radec();
        int _Y;
        int _M;
        int _D;
        int _h;
        int _m;
        int _s;
        double _t;
        double _diff;
        double limit(double x);
};

#endif
