#include "Arduino.h"
#include "AstroCalcs.h"

AstroCalcs::AstroCalcs(double longitude, double latitude){
    _latitude = latitude;
    _longitude = longitude;
}

//helpers
void AstroCalcs::jdify(){
  int A = floor(_Y/100);
  int B = floor(A/4);
  int C = floor(2-A+B);
  int E = floor(365.25*(_Y+4716));
  int F = floor(30.6001*(_M+1));
  _JD = double(C) + double(_D) + double(E) + double(F) -1524.5 + double(_h)/24.0 + double(_m)/1440.0 + double(_s)/86400.0;
}

void AstroCalcs::bigt(){
  _t = ((_JD - 2451545.0) / 36525.0);
}

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

void AstroCalcs::hmsify(){
    double h = (_ra / 360) * 24;
	_rah = floor((_ra / 360) * 24);
	double m = (h - _rah) * 60;
	_ram = floor(m);
	_ras = (m - _ram) * 60;
}

void AstroCalcs::dmsify(){
    _decd = floor(_dec);
	double m = (_dec - _decd) * 60;
	_decm = floor(m);
	_decs = floor((m - _decm) * 60);
}

void AstroCalcs::altaz(){
    double h = radians(_ha);
	double d = radians(_dec);
	double l = radians(_latitude);
	double azimuth = PI + atan2(sin(h), cos(h) * sin(l) - tan(d) * cos(l));
	double altitude = asin(sin(l) * sin(d) + cos(h) * cos(d) * cos(l));
	_az = degrees(azimuth);
	_alt = degrees(altitude);
}

void AstroCalcs::precess(){
    dmsify();
    hmsify();
    int t = _Y - 2000;
	double m = (3.0749 + 0.0000186 * t) * t;
	double n = (20.043 - 0.000085 * t) * t;
	double ns = (1.3362 - 0.0000056 * t) * t;
	double rr = m + ns * sin(radians(_ra)) * tan(radians(_dec));
	double dd = n * cos(radians(_ra));
    setRADEC(_ra + (rr / 3600)*15, _dec + dd/3600);
}

void AstroCalcs::refract(){
	//1.02cot(h+10.3/(h+5.11))
	double R = 1.02 * (1 / tan(radians(_alt + 10.3 / (_alt + 5.11))));
	_alt = _alt + (R / 3600);
}

void AstroCalcs::radec(){
    double altitude = radians(_alt);
	double azimuth = radians(_az);
	double l = radians(_latitude);
	double t = radians(_LST);
	double d = asin(sin(altitude) * sin(l) + cos(altitude) * cos(azimuth) * cos(l));
	double h = asin(sin(azimuth) * cos(altitude) / cos(d));
	d = degrees(d);
	double r = degrees(t - (PI*2 - h));
    setRADEC(r, d);
}


//ones that will be used by user
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

double AstroCalcs::getLST(){
    return _LST;
}

void AstroCalcs::setRADEC(double ra, double dec){
    _ra = ra;
    _dec = dec;
    altaz();
    hmsify();
    dmsify();
    _ha = _LST - _ra;
}

double AstroCalcs::getHA(){
    _ha = _LST - _ra;
    return _ha;
}

double AstroCalcs::getDec(){
    return _dec;
}