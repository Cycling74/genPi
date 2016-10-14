#include "gen_exported.h"

namespace gen_exported {

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
  to licensing (at) cycling74.com.  The determination of whether a use is commercial use or non-commercial use is based
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

// global noise generator
Noise noise;
static const int GENLIB_LOOPCOUNT_BAIL = 100000;


// The State struct contains all the state and procedures for the gendsp kernel
typedef struct State {
	CommonState __commonstate;
	SineCycle __m_cycle_12;
	SineData __sinedata;
	int __exception;
	int vectorsize;
	t_sample __m_count_8;
	t_sample m_frequency_1;
	t_sample __m_carry_10;
	t_sample samplerate;
	t_sample __m_slide_5;
	t_sample m_formant_2;
	t_sample m_velocity_4;
	t_sample m_period_3;
	// re-initialize all member variables;
	inline void reset(t_param __sr, int __vs) {
		__exception = 0;
		vectorsize = __vs;
		samplerate = __sr;
		m_frequency_1 = 440;
		m_formant_2 = 2;
		m_period_3 = 4410;
		m_velocity_4 = 0;
		__m_slide_5 = 0;
		__m_count_8 = 0;
		__m_carry_10 = 0;
		__m_cycle_12.reset(samplerate, 0);
		genlib_reset_complete(this);
		
	};
	// the signal processing routine;
	inline int perform(t_sample ** __ins, t_sample ** __outs, int __n) {
		vectorsize = __n;
		const t_sample * __in1 = __ins[0];
		t_sample * __out1 = __outs[0];
		t_sample * __out2 = __outs[1];
		if (__exception) {
			return __exception;
			
		} else if (( (__in1 == 0) || (__out1 == 0) || (__out2 == 0) )) {
			__exception = GENLIB_ERR_NULL_BUFFER;
			return __exception;
			
		};
		t_sample iup_6 = (1 / maximum(1, abs(100)));
		t_sample idown_7 = (1 / maximum(1, abs(10000)));
		t_sample rdiv_679 = safediv(1, m_period_3);
		// the main sample loop;
		while ((__n--)) {
			const t_sample in1 = (*(__in1++));
			__m_slide_5 = fixdenorm((__m_slide_5 + (((m_velocity_4 > __m_slide_5) ? iup_6 : idown_7) * (m_velocity_4 - __m_slide_5))));
			t_sample slide_671 = __m_slide_5;
			__m_count_8 = (0 ? 0 : (fixdenorm(__m_count_8 + rdiv_679)));
			int carry_9 = 0;
			if ((0 != 0)) {
				__m_count_8 = 0;
				__m_carry_10 = 0;
				
			} else if (((1 > 0) && (__m_count_8 >= 1))) {
				int wraps_11 = (__m_count_8 / 1);
				__m_carry_10 = (__m_carry_10 + wraps_11);
				__m_count_8 = (__m_count_8 - (wraps_11 * 1));
				carry_9 = 1;
				
			};
			t_sample counter_675 = __m_count_8;
			int counter_676 = carry_9;
			int counter_677 = __m_carry_10;
			t_sample s = (counter_675 * m_formant_2);
			t_sample clipped = ((s <= 0) ? 0 : ((s >= 1) ? 1 : s));
			t_sample scaled = ((clipped * 3.1415926535898) * 2);
			t_sample expr_684 = ((1 - cos(scaled)) * 0.5);
			__m_cycle_12.freq(m_frequency_1);
			t_sample cycle_673 = __m_cycle_12(__sinedata);
			t_sample cycleindex_674 = __m_cycle_12.phase();
			t_sample mul_680 = (expr_684 * cycle_673);
			t_sample mul_672 = (mul_680 * slide_671);
			t_sample out2 = mul_672;
			t_sample out1 = mul_672;
			// assign results to output buffer;
			(*(__out1++)) = out1;
			(*(__out2++)) = out2;
			
		};
		return __exception;
		
	};
	inline void set_frequency(t_param _value) {
		m_frequency_1 = (_value < 1 ? 1 : (_value > 10000 ? 10000 : _value));
	};
	inline void set_formant(t_param _value) {
		m_formant_2 = (_value < 1 ? 1 : (_value > 255 ? 255 : _value));
	};
	inline void set_period(t_param _value) {
		m_period_3 = (_value < 1 ? 1 : (_value > 10000 ? 10000 : _value));
	};
	inline void set_velocity(t_param _value) {
		m_velocity_4 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	
} State;


///
///	Configuration for the genlib API
///

/// Number of signal inputs and outputs

int gen_kernel_numins = 1;
int gen_kernel_numouts = 2;

int num_inputs() { return gen_kernel_numins; }
int num_outputs() { return gen_kernel_numouts; }
int num_params() { return 4; }

/// Assistive lables for the signal inputs and outputs

const char *gen_kernel_innames[] = { "in1" };
const char *gen_kernel_outnames[] = { "out1", "out2" };

/// Invoke the signal process of a State object

int perform(CommonState *cself, t_sample **ins, long numins, t_sample **outs, long numouts, long n) {
	State* self = (State *)cself;
	return self->perform(ins, outs, n);
}

/// Reset all parameters and stateful operators of a State object

void reset(CommonState *cself) {
	State* self = (State *)cself;
	self->reset(cself->sr, cself->vs);
}

/// Set a parameter of a State object

void setparameter(CommonState *cself, long index, t_param value, void *ref) {
	State *self = (State *)cself;
	switch (index) {
		case 0: self->set_formant(value); break;
		case 1: self->set_frequency(value); break;
		case 2: self->set_period(value); break;
		case 3: self->set_velocity(value); break;
		
		default: break;
	}
}

/// Get the value of a parameter of a State object

void getparameter(CommonState *cself, long index, t_param *value) {
	State *self = (State *)cself;
	switch (index) {
		case 0: *value = self->m_formant_2; break;
		case 1: *value = self->m_frequency_1; break;
		case 2: *value = self->m_period_3; break;
		case 3: *value = self->m_velocity_4; break;
		
		default: break;
	}
}

/// Get the name of a parameter of a State object

const char *getparametername(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].name;
	}
	return 0;
}

/// Get the minimum value of a parameter of a State object

t_param getparametermin(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmin;
	}
	return 0;
}

/// Get the maximum value of a parameter of a State object

t_param getparametermax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmax;
	}
	return 0;
}

/// Get parameter of a State object has a minimum and maximum value

char getparameterhasminmax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].hasminmax;
	}
	return 0;
}

/// Get the units of a parameter of a State object

const char *getparameterunits(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].units;
	}
	return 0;
}

/// Get the size of the state of all parameters of a State object

size_t getstatesize(CommonState *cself) {
	return genlib_getstatesize(cself, &getparameter);
}

/// Get the state of all parameters of a State object

short getstate(CommonState *cself, char *state) {
	return genlib_getstate(cself, state, &getparameter);
}

/// set the state of all parameters of a State object

short setstate(CommonState *cself, const char *state) {
	return genlib_setstate(cself, state, &setparameter);
}

/// Allocate and configure a new State object and it's internal CommonState:

void *create(t_param sr, long vs) {
	State *self = new State;
	self->reset(sr, vs);
	ParamInfo *pi;
	self->__commonstate.inputnames = gen_kernel_innames;
	self->__commonstate.outputnames = gen_kernel_outnames;
	self->__commonstate.numins = gen_kernel_numins;
	self->__commonstate.numouts = gen_kernel_numouts;
	self->__commonstate.sr = sr;
	self->__commonstate.vs = vs;
	self->__commonstate.params = (ParamInfo *)genlib_sysmem_newptr(4 * sizeof(ParamInfo));
	self->__commonstate.numparams = 4;
	// initialize parameter 0 ("m_formant_2")
	pi = self->__commonstate.params + 0;
	pi->name = "formant";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_formant_2;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 1;
	pi->outputmax = 255;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 1 ("m_frequency_1")
	pi = self->__commonstate.params + 1;
	pi->name = "frequency";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_frequency_1;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 1;
	pi->outputmax = 10000;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 2 ("m_period_3")
	pi = self->__commonstate.params + 2;
	pi->name = "period";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_period_3;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 1;
	pi->outputmax = 10000;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 3 ("m_velocity_4")
	pi = self->__commonstate.params + 3;
	pi->name = "velocity";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_velocity_4;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	
	return self;
}

/// Release all resources and memory used by a State object:

void destroy(CommonState *cself) {
	State *self = (State *)cself;
	genlib_sysmem_freeptr(cself->params);
		
	delete self;
}


} // gen_exported::
