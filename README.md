# AstroCalcs
A (to be) Arduino library of astronomy calculations
Currently only a python script as I figure it out. Some of them have been converted to C++ for arduino, but not all of them.

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


TODO: better decscriptions of functions, and combine some of the functions and simplify them as much as possible. Also need to make an actual arduino library, which is kind of imporant considering that it is the main feature of this repo.
