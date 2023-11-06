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

#include "Arduino.h"
#include "AstroCalcs.h"

/**
 * Constructor for the position class
 * @param right_ascention the right ascention
 * @param declination the declination
 * @param offset the offset from these coordinates (only used occasionally)
 * @param latitude the latitude which these positions apply
 * @param LST the local sidereal time
 * TODO: work in radians
*/
Position::Position(double right_ascention, double declination, int offset, double latitude, double LST)
{
    this->ra = right_ascention;
    this->dec = declination;

    this->ra = limit(this->ra);

    LST = limit(LST + SECONDS_TO_LST(offset));

    this->ha = limit(LST - this->ra);

    double h = radians(this->ha);
	double d = radians(this->dec);
	double l = radians(latitude);
	double azimuth = atan2(sin(h), cos(h) * sin(l) - tan(d) * cos(l));
	double altitude = asin(sin(l) * sin(d) + cos(h) * cos(d) * cos(l));
	this->az = limit(-degrees(azimuth));
	this->alt = -degrees(altitude);

    double hours = (this->ra / 360) * 24;
	this->rah = floor((this->ra / 360) * 24);
	double minutes = (hours - this->rah) * 60;
	this->ram = floor(minutes);
	this->ras = (minutes - this->ram) * 60;

    if(this->dec >=0.0){
        this->decd = floor(this->dec);
        minutes = (this->dec - this->decd) * 60;
        this->decm = floor(minutes);
        this->decs = (minutes - this->decm) * 60;
    }
    else{
        this->decd = ceil(this->dec);
        minutes = (this->dec - this->decd) * 60;
        this->decm = ceil(minutes);
        this->decs = (minutes - this->decm) * 60;
    }

    this->azd = floor(this->az);
    minutes = (this->az - this->azd) * 60;
    this->azm = floor(minutes);
    this->azs = (minutes - this->azm) * 60;

    if(this->alt >=0.0){
        this->altd = floor(this->alt);
        minutes = (this->alt - this->altd) * 60;
        this->altm = floor(minutes);
        this->alts = (minutes - this->altm) * 60;
    }
    else{
        this->altd = ceil(this->alt);
        minutes = (this->alt - this->altd) * 60;
        this->altm = ceil(minutes);
        this->alts = (minutes - this->altm) * 60;
    }
    
}

/**
 * Position::limit
 * @param x the coordinate
 * @return x, but between 0 and 360 degrees
*/
double Position::limit(double x)
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

/**
 * Astrocalcs constructor
 * @param longitude the longitude of the telescope
 * @param latitude the latitude of the telescope
*/
AstroCalcs::AstroCalcs(double longitude, double latitude){
    _latitude = latitude;
    _longitude = longitude;
}

/* 
    jdify
    Calculates and sets the julian date
    updates _JD
*/
void AstroCalcs::jdify(){
  int A = floor(_Y/100);
  int B = floor(A/4);
  int C = floor(2-A+B);
  int E = floor(365.25*(_Y+4716));
  int F = floor(30.6001*(_M+1));
  _JD = double(C) + double(_D) + double(E) + double(F) -1524.5 + double(_h)/24.0 + double(_m)/1440.0 + double(_s)/86400.0;
}

/*
    bigt
    calculates the value of T for use in calculating the local sidereal time
    updates _t using _JD
*/
void AstroCalcs::bigt(){
  _t = ((_JD - 2451545.0) / 36525.0);
}

/*
    gmst
    calculates the Greenwich Mean Sidereal Time using the julian date and T
    uses _JD and _t to calculate the GMST
    returns the GMST
*/
double AstroCalcs::gmst(){
  double thetazero = 280.46061837 + 360.98564736629 * (_JD - 2451545.0) + 0.000387933 * (_t*_t) - (_t*_t*_t) / 38710000.0;
  while(thetazero > 360){
    thetazero -= 360.0;
  }
  while(thetazero < 0){
    thetazero += 360.0;
  }
  return thetazero;
}


/*
    lst
    calculates the local sidereal time from longitude and GMST.
*/
void AstroCalcs::lst(){
	double gmstdeg = _h * 15 + _m * 15 / 60 + _s * 15 / 3600;
    jdify();
    bigt();
	double g = gmst();
	double d = gmstdeg - g;
	double L = g + _longitude;
    _LST = L;
    _diff = d;
}

/*
    hmsify
    converts decimal ra into hours/minutes/seconds
*/
void AstroCalcs::hmsify(){
    double h = (_ra / 360) * 24;
	_rah = floor((_ra / 360) * 24);
	double m = (h - _rah) * 60;
	_ram = floor(m);
	_ras = (m - _ram) * 60;
}


/*
    dmsify
    converts decimal declination into degrees/minutes/seconds
*/
void AstroCalcs::dmsify()
{
    if(_dec >=0.0){
        _decd = floor(_dec);
        double m = (_dec - _decd) * 60;
        _decm = floor(m);
        _decs = (m - _decm) * 60;
    }
    else{
        _decd = ceil(_dec);
        double m = (_dec - _decd) * 60;
        _decm = ceil(m);
        _decs = (m - _decm) * 60;
    }
}


/*
    altaz
    converts ra/dec into alt/az
*/
void AstroCalcs::altaz()
{
    double h = radians(_ha);
	double d = radians(_dec);
	double l = radians(_latitude);
	double azimuth = atan2(sin(h), cos(h) * sin(l) - tan(d) * cos(l));
	double altitude = asin(sin(l) * sin(d) + cos(h) * cos(d) * cos(l));
	_az = degrees(azimuth);
    if(_az < 0.0){
        _az = _az + 360.0;
    }
	_alt = degrees(altitude);
}

/**
 * AstroCalcs::precess
 * See https://bbastrodesigns.com/lib/coordLib.js line 636
 * Corrects the J2000 coordinate for precession
 * TODO: work in radians
*/
void AstroCalcs::precess()
{
    // make sure the variables are updated
    dmsify();
    hmsify();

    // get the years since 2000
    // TODO: catch errors where year <= 2000
    int t = _Y - 2000;

    // 
	double m = (3.0749 + 0.0000186 * t) * t;
	double n = (20.043 - 0.000085 * t) * t;
	double ns = (1.3362 - 0.0000056 * t) * t;
	double rr = m + ns * sin(radians(_ra)) * tan(radians(_dec));
	double dd = n * cos(radians(_ra));
    setRADEC(_ra + (rr / 3600)*15, _dec + dd/3600);
}


/*
    refract
    corrects an alt/az for refraction
*/
void AstroCalcs::refract(){
	//1.02cot(h+10.3/(h+5.11))
	double R = 1.02 * (1 / tan(radians(_alt + 10.3 / (_alt + 5.11))));
	_alt = _alt + (R / 3600);
}


/*
    radec
    converts an alt/az into ra/dec
*/
void AstroCalcs::radec(){
    double altitude = radians(_alt);
	double azimuth = radians(_az);
    azimuth = PI + azimuth;
    if(azimuth > 2*PI){
        azimuth = azimuth - 2*PI;
    }
	double l = radians(_latitude);
	double t = radians(_LST);
	double d = asin(sin(altitude) * sin(l) + cos(altitude) * cos(azimuth) * cos(l));
	double h = asin(sin(azimuth) * cos(altitude) / cos(d));
	d = degrees(d);
	double r = degrees(t - (PI*2 - h));
    if(r < 0.0){
        r = r + 360.0;
    }
    setRADEC(r, d);
}


//public functions

/**
 * AstroCalcs::updateTime
 * updates the time in the library (also updating LST)
 * @param Y year
 * @param M month
 * @param D day 
 * @param h hour
 * @param m minute
 * @param s second
*/
void AstroCalcs::updateTime(int Y, int M, int D, int h, int m, int s){
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
}

/*
    timeVars
    returns the time as a formatted string
*/
String AstroCalcs::timeVars(){
    return String(_Y)+"|"+String(_M)+"|"+String(_D)+"|"+ String(_h)+"|"+ String(_m)+"|"+ String(_s)+"|"+ String(_LST)+"|"+ String(_diff)+"|"+ String(_JD)+"|"+String(_t);
}

/*
    updateTimeManual
    :: String s :: the time formatted string
    updates the time from a time formatted string
*/
void AstroCalcs::updateTimeManual(String s){
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
    j = i+1;
    i = s.indexOf("|", j);
    _JD = s.substring(j, i).toFloat();
    j = i+1;
    i = s.indexOf("|", j);
    _t = s.substring(j, i).toFloat();
}

/*
    calcPosJ2000
    Calculates the on date ra and dec from a J2000 ra and dec, factoring for precession and refraction
*/
void AstroCalcs::calcPosJ2000(double ra, double dec){
    _ra = ra;
    _dec = dec;
    precess();
    _ha = _LST - _ra;
    altaz();
    refract();
    radec();
    _ha = _LST - _ra;
    
}

/*
    radecify
    converts hour/minute/second ra and degree/minute/second declination into decimal ra/dec and updates them.
    :: int rah     :: the right ascention hours
    :: int ram     :: the right astention minutes
    :: double ras  :: the right ascention seconds
    :: int decd    :: the declination degrees
    :: int decm    :: the declination minutes
    :: double decs :: the declination seconds
    updates position variables
*/
void AstroCalcs::radecify(int rah, int ram, double ras, int decd, int decm, double decs){
    _ra = rah * 15 + ram * 15 / 60 + ras * 15 / 3600;
	_dec = decd + decm / 60 + decs / 3600;
    _rah = rah;
    _ram = ram;
    _ras = ras;
    _decd = decd;
    _decm = decm;
    _decs = decs;
} 


/*
    getLST
    returns the Local sidereal time
*/
double AstroCalcs::getLST(){
    return _LST;
}


/*
    setRADEC
    sets the ra and declination, and updates related variables
    :: double ra  :: the right ascention
    :: double dec :: the declination
*/
void AstroCalcs::setRADEC(double ra, double dec){
    _ra = ra;
    _dec = dec;
    _ha = _LST - _ra;
    altaz();
    hmsify();
    dmsify();
}

/*
    getHA
    returns the hour angle of the current target
*/
double AstroCalcs::getHA(){
    _ha = limit(_LST - _ra);
    return _ha;
}


/*
    getDec
    returns the current declination
*/
double AstroCalcs::getDec(){
    return _dec;
}


/*
    setAltAz
    sets the alt and az
    :: double alt :: the altitude
    :: double az  :: the azimuth
    updates position variables
*/
void AstroCalcs::setAltAz(double alt, double az){
    _alt = alt;
    _az = az;
    radec();
}


/*
    getRa
    returns the current right ascention, between 0 and 360 degrees
*/
double AstroCalcs::getRA(){
    _ra = limit(_ra);
    return _ra;
}


/*
    calcRA
    :: double altitude :: the altitude of a celestial object
    :: double azimuth  :: the azimuth of a celestial object
    returns the right ascention of the celestial object
*/
double AstroCalcs::calcRA(double altitude, double azimuth)
{
    azimuth = PI + azimuth;
    if(azimuth > 2*PI){
        azimuth = azimuth - 2*PI;
    }
	double l = radians(_latitude);
	double t = radians(_LST);
	double d = asin(sin(altitude) * sin(l) + cos(altitude) * cos(azimuth) * cos(l));
	double h = asin(sin(azimuth) * cos(altitude) / cos(d));
	//d = degrees(d);
	double r = limit(degrees(t - (PI*2 - h)));
    return r;
}

/*
    calcDec
    :: double altitude :: the altitude of a celestial object
    :: double azimuth  :: the azimuth of a celestial object
    returns the declination of the celestial object
*/
double AstroCalcs::calcDec(double altitude, double azimuth)
{
    azimuth = PI + azimuth;
    if(azimuth > 2*PI){
        azimuth = azimuth - 2*PI;
    }
	double l = radians(_latitude);
	//double t = radians(_LST);
	double d = asin(sin(altitude) * sin(l) + cos(altitude) * cos(azimuth) * cos(l));
	//double h = asin(sin(azimuth) * cos(altitude) / cos(d));
	d = degrees(d);
	/*double r = degrees(t - (PI*2 - h));
    if(r < 0.0){
        r = r + 360.0;
    }*/
    return d;
}

/*
    calcAlt
    :: double right_ascention :: the right ascention of a celestial object
    :: double declination     :: the declination of a celestial object
    returns the altitude of the celestial object above the horizon
*/
double AstroCalcs::calcAlt(double right_ascention, double declination)
{
    double h = radians(_LST - right_ascention);
	double d = radians(declination);
	double l = radians(_latitude);
	double altitude = asin(sin(l) * sin(d) + cos(h) * cos(d) * cos(l));
	_alt = degrees(altitude);
    return altitude;
}

/*
    calcAz
    :: double right_ascention :: the right ascention of a celestial object
    :: double declination     :: the declination of a celestial object
    returns the azimuth of the celestial object
*/
double AstroCalcs::calcAz(double right_ascention, double declination)
{
    double h = radians(_LST - right_ascention);
	double d = radians(declination);
	double l = radians(_latitude);
	double azimuth = atan2(sin(h), cos(h) * sin(l) - tan(d) * cos(l));
	_az = degrees(azimuth);
    if(_az < 0.0){
        _az = _az + 360.0;
    }
    return azimuth;
}

Position AstroCalcs::getPosition(int offset)
{
    return Position(_ra, _dec, offset, _latitude, _LST);
}

double AstroCalcs::limit(double x)
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
