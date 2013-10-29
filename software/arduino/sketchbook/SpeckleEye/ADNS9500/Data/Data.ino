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

#include <SPI.h>
#include <adns9500.h>

ADNS9500 sensor;
using namespace ADNS9500_CONSTANTS;

int x;
int y;
int shutter;
byte maximumPixel;
byte surfaceQuality;
byte minimumPixel;
byte pixelSum;
int framePeriod;
byte motion;
byte observation;

int ledPin = 13;

boolean burstMode = false;

void loop()
{
  digitalWrite(ledPin, HIGH);

  if (sensor.readHasMoved())
  {
    digitalWrite(ledPin, LOW);

    if (burstMode)
      readValuesBurst();
    else
      readValues();  

    Serial.print(x, DEC);
    Serial.print('\t');

    Serial.print(y, DEC);
    Serial.print('\t');

    Serial.print(maximumPixel, DEC);
    Serial.print('\t');

    Serial.print(pixelSum, DEC);
    Serial.print('\t');

    Serial.print(surfaceQuality, DEC);
    Serial.print('\t');

    Serial.print(shutter, DEC);
    Serial.print('\t');

    Serial.print('\n');
    Serial.print('\0');
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  sensor.init();  
}

void readValues()
{
  sensor.readMotion(&x, &y);
  maximumPixel = sensor.read(Maximum_Pixel);
  minimumPixel = sensor.read(Maximum_Pixel);
  pixelSum = sensor.read(Pixel_Sum);
  surfaceQuality = sensor.read(SQUAL);
  shutter = sensor.readShutter();
  motion = sensor.read(Motion);
  surfaceQuality = sensor.read(Observation);
  //skipping framePeriod
}

void readValuesBurst()
{
  sensor.readBurst(&x, &y, &surfaceQuality, &shutter, &pixelSum, &maximumPixel, &minimumPixel, 
		   &framePeriod, &motion, &observation);
}





