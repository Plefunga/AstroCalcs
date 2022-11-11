# AstroCalcs
An Arduino library of astronomy calculations that is still in development, and hence contains many bugs.
There is a python version of this which is used for testing. It also has a bug in it where it refracts the right ascention around 180 degrees every now and then.

Current functions:
- jdify(year, month, day, hour, minute, second): convert date into julian date
- bigt(julian date): a helper function for gmst
- gmst(T, julian date): calculate grenwich mean sidereal time
- lst(year, month, day, hour, minute, second, longitude): calculate the local sidereal time
- abs_sort(dictionary): sorts the stars list by the absolute value of the magnitude
- hmsify(right ascention, declination): converts the decimal right ascention or hour angle and declination to hours minutes seconds / degrees minutes seconds. TODO make it two functions, one for hour-minute-second and one for degree-minute-second
- altaz(hour angle, declination, latitude): convert hour angle and declination to altitude and azimuth (used for refraction)
- precess(right ascention, declination, year, right ascention seconds, declination seconds): does a rough conversion between J2000 coordinates to on-date coordinates.
- refract(altitude): adjusts an altitude to refract
- radec(altitude, azimuth, latitude, local sidereal time): calculates the right ascention and declination.
- getHA(): returns Hour Angle.
- setRADEC(ra, dec): sets RA/DEC.
- getDec(): gets declination.


TODO: 
- better decscriptions of functions, and combine some of the functions and simplify them as much as possible. 
- debug the library.
- Aberration and Nuation calculations.
