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

#include "Arduino.h"
#include "AstroCalcs.h"
#include "Position.h"


AstroCalcs::AstroCalcs(double longitude, double latitude)
{
    _latitude = latitude;
    _longitude = longitude;

    this->curr_pos = Position(0.0, 0.0, latitude, 0.0);
}


/*void AstroCalcs::jdify()
{
  int A = floor(_Y/100);
  int B = floor(A/4);
  int C = floor(2-A+B);
  int E = floor(365.25*(_Y+4716)) - 2451545;
  int F = floor(30.6001*(_M+1));
  _JD = double(C) + double(_D) + double(E) + double(F) -1524.5 + double(_h)/24.0 + double(_m)/1440.0 + double(_s)/86400.0;
}


void AstroCalcs::bigt()
{
  _t = ((_JD/* - 2451545.0*//*) / 36525.0);
}


double AstroCalcs::gmst()
{
  double thetazero = 280.46061837 + 360.98564736629 * (_JD/* - 2451545.0*//*) + 0.000387933 * (_t*_t) - (_t*_t*_t) / 38710000.0;
  while(thetazero > 360){
    thetazero -= 360.0;
  }
  while(thetazero < 0){
    thetazero += 360.0;
  }
  return thetazero;
}*/



void AstroCalcs::lst()
{
    int A = floor(_Y/100);
    int B = floor(A/4);
    int C = floor(2-A+B);
    int E = floor(365.25*(_Y+4716)) - 2451545;
    int F = floor(30.6001*(_M+1));
    double jd = double(C) + double(_D) + double(E) + double(F) -1524.5 + double(_h)/24.0 + double(_m)/1440.0 + double(_s)/86400.0;

    double t = ((jd/* - 2451545.0*/) / 36525.0);
    double thetazero = 280.46061837 + 360.98564736629 * (jd/* - 2451545.0*/) + 0.000387933 * (t*t) - (t*t*t) / 38710000.0;
    while(thetazero > 360){
        thetazero -= 360.0;
    }
    while(thetazero < 0){
        thetazero += 360.0;
    }

	double gmstdeg = _h * 15 + _m * 15 / 60 + _s * 15 / 3600;
    //jdify();
    //bigt();
	//double g = gmst();
	double d = gmstdeg - thetazero;
	double L = thetazero + _longitude;

    while(L > 360.0)
    {
        L -= 360.0;
    }
    while(L < 0.0)
    {
        L += 360.0;
    }
    _LST = L;
    _diff = d;
}


void AstroCalcs::precess()
{
    // some time based variables
    double T = ((double)_Y - 2000.0) / 100.0;
    double M = 307.0 * T;
    double N = 134.0 * T;
    double S = 2004.0 * T;

    // convert to seconds
    double ra_seconds = (3600.0 * curr_pos.ra)/15.0;
    double dec_seconds = 3600.0 * curr_pos.dec;

    // precess
    double precessed_ra_seconds = ra_seconds + M + N * sin(curr_pos.ra * PI/180.0)*tan(curr_pos.dec * PI/180.0);
    double precessed_dec_seconds = dec_seconds + S * cos(curr_pos.ra * PI/180.0);

    // convert back to decimal
    double ra_decimal = (precessed_ra_seconds / 3600.0) * 15.0;
    double dec_decimal = precessed_dec_seconds / 3600.0;

    this->curr_pos = Position(ra_decimal, dec_decimal, this->_latitude, this->_LST);
}

Position AstroCalcs::precess_curr_pos()
{
    // some time based variables
    double T = ((double)_Y - 2000.0) / 100.0;
    double M = 307.0 * T;
    double N = 134.0 * T;
    double S = 2004.0 * T;

    // convert to seconds
    double ra_seconds = (3600.0 * curr_pos.ra)/15.0;
    double dec_seconds = 3600.0 * curr_pos.dec;

    // precess
    double precessed_ra_seconds = ra_seconds + M + N * sin(curr_pos.ra * PI/180.0)*tan(curr_pos.dec * PI/180.0);
    double precessed_dec_seconds = dec_seconds + S * cos(curr_pos.ra * PI/180.0);

    // convert back to decimal
    double ra_decimal = (precessed_ra_seconds / 3600.0) * 15.0;
    double dec_decimal = precessed_dec_seconds / 3600.0;

    Position p = Position(ra_decimal, dec_decimal, this->_latitude, this->curr_pos.LST);
    
    return p;
}


void AstroCalcs::refract()
{
	//1.02cot(h+10.3/(h+5.11))
	double R = 1.02 * (1 / tan(radians(this->curr_pos.alt + 10.3 / (this->curr_pos.alt + 5.11))));
	this->curr_pos.alt = this->curr_pos.alt + (R / 3600.0);
    this->setAltAz(this->curr_pos.alt, this->curr_pos.az);
}


//public functions


void AstroCalcs::updateTime(int Y, int M, int D, int h, int m, int s)
{
    if (M <= 2){
        M = M + 12;
        Y = Y - 1;
    }
    _Y = Y;
    _M = M;
    _D = D;
    _h = h;
    _m = m;
    _s = s;
    lst();

    this->curr_pos.updateLST(this->_LST);
}


String AstroCalcs::timeVars()
{
    return String(_Y)+"|"+String(_M)+"|"+String(_D)+"|"+ String(_h)+"|"+ String(_m)+"|"+ String(_s)+"|"+ String(_LST)+"|"+ String(_diff)/*+"|"+ String(_JD)+"|"+String(_t)*/;
}


void AstroCalcs::updateTimeManual(String s)
{
    int j = 0;
    int i = s.indexOf("|", j);
    _Y = s.substring(j, i).toInt();
    j = i+1;
    i = s.indexOf("|", j);
    _M = s.substring(j, i).toInt();
    j = i+1;
    i = s.indexOf("|", j);
    _D = s.substring(j, i).toInt();
    j = i+1;
    i = s.indexOf("|", j);
    _h = s.substring(j, i).toInt();
    j = i+1;
    i = s.indexOf("|", j);
    _m = s.substring(j, i).toInt();
    j = i+1;
    i = s.indexOf("|", j);
    _s = s.substring(j, i).toInt();
    j = i+1;
    i = s.indexOf("|", j);
    _LST = s.substring(j, i).toFloat();
    j = i+1;
    i = s.indexOf("|", j);
    _diff = s.substring(j, i).toFloat();
    //j = i+1;
    //i = s.indexOf("|", j);
    //_JD = s.substring(j, i).toFloat();
    //j = i+1;
    //i = s.indexOf("|", j);
    //_t = s.substring(j, i).toFloat();

    this->curr_pos.updateLST(this->_LST);
}


void AstroCalcs::calcPosJ2000(double ra, double dec)
{
    this->curr_pos = Position(ra, dec, this->_latitude, this->_LST);
    precess();
    //refract();
    //curr_pos.altAz();
    // might not do refraction
}



void AstroCalcs::setRADEC(double ra, double dec)
{
    this->curr_pos = Position(ra, dec, this->_latitude, this->_LST);
}

void AstroCalcs::setAltAz(double alt, double az)
{
    double altitude = radians(alt);
	double azimuth = radians(az);

    azimuth = PI + azimuth;
    if(azimuth > 2*PI){
        azimuth = azimuth - 2*PI;
    }

	double l = radians(this->_latitude);
	double t = radians(this->_LST);

	double d = asin(sin(altitude) * sin(l) + cos(altitude) * cos(azimuth) * cos(l));
	double h = asin(sin(azimuth) * cos(altitude) / cos(d));

	double dec = degrees(d);
	double ra = degrees(t - (PI*2 - h));

    if(ra < 0.0){
        ra = ra + 360.0;
    }

    this->curr_pos = Position(ra, dec, this->_latitude, this->_LST);
}


double AstroCalcs::getHA()
{
    this->curr_pos.updateLST(this->_LST);
    return this->curr_pos.ha;
}
double AstroCalcs::getRA()
{
    return this->curr_pos.ra;
}
double AstroCalcs::getDec()
{
    return this->curr_pos.dec;
}

double AstroCalcs::getLST()
{
    return _LST;
}
