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

#ifndef _ADNS_H_
#define _ADNS_H_

#include <WProgram.h>
#include <SPI.h>
#include <stdlib.h>

class ADNS
{
public:	

	// Constructor
	ADNS();

	// tsrad constant for needed delay between certain operations, see ADNS data sheet
	unsigned int tsrad;

	// Initializes chip and SPI communication
	virtual void init();
	
	// --- Methods for single read/write ---
	
	// A complete read operation to read out a single value from a register.
	// Includes overhead for selecting/deselecting the chip, so don't use if you expect
	// to do multiple reads in sequence.	
	virtual byte read(byte reg); 
	
	// Complete write operation to write single value to register. 
	// Includes overhead for selecting/deselecting the chip, so don't use if you expect
	// to do multiple writes in sequence.
	virtual void write(byte reg, byte value);
	
	// --- Methods for multiple and burst reads ---
	
	// Start of a read operation from a register. Typically used in the beginning of a
	// burst read, where this command would be folowed by multiple read operations. 
	// Selects chip and specifies the registry to read from. 
	virtual void readBegin(byte reg); 
	
	// Completes a sequence of reads and deselects the chip
	virtual void readEnd(); 

	// Basic read operation, specifying register and assuming that chip is selected.
	// Optional delay in tsrad ms. Useful for executing multiple reads in fast
	// sequence after chip has been selected.
	virtual byte readOperation(byte reg, bool delay = false);
		
	// Operation to read out result from register, assuming that register has been 
	// specified and that chip is selected. Optional delay in tsrad ms. Useful for 
	// burst reads. 
	virtual byte readResult(bool delay = false); // Reads out single result
	
	// --- Static utility methods ---

	// Conversions from 2's complement 
	static int convert2sComplementToDecimal(byte b);
	static int convert2sComplementToDecimal(int b);
	static void convert2sComplementToDecimal(byte xl, byte yl, byte xyh, int * x, int * y);
	static void convert2sComplementToDecimal(byte xl, byte xh, byte yl, byte yh, int * x, int * y);

	// Trivial flip of a bit in a byte
	static void flip(byte * b, int bit);	
};

#endif
