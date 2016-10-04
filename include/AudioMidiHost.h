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

#ifndef _AudioMidiHost_h_
#define _AudioMidiHost_h_

// pure virtual base class for audio/midi backends (portaudio, rtaudio, etc.)

#include <string>
#include <vector>

namespace GenPi {

	class AudioMidiHost {

	public:

		virtual ~AudioMidiHost() {}

		virtual int setup() = 0;
		virtual int run() = 0;
		virtual void shutdown() = 0;
		virtual int restart() = 0;

		virtual std::vector<std::string> getAudioInDevices() const = 0;
		virtual int setAudioInDevice(int device) = 0;

		virtual std::vector<std::string> getAudioOutDevices() const = 0;
		virtual int setAudioOutDevice(int device) = 0;

		virtual std::vector<std::string> getMidiInPorts() const = 0;
		virtual int setMidiInPort(int port) = 0;

		virtual std::vector<std::string> getMidiOutPorts() const = 0;
		virtual int setMidiOutPort(int port) = 0;

		virtual std::string getName() const = 0;

	};

}

#endif
