/* -------------------------------------------------------------------------
 *
 *	SpeckleSense / SpeckleEye
 *
 *	SpeckleSense exploits laser speckle sensing for precise, high-speed, low-latency 
 *	motion tracking, which can be applied to a wide range of interaction scenarios 
 * 	and devices. 
 * 
 *	www.specklesense.org
 *
 *	SpeckleEye: Gestural Interaction for Embedded Electronics in Ubiquitous Computing.
 *	Olwal, A., Bardagjy, A., Zizka, J., and Raskar, R.
 *	CHI 2012 Extended Abstracts (SIGCHI Conference on Human Factors in Computing Systems), 
 *	Austin, TX, May 5-10, 2012, pp. 2237-2242.
 * 
 *	SpeckleSense: Fast, Precise, Low-cost and Compact Motion Sensing using Laser Speckle.
 *	Zizka, J., Olwal, A., and Raskar, R.
 *	Proceedings of UIST 2011 (ACM Symposium on User Interface Software and Technology),
 *	Santa Barbara, CA, Oct 16-19, 2011, pp. 489-498.
 *
 *	Copyright (C) 2013, Alex Olwal, www.olwal.com
 *
 *	SpeckleSense / SpeckleEye is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	SpeckleSense / SpeckleEye is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with SpeckleSense / SpeckleEye.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  -------------------------------------------------------------------------
 */

#include "adns3000.h"

using namespace ADNS3000_CONSTANTS;

// Initialize with:
// resolution = { DPI_1000 = 0, DPI_250, DPI_500, DPI_1250, DPI_1500, DPI_1750, DPI_2000 }
// use12bits = 8- or 12-bit motion reporting
void 
ADNS3000::init(int resolution, bool use12Bits)
{
	ADNS::init();
	writeResolution(resolution, use12Bits);
}

// Burst read of relative x/y, surface quality, shutter time, pixel max, and pixel accum
bool
ADNS3000::readBurst(int * x, int * y, 
					byte * squal, int * shut, 
					byte * pix_max, byte * pix_accum)
{
	readBegin(MOTION_BURST); // Prepare for burst read, then read out all values

		// Get relative motion (low bits)
		xl = SPI.transfer(0);
		yl = SPI.transfer(0);

		if (using12Bits) // Additional read out of high bits if 12-bit
			xyh = SPI.transfer(0); // Get high bits for relative motion

		*squal = SPI.transfer(0); // Get Surface quality
		
		shut_hi = SPI.transfer(0); // Get Shutter open time upper byte
		shut_lo = SPI.transfer(0); // Get Shutter open time lower byte

		*pix_max = SPI.transfer(0); // Get Maximum pixel value
		*pix_accum = SPI.transfer(0); // Get Average pixel value

	readEnd();

	// Convert relative motion data from 2's complement to decimal 
	if (using12Bits)
		convert2sComplementToDecimal(xl, yl, xyh, x, y);
	else
	{
		*x = convert2sComplementToDecimal(xl);
		*y = convert2sComplementToDecimal(yl);
	}

	*shut = (shut_hi << 8) | shut_lo; // Fuse low and high bit for shutter

	return true;
}

//22x22 = 484 pixels
//See page 22 in ADNS3000 datasheet
int
ADNS3000::readPixels(byte * buffer)
{	
	int count = 0;

	digitalWrite(SS, LOW ); // Select the chip

		SPI.transfer( PIX_GRAB | 0x80 ); // Select the register (address byte needs MSB = 1 to indicate write)
		SPI.transfer( 1 ); // Writing to PIX_GRAB resets pixel position to 0x0
		delayMicroseconds(tsrad); // Needed? 

		byte * b = buffer;
		
		for (int i = 0; i < 484; i++, b++)
		{
			SPI.transfer( PIX_GRAB ); // Select the register for reading 
			*b = SPI.transfer( 0 ); // Burst data

//			delayMicroseconds(tsrad); //needed?

//			if ( bitRead( *b, 7 ) == 1 ) // Pixel grabber data valid  
//				count++;
/*
			if ( bitRead( *b, 7 ) != 1 ) // Pixel grabber data not valid  
				return i;
*/
//			delayMicroseconds(tsrad); //wait 4 us -> needed? 

			// Can we just keep retrying? (not pretty...)
			while ( bitRead( *b, 7 ) != 1 ) // While Pixel grabber data not valid  
			{	
				SPI.transfer( PIX_GRAB ); // Select the register for reading 
				*b = SPI.transfer( 0 ); // Read data
//				delayMicroseconds(tsrad); //wait 4 us -> needed?

/*				Serial.print(" ");
				Serial.print( count );
				count++;
*/			}

//			Serial.println();

//			count = 0;

			*b = *b & 0x7F; // Get rid of the bit for validity
		}

	digitalWrite(SS, HIGH); // Deselect chip

	return count;
}

// TODO: Look into this -- is this correct?!
void 
ADNS3000::writeResolutionHigh(bool use12Bits)
{
	// Initialize sensor for 12-bit reporting and 2000 dpi
	byte control = 0;

	if (use12Bits)
		bitSet(control, 7); // 0/1 = 8-bit/12-bit motion reporting

	bitSet(control, 5); // 0/1 disable/enable resolution settings

	bitSet(control, 4); // [4:2] = resolution
	bitSet(control, 3); // 0x06 = 2000 dpi
	bitClear(control, 2); 

	write(MOUSE_CTRL, control);  

	using12Bits = use12Bits;
}

// TODO: Look into this -- is this correct?!
// Set tracking resolution
void
ADNS3000::writeResolution(int resolution, bool use12Bits)
{
	byte control = 0;

	if (use12Bits)
		bitSet(control, 7); // 0/1 = 8-bit/12-bit motion reporting

	bitSet(control, 5); // 0/1 disable/enable resolution settings

/*	byte res = 0;
	switch (resolution)
	{
		case DPI_1000:	res = 0;	break;
		case DPI_250:	res = 1;	break;
		case DPI_500:	res = 2;	break;
		case DPI_1250:	res = 3;	break;
		case DPI_1500:	res = 4;	break;
		case DPI_1750:	res = 5;	break;
		case DPI_2000:	res = 6;	break;
	}
*/
	// Only set if in the range DPI_1000 = 0, DPI_2000 = 6, see enums
	if (resolution >= DPI_1000 && resolution <= DPI_2000)
		control |= ( resolution << 2 );

	write(MOUSE_CTRL, control);

	using12Bits = use12Bits;
}

// Checks whether motion was detected in last frame, which means that chip is ready for motion read out
bool
ADNS3000::readHasMoved()
{
	return (read(MOTION_ST) & 0x80) == 0x80;
}

// Read motion data
void
ADNS3000::readMotion(int * x, int * y)
{
	if (using12Bits)
		readMotion12bit(x, y);
	else
		readMotion8bit(x, y);
}

// Read 8-bit motion data (chip must have been set to 8-bit)
void
ADNS3000::readMotion8bit(int * x, int * y)
{
//	xl = read(DELTA_X);
//	yl = read(DELTA_Y);
//	Avoid redundant calls in above two methods, through explicit operations

	digitalWrite(SS, LOW); // Select chip

		SPI.transfer(DELTA_X);		// Write the register
		delayMicroseconds(tsrad);	// Wait
		xl = SPI.transfer(0);		// Read X

		delayMicroseconds(tsrad);	// Wait

		SPI.transfer(DELTA_Y);		// Write the register
		delayMicroseconds(tsrad);	// Wait
		yl = SPI.transfer(0);		// Read y

	digitalWrite(SS, HIGH); // Deselect chip

	*x = convert2sComplementToDecimal(xl);
	*y = convert2sComplementToDecimal(yl);
}

// Read 12-bit motion data (chip must have been set to 12-bit)
void
ADNS3000::readMotion12bit(int * x, int * y)
{
	digitalWrite(SS, LOW); // Select chip

		SPI.transfer(DELTA_X);		// Write the register
		delayMicroseconds(tsrad);	// Wait
		xl = SPI.transfer(0);		// Read x low bits

		delayMicroseconds(tsrad);	// Wait

		SPI.transfer(DELTA_Y);		// Write the register
		delayMicroseconds(tsrad);	// Wait
		yl = SPI.transfer(0);		// Read y low bits

		SPI.transfer(DELTA_XY_HIGH);// Write the register
		delayMicroseconds(tsrad);	// Wait
		xyh = SPI.transfer(0);		// Read x, y high bits

	digitalWrite(SS, HIGH); // Deselect chip

	convert2sComplementToDecimal(xl, yl, xyh, x, y);
}

// Read shutter time
void 
ADNS3000::readShutter(int * shutter)
{
	*shutter = readShutter();
}

// Read shutter time
int
ADNS3000::readShutter()
{
	return ( read(SHUT_HI) << 8 ) | read(SHUT_LO);
}

// Constructor
ADNS3000::ADNS3000() : ADNS(), using12Bits(false)
{
	tsrad = 10;
}
