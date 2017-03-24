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

#ifndef _RtAudioMidiHost_h_
#define _RtAudioMidiHost_h_

#include "GenPi.h"
#include "RtAudio.h"
#include "RtMidi.h"
#include "AudioMidiHost.h"
#include "Processor.h"

// audio-driven audio + midi host

namespace GenPi {

	class RtAudioMidiHost : public AudioMidiHost {

	public:

		RtAudioMidiHost()
		: m_audio(nullptr)
		, m_midiin(nullptr)
		, m_midiout(nullptr)
		, m_ip(nullptr)
		, m_op(nullptr)
		{
			if (init()) {
				if (m_audio) {
					m_info.audioInDevice = m_audio->getDefaultInputDevice();
					m_info.audioOutDevice = m_audio->getDefaultOutputDevice();
				}
			}
		}

		RtAudioMidiHost(RtAudioMidiHost const &) = delete;
		void operator=(RtAudioMidiHost const &) = delete;

		enum AudioDeviceType {
			Input,
			Output,
			All
		};

		virtual std::vector<std::string> getAudioInDevices() const {
			return getAudioDevices(Input);
		}

		virtual int setAudioInDevice(int device) {
			int rv = -1;
			std::vector<std::string> devices = getAudioInDevices();
			if (device >= 0 && device < devices.size()) {
				std::string deviceName = devices[device];
				std::vector<std::string> all = getAudioDevices(All);
				for (int i = 0, size = all.size(); i < size; i++) {
					if (all[i] == deviceName) {
						m_info.audioInDevice = i;
						rv = 0;
						break;
					}
				}
			}
			return rv;
		}

		virtual std::vector<std::string> getAudioOutDevices() const {
			return getAudioDevices(Output);
		}

		virtual int setAudioOutDevice(int device) {
			int rv = -1;
			std::vector<std::string> devices = getAudioOutDevices();
			if (device >= 0 && device < devices.size()) {
				std::string deviceName = devices[device];
				std::vector<std::string> all = getAudioDevices(All);
				for (int i = 0, size = all.size(); i < size; i++) {
					if (all[i] == deviceName) {
						m_info.audioOutDevice = i;
						rv = 0;
						break;
					}
				}
			}
			return rv;
		}

		virtual int setup() {
			if (m_midiin) {
				try {
					m_midiin->openPort(m_info.midiInPort);
					m_midiout->openPort(m_info.midiOutPort);
				}
				catch (RtMidiError &e) {
					e.printMessage();
				}
			}

			RtAudio::StreamParameters iParams;
			iParams.deviceId = m_info.audioInDevice;
			iParams.nChannels = m_info.numChannels;
			iParams.firstChannel = 0; // offset

			RtAudio::StreamParameters oParams;
			oParams.deviceId = m_info.audioOutDevice;
			oParams.nChannels = m_info.numChannels;
			oParams.firstChannel = 0; // offset

			RtAudio::StreamOptions options;
			options.flags =   RTAUDIO_NONINTERLEAVED
							| RTAUDIO_MINIMIZE_LATENCY
							| RTAUDIO_SCHEDULE_REALTIME
							;
			options.priority = 1; // goes with RTAUDIO_SCHEDULE_REALTIME
#ifdef LINUX // this appears to be necessary, needs further investigation since I was testing in a VM
			options.numberOfBuffers = 16;
#endif
			try {
				unsigned int sampleRate = (unsigned int)Globals::sampleRate;
				unsigned int bufferFrames = (unsigned int)Globals::blockSize;
				unsigned int inputDeviceCount = getAudioInDevices().size();
				unsigned int outputDeviceCount = getAudioOutDevices().size();
#if 0
				RtAudio::DeviceInfo info = m_audio->getDeviceInfo(0);
				std::cout << "\n" << info.name << ", supports " << info.inputChannels << " input channels and " << info.outputChannels << " output channels. " << info.preferredSampleRate << " is the preferred sample rate." << std::endl;
#endif
				m_audio->openStream(outputDeviceCount > 0 ? &oParams : nullptr,
						inputDeviceCount > 0 ? &iParams : nullptr,
						Globals::floatBits == 32 ? RTAUDIO_FLOAT32 : RTAUDIO_FLOAT64,
									sampleRate, &bufferFrames, callback,
									this /*userData*/, &options, nullptr /*errorCallback*/);

				// TODO: this line is necessary on RasPi to avoid a crash, but not on OSX
				// would be good to understand why :: probably because RasPi has a larger
				// minimum buffer, therefore overrunning our default 32 frames.
				if (bufferFrames != (unsigned int)Globals::blockSize) {
					m_processor.prepare((double)sampleRate, (size_t)bufferFrames);
				}
				if (!(options.flags & RTAUDIO_NONINTERLEAVED)) {
					m_info.audioInterleaved = true;
					// allocate non-interleaved buffers
					m_ip = new t_sample[bufferFrames * m_info.numChannels];
					m_op = new t_sample[bufferFrames * m_info.numChannels];
				} else {
					m_info.audioInterleaved = false;
				}
			}
			catch (RtAudioError& e) {
				std::cout << "\n" << e.getMessage() << "\n" << std::endl;
				return -1;
			}
			return 0;
		}

		virtual int run() {

			if (m_audio && (!m_audio->isStreamOpen() || m_audio->isStreamRunning())) return 0;

			try {
				m_audio->startStream();
			}
			catch (RtAudioError& e) {
				std::cout << "\n" << e.getMessage() << "\n" << std::endl;
				return -1;
			}
			return 0;
		}

		virtual void shutdown() {
			if (m_audio) {
				if (m_audio->isStreamRunning()) {
					m_audio->abortStream();
				}
				if (m_audio->isStreamOpen()) {
					m_audio->closeStream();
				}
				delete m_audio;
				m_audio = nullptr;
			}

			if (m_midiin) {
#if 0
				if (m_midiin->isPortOpen()) {
					m_midiin->closePort();
				}
#endif
				try {
					m_midiin->closePort(); // old API doesn't include isPortOpen()
				}
				catch(RtMidiError &e) {
					;
				}
				delete m_midiin;
				m_midiin = nullptr;
			}
			if (m_midiout) {
#if 0
				if (m_midiout->isPortOpen()) {
					m_midiout->closePort();
				}
#endif
				try {
					m_midiout->closePort(); // old API doesn't include isPortOpen()
				}
				catch(RtMidiError &e) {
					;
				}
				delete m_midiout;
				m_midiout = nullptr;
			}

			if (m_ip) {
				delete[] m_ip;
				m_ip = nullptr;
			}
			if (m_op) {
				delete[] m_op;
				m_op = nullptr;
			}
		}

		virtual int restart() {
			if (!m_audio || !m_audio->isStreamOpen()) return -1; // we're not inited yet

			int rv = 0;
			shutdown();
			if (init()) {
				setup();
			} else rv = -1;
			return rv;
		}

		virtual ~RtAudioMidiHost() {
			shutdown();
		}

		virtual std::string getName() const { return "RtAudio"; }

		virtual std::vector<std::string> getMidiInPorts() const {
			std::vector<std::string> ports;
			if (m_midiin) {
				int ct = m_midiin->getPortCount();
				for (int i = 0; i < ct; i++) {
					ports.push_back(m_midiin->getPortName(i));
				}
			}
			return ports;
		}

		virtual int setMidiInPort(int port) {
			if (m_midiin && port >= 0 && port < m_midiin->getPortCount()) {
				m_info.midiInPort = port;
				return 0;
			}
			return -1;
		}

		virtual std::vector<std::string> getMidiOutPorts() const {
			std::vector<std::string> ports;
			if (m_midiout) {
				int ct = m_midiout->getPortCount();
				for (int i = 0; i < ct; i++) {
					ports.push_back(m_midiout->getPortName(i));
				}
			}
			return ports;
		}

		virtual int setMidiOutPort(int port) {
			if (m_midiout && port >= 0 && port < m_midiout->getPortCount()) {
				m_info.midiOutPort = port;
				return 0;
			}
			return -1;
		}

	private:
		struct RtAudioMidiInfo {
			int audioInDevice;
			int audioOutDevice;
			int midiInPort;
			int midiOutPort;
			int numChannels;
			bool audioInterleaved;

			RtAudioMidiInfo() :
			audioInDevice(0),
			audioOutDevice(0),
			midiInPort(0),
			midiOutPort(0),
			numChannels(2),
			audioInterleaved(false) {}
		};

		static int callback(void* outputBuffer, void* inputBuffer,
				 unsigned int nFrames,
				 double streamTime,
				 RtAudioStreamStatus status,
				 void *userData)
		{
			RtAudioMidiHost* rth = (RtAudioMidiHost *)userData;

#ifdef ENABLE_MIDI
			rth->getPendingMidi(rth);
#endif

			t_sample* ip[rth->m_info.numChannels];
			t_sample* op[rth->m_info.numChannels];

			if (rth->m_info.audioInterleaved) {
				// perform an de-interleave
				for (int i = 0; i < rth->m_info.numChannels; i++) {
					ip[i] = rth->m_ip + (i * nFrames);
					op[i] = rth->m_op + (i * nFrames);
					for (int j = 0; j < nFrames; j++) {
						ip[i][j] = ((t_sample *)inputBuffer)[(j * rth->m_info.numChannels) + i];
					}
				}
			} else {
				// we are already deinterleaved, just set up the pointers
				for (int i = 0; i < rth->m_info.numChannels; i++) {
					ip[i] = (t_sample*)inputBuffer + (i * nFrames);
					op[i] = (t_sample*)outputBuffer + (i * nFrames);
				}
			}

			rth->m_processor.process((t_sample**)ip, rth->m_info.numChannels,
									 (t_sample**)op, rth->m_info.numChannels,
									 nFrames);

			if (rth->m_info.audioInterleaved) {
				// perform an interleave
				for (int i = 0; i < rth->m_info.numChannels; i++) {
					for (int j = 0; j < nFrames; j++) {
						((t_sample *)outputBuffer)[(j * rth->m_info.numChannels) + i] = op[i][j];
					}
				}
			}

#ifdef ENABLE_MIDI
			rth->processMidiOutput();
#endif

			return 0; // keep going
		}

		int getParameterIndexForName(const char* name) {
			int numParams = getGenObject().getNumParameters();
			for (int i = 0; i < numParams; i++) {
				const char* pName = getGenObject().getParameterName(i);
				if (!strcmp(pName, name)) {
					return i;
				}
			}
			return -1;
		}

#ifdef ENABLE_MIDI
		void getPendingMidi(RtAudioMidiHost* rth) {
			if (m_midiin && m_hasMidiMap) {
				while (1) {
					std::vector<unsigned char> message;
					double timeStamp = 0;

					try {
						timeStamp = m_midiin->getMessage(&message);
					}
					catch(RtMidiError &e) {
						return;
					}
					if (int bytes = message.size()) {
						if (bytes == 3 && message[0] & 0xB0) { // controller change
							int cntlNum = message[1] & 0x7F;
							int cntlVal = message[2] & 0x7F;

							std::string mapName;
							mapName = 'A' + (cntlNum - 20); // start with cntl 20 = A and onward and upward

							auto found = m_midimap.find(mapName);
							if (found != m_midimap.end()) {
								int paramNum = found->second;
								t_param min, max;
								t_param val;
								if (!getGenObject().getParameterMinMax(paramNum, &min, &max)) {
									val = (cntlVal / 127.) * (max - min) + min;
								}
								else {
									val = (cntlVal / 127.); // assume 0-1
								}
								getGenObject().setParameterValue(paramNum, val);
							}
						}

						// example for note message
						/*
						 int freqIndex; // freq param
						 int veloIndex; // velo param
						if (bytes == 3 && message[0] & 0x80) { // note on or off
							int midiNote = message[1] & 0x7F;
							int midiVelo = message[2] & 0x7F;

							if (!(message[0] & 0x10)) { // note off
								midiVelo = 0;
							}

							t_param freq = 440. * (exp(0.057762265 * ((t_param)midiNote - 69.))); // convert to Hz
							t_param velo = ((t_param)midiVelo / 127.); // 0.-1.
							getGenObject().setParameterValue(freqIndex, freq);
							if (veloIndex > -1) {
								getGenObject().setParameterValue(veloIndex, velo);
							}
						}
						*/
#if 0
						std::cout << "Got MIDI Input:" << std::endl;
						for (int i = 0; i < bytes; i++) {
							std::cout << "byte " << i << ": " << (int)message[i] << std::endl;
						}
#endif
					}
					else break;
				}
			}
		}

		void processMidiOutput() {
			// there's no way to generate outgoing MIDI in Gen
			// but here's where you'd send MIDI out from the perform loop
			if (m_midiout) {

			}
		}
#endif

		bool init() {
			RtAudio::Api api = RtAudio::UNSPECIFIED;

#ifdef RASPI
			api = RtAudio::LINUX_ALSA;
#endif
			if (!initAudio(api)) {
				return false;
			}

#if ENABLE_MIDI
			initMidiIn();
			initMidiOut();
			initMidiMap();
#endif
			return true;
		}

		bool initAudio(RtAudio::Api api) {
			try {
				m_audio = new RtAudio(api);
			}
			catch(RtAudioError& e) {
				std::cout << "\nCould not create RtAudio device!\n" << e.getMessage() << std::endl;
				return false;
			}
			if (m_audio->getDeviceCount() < 1) {
				std::cout << "\nNo audio devices found!" << std::endl;;
				delete m_audio;
				m_audio = nullptr;
				return false;
			}
			return true;
		}

		bool initMidiIn() {
			try {
				m_midiin = new RtMidiIn();
			}
			catch(RtMidiError &e) {
				std::cout << "\nCould not create RtMidi Input!\n" << e.getMessage() << std::endl;
				return false;
			}
			if (m_midiin->getPortCount() < 1) {
				std::cout << "\nNo MIDI input ports found!" << std::endl;
				delete m_midiin;
				m_midiin = nullptr;
				return false;
			}
			m_midiin->ignoreTypes(true, true, true); /* sysex, timing, active sensing */
			return true;
		}

		bool initMidiOut() {
			try {
				m_midiout = new RtMidiOut();
			}
			catch(RtMidiError &e) {
				std::cout << "\nCould not create RtMidi Output!\n" << e.getMessage() << std::endl;
				return false;
			}
			if (m_midiout->getPortCount() < 1) {
				std::cout << "\nNo MIDI output ports found!" << std::endl;
				delete m_midiout;
				m_midiout = nullptr;
				return false;
			}
			return true;
		}

#ifdef ENABLE_MIDI
		bool initMidiMap() {
			m_midimap["A"] = getParameterIndexForName("A");
			m_midimap["B"] = getParameterIndexForName("B");
			m_midimap["C"] = getParameterIndexForName("C");
			m_midimap["D"] = getParameterIndexForName("D");
			m_midimap["E"] = getParameterIndexForName("E");
			m_midimap["F"] = getParameterIndexForName("F");
			m_midimap["G"] = getParameterIndexForName("G");
			m_midimap["H"] = getParameterIndexForName("H");
			m_hasMidiMap = false;
			for (auto it = m_midimap.begin(); it != m_midimap.end(); it++) {
				if (it->second > 0) {
					m_hasMidiMap = true;
					break;
				}
			}
			return true;
		}
#endif

		std::vector<std::string> getAudioDevices(AudioDeviceType way) const {
			std::vector<std::string> devices;
			if (m_audio) {
				int ct = m_audio->getDeviceCount();
				for (int i = 0; i < ct; i++) {
					try {
						RtAudio::DeviceInfo info = m_audio->getDeviceInfo(i);
						if (
							   (way == Input && info.inputChannels)
							|| (way == Output && info.outputChannels)
							|| (way == All))
						{
							devices.push_back(info.name);
						}
					}
					catch(RtAudioError& e) {
						std::cout << "\n" << e.getMessage() << "\n" << std::endl;
						devices.push_back("<unknown>");
					}
				}
			}
			return devices;
		}

		RtAudio*        m_audio; // pointers due to possible throw in constructor
		RtMidiIn*		m_midiin;
		RtMidiOut*		m_midiout;
		Processor       m_processor;
		t_sample*       m_ip;
		t_sample*       m_op;
		RtAudioMidiInfo m_info;
#ifdef ENABLE_MIDI
		std::map<std::string, int> m_midimap;
		bool			m_hasMidiMap;
#endif

	};

}

#endif
