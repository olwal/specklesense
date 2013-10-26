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

#ifndef _ADNS_9500_H_
#define _ADNS_9500_H_

#include <WProgram.h>
#include <SPI.h>
#include <stdlib.h>

#include "adns.h"

namespace ADNS9500_CONSTANTS
{
	// ADNS-9500 Registers (names match datasheet)
	const byte Product_ID						= 0x00; // R = 0x33
	const byte Revision_ID						= 0x01; // R = 0x03
	const byte Motion							= 0x02; // R = 0x00
	const byte Delta_X_L						= 0x03; // R = 0x00
	const byte Delta_X_H						= 0x04; // R = 0x00
	const byte Delta_Y_L						= 0x05; // R = 0x00
	const byte Delta_Y_H						= 0x06; // R = 0x00
	const byte SQUAL							= 0x07; // R = 0x00
	const byte Pixel_Sum						= 0x08; // R = 0x00
	const byte Maximum_Pixel					= 0x09; // R = 0x00
	const byte Minimum_Pixel					= 0x0a; // R = 0x00
	const byte Shutter_Lower					= 0x0b; // R = 0x20
	const byte Shutter_Upper					= 0x0c; // R = 0x4e
	const byte Frame_Period_Lower				= 0x0d; // R = 0xc0
	const byte Frame_Period_Upper				= 0x0e; // R = 0x5d
	const byte Configuration_I					= 0x0f; // R/W = 0x12
	const byte Configuration_II					= 0x10; // R/W = 0x00
	const byte Frame_Capture					= 0x12; // R/W = 0x00
	const byte SROM_Enable						= 0x13; // W = 0x00
	const byte Run_Downshift					= 0x14; // R/W = 0x32
	const byte Rest1_Rate						= 0x15; // R/W = 0x01
	const byte Rest1_Downshift					= 0x16; // R/W = 0x1f
	const byte Rest2_Rate						= 0x17; // R/W = 0x09
	const byte Rest2_Downshift					= 0x18; // R/W = 0xbc
	const byte Rest3_Rate						= 0x19; // R/W = 0x31
	const byte Frame_Period_Max_Bound_Lower		= 0x1a; // R/W = 0xc0
	const byte Frame_Period_Max_Bound_Upper		= 0x1b; // R/W = 0x5d
	const byte Frame_Period_Min_Bound_Lower		= 0x1c; // R/W = 0xa0
	const byte Frame_Period_Min_Bound_Upper		= 0x1d; // R/W = 0x0f
	const byte Shutter_Max_Bound_Lower			= 0x1e; // R/W = 0x20
	const byte Shutter_Max_Bound_Upper			= 0x1f; // R/W = 0x4e
	const byte LASER_CTRL0						= 0x20; // R/W = 0x01
//	const byte = 0x21 - 0x23 Reserved
	const byte Observation						= 0x24; // R/W = 0x00
	const byte Data_Out_Lower					= 0x25; // R = Undefined
	const byte Data_Out_Upper					= 0x26; // R = Undefined
//	const byte = 0x27 - 0x29 Reserved
	const byte SROM_ID							= 0x2a; // R = 0x00
	const byte Lift_Detection_Thr				= 0x2e; // R/W = 0x10
	const byte Conifguration_V					= 0x2f; // R/W = 0x12
//	const byte = 0x30 - 0x38 Reserved
	const byte Conifguration_IV					= 0x39; // R/W = 0x00
	const byte Power_Up_Reset					= 0x3a; // W = NA
	const byte Shutdown							= 0x3b; // W = Undefined
//	const byte = 0x3c - 0x3e Reserved
	const byte Inverse_Product_ID				= 0x3f; // R = 0xcc
//	const byte = 0x40 – 0x4f Reserved
	const byte  Motion_Burst					= 0x50; // R = 0x00
	const byte SROM_Load_Burst					= 0x62; // W = Undefined
	const byte Pixel_Burst						= 0x64; // R = 0x00

	enum { DPI_1000 = 0, DPI_250, DPI_500, DPI_1250, DPI_1500, DPI_1750, DPI_2000 };

}

class ADNS9500 : public ADNS
{

public:
	// --- Exposed variables to store latest read values ---

	// Motion bytes
	byte xl; // X low bytes
	byte yl; // Y low bytes
	byte xh; // X high bytes
	byte yh; // Y high bytes

	// Shutter time
	byte shut_lo; // Shutter time low bytes
	byte shut_hi; // Shutter time high bytes

	// Frame period
	byte frame_period_lo; // Frame period low bytes
	byte frame_period_hi; // Frame period high bytes

	// Constant
	float tbexit;

	// Motion detection
	byte motion;

	// --- Setup and configuration ---

	// Constructor
	ADNS9500();
	
	// Initalize chip
	void init();
	
	// --- Specific read operations ---
	
	// Frame motion detection
	// Returns true if motion has been detected, which means that chip is ready for motion read out
	bool readHasMoved();
	
	// Read motion (16-bit)
	void readMotion(int * x, int * y);

	// Read shutter time
	void readShutter(int * shutter);
	int	readShutter();

	// Start burst reading (selects chip and selects register)
	void startBurst();

	// End burst reading (deselects chip)
	void endBurst();

	// Burst operation, reads all parameters through SPI communication and combines/converts bytes. 
	// Assumes that burst mode has been initialized (startBurst()) and will be ended after this call (endBurst()).
	void readBurstOperation(int * x, int * y,
						byte * surfaceQuality, int * shutter,
						byte * maximumPixel, byte * minimumPixel, byte * pixelSum,
						int * framePeriod, byte * motion, byte * observation
						);

	// Single burst read for all parameters. 
	// Essentially: startBurst(); readBurstOperation(...); endBurst();
	void readBurst(int * x, int * y,
						byte * surfaceQuality, int * shutter,
						byte * maximumPixel, byte * minimumPixel, byte * pixelSum,
						int * framePeriod, byte * motion, byte * observation
						);

	// --- Frame capture, 30x30 = 900 pixels, see ADNS-9500 datasheet, page 19 ---
	
	// Prepare frame capture
	void startFrameCapture();

	// End frame capture
	// TODO: Check that this resets the chip back, so that motion detection works again
	void endFrameCapture();
	
	// Frame capture operations, which can be called repeatedly to stream frames
	int	frameCaptureOperation(byte * buffer);

	// --- Utility methods ---
	
	// Wait for a single frame
	// TODO: Fix hardcoded assumption of 11750 fps?
	void waitFrame();

	// Reset the chip
	void writeResetChip();

	// Enable/Disable laser
	// TODO: Missing code to actually write the register? 
	void writeLaserForceDisabled(bool forceDisabled);	
};

#endif
