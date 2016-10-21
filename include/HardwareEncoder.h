/*******************************************************************************************************************
 Cycling '74 License for Max-Generated Code for Export
 Copyright (c) 2016 Cycling '74
 The code that Max generates automatically and that end users are capable of exporting and using, and any
 associated documentation files (the “Software”) is a work of authorship for which Cycling '74 is the author
 and owner for copyright purposes.  A license is hereby granted, free of charge, to any person obtaining a
 copy of the Software (“Licensee”) to use, copy, modify, merge, publish, and distribute copies of the Software,
 and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 The Software is licensed to Licensee only for non-commercial use. Users who wish to make commercial use of the
 Software must contact the copyright owner to determine if a license for commercial use is available, and the
 terms and conditions for same, which may include fees or royalties. For commercial use, please send inquiries
 to licensing@cycling74.com.  The determination of whether a use is commercial use or non-commercial use is based
 upon the use, not the user. The Software may be used by individuals, institutions, governments, corporations, or
 other business whether for-profit or non-profit so long as the use itself is not a commercialization of the
 materials or a use that generates or is intended to generate income, revenue, sales or profit.
 The above copyright notice and this license shall be included in all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 *******************************************************************************************************************/

#ifndef _HardwareEncoder_h_
#define _HardwareEncoder_h_

#if defined(RASPI) && defined(WIRINGPI)

#include <unistd.h>
#include <wiringPi.h>
#include <thread>
#include <climits>
#include "Object.h"

struct encoder;
extern "C" struct encoder* setupencoder(int pin_a, int pin_b);

extern "C" {
#include "rotaryencoder.h"
}

namespace GenPi {

	class HardwareEncoder {

	public:

		HardwareEncoder(
						int paramIndex = -1, // we want to provide a name-based automapping, as well
						int pin1 = 3, // pins are wiringPi-numbered
						int pin2 = 4)
		: mSampleThread(&HardwareEncoder::sample, this)
		, mEncoder(nullptr)
		, mCurrentValue(INT_MAX)
		, mParameterIndex(paramIndex)
		{
			wiringPiSetup(); // initialize the wiringPi library.
			// NOTE: this requires root, absurdly. There is a better way to handle this
			// but it requires calling the 'gpio' utility via system() to configure the
			// pins on the fly, and is more than I want to deal with for a proof of concept
			// implementation right now. http://wiringpi.com/reference/setup/
			mEncoder = setupencoder(pin1, pin2);
			if (paramIndex >= 0) {
				mRunning = true;
			}
		}


		~HardwareEncoder() {
			mRunning = false;
			if (mSampleThread.joinable())
				mSampleThread.join();
		}

		void setParameter(int index) {
			if (index >= 0) {
				if (index < getGenObject().getNumParameters()) {
					mParameterIndex = index;
					mRunning = true;
				} // else no change
			}
			else {
				mParameterIndex = -1;
				mRunning = false;
			}
		}

	private:

		void sample() {
			while (mRunning) {
				if (mEncoder) {
					// endless encoder logic
					int value = getEncoderValue(mEncoder);

					if (value != mCurrentValue) {
						t_param fdelta = (t_param)(value - mCurrentValue);
						t_param curvalue, fvalue, min, max;
						// printf("DEBUG: got encoder value %d (delta %0.3f)\n", value, fdelta);
						curvalue = getGenObject().getParameterValue(mParameterIndex);
						fvalue = curvalue;
						if (!getGenObject().getParameterMinMax(mParameterIndex, &min, &max)) {
							t_param stepsize = (max - min) / 127.;
							fdelta *= stepsize;
							fvalue += fdelta;
							fvalue = fvalue < min ? min : fvalue > max ? max : fvalue;
						} else {
							fvalue += fdelta;
						}
						if (fvalue != curvalue) {
							getGenObject().setParameterValue(mParameterIndex, fvalue);
							// printf("DEBUG: sending value %0.3f\n", fvalue);
						}
						mCurrentValue = value;
					}
				}
				usleep(100);
			}
		}

		int getEncoderValue(struct encoder* enc) {
			return enc->value;
		}

		std::thread					mSampleThread;
		struct encoder				*mEncoder;
		bool						mRunning;
		int                         mCurrentValue;
		int							mParameterIndex;

	};

// wiringPi definitions of INPUT and OUTPUT will interfere with RtAudio
#undef INPUT
#undef OUTPUT

} // namespace GenPi

#else

namespace GenPi {

	class HardwareEncoder { // dummy

	public:
		HardwareEncoder(int paramIndex = 0,
						int pin1 = 3,
						int pin2 = 4)
		{
		}

	};

}

#endif
#endif
