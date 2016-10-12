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

#ifndef _Object_h_
#define _Object_h_

#include "Globals.h"
#include "gen_export.h"

namespace GenPi {

	class Object {

	public:
		static Object& getInstance() {
			static Object theInstance;
			pInstance = &theInstance;
			return *pInstance;
		}

		int getNumParameters() { return gen_export::num_params(); }
		int getNumInputChannels() { return gen_export::num_inputs(); }
		int getNumOutputChannels() { return gen_export::num_outputs(); }

		const char* getParameterName(long index) {
			if (m_genObject && index >= 0 && index < getNumParameters()) {
				return gen_export::getparametername(m_genObject, index);
			}
			return "";
		}

		int getParameterMinMax(long index, t_param* min, t_param* max) {
			if (m_genObject && index >= 0 && index < getNumParameters()) {
				if (gen_export::getparameterhasminmax(m_genObject, index)) {
					*min = gen_export::getparametermin(m_genObject, index);
					*max = gen_export::getparametermax(m_genObject, index);
					return 0;
				}
			}
			return -1;
		}

		t_param getParameterValue(long index) {
			if (m_genObject && index >= 0 && index < getNumParameters()) {
				t_param value;
				gen_export::getparameter(m_genObject, index, &value);
				return value;
			}
			return -1;
		}

		void setParameterValue(long index, t_param value) {
			if (m_genObject && index >= 0 && index < getNumParameters()) {
				gen_export::setparameter(m_genObject, index, value, nullptr);
			}
		}

		void perform(t_sample** ins, long numIns, t_sample** outs, long numOuts, long numFrames) {
			if (m_genObject) {
				gen_export::perform(m_genObject, ins, numIns, outs, numOuts, numFrames);
			}
		}

	private:

		static Object *pInstance;

		Object() {
			m_genObject = (CommonState*)gen_export::create(Globals::sampleRate, Globals::blockSize);
		}

		Object(const Object& rs) {
			pInstance = rs.pInstance;
		}

		Object &operator = (const Object& rs) {
			if (this != &rs) {
				pInstance = rs.pInstance;
			}
			return *this;
		}

		~Object() {
			if (m_genObject) {
				gen_export::destroy(m_genObject);
			}
		}

		CommonState* m_genObject;

	};

	static inline Object& getGenObject() {
		return Object::getInstance();
	}

}

#endif
