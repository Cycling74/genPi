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

#ifndef _Host_h_
#define _Host_h_

#include "gen_export.h"
#include "AudioMidiHost.h"
#include "ParameterHost.h"

// composed class for audio/midi hosting + parameter support

namespace GenPi {

	template <class C>
	class Host {
		static_assert(std::is_base_of<AudioMidiHost, C>::value, "Host template parameter must derive from AudioMidiHost");

	public:
		Host() {}
		~Host() {}

		// AudioMidiHost
		int setup() { return m_audio_host.setup(); }
		int run() { return m_audio_host.run(); }
		void shutdown() { m_audio_host.shutdown(); }
		int restart() { return m_audio_host.restart(); }
		std::vector<std::string> getAudioInDevices() { return m_audio_host.getAudioInDevices(); }
		int setAudioInDevice(int device) { return m_audio_host.setAudioInDevice(device); }
		std::vector<std::string> getAudioOutDevices() { return m_audio_host.getAudioOutDevices(); }
		int setAudioOutDevice(int device) { return m_audio_host.setAudioOutDevice(device); }
		std::vector<std::string> getMidiInPorts() const { return m_audio_host.getMidiInPorts(); }
		int setMidiInPort(int port) { return m_audio_host.setMidiInPort(port); }
		std::vector<std::string> getMidiOutPorts() const { return m_audio_host.getMidiOutPorts(); }
		int setMidiOutPort(int port) { return m_audio_host.setMidiOutPort(port); }
		std::string getName() const { return m_audio_host.getName(); }

		// ParameterHost
		std::vector<std::string> getParameters() const { return m_param_host.getParameters(); }
		int getParameterMinMax(int index, t_param* min, t_param* max) const { return m_param_host.getParameterMinMax(index, min, max); }
		t_param getParameterValue(int index) const { return m_param_host.getParameterValue(index); }
		void setParameterValue(int index, t_param value) { m_param_host.setParameterValue(index, value); }

	private:
		C m_audio_host;
		ParameterHost m_param_host;

	};

}

#endif
