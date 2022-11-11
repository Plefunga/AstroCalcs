#ifndef AstroCalcs_h
#define AstroCalcs_h
#include "Arduino.h"
class AstroCalcs{
    public:
        AstroCalcs(double longitude, double latitude);
        void updateTime(int Y, int M, int D, int h, int m, int s);
        void calcPosJ2000(double ra, double dec);
        void radecify(int rah, int ram, double ras, int decd, int decm, double decs);
        double getLST();
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
        double _longitude;
        double _latitude;
        int _Y;
        int _M;
        int _D;
        int _h;
        int _m;
        int _s;
        double _t;
        double _diff;
};

#endif