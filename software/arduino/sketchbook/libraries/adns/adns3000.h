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

#ifndef _ADNS_3000_H_
#define _ADNS_3000_H_

#include <WProgram.h>
#include <SPI.h>
#include <stdlib.h>

#include "adns.h"

namespace ADNS3000_CONSTANTS
{
	// ADNS-3000 Registers (names match datasheet)
	const byte PROD_ID          = 0x00; // Product ID
	const byte REV_ID           = 0x01; // Revision ID
	const byte MOTION_ST        = 0x02; // Motion Status
	const byte DELTA_X          = 0x03; // Lower byte of Delta Y
	const byte DELTA_Y          = 0x04; // Lower byte of Delta X
	const byte SQUAL            = 0x05; // Surface quality
	const byte SHUT_HI          = 0x06; // Shutter open time upper byte
	const byte SHUT_LO          = 0x07; // Shutter open time lower byte

	const byte PIX_MAX          = 0x08; // Maximum pixel value
	const byte PIX_ACCUM        = 0x09; // Average pixel value
	const byte PIX_MIN          = 0x0a; // Minimum pixel value
	const byte PIX_GRAB         = 0x0b; // Pixel grabber
	const byte DELTA_XY_HIGH    = 0x0c; // Upper 4 bits of Delta x and y 
	const byte MOUSE_CTRL       = 0x0d; // Mouse control
	const byte RUN_DOWNSHIFT    = 0x0e; // Run to rest1 time
	const byte REST1_PERIOD     = 0x0f; // Rest 1 period

	const byte REST1_DOWNSHIFT  = 0x10; // Rest 1 to rest 2 time
	const byte REST2_PERIOD     = 0x11; // Rest 2 period
	const byte REST2_DOWNSHIFT  = 0x12; // Rest 2 to rest 3 time
	const byte REST3_PERIOD     = 0x13; // Rest 3 period
	const byte PERFORMANCE      = 0x22; // Performance
	const byte RESET            = 0x3A; // Reset
	const byte NOT_REV_ID       = 0x3F; // Inverted revision ID
	const byte LED_CTRL         = 0x40; // LED control

	const byte MOTION_CTRL      = 0x41; // Motion control
	const byte BURST_READ_FIRST = 0x42; // Burst read starting register
	const byte REST_MODE_CONFIG = 0x45; // Rest mode configuration
	const byte MOTION_BURST     = 0x63; // Burst read

	// Tracking resolution modes
	enum { DPI_1000 = 0, DPI_250, DPI_500, DPI_1250, DPI_1500, DPI_1750, DPI_2000 };
}

class ADNS3000 : public ADNS
{	
	// Controls whether we will be using 8- or 12-bit motion data
	bool using12Bits; 

	// Methods to read out motion depending on 8- or 12-bit motion mode
	void readMotion8bit(int * x, int * y);
	void readMotion12bit(int * x, int * y);

public:

	// Exposed variables to store latest read values
	byte xl; // Low x motion byte
	byte yl; // Low y motion byte
	byte xyh; // High xy motion bytes
	byte shut_lo; // Low shutter byte
	byte shut_hi; // High shutter byte
	
	// --- Setup and configuration ---

	// Constructor
	ADNS3000();
	
	// Initialize with:
	// resolution = { DPI_1000 = 0, DPI_250, DPI_500, DPI_1250, DPI_1500, DPI_1750, DPI_2000 }
	// use12bits = 8- or 12-bit motion reporting
	void	init(int resolution = ADNS3000_CONSTANTS::DPI_2000, bool use12Bits = true);
	
	// Configure tracking resolution (DPI) and motion bits (8 or 12 bit mode)
	void writeResolutionHigh(bool use12Bits = true);
	void writeResolution(int resolution = ADNS3000_CONSTANTS::DPI_2000, bool use12Bits = true);
	
	// --- Specific read operations ---
	
	// Frame motion detection
	// Returns true if motion has been detected, which means that chip is ready for motion read out
	bool readHasMoved();
	
	// Read out full pixel 22x22 = 484 pixel buffer (NB! Slow on ADNS3000)
	int	readPixels(byte * buffer);
	
	// Read motion (will choose proper method based on how use12bits is set) 
	void readMotion(int * x, int * y);
	
	// Read shutter speed
	void readShutter(int * shutter); 
	int	readShutter();
	
	// Burst read of relative x/y, surface quality, shutter time, 
	// pixel max, and pixel accum
	bool readBurst(int * x, int * y, byte * squal, int * shut, byte * pix_max, byte * pix_accum); 
};

#endif
