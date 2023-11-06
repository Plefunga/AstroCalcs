# AstroCalcs
An Arduino library of astronomy calculations that is mostly finished.

Current functions:
- jdify(year, month, day, hour, minute, second): convert date into julian date
- bigt(julian date): a helper function for gmst
- gmst(T, julian date): calculate grenwich mean sidereal time
- lst(year, month, day, hour, minute, second, longitude): calculate the local sidereal time
- hmsify(right ascention, declination): converts the decimal right ascention or hour angle and declination to hours minutes seconds / degrees minutes seconds. TODO make it two functions, one for hour-minute-second and one for degree-minute-second
- altaz(hour angle, declination, latitude): convert hour angle and declination to altitude and azimuth (used for refraction)
- precess(right ascention, declination, year, right ascention seconds, declination seconds): does a rough conversion between J2000 coordinates to on-date coordinates.
- refract(altitude): adjusts an altitude to refract
- radec(altitude, azimuth, latitude, local sidereal time): calculates the right ascention and declination.
- getHA(): returns Hour Angle.
- setRADEC(ra, dec): sets RA/DEC.
- getDec(): gets declination.


#TODO: 
- better decscriptions of functions, and combine some of the functions and simplify them as much as possible.
- Extensively test the library
- Aberration and Nuation calculations.

## Other things to do
1. Re-make the position class to store vars in radians, and ditch the offset thing in the constructor, and instead make it a function (Position::offset(degrees), Position::offset(degrees, minutes, seconds), Position::offset(radians), Position::offset(hours, minutes, seconds), etc.).
This will involve some way of it converting between internal radian units to the external units of both degrees/minutes/seconds (or hours/minutes/seconds) and the decimal degrees. (maybe just have functions to output those when necessary? something like Position::dms(*degrees, *minutes, *seconds))

2. Incorporate the position class into the functions, making them cleaner

3. Create a (private) flip function that will flip the Ra/dec if neccessary (see implementation in Stellarium Interface). This will create a new tuplet of ra/dec. This function will be called at the end of any public AstroCalcs function that modifies a coordinate, and the flipped coordinates will be what is returned.
