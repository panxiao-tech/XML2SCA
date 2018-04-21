/**
 * This file is automatically generated using [XML2SCA] 
 * Contact : panxiao.tech@gmail.com
 * Web     : http://panxiao.tech/tools/xml2sca/
 * 
 * @file    sensor_adc_example/tb/adc/sc_top.h
 * @author  Xiao Pan (pan@cs.uni-kl.de)
 * @date    10:46:05 Apr 16 2018
 * @section LICENSE License (ADD YOUR LICENSE HERE)
 *  
 * @section DESCRIPTION Description (ADD YOUR DESCRIPTION HERE)
 *          Example project of [XML2AMS]
 * 
 */


#ifndef sc_top_h_

#define sc_top_h_
#include "../../src/basic/json.hpp"
#include "../../src/basic/file_parse.h"
#include "../../src/sca_tdf_adc.h"


//!
//! @brief stimuli module
//!
SCA_TDF_MODULE(sca_tdf_stimuli)
{
	 sca_tdf::sc_in<uint16_t>  pin_conv;          
	 sca_tdf::sca_out<double>  pout_pos;          
	 sca_tdf::sca_out<double>  pout_neg;          

	// constructor
	sca_tdf_stimuli(sc_core::sc_module_name nm){}


	void processing()
	{
		//output port
		pout_pos.write(1.0);
		pout_neg.write(0);

		//input port
//        std::cout<< " readout adc-pout[conv] = " << pin_conv.read()<<std::endl; 
	}
};


//! 
//! @brief testbench toplevel module
//! 
SC_MODULE(sc_top) 
{
	sca_tdf_stimuli      *i_stimuli;
	sca_tdf_adc          *i_adc;

	sc_core::sc_signal<uint16_t>          sig_from_adc_conv;
	sca_tdf::sca_signal<double>            sig_to_adc_pos;
	sca_tdf::sca_signal<double>            sig_to_adc_neg;

	// sample period
	sca_core::sca_time tstep;

	// trace file
	sca_util::sca_trace_file* tf;
	std::string trace_format; 


	// Constructor
	sc_top(sc_core::sc_module_name nm,nlohmann::json  _configs):sc_module(nm)
	{ 

		// get time step from json configuration file  
		tstep = sca_core::sca_time(_configs["system"]["tstep"] ,sc_core::SC_SEC);

		// trace file
		trace_format = _configs["system"]["tracefile"];
		tf = trace_format.compare("vcd") == 0 ?
			sca_util::sca_create_vcd_trace_file("tracefile_adc_tb"):
			sca_util::sca_create_tabular_trace_file("tracefile_adc_tb");

		// top level instances
		i_stimuli      = new sca_tdf_stimuli("i_stimuli");
		i_adc          = new sca_tdf_adc("i_adc",
			            (double)       _configs["i_adc"]["vref"],
			            (unsigned int) _configs["i_adc"]["res"],
			            (double)       _configs["i_adc"]["fs"],
			            (int)          _configs["i_adc"]["rate"]);

		// interconnections 
		i_adc->pout_conv(sig_from_adc_conv);
		i_stimuli->pin_conv(sig_from_adc_conv);
		i_adc->pin_pos(sig_to_adc_pos);
		i_stimuli->pout_pos(sig_to_adc_pos);
		i_adc->pin_neg(sig_to_adc_neg);
		i_stimuli->pout_neg(sig_to_adc_neg);

		// setup timestep
		i_stimuli->set_timestep(tstep);
		sca_util::sca_trace(tf, sig_from_adc_conv, "sig_from_adc_conv");
		sca_util::sca_trace(tf, sig_to_adc_pos, "sig_to_adc_pos");
		sca_util::sca_trace(tf, sig_to_adc_neg, "sig_to_adc_neg");

	};


	// Destructor
	~sc_top()
	{
		if(trace_format.compare("vcd")) 
			 sca_close_tabular_trace_file(tf);
		 else
			 sca_close_vcd_trace_file(tf);
	}


};

#endif 