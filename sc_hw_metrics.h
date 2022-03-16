/*
 * Copyright (c) 2022, Fraunhofer IESE
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors:
 *    Matthias Jung
 */

#include <iostream>
#include <systemc.h>
#include <numeric>

namespace sc_hw_metrics {

    SC_MODULE(coverage) {
        sc_in<double> input;
        sc_out<double> output;
        double dc;

        SC_HAS_PROCESS(coverage);
        coverage(sc_module_name name, double dc) : input("input"),
                                                   output("output"),
                                                   dc(dc)
        {
            SC_METHOD(compute_fit);
            sensitive << input;
        }

        void compute_fit()
        {
            output.write(input.read()*(1-dc));
        }
    };

    template <class T>
    class sc_split_out : public sc_port<sc_signal_inout_if<T>,0,SC_ONE_OR_MORE_BOUND>
    {
    public:
        std::vector<double> split_rates;

        void bind(sc_interface& interface , double rate)
        {
            sc_port_base::bind(interface);
            split_rates.push_back(rate);
        }

        void bind(sc_out<double>& parent, double rate)
        {
            sc_port_base::bind(parent);
            split_rates.push_back(rate);
        }
    };

    SC_MODULE(split) {
        sc_in<double> input;
        sc_split_out<double> outputs;

        SC_CTOR(split) : input("input") {
            SC_METHOD(compute_fit);
            sensitive << input;
        }

        void compute_fit() {
            for(int i=0; i < outputs.size(); i++) {
                double rate = outputs.split_rates.at(i);
                outputs[i]->write(input.read()*rate);
            }
        }

        // Could be used to calculate unseen dormant faults (safe faults)
        //void before_end_of_elaboration() {
        //    double total_rate;
        //    for (auto& n : outputs.split_rates) {
        //        total_rate += n;
        //    }
        //    sc_assert(total_rate == 1.0);
        //}

    };

    SC_MODULE(sum) {
        sc_port<sc_signal_in_if<double>, 0, SC_ONE_OR_MORE_BOUND> inputs;
        sc_out<double> output;

        SC_CTOR(sum) : output("output") {
            SC_METHOD(compute_fit);
            sensitive << inputs;
        }

        void compute_fit() {
            double sum = 0.0;
            for(int i=0; i < inputs.size(); i++) {
                sum += inputs[i]->read();
            }
            output.write(sum);
        }
    };

    SC_MODULE(asil) {
        sc_in<double> residual;
        sc_in<double> latent;

        double spfm;
        double lfm;
        std::string asil_level;

        SC_CTOR(asil) {
            SC_METHOD(compute);
            sensitive << residual << latent;
        }

        void compute() {
            spfm = 100*(1-(residual/(0.7*287+0.0748*287)));
            lfm = 100*(1-(latent/(0.7*287+0.0748*287)));

            if(residual < 1000.0) {
                asil_level = "ASIL-A";
            }

            if(spfm > 90.0 && lfm > 60.0 && residual < 100.0) {
                asil_level = "ASIL-B";
            }

            if(spfm > 97.0 && lfm > 80.0 && residual < 100.0) {
                asil_level = "ASIL-C";
            }

            if(spfm > 99.0 && lfm > 90.0 && residual < 10.0) {
                asil_level = "ASIL-D";
            }
        }

        ~asil() {
            std::cout << "RES:  " << residual   << std::endl;
            std::cout << "LAT:  " << latent     << std::endl;
            std::cout << "SPFM: " << spfm       << "%" << std::endl;
            std::cout << "LFM:  " << lfm        << "%" << std::endl;
            std::cout << "ASIL: " << asil_level << std::endl;
        }
    };

}

