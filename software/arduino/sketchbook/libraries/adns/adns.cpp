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

#include "adns.h"

// Initializes chip and SPI communication
void 
ADNS::init()
{
	// Initialize SPI
	SPI.begin();
	SPI.setDataMode(SPI_MODE3);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV32); // can run up to DIV16 (1MHz)

//	writeResolution(resolution, use12Bits);
}

// Write value to a register
void 
ADNS::write(byte reg, byte value)
{
	// To write to a register, the ADNS expects two bytes:
	// the address byte and the data byte. 
	// The first byte has 1 as its MSB to indicate a write sequence
	reg = reg | 0x80;

	digitalWrite(SS, LOW); 	// Select chip
	
	SPI.transfer(reg);	// Specify register
	SPI.transfer(value); 	// Write value

	digitalWrite(SS, HIGH); // Deselect chip
}

// Basic read operation, specifying register and assuming that chip is selected.
// Optional delay in tsrad ms.
// Useful for executing multiple reads in fast sequence after chip has been selected.
byte
ADNS::readOperation(byte reg, bool delay)
{
	if (delay)
		delayMicroseconds(tsrad);

	SPI.transfer(reg);

	return SPI.transfer(0x00);
}

// Operation to read out result from register, assuming that register has been specified and that chip is selected. Optional delay in tsrad ms. Useful for burst reads. 
byte
ADNS::readResult(bool delay)
{
	if (delay)
		delayMicroseconds(tsrad);

	return SPI.transfer(0x00);
}

// Start of a read operation from a register. Typically used in the beginning of a burst read, where this command would be folowed by multiple read operations. 
void
ADNS::readBegin(byte reg)
{
	// To read from a register, the ADNS expects a register address reg,
	// the first byte has a 0 as its MSB to indicate a read sequence

	// Select the chip
	digitalWrite(SS, LOW);

	// Write the register
	SPI.transfer(reg);

	// Might need some NOPs here for tsrad 4us delay
	delayMicroseconds(tsrad); // This function is supposedly awful, look into replacing
}

// A complete read operation to read out a single value from a register.
// Includes overhead for selecting/deselecting the chip, so don't use if you expect
// to do multiple reads in sequence.
byte 
ADNS::read(byte reg)
{
	readBegin(reg); // Select chip, specify register
	byte result = SPI.transfer(0x00); // Read a byte
	readEnd(); // Deselect chip
	
	return(result); // Return the result
}

// End of a read operation, which deselects the chip.
void
ADNS::readEnd()
{
	digitalWrite(SS, HIGH); // Deselect the chip
}

// Static
// Invert one bit in byte
void
ADNS::flip(byte * b, int bit)
{
	bitWrite( *b, bit, bitRead(*b, bit) == 1 ? 0 : 1 );
}

// Static
// Convert a byte in 2's complement to decimal
int 
ADNS::convert2sComplementToDecimal(byte b)
{
/*	if (bitRead(b, 7) == 1) //negative
	{
		b = ~b;
		b += 1;
		return -b;
	}
	else
		return b;
*/
	return bitRead(b, 7) ? -( (byte)~b + 1 ) : b;
}

// Static
// Convert 2's complement integer to decimal
int
ADNS::convert2sComplementToDecimal(int b)
{
	return bitRead(b, 15) ? -( (int)~b + 1 ) : b;
}

// Static
// Convert two 12-bit values in 2's complement to two integers.
//
// TODO: Should be written more efficiently, now verbose to make it easy to follow.
// Addresses a special case when 2 values * 12 bits = 24 bits are distributed across
// 3 bytes. 
// x = xyh[7:4] + xl[7:0]
// y = xyh[3:0] + yl[7:0]
void
ADNS::convert2sComplementToDecimal(byte xl, byte yl, byte xyh, int * x, int * y)
{
	//x = xyh[7:4] + x[7:0]
	//y = xyh[3:0] + y[7:0]

	if ( bitRead(xyh, 7) ) //sign bit set
	{
		*x = 0;

		for (int i = 7; i >= 4; i--) //invert top bits
			bitWrite( xyh, i, !bitRead(xyh, i) );
		
		xl = ~xl; //invert lower bits
		*x = ( (xyh & 0xF0) << 4 ) | xl; //fuse them
		*x += 1; 
		*x = *x & 0x0FFF; //in case of overflow after addition -- needed?
		*x = -*x;
	}
	else
		*x = ( (xyh & 0xF0) << 4 ) | xl; //fuse them


	if ( bitRead(xyh, 3) ) //sign bit set
	{
		*y = 0;

		for (int i = 3; i >= 0; i--) //invert top bits
			bitWrite( xyh, i, !bitRead(xyh, i) );
		
		yl = ~yl; //invert lower bits
		*y = ( (xyh & 0x0F) << 8 ) | yl; //fuse them
		*y += 1; 
		*y = *y & 0x0FFF; //in case of overflow after addition -- needed? 
		*y = -*y;
	}
	else
		*y = ( (xyh & 0x0F) << 8 ) | yl; //fuse them
}

// Static
// Convert two 16-bit values in 2's complement to two integers.
void
ADNS::convert2sComplementToDecimal(byte xl, byte xh, byte yl, byte yh, int * x, int * y)
{
	*x = ( xh << 8 ) | xl; //fuse them
	*y = ( yh << 8 ) | yl; //fuse them

	*x = convert2sComplementToDecimal(*x);
	*y = convert2sComplementToDecimal(*y);
}

// Constructor
ADNS::ADNS() : tsrad(10)
{

}
