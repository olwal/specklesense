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

#include "adns9500.h"

using namespace ADNS9500_CONSTANTS;

// Initialization
void 
ADNS9500::init()
{
	ADNS::init();
}

// Checks whether motion was detected in last frame, which means that chip is ready for motion read out
bool
ADNS9500::readHasMoved()
{
	return (read(Motion) & 0x80) == 0x80;
}

// Read motion (16-bit)
void
ADNS9500::readMotion(int * x, int * y)
{
	digitalWrite(SS, LOW); //select chip

		SPI.transfer(Delta_X_L);	// Write the register
		delayMicroseconds(tsrad);	// Wait
		xl = SPI.transfer(0);		// Read X low bytes
		delayMicroseconds(tsrad);	// Wait

		SPI.transfer(Delta_X_H);	// Write the register
		delayMicroseconds(tsrad);	// Wait
		xh = SPI.transfer(0);		// Read X high bytes
		delayMicroseconds(tsrad);	// Wait

		SPI.transfer(Delta_Y_L);	// Write the register
		delayMicroseconds(tsrad);	// Wait
		yl = SPI.transfer(0);		// Read Y low bytes
		delayMicroseconds(tsrad);	// Wait

		SPI.transfer(Delta_Y_H);	// Write the register
		delayMicroseconds(tsrad);	// Wait
		yh = SPI.transfer(0);		// Read Y high bytes
		delayMicroseconds(tsrad);	// Wait

	digitalWrite(SS, HIGH); //deselect chip

	convert2sComplementToDecimal(xl, xh, yl, yh, x, y);
}

/*
From ADNS9500 data sheet, page 18

Procedure to start motion burst:
	1. Lower NCS
	2. Send 0x50 to Motion_Burst register.
	3. Wait for one frame. (This only applicable in Run mode
	   for wakeup but not require for rest mode)
	4. Start reading SPI Data continuously up to 14bytes.
	   Motion burst may be terminated by pulling NCS high
	   for at least t[BEXIT].
	5. To read new motion burst data, repeating from step 1.
	6. Write any value to Motion register (address 0x02) to
	   clear any residual motion.

	BYTE [00] = Motion
	BYTE [01] = Observation
	BYTE [02] = Delta_X_L
	BYTE [03] = Delta_X_H
	BYTE [04] = Delta_Y_L
	BYTE [05] = Delta_Y_H
	BYTE [06] = SQUAL
	BYTE [07] = Pixel_Sum
	BYTE [08] = Maximum_Pixel
	BYTE [09] = Minimum_Pixel
	BYTE [10] = Shutter_Upper
	BYTE [11] = Shutter_Lower
	BYTE [12] = Frame_Period_Upper
	BYTE [13] = Frame_Period_Lower
*/

// Start burst reading (selects chip and selects register)
void
ADNS9500::startBurst()
{
	digitalWrite(SS, LOW);
	readBegin(Motion_Burst);

	waitFrame();
}

// End burst reading (deselects chip)
void
ADNS9500::endBurst()
{
	digitalWrite(SS, HIGH);
	delayMicroseconds(tbexit);
}

// Single burst read for all parameters. 
// Essentially: startBurst(); readBurstOperation(...); endBurst();
void
ADNS9500::readBurst(int * x, int * y,
					byte * surfaceQuality, int * shutter,
					byte * pixelSum, byte * maximumPixel, byte * minimumPixel,
					int * framePeriod, byte * motion, byte * observation
					)
{
	startBurst();

	readBurstOperation(x, y, surfaceQuality, shutter, pixelSum, maximumPixel, minimumPixel,
			  framePeriod, motion, observation);

	endBurst();
}

// Burst operation, reads all parameters through SPI communication and combines/converts bytes. 
// Assumes that burst mode has been initialized (startBurst()) and will be ended after this call (endBurst()).
void
ADNS9500::readBurstOperation(int * x, int * y,
					byte * surfaceQuality, int * shutter,
					byte * pixelSum, byte * maximumPixel, byte * minimumPixel,
					int * framePeriod, byte * motion, byte * observation
					)
{
	*motion = SPI.transfer(0);
	*observation = SPI.transfer(0);
	xl = SPI.transfer(0);
	xh = SPI.transfer(0);
	yl = SPI.transfer(0);
	yh = SPI.transfer(0);
	*surfaceQuality = SPI.transfer(0);
	*pixelSum = SPI.transfer(0);
	*maximumPixel = SPI.transfer(0);
	*minimumPixel = SPI.transfer(0);
	shut_hi = SPI.transfer(0);
	shut_lo = SPI.transfer(0);

	frame_period_hi = SPI.transfer(0);
	frame_period_lo = SPI.transfer(0);

	*shutter = (shut_hi << 8) | shut_lo;
	*framePeriod = (frame_period_hi << 8) | frame_period_lo;

	convert2sComplementToDecimal(xl, xh, yl, yh, x, y);
}

// Read shutter time
void
ADNS9500::readShutter(int * shutter)
{
	*shutter = readShutter();
}

// Read shutter time
int
ADNS9500::readShutter()
{
	return ( read(Shutter_Upper) << 8 ) | read(Shutter_Lower);
}

// Wait for a single frame
// TODO: Fix hardcoded assumption of 11750 fps?
void
ADNS9500::waitFrame()
{
//	wait one frame -- assuming we are running at 11750 fps
//	delayMicroseconds(1000000.0f/11750);
	delayMicroseconds(85);
}

/*  Procedure of Frame Capture:
	1. Reset the chip by writing 0x5a to Power_Up_Reset
	register (address 0x3a).
	2. Enable laser by setting Forced_Disable bit (bit-0) of
	LASER_CTRL0 register to 0.
	3.  Write 0x93 to Frame_Capture register.
	4.  Write 0xc5 to Frame_Capture register.
	5.  Wait for two frames.
	6. Check for i rst pixel by reading bit zero of Motion
	register. If =1, i rst pixel is available.
	7. Continue read from Pixel_Burst register until all 900
	pixels are transferred.
	8.  Continue step 3-7 to capture another frame.
	Note: Manual reset and SROM download are needed after frame capture
	to restore navigation for motion reading.
 */

// Enable/Disable laser
// TODO: Missing code to actually write the register? 
void
ADNS9500::writeLaserForceDisabled(bool forceDisabled)
{
	byte laserCtrl0 = read(LASER_CTRL0);
	
/*	if (!forceDisabled)
		laserCtrl0 &= 0xFE; // Set bit 0 = 0
	else
		laserCtrl0 |= 0x01; // Set bit 0 = 1
*/		

	//bitWrite(laserCtrl0, 0, forceDisabled ? 0 : 1);	
	bitWrite(laserCtrl0, 0, forceDisabled);

	//TODO: Need to test that this is working as intended
	write(LASER_CTRL0, laserCtrl0);
}

// Reset the chip
void
ADNS9500::writeResetChip()
{
	write(Power_Up_Reset, 0x5A);
}

// Start frame capture
void
ADNS9500::startFrameCapture()
{
//	Serial.println("write reset");
	writeResetChip();
//	Serial.println("write laser force disabled");
	writeLaserForceDisabled(false);
}

// End frame capture
// TODO: Check that this resets the chip back, so that motion detection works again
void
ADNS9500::endFrameCapture()
{
	//TODO: Check that this resets the chip back, so that motion detection works again
	writeResetChip();
	writeLaserForceDisabled(true);
}

//30x30 pixels
//See page 19
int
ADNS9500::frameCaptureOperation(byte * buffer)
{
//	Serial.println("write frame capture");
	write(Frame_Capture, 0x93);
	write(Frame_Capture, 0xc5);

//	Serial.println("wait frame x 2");
	
	waitFrame();
	waitFrame();

//	motion = 0;

//	for (motion = 0; bitRead(motion, 0) != 1; delay(1))
//		motion = read(Motion);

//	Serial.println("starting to read");

	digitalWrite(SS, LOW); //select the chip

		byte * b = buffer;
		int i;

		SPI.transfer(Pixel_Burst);

		for (i = 0; i < 900; i++, b++)
			*b = SPI.transfer( 0 ); //burst data

	digitalWrite(SS, HIGH); //select the chip

	return i;
}

// Constructor
ADNS9500::ADNS9500() : ADNS()
{
	tsrad = 100; // 100 us, see ADNS-9500 spec, page 12
	tbexit = 0.5; // 500 ns
}
