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
#include "../sc_hw_metrics.h"

using namespace sc_hw_metrics;

SC_MODULE(DRAM)
{
    sc_out<double> SBE, DBE;//, MBE, WD;
    double E_SBE, E_DBE; //, E_MBE, E_WD;

    SC_CTOR(DRAM) : E_SBE(0.7*287), E_DBE(0.0748*287),// E_MBE(0.06*287e-9), E_WD(0.0748*287e-9),
                    SBE("SBE"), DBE("DBE")//, MBE("MBE"), WD("WD")
    {
        SC_METHOD(compute);
    }

    void compute () {
        SBE.write(E_SBE);
        DBE.write(E_DBE);
        //MBE.write(E_MBE);
        //WD.write(E_WD);
    }
};

SC_MODULE(DRAM_SEC_ECC)
{

    sc_in<double> I_SBE, I_DBE;//, I_MBE, I_WD;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_LAT_SBE, O_LAT_DBE;//, O_MBE, O_WD;
    coverage sec_coverage;
    split sec_split;
    pass sec_pass;

    SC_CTOR(DRAM_SEC_ECC) : I_SBE("I_SBE"), I_DBE("I_DBE"),// I_MBE("I_MBE"), I_WD("I_WD"),
                            O_RES_SBE("O_RES_SBE"), O_RES_DBE("O_RES_DBE"), O_RES_TBE("O_RES_TBE"),
                            O_LAT_SBE("O_LAT_SBE"), O_LAT_DBE("O_LAT_DBE"),
                            sec_coverage("SEC_Coverage", 1.0),
                            sec_split("SEC_split"),
                            sec_pass("SEC_PASS")
    {
        sec_coverage.input(I_SBE);
        sec_coverage.output(O_RES_SBE);
        sec_coverage.latent(O_LAT_SBE);

        sec_split.input(I_DBE);
        sec_split.outputs.bind(O_RES_DBE, 0.83);
        sec_split.outputs.bind(O_RES_TBE, 0.17);

        sec_pass.input(I_DBE);
        sec_pass.output(O_LAT_DBE);
    }

};

SC_MODULE(DRAM_SEC_TRIM)
{
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_LAT_SBE, I_LAT_DBE;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_LAT_SBE, O_LAT_DBE;

    split res_sbe_split, res_dbe_split, res_tbe_split, lat_sbe_split, lat_dbe_split;
    sum res_sbe_sum, res_dbe_sum, lat_sbe_sum;

    sc_signal<double> s1, s2, s3, s4, s5, s7, s8;

    SC_CTOR(DRAM_SEC_TRIM) : I_RES_SBE("I_RES_SBE"), I_RES_DBE("I_RES_DBE"), I_RES_TBE("I_RES_TBE"), I_LAT_SBE("I_LAT_SBE"), I_LAT_DBE("I_LAT_DBE"),
                             O_RES_SBE("O_RES_SBE"), O_RES_DBE("O_RES_DBE"), O_RES_TBE("O_RES_TBE"), O_LAT_SBE("O_LAT_SBE"), O_LAT_DBE("O_LAT_DBE"),
                             res_sbe_split("RES_SBE_SPLIT"), res_dbe_split("RES_DBE_SPLIT"), res_tbe_split("RES_TBE_SPLIT"),
                             lat_sbe_split("LAT_SBE_SPLIT"), lat_dbe_split("LAT_DBE_SPLIT"),
                             res_sbe_sum("RES_SBE_SUM"), res_dbe_sum("RES_DBE_SUM"), lat_sbe_sum("LAT_SBE_SUM"),
                             s1("s1"), s2("s2"), s3("s3"), s4("s4"), s5("s5"), s7("s7"), s8("s8")

    {
        res_sbe_split.input(I_RES_SBE);
        res_dbe_split.input(I_RES_DBE);
        res_tbe_split.input(I_RES_TBE);
        lat_sbe_split.input(I_LAT_SBE);
        lat_dbe_split.input(I_LAT_DBE);

        res_sbe_split.outputs.bind(s1, 0.94);

        res_dbe_split.outputs.bind(s2, 0.11);
        res_dbe_split.outputs.bind(s3, 0.89);

        res_tbe_split.outputs.bind(s4, 0.009);
        res_tbe_split.outputs.bind(s5, 0.15);
        res_tbe_split.outputs.bind(O_RES_TBE, 0.83);

        lat_sbe_split.outputs.bind(s7, 0.94);

        lat_dbe_split.outputs.bind(s8, 0.11);
        lat_dbe_split.outputs.bind(O_LAT_DBE, 0.89);

        res_sbe_sum.inputs.bind(s1);
        res_sbe_sum.inputs.bind(s2);
        res_sbe_sum.inputs.bind(s4);
        res_sbe_sum.output(O_RES_SBE);

        res_dbe_sum.inputs.bind(s3);
        res_dbe_sum.inputs.bind(s5);
        res_dbe_sum.output(O_RES_DBE);

        lat_sbe_sum.inputs.bind(s7);
        lat_sbe_sum.inputs.bind(s8);
        lat_sbe_sum.output(O_LAT_SBE);
    }
};

SC_MODULE(DRAM_BUS_TRIM)
{
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_LAT_SBE, I_LAT_DBE;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_LAT_SBE, O_LAT_DBE;

    split res_sbe_split, res_dbe_split, res_tbe_split, lat_sbe_split, lat_dbe_split;
    sum res_sbe_sum, res_dbe_sum, lat_sbe_sum;
    pass res_tbe_pass, lat_dbe_pass;

    sc_signal<double> s1, s2, s3, s4, s5, s6, s7, s8, s9;

    SC_CTOR(DRAM_BUS_TRIM) : I_RES_SBE("I_RES_SBE"), I_RES_DBE("I_RES_DBE"), I_RES_TBE("I_RES_TBE"), I_LAT_SBE("I_LAT_SBE"), I_LAT_DBE("I_LAT_DBE"),
                             O_RES_SBE("O_RES_SBE"), O_RES_DBE("O_RES_DBE"), O_RES_TBE("O_RES_TBE"), O_LAT_SBE("O_LAT_SBE"), O_LAT_DBE("O_LAT_DBE"),
                             res_sbe_split("RES_SBE_SPLIT"), res_dbe_split("RES_DBE_SPLIT"), res_tbe_split("RES_TBE_SPLIT"),
                             lat_sbe_split("LAT_SBE_SPLIT"), lat_dbe_split("LAT_DBE_SPLIT"),
                             res_sbe_sum("RES_SBE_SUM"), res_dbe_sum("RES_DBE_SUM"), lat_sbe_sum("LAT_SBE_SUM"),
                             res_tbe_pass("RES_TBE_PASS"), lat_dbe_pass("LAT_DBE_PASS"),
                             s1("s1"), s2("s2"), s3("s3"), s4("s4"), s5("s5"), s6("s6"), s7("s7"), s8("s8"), s9("s9")

    {
        res_sbe_split.input(I_RES_SBE);
        res_dbe_split.input(I_RES_DBE);
        res_tbe_split.input(I_RES_TBE);
        lat_sbe_split.input(I_LAT_SBE);
        lat_dbe_split.input(I_LAT_DBE);

        res_sbe_split.outputs.bind(s1, 0.438);

        res_dbe_split.outputs.bind(s2, 0.496);
        res_dbe_split.outputs.bind(s3, 0.314);

        res_tbe_split.outputs.bind(s4, 0.325);
        res_tbe_split.outputs.bind(s5, 0.419);
        res_tbe_split.outputs.bind(s6, 0.175);

        lat_sbe_split.outputs.bind(s7, 0.438);

        lat_dbe_split.outputs.bind(s8, 0.496);
        lat_dbe_split.outputs.bind(s9, 0.314);

        res_sbe_sum.inputs.bind(s1);
        res_sbe_sum.inputs.bind(s2);
        res_sbe_sum.inputs.bind(s4);
        res_sbe_sum.output.bind(O_RES_SBE);

        res_dbe_sum.inputs.bind(s3);
        res_dbe_sum.inputs.bind(s5);
        res_dbe_sum.output.bind(O_RES_DBE);

        lat_sbe_sum.inputs(s7);
        lat_sbe_sum.inputs(s8);
        lat_sbe_sum.output.bind(O_LAT_SBE);

        res_tbe_pass.input(s6);
        res_tbe_pass.output(O_RES_TBE);

        lat_dbe_pass.input(s9);
        lat_dbe_pass.output(O_LAT_DBE);
    }
};

SC_MODULE(DRAM_SEC_DED) 
{
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_LAT_SBE, I_LAT_DBE;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_RES_MBE, O_LAT_SBE, O_LAT_DBE, O_LAT_TBE;

    coverage res_sbe_cov, res_dbe_cov, res_tbe_cov, lat_sbe_cov, lat_dbe_cov;
    split res_tbe_split;
    sum lat_sbe_sum, lat_dbe_sum;
    pass lat_tbe_pass;
    sc_signal<double> s1, s2, s3, s4, s5;

    SC_CTOR(DRAM_SEC_DED) : I_RES_SBE("I_RES_SBE"), I_RES_DBE("I_RES_DBE"), I_RES_TBE("I_RES_TBE"), I_LAT_SBE("I_LAT_SBE"), I_LAT_DBE("I_LAT_DBE"),
                            O_RES_SBE("O_RES_SBE"), O_RES_DBE("O_RES_DBE"), O_RES_TBE("O_RES_TBE"), O_RES_MBE("O_RES_MBE"), O_LAT_SBE("O_LAT_SBE"), O_LAT_DBE("O_LAT_DBE"), O_LAT_TBE("O_LAT_TBE"),
                            res_sbe_cov("RES_SBE_COV", 1.0), res_dbe_cov("RES_DBE_COV", 1.0), res_tbe_cov("RES_TBE_COV",1.0),
                            lat_sbe_cov("LAT_SBE_COV", 1.0), lat_dbe_cov("LAT_DBE_COV", 1.0),
                            res_tbe_split("RES_TBE_SPLIT"),
                            lat_sbe_sum("LAT_SBE_SUM"), lat_dbe_sum("LAT_DBE_SUM"),
                            lat_tbe_pass("LAT_TBE_PASS"),
                            s1("s1"), s2("s2"), s3("s3"), s4("s4"), s5("s5")
    {
        res_sbe_cov.input(I_RES_SBE);
        res_dbe_cov.input(I_RES_DBE);
        res_tbe_split.input(I_RES_TBE);

        lat_sbe_cov.input(I_LAT_SBE);
        lat_dbe_cov.input(I_LAT_DBE);

        res_sbe_cov.output(O_RES_SBE);
        res_dbe_cov.output(O_RES_DBE);
        res_tbe_cov.output(O_RES_TBE);

        res_tbe_split.outputs.bind(s1, 0.44);
        res_tbe_split.outputs.bind(O_RES_MBE, 0.56);

        res_tbe_cov.input(s1);
        lat_sbe_cov.output.bind(s2);
        lat_dbe_cov.output.bind(s3);
        
        res_sbe_cov.latent.bind(s4);
        res_dbe_cov.latent.bind(s5);

        lat_sbe_sum.inputs.bind(s2);
        lat_sbe_sum.inputs.bind(s4);
        lat_sbe_sum.output.bind(O_LAT_SBE);

        lat_dbe_sum.inputs.bind(s3);
        lat_dbe_sum.inputs.bind(s5);
        lat_dbe_sum.output.bind(O_LAT_DBE);

        lat_tbe_pass.input(I_RES_TBE);
        lat_tbe_pass.output(O_LAT_TBE);
    }
};

SC_MODULE(DRAM_SEC_DED_TRIM) 
{
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_RES_MBE, I_LAT_SBE, I_LAT_DBE, I_LAT_TBE;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_RES_MBE, O_LAT_SBE, O_LAT_DBE, O_LAT_TBE;

    split res_sbe_split, res_dbe_split, res_tbe_split, lat_sbe_split, lat_dbe_split, lat_tbe_split;
    sum res_sbe_sum, res_dbe_sum, lat_sbe_sum, lat_dbe_sum;
    pass res_tbe_pass, res_mbe_pass, lat_tbe_pass;

    sc_signal<double> s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;

    SC_CTOR(DRAM_SEC_DED_TRIM) : I_RES_SBE("I_RES_SBE"), I_RES_DBE("I_RES_DBE"), I_RES_TBE("I_RES_TBE"), I_RES_MBE("I_RES_MBE"), I_LAT_SBE("I_LAT_SBE"), I_LAT_DBE("I_LAT_DBE"), I_LAT_TBE("I_LAT_TBE"),
                                 O_RES_SBE("O_RES_SBE"), O_RES_DBE("O_RES_DBE"), O_RES_TBE("O_RES_TBE"), O_RES_MBE("O_RES_MBE"), O_LAT_SBE("O_LAT_SBE"), O_LAT_DBE("O_LAT_DBE"), O_LAT_TBE("O_LAT_TBE"),
                                 res_sbe_split("RES_SBE_SPLIT"), res_dbe_split("RES_DBE_SPLIT"), res_tbe_split("RES_TBE_SPLIT"),
                                 lat_sbe_split("LAT_SBE_SPLIT"), lat_dbe_split("LAT_DBE_SPLIT"), lat_tbe_split("LAT_TBE_SPLIT"),
                                 res_sbe_sum("RES_SBE_SUM"), res_dbe_sum("RES_DBE_SUM"), lat_sbe_sum("LAT_SBE_SUM"), lat_dbe_sum("LAT_DBE_SUM"),
                                 res_tbe_pass("RES_TBE_PASS"), res_mbe_pass("RES_MBE_PASS"), lat_tbe_pass("LAT_TBE_PASS"),
                                 s0("s0"), s1("s1"), s2("s2"), s3("s3"), s4("s4"), s5("s5"), s6("s6"), s7("s7"), s8("s8"), s9("s9"), s10("s10"), s11("s11")
    {
        res_sbe_split.input(I_RES_SBE);
        res_dbe_split.input(I_RES_DBE);
        res_tbe_split.input(I_RES_TBE);
        lat_sbe_split.input(I_LAT_SBE);
        lat_dbe_split.input(I_LAT_DBE);
        lat_tbe_split.input(I_LAT_TBE);

        res_sbe_split.outputs.bind(s0,0.89);

        res_dbe_split.outputs.bind(s1,0.20);
        res_dbe_split.outputs.bind(s2,0.79);

        res_tbe_split.outputs.bind(s3,0.03);
        res_tbe_split.outputs.bind(s4,0.27);
        res_tbe_split.outputs.bind(s5,0.70);

        lat_sbe_split.outputs.bind(s6,0.89);

        lat_dbe_split.outputs.bind(s7,0.20);
        lat_dbe_split.outputs.bind(s8,0.79);

        lat_tbe_split.outputs.bind(s9,0.03);
        lat_tbe_split.outputs.bind(s10,0.27);
        lat_tbe_split.outputs.bind(s11,0.70);

        res_sbe_sum.inputs.bind(s0);
        res_sbe_sum.inputs.bind(s1);
        res_sbe_sum.inputs.bind(s3);
        res_sbe_sum.output.bind(O_RES_SBE);

        res_dbe_sum.inputs.bind(s2);
        res_dbe_sum.inputs.bind(s4);
        res_dbe_sum.output.bind(O_RES_DBE);

        lat_sbe_sum.inputs.bind(s6);
        lat_sbe_sum.inputs.bind(s7);
        lat_sbe_sum.inputs.bind(s9);
        lat_sbe_sum.output.bind(O_LAT_SBE);

        lat_dbe_sum.inputs.bind(s8);
        lat_dbe_sum.inputs.bind(s10);
        lat_dbe_sum.output.bind(O_LAT_DBE);

        res_tbe_pass.input(s5);
        res_tbe_pass.output(O_RES_TBE);

        res_mbe_pass.input(I_RES_MBE);
        res_mbe_pass.output(O_RES_MBE);

        lat_tbe_pass.input(s11);
        lat_tbe_pass.output(O_LAT_TBE);
    }
};

int sc_main (int __attribute__((unused)) sc_argc, char __attribute__((unused)) *sc_argv[])
{
    DRAM dram("DRAM");
    DRAM_SEC_ECC sec_ecc("DRAM_SEC_ECC");
    DRAM_SEC_TRIM sec_trim("DRAM_SEC_TRIM");
    DRAM_BUS_TRIM bus_trim("DRAM_BUS_TRIM");
    DRAM_SEC_DED sec_ded("DRAM_SEC_DED");
    DRAM_SEC_DED_TRIM sec_ded_trim("DRAM_SEC_DED_TRIM");
    sum residual("RESIDUAL"), latent("LATENT");
    asil calculate_asil("ASIL");

    sc_signal<double> a1("a1"), a2("a2");

    dram.SBE.bind(a1);
    dram.DBE.bind(a2);

    sec_ecc.I_SBE.bind(a1);
    sec_ecc.I_DBE.bind(a2);

    sc_signal<double> b1("b1"), b2("b2"), b3("b3"), b4("b4"), b5("b5");

    sec_ecc.O_RES_SBE.bind(b1);
    sec_ecc.O_RES_DBE.bind(b2);
    sec_ecc.O_RES_TBE.bind(b3);
    sec_ecc.O_LAT_SBE.bind(b4);
    sec_ecc.O_LAT_DBE.bind(b5);

    sec_trim.I_RES_SBE.bind(b1);
    sec_trim.I_RES_DBE.bind(b2);
    sec_trim.I_RES_TBE.bind(b3);
    sec_trim.I_LAT_SBE.bind(b4);
    sec_trim.I_LAT_DBE.bind(b5);

    sc_signal<double> c1("c1"), c2("c2"), c3("c3"), c4("c4"), c5("c5");

    sec_trim.O_RES_SBE.bind(c1);
    sec_trim.O_RES_DBE.bind(c2);
    sec_trim.O_RES_TBE.bind(c3);
    sec_trim.O_LAT_SBE.bind(c4);
    sec_trim.O_LAT_DBE.bind(c5);

    bus_trim.I_RES_SBE.bind(c1);
    bus_trim.I_RES_DBE.bind(c2);
    bus_trim.I_RES_TBE.bind(c3);
    bus_trim.I_LAT_SBE.bind(c4);
    bus_trim.I_LAT_DBE.bind(c5);

    sc_signal<double> d1("d1"), d2("d2"), d3("d3"), d4("d4"), d5("d5");

    bus_trim.O_RES_SBE.bind(d1);
    bus_trim.O_RES_DBE.bind(d2);
    bus_trim.O_RES_TBE.bind(d3);
    bus_trim.O_LAT_SBE.bind(d4);
    bus_trim.O_LAT_DBE.bind(d5);

    sec_ded.I_RES_SBE.bind(d1);
    sec_ded.I_RES_DBE.bind(d2);
    sec_ded.I_RES_TBE.bind(d3);
    sec_ded.I_LAT_SBE.bind(d4);
    sec_ded.I_LAT_DBE.bind(d5);

    sc_signal<double> e1("e1"), e2("e2"), e3("e3"), e4("e4"), e5("e5"), e6("e6"), e7("e7");

    sec_ded.O_RES_SBE.bind(e1);
    sec_ded.O_RES_DBE.bind(e2);
    sec_ded.O_RES_TBE.bind(e3);
    sec_ded.O_RES_MBE.bind(e4);
    sec_ded.O_LAT_SBE.bind(e5);
    sec_ded.O_LAT_DBE.bind(e6);
    sec_ded.O_LAT_TBE.bind(e7);

    sec_ded_trim.I_RES_SBE.bind(e1);
    sec_ded_trim.I_RES_DBE.bind(e2);
    sec_ded_trim.I_RES_TBE.bind(e3);
    sec_ded_trim.I_RES_MBE.bind(e4);
    sec_ded_trim.I_LAT_SBE.bind(e5);
    sec_ded_trim.I_LAT_DBE.bind(e6);
    sec_ded_trim.I_LAT_TBE.bind(e7);

    sc_signal<double> f1("f1"), f2("f2"), f3("f3"), f4("f4"), f5("f5"), f6("f6"), f7("f7");

    sec_ded_trim.O_RES_SBE.bind(f1);
    sec_ded_trim.O_RES_DBE.bind(f2);
    sec_ded_trim.O_RES_TBE.bind(f3);
    sec_ded_trim.O_RES_MBE.bind(f4);
    sec_ded_trim.O_LAT_SBE.bind(f5);
    sec_ded_trim.O_LAT_DBE.bind(f6);
    sec_ded_trim.O_LAT_TBE.bind(f7);

    residual.inputs.bind(f1);
    residual.inputs.bind(f2);
    residual.inputs.bind(f3);
    residual.inputs.bind(f4);

    latent.inputs.bind(f5);
    latent.inputs.bind(f6);
    latent.inputs.bind(f7);

    sc_signal<double> latent_result("latent_result"), residual_result("residual_result");

    residual.output.bind(residual_result);
    latent.output.bind(latent_result);

    calculate_asil.residual.bind(residual_result);
    calculate_asil.latent.bind(latent_result);

    sc_start();

    std::cout << "B: RES_SBE: " << b1 << std::endl;
    std::cout << "B: RES_DBE: " << b2 << std::endl;
    std::cout << "B: RES_TBE: " << b3 << std::endl;
    std::cout << "B: LAT_SBE: " << b4 << std::endl;
    std::cout << "B: LAT_DBE: " << b5 << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "C: RES_SBE: " << c1 << std::endl;
    std::cout << "C: RES_DBE: " << c2 << std::endl;
    std::cout << "C: RES_TBE: " << c3 << std::endl;
    std::cout << "C: LAT_SBE: " << c4 << std::endl;
    std::cout << "C: LAT_DBE: " << c5 << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "D: RES_SBE: " << d1 << std::endl;
    std::cout << "D: RES_DBE: " << d2 << std::endl;
    std::cout << "D: RES_TBE: " << d3 << std::endl;
    std::cout << "D: LAT_SBE: " << d4 << std::endl;
    std::cout << "D: LAT_DBE: " << d5 << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "E: RES_SBE: " << e1 << std::endl;
    std::cout << "E: RES_DBE: " << e2 << std::endl;
    std::cout << "E: RES_TBE: " << e3 << std::endl;
    std::cout << "E: RES_MBE: " << e4 << std::endl;
    std::cout << "E: LAT_SBE: " << e5 << std::endl;
    std::cout << "E: LAT_DBE: " << e6 << std::endl;
    std::cout << "E: LAT_TBE: " << e7 << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "F: RES_SBE: " << f1 << std::endl;
    std::cout << "F: RES_DBE: " << f2 << std::endl;
    std::cout << "F: RES_TBE: " << f3 << std::endl;
    std::cout << "F: RES_MBE: " << f4 << std::endl;
    std::cout << "F: LAT_SBE: " << f5 << std::endl;
    std::cout << "F: LAT_DBE: " << f6 << std::endl;
    std::cout << "F: LAT_TBE: " << f7 << std::endl;
    std::cout << "------------------------------ " << std::endl;

    //RES:  0.29685
    //LAT:  15.7815
    //SPFM:  99.8665%
    //LFM: 92.903%
    //ASIL: ASIL-D

    return 0;
}
