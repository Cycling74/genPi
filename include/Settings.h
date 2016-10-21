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

#ifndef _Settings_h_
#define _Settings_h_

#include "jsonxx.h"

namespace GenPi {

	class Settings {

	public:

		Settings() {}
		~Settings() {}

		template <typename T>
		void set(std::string key, const T& value) { m_state << key << value; }
		// override for the Settings case
		void set(std::string key, const Settings& value) { m_state << key << value.m_state; }

		bool parseText(std::string& text) { return m_state.parse(text); }
		std::string getText() const { return m_state.json(); }

		int get(std::string key, std::string* value) const { return get<std::string, jsonxx::String>(key, value); }
		int get(std::string key, int* value) const { return get<int, jsonxx::Number>(key, value); }
		int get(std::string key, double* value) const { return get<double, jsonxx::Number>(key, value); }
		int get(std::string key, bool* value) const { return get<bool, jsonxx::Boolean>(key, value); }
		int get(std::string key, Settings* value) const { return get<Settings, jsonxx::Object>(key, value); }

		std::vector<std::string> getKeys() {
			std::vector<std::string> keys;
			jsonxx::Object::container container = m_state.kv_map();
			for (auto it = container.begin(); it != container.end(); it++) {
				keys.push_back(it->first);
			}
			return keys;
		}

		bool empty() { return m_state.empty(); }

		// currently not necessary, but might be at some point
		friend std::ostream& operator<<(std::ostream& os, const Settings& obj);
		friend std::istream& operator>>(std::istream& is, Settings& obj);

	private:

		Settings(jsonxx::Object& o) : m_state(o) {}
		Settings& operator=(const jsonxx::Object& o) {
			m_state = o;
			return *this;
		}

		template <typename T, typename U>
		int get(std::string key, T* value) const {
			if (m_state.has<U>(key)) {
				*value = (U)m_state.get<U>(key);
				return 0;
			}
			return -1;
		}

		jsonxx::Object m_state;

	};

	std::ostream& operator<<(std::ostream& os, const Settings& obj) {
		os << obj.getText();
		return os;
	}

	std::istream& operator>>(std::istream& is, Settings& obj) {
		std::string s(std::istreambuf_iterator<char>(is), {});
		if (!obj.parseText(s)) {
			is.setstate(std::ios::failbit);
		}
		return is;
	}

}

#endif
