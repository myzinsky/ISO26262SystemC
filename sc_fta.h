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
#include <systemc>

namespace sc_fta {

    class prob {

    public:

        double value;

        prob(double value = 0.0) {
            sc_assert((value >= 0.0) && (value <= 1.0));
            this->value = value;
        };

        prob& operator=(const prob& p) {
            this->value = p.value;
            return *this;
        }

        prob& operator=(const double& d) {
            this->value = d;
            return *this;
        }

        bool operator==(const prob& p) const {
            return this->value == p.value;
        }

        bool operator==(const double& d) const {
            return this->value == d;
        }

        prob operator||(const prob &p) const {
            return prob((this->value + p.value) - this->value * p.value);
        }

        prob operator~() const {
            return prob(1.0-this->value);
        }

        prob operator!() const {
            return prob(1.0-this->value);
        }

        prob operator&&(const prob &p) const {
            return prob(this->value * p.value);
        }

        inline friend void sc_trace(sc_core::sc_trace_file *tf, const prob & p, const std::string & name) {
            sc_trace(tf, p.value, name + ".probability");
        }

        inline friend std::ostream& operator << (std::ostream& os, prob const & p) {
            return os << p.value << " (" << (p.value * 1e9) << " FIT)";
        }
    };
}