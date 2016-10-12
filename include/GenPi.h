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

#ifndef _GenPi_h_
#define _GenPi_h_

#include <cstring>
#include "gen_export.h"
#include "Host.h"
#include "Settings.h"
#include "Globals.h"
#include "HardwareEncoder.h"

namespace GenPi {

	static std::string getWorkingPath(int argc, const char* argv[]);

	template <class C>
	class Runner {
		static_assert(std::is_base_of<AudioMidiHost, C>::value, "Runner template parameter must derive from AudioMidiHost");

	public:

		Runner(int argc, const char* argv[]) {
			m_workingPath = getWorkingPath(argc, argv);
		}

		~Runner() {}

		int setup() {
			setupSettings();
			return m_host.setup();
		}

		int run() {
			return m_host.run();
		}

		void shutdown() {
			shutdownSettings();
			return m_host.shutdown();
		}

		int configure() { // TODO Configuration could be its own class, I suppose
			int rv = 0;

			std::cout << "\nSelect from the following choices:\n"
                << "[0] Modify Parameter\n"
				<< "[1] Configure Audio Input\n"
				<< "[2] Configure Audio Output\n"
            /*
				<< "[3] Configure Midi Input\n"
				<< "[4] Configure Midi Output\n"
             */
				<< "[q] Quit" << std::endl;

			std::cout << "\n[0 - 2, q]: ";
			int config = getchar();

			switch(config) {
                case '0': modifyParameter(); break;
				case '1': configureAudioInput(); break;
				case '2': configureAudioOutput(); break;
                /*
				case '3': configureMidiInput(); break;
				case '4': configureMidiOutput(); break;
                */
				case 'q': shutdown(); rv = 1; break;
				case -1: rv = -1; break;
				default: break;
			}
			std::cout << std::endl;
			return rv;
		}

	private:

		int getchar() {
			std::string line;
			try {
				std::getline(std::cin, line);
			}
			catch(std::exception &e) {
				return -1;
			}
			return (line.length() == 1 ? *(line.c_str()) : 0);
		}

		void configureAudioInput() {
			std::vector<std::string> devices = m_host.getAudioInDevices();
			if (devices.size()) {
				std::cout << "\nSelect from the following:" << std::endl;
				for (int i = 0; i < devices.size(); i++) {
					std::cout << "[" << i << "]: " << devices[i] << std::endl;
				}
				std::cout << "\n[0 - " << devices.size()-1 << "]: ";
				int device = getchar() - 48;
				if (!setAudioInput(device)) {
					m_settings.set("AudioInput", devices[device]);
				}
			} else {
				std::cout << "\nNo audio input devices." << std::endl;
			}
		}

		int setAudioInput(int device) {
			if (!m_host.setAudioInDevice(device)) {
				restart();
				return 0;
			}
			return -1;
		}

		void configureAudioOutput() {
			std::vector<std::string> devices = m_host.getAudioOutDevices();
			if (devices.size()) {
				std::cout << "\nSelect from the following:" << std::endl;
				for (int i = 0; i < devices.size(); i++) {
					std::cout << "[" << i << "]: " << devices[i] << std::endl;
				}
				std::cout << "\n[0 - " << devices.size()-1 << "]: ";
				int device = getchar() - 48;
				if (!setAudioOutput(device)) {
					m_settings.set("AudioOutput", devices[device]);
				}
			} else {
				std::cout << "\nNo audio output devices." << std::endl;
			}
		}

		int setAudioOutput(int device) {
			if (!m_host.setAudioOutDevice(device)) {
				restart();
				return 0;
			}
			return -1;
		}

        /*
		void configureMidiInput() {
			std::vector<std::string> ports = m_host.getMidiInPorts();
			if (ports.size()) {
				std::cout << "\nSelect from the following:" << std::endl;
				for (int i = 0; i < ports.size(); i++) {
					std::cout << "[" << i << "]: " << ports[i] << std::endl;
				}
				std::cout << "\n[0 - " << ports.size()-1 << "]: ";
				int port = getchar() - 48;
				if (!setMidiInput(port)) {
					m_settings.set("MidiInput", ports[port]);
				}
			} else {
				std::cout << "\nNo MIDI input devices." << std::endl;
			}
		}

		int setMidiInput(int port) {
			if (!m_host.setMidiInPort(port)) {
				restart();
				return 0;
			}
			return -1;
		}

		void configureMidiOutput() {
			std::vector<std::string> ports = m_host.getMidiOutPorts();
			if (ports.size()) {
				std::cout << "\nSelect from the following:" << std::endl;
				for (int i = 0; i < ports.size(); i++) {
					std::cout << "[" << i << "]: " << ports[i] << std::endl;
				}
				std::cout << "\n[0 - " << ports.size()-1 << "]: ";
				int port = getchar() - 48;
				if (!setMidiOutput(port)) {
					m_settings.set("MidiOutput", ports[port]);
				}
			} else {
				std::cout << "\nNo MIDI output devices." << std::endl;
			}
		}

		int setMidiOutput(int port) {
			if (!m_host.setMidiOutPort(port)) {
				restart();
				return 0;
			}
			return -1;
		}
        */

        void modifyParameter() {
			std::vector<std::string> params = m_host.getParameters();
			if (params.size()) {
				while (1) {
					std::cout << "\nSelect from the following:" << std::endl;
					for (int i = 0; i < params.size(); i++) {
						std::cout << "[" << i << "] " << params[i] << std::endl;
					}
					std::cout << "[b] Back to Main Menu" << std::endl;
					std::cout << "\n[0 - " << params.size()-1 << ", b]: ";

					int index = getchar();
					if (index != 'b') {
						index -= 48;
						if (index < params.size()) {
							t_param val = m_host.getParameterValue(index);
							std::cout << "\nCurrent value: " << val << ". New value?: ";
							std::string newvalstr;
							std::getline(std::cin, newvalstr);
							if (!newvalstr.empty()) {
								try {
									double newval = std::stod(newvalstr);
									t_param min, max;
									m_host.getParameterMinMax(index, &min, &max);
									newval = (newval < min) ? min : (newval > max) ? max : newval;
									m_host.setParameterValue(index, t_param(newval));
								}
								catch(std::exception &e) {
									std::cout << "\nI take exception!" << std::endl;
								}
							}
						}
					} else break;
				}
			} else {
				std::cout << "\nNo parameters." << std::endl;
			}
		}

		void restart() {
			m_host.restart();
			run();
		}

		void setupSettings() {
			std::string filepath(m_workingPath + "prefs.json");
			std::ifstream file(filepath);
			std::string content(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()) );

			if (!m_settings.parseText(content)) return;

			std::string value;
			if (!m_settings.get("AudioInput", value)) {
				std::vector<std::string> devices = m_host.getAudioInDevices();
				for (int i = 0, size = devices.size(); i < size; i++) {
					if (devices[i] == value) {
						setAudioInput(i);
						std::cout << "Setting audio input from settings: " << value << std::endl;
						break;
					}
				}
			}
			if (!m_settings.get("AudioOutput", value)) {
				std::vector<std::string> devices = m_host.getAudioOutDevices();
				for (int i = 0, size = devices.size(); i < size; i++) {
					if (devices[i] == value) {
						setAudioOutput(i);
						std::cout << "Setting audio output from settings: " << value << std::endl;
						break;
					}
				}
			}
            /*
			if (!m_settings.get("MidiInput", value)) {
				std::vector<std::string> ports = m_host.getMidiInPorts();
				for (int i = 0, size = ports.size(); i < size; i++) {
					if (ports[i] == value) {
						setMidiInput(i);
						std::cout << "Setting midi input from settings: " << value << std::endl;
						break;
					}
				}
			}
			if (!m_settings.get("MidiOutput", value)) {
				std::vector<std::string> ports = m_host.getMidiOutPorts();
				for (int i = 0, size = ports.size(); i < size; i++) {
					if (ports[i] == value) {
						setMidiOutput(i);
						std::cout << "Setting midi output from settings: " << value << std::endl;
						break;
					}
				}
			}
            */
		}

		void shutdownSettings() {
			std::string filepath(m_workingPath + "prefs.json");
			std::ofstream ofstr(filepath);
			ofstr << m_settings;
		}

		Host<C> m_host;
		Settings m_settings;
		std::string m_workingPath;
		HardwareEncoder m_encoder;

	};

	static std::string getWorkingPath(int argc, const char* argv[]) {
		if (argc < 1)
			throw std::invalid_argument("argument missing for path to binary");

		std::string pstr;
		char* cstr = realpath(argv[0], nullptr);
		if (cstr) {
			pstr = cstr;
			free(cstr);
		} else {
			throw std::invalid_argument("could not get realpath");
		}

		char* tstr = new char[pstr.length() + 1];
		char* c;

		strncpy(tstr, pstr.c_str(), pstr.length() + 1);
		c = strrchr(tstr, '/');
		if (!c)
			throw std::invalid_argument("cannot parse path separators");
		*c = 0; // slice off the executable to get just the build folder

		pstr = tstr;
		pstr += "/";

		delete[] tstr;

		// std::cout <<  "working-path is " << pstr << std::endl;

		return pstr;
	}

}

#endif
