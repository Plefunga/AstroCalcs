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

void AstroCalcs::altaz(){
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
    azimuth = PI + azimuth; //was 180 degrees too small
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

String AstroCalcs::timeVars(){
    return String(_Y)+"|"+String(_M)+"|"+String(_D)+"|"+ String(_h)+"|"+ String(_m)+"|"+ String(_s)+"|"+ String(_LST)+"|"+ String(_diff)+"|"+ String(_JD)+"|"+String(_t);
}

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
    _ha = _LST - _ra;
    altaz();
    hmsify();
    dmsify();
}

double AstroCalcs::getHA(){
    _ha = _LST - _ra;
    return _ha;
}

double AstroCalcs::getDec(){
    return _dec;
}

void AstroCalcs::setAltAz(double alt, double az){
    _alt = alt;
    _az = az;
    radec();
}

double AstroCalcs::getRA(){
    if(_ra < 0){
        _ra = _ra + 360;
    }
    return _ra;
}


/* ALIGNMENT in psuedocode
array getAlignStarPos(int rank){
    _stars = stars.json.parse(); //stars is an array of [id, name, ra, dec, raint]
    _star = _stars.getbeststar(rank); //star is an array of [name, ra, dec]. rank is to get a different star if u dont like the best one.
    calcPosJ200(star[ra], star[dec]); //do required calcs
    return [star[name], star[ra], star[dec]] //maybe return ha instead of ra? go check what I normally do.
}

array getbeststar(int rank){
    iterate over array to make another array of just distance from meridian
    sort array by distance from meridian
    match the rank index star to the star from the database using id
    convert the ra and dec strings into integers
    check if the star is above the horizon. return an error code if below horizon.
    return the star as [name, ra, dec] array. (also check whether ha is a better unit).
}

Alignment process:
user presses align button (connected to SI)
SI calls getAlignStarPos(0)
AC returns polaralign star
SI displays:
|align to [star]?|
|Ra: xx:xx:xx|
|dec: xxx*xx:xx|
|press align to continue or [button] to choose other star
user presees align:
SI sends alignment move command to MD (not yet implemented)
MD moves
SI waits for confirmation of moves (not yet implemented)
SI then displays:
|do not use joystick to slew to star|
|use physical knobs on mount|
|once done press align|
|press back to choose another star|
user presses align:
time is updated and all variables are updated as well
motorpositions are updates so that the 0 position is the home position, and the variables on MD are also updated.
MD sends a complete command (not implemented)
SI displays
|done!|

user presses back:
rank is incremented by one
process starts again.
*/