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

#include <systemc.h>
#include <iostream>
#include "../sc_fta.h"

using namespace sc_fta;

SC_MODULE(DRAM) {
    sc_out<prob> SBE, DBE, MBE, WD;
    prob E_SBE, E_DBE, E_MBE, E_WD;

    SC_CTOR(DRAM) : E_SBE(0.7*287e-9), E_DBE(0.0748*287e-9), E_MBE(0.06*287e-9), E_WD(0.0748*287e-9),
                    SBE("SBE"), DBE("DBE"), MBE("MBE"), WD("WD")
    {
        SC_METHOD(compute_prob);
    }

    void compute_prob () {
        SBE.write(E_SBE);
        DBE.write(E_DBE);
        MBE.write(E_MBE);
        WD.write(E_WD);
    }
};

SC_MODULE(DRAM_SEC_ECC) {

    sc_in<prob> I_SBE, I_DBE, I_MBE, I_WD;
    sc_out<prob> O_SBE, O_DBE, O_TBE, O_MBE, O_WD;

    prob E_SEC_DEFECT;
    prob E_THIRD_ERROR;

    SC_CTOR(DRAM_SEC_ECC) : E_SEC_DEFECT(0.1e-9), E_THIRD_ERROR(0.17),
                            I_SBE("I_SBE"), I_DBE("I_DBE"), I_MBE("I_MBE"), I_WD("I_WD"),
                            O_SBE("O_SBE"), O_DBE("O_DBE"), O_TBE("O_TBE"), O_MBE("O_MBE"), O_WD("O_WD")
    {
        SC_METHOD(compute_prob);
        sensitive << I_SBE << I_DBE << I_MBE << I_WD;
    }

    void compute_prob() {
        O_SBE.write(
            E_SEC_DEFECT && I_SBE.read()
        );

        O_DBE.write(
            (E_SEC_DEFECT && I_DBE.read()) || (!(E_SEC_DEFECT) && I_DBE.read() && !(E_THIRD_ERROR))
        );

        O_TBE.write(
            !(E_SEC_DEFECT) && I_DBE.read() && E_THIRD_ERROR
        );

        O_MBE.write(
            I_MBE.read()
        );

        O_WD.write(
            I_WD.read()
        );
    }
};

SC_MODULE(SEC_ECC_TRIM)
{
    sc_in<prob>  I_SBE, I_DBE, I_TBE, I_MBE, I_WD;
    sc_out<prob> O_SBE, O_DBE, O_TBE, O_MBE, O_WD;

    prob E_0_1_TRIM, E_1_2_TRIM, E_0_2_TRIM, E_2_3_TRIM, E_1_3_TRIM, E_0_3_TRIM;

    SC_CTOR(SEC_ECC_TRIM) : E_0_1_TRIM(0.94), E_1_2_TRIM(0.11), E_0_2_TRIM(0.89), E_2_3_TRIM(0.009), E_1_3_TRIM(0.15), E_0_3_TRIM(0.83),
                            I_SBE("I_SBE"), I_DBE("I_DBE"), I_TBE("I_TBE"), I_MBE("I_MBE"), I_WD("I_WD"),
                            O_SBE("O_SBE"), O_DBE("O_DBE"), O_TBE("O_TBE"), O_MBE("O_MBE"), O_WD("O_WD") 
    {
        SC_METHOD(compute_prob);
        sensitive << I_SBE << I_DBE << I_TBE << I_MBE << I_WD;
    }

    void compute_prob() {
        O_SBE.write(
            (E_0_1_TRIM && I_SBE.read()) || (E_1_2_TRIM && I_DBE.read()) || (E_2_3_TRIM && I_TBE.read())
        );

        O_DBE.write(
            (E_0_2_TRIM && I_DBE.read()) || (E_1_3_TRIM && I_TBE.read())
        );

        O_TBE.write(
            (E_0_3_TRIM && I_TBE.read())
        );

        O_MBE.write(
            I_MBE.read()
        );

        O_WD.write(
            I_WD.read()
        );
    }
};

int sc_main (int __attribute__((unused)) sc_argc, char __attribute__((unused)) *sc_argv[])
{
    DRAM dram("DRAM");
    DRAM_SEC_ECC sec_ecc("DRAM_SEC_ECC");
    SEC_ECC_TRIM sec_ecc_trim("SEC_ECC_TRIM");

    sc_signal<prob> a1("a1");
    sc_signal<prob> a2("a2");
    sc_signal<prob> a3("a3");
    sc_signal<prob> a4("a4");

    sec_ecc.I_SBE.bind(a1);
    sec_ecc.I_DBE.bind(a2);
    sec_ecc.I_MBE.bind(a3);
    sec_ecc.I_WD.bind(a4);

    dram.SBE.bind(a1);
    dram.DBE.bind(a2);
    dram.MBE.bind(a3);
    dram.WD.bind(a4);

    sc_signal<prob> b1("b1");
    sc_signal<prob> b2("b2");
    sc_signal<prob> b3("b3");
    sc_signal<prob> b4("b4");
    sc_signal<prob> b5("b5");

    sec_ecc.O_SBE.bind(b1);
    sec_ecc.O_DBE.bind(b2);
    sec_ecc.O_TBE.bind(b3);
    sec_ecc.O_MBE.bind(b4);
    sec_ecc.O_WD.bind(b5);

    sec_ecc_trim.I_SBE.bind(b1);
    sec_ecc_trim.I_DBE.bind(b2);
    sec_ecc_trim.I_TBE.bind(b3);
    sec_ecc_trim.I_MBE.bind(b4);
    sec_ecc_trim.I_WD.bind(b5);

    // SBE: 2.009e-17 (2.009e-08 FIT)
    // DBE: 1.78181e-08 (17.8181 FIT)
    // TBE: 3.64949e-09 (3.64949 FIT)
    // MBE: 1.722e-08 (17.22 FIT)
    // WD:  2.14676e-08 (21.4676 FIT)

    sc_signal<prob> c1("c1");
    sc_signal<prob> c2("c2");
    sc_signal<prob> c3("c3");
    sc_signal<prob> c4("c4");
    sc_signal<prob> c5("c5");

    sec_ecc_trim.O_SBE.bind(c1);
    sec_ecc_trim.O_DBE.bind(c2);
    sec_ecc_trim.O_TBE.bind(c3);
    sec_ecc_trim.O_MBE.bind(c4);
    sec_ecc_trim.O_WD.bind(c5);

    // SBE: 1.99284e-09 (1.99284 FIT)
    // DBE: 1.64055e-08 (16.4055 FIT)
    // TBE: 3.02908e-09 (3.02908 FIT)
    // MBE: 1.722e-08 (17.22 FIT)
    // WD:  2.14676e-08 (21.4676 FIT)

    sc_start();

    std::cout << "SBE: " << c1 << std::endl;
    std::cout << "DBE: " << c2 << std::endl;
    std::cout << "TBE: " << c3 << std::endl;
    std::cout << "MBE: " << c4 << std::endl;
    std::cout << "WD:  " << c5 << std::endl;

    return 0;
}