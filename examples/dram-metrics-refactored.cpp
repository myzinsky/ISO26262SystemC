/*
 * Copyright (c) 2024, Fraunhofer IESE
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
 *    Derek Christ
 */

#include <sc_hw_metrics.h>

#include <iostream>
#include <systemc>

using namespace sc_hw_metrics;
using namespace sc_core;

SC_MODULE(DRAM)
{
    sc_out<double> SBE, DBE, MBE, WD;
    double E_SBE, E_DBE, E_MBE, E_WD;

    DRAM(const sc_module_name& name, double DRAM_FIT) :
        sc_module(name),
        E_SBE(0.7 * DRAM_FIT),
        E_DBE(0.0748 * DRAM_FIT),
        E_MBE(0.0748 * DRAM_FIT),
        E_WD(0.0748 * DRAM_FIT),
        SBE("SBE"),
        DBE("DBE"),
        MBE("MBE"),
        WD("WD")
    {
        SC_METHOD(compute);
    }

    void compute()
    {
        SBE.write(E_SBE);
        DBE.write(E_DBE);
        MBE.write(E_MBE);
        WD.write(E_WD);
    }
};

SC_MODULE(DRAM_SEC_ECC)
{

    sc_in<double> I_SBE, I_DBE, I_MBE, I_WD;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_LAT_SBE, O_LAT_DBE, O_LAT_SEC_BROKEN,
        O_RES_MBE, O_RES_WD;
    coverage sec_coverage;
    split sec_split;
    pass sec_pass, mbe_pass, wd_pass;
    basic_event sec_broken;

    DRAM_SEC_ECC(const sc_module_name& name) :
        sc_module(name),
        I_SBE("I_SBE"),
        I_DBE("I_DBE"),
        I_MBE("I_MBE"),
        I_WD("I_WD"),
        O_RES_SBE("O_RES_SBE"),
        O_RES_DBE("O_RES_DBE"),
        O_RES_TBE("O_RES_TBE"),
        O_LAT_SBE("O_LAT_SBE"),
        O_LAT_DBE("O_LAT_DBE"),
        O_LAT_SEC_BROKEN("O_LAT_SEC_BROKEN"),
        O_RES_MBE("O_RES_MBE"),
        O_RES_WD("O_RES_WD"),
        sec_coverage("SEC_Coverage", 1.0),
        sec_split("SEC_split"),
        sec_pass("SEC_PASS"),
        mbe_pass("MBE_PASS"),
        wd_pass("WD_PASS"),
        sec_broken("SEC_BROKEN", 0.1)
    {
        sec_coverage.input(I_SBE);
        sec_coverage.output(O_RES_SBE);
        sec_coverage.latent(O_LAT_SBE);

        sec_split.input(I_DBE);
        sec_split.outputs.bind(O_RES_DBE, 0.83);
        sec_split.outputs.bind(O_RES_TBE, 0.17);

        sec_pass.input(I_DBE);
        sec_pass.output(O_LAT_DBE);

        sec_broken.output(O_LAT_SEC_BROKEN);

        mbe_pass.input(I_MBE);
        mbe_pass.output(O_RES_MBE);

        wd_pass.input(I_WD);
        wd_pass.output(O_RES_WD);
    }
};

SC_MODULE(DRAM_SEC_TRIM)
{
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_LAT_SBE, I_LAT_DBE, I_RES_MBE, I_RES_WD;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_LAT_SBE, O_LAT_DBE, O_RES_MBE, O_RES_WD;

    split res_sbe_split, res_dbe_split, res_tbe_split, lat_sbe_split, lat_dbe_split;
    sum res_sbe_sum, res_dbe_sum, lat_sbe_sum;
    pass mbe_pass, wd_pass;

    sc_signal<double> s1, s2, s3, s4, s5, s7, s8;

    DRAM_SEC_TRIM(const sc_module_name& name) :
        sc_module(name),
        I_RES_SBE("I_RES_SBE"),
        I_RES_DBE("I_RES_DBE"),
        I_RES_TBE("I_RES_TBE"),
        I_LAT_SBE("I_LAT_SBE"),
        I_LAT_DBE("I_LAT_DBE"),
        I_RES_MBE("I_RES_MBE"),
        I_RES_WD("I_RES_WD"),
        O_RES_SBE("O_RES_SBE"),
        O_RES_DBE("O_RES_DBE"),
        O_RES_TBE("O_RES_TBE"),
        O_LAT_SBE("O_LAT_SBE"),
        O_LAT_DBE("O_LAT_DBE"),
        O_RES_MBE("O_RES_MBE"),
        O_RES_WD("O_RES_WD"),
        res_sbe_split("RES_SBE_SPLIT"),
        res_dbe_split("RES_DBE_SPLIT"),
        res_tbe_split("RES_TBE_SPLIT"),
        lat_sbe_split("LAT_SBE_SPLIT"),
        lat_dbe_split("LAT_DBE_SPLIT"),
        res_sbe_sum("RES_SBE_SUM"),
        res_dbe_sum("RES_DBE_SUM"),
        lat_sbe_sum("LAT_SBE_SUM"),
        mbe_pass("MBE_PASS"),
        wd_pass("WD_PASS"),
        s1("s1"),
        s2("s2"),
        s3("s3"),
        s4("s4"),
        s5("s5"),
        s7("s7"),
        s8("s8")

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

        mbe_pass.input(I_RES_MBE);
        mbe_pass.output(O_RES_MBE);

        wd_pass.input(I_RES_WD);
        wd_pass.output(O_RES_WD);
    }
};

SC_MODULE(DRAM_BUS_TRIM)
{
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_LAT_SBE, I_LAT_DBE, I_RES_MBE, I_RES_WD;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_LAT_SBE, O_LAT_DBE, O_RES_MBE, O_RES_WD,
        O_RES_AZ;

    split res_sbe_split, res_dbe_split, res_tbe_split, lat_sbe_split, lat_dbe_split;
    sum res_sbe_sum, res_dbe_sum, lat_sbe_sum, res_mbe_sum;
    pass res_tbe_pass, lat_dbe_pass, res_wd_pass;
    basic_event dq_upset, all_zero;

    sc_signal<double> s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;

    DRAM_BUS_TRIM(const sc_module_name& name, double DRAM_FIT) :
        sc_module(name),
        I_RES_SBE("I_RES_SBE"),
        I_RES_DBE("I_RES_DBE"),
        I_RES_TBE("I_RES_TBE"),
        I_LAT_SBE("I_LAT_SBE"),
        I_LAT_DBE("I_LAT_DBE"),
        I_RES_MBE("I_RES_MBE"),
        I_RES_WD("I_RES_WD"),
        O_RES_SBE("O_RES_SBE"),
        O_RES_DBE("O_RES_DBE"),
        O_RES_TBE("O_RES_TBE"),
        O_LAT_SBE("O_LAT_SBE"),
        O_LAT_DBE("O_LAT_DBE"),
        O_RES_MBE("O_RES_MBE"),
        O_RES_WD("O_RES_WD"),
        O_RES_AZ("O_RES_AZ"),
        res_sbe_split("RES_SBE_SPLIT"),
        res_dbe_split("RES_DBE_SPLIT"),
        res_tbe_split("RES_TBE_SPLIT"),
        lat_sbe_split("LAT_SBE_SPLIT"),
        lat_dbe_split("LAT_DBE_SPLIT"),
        res_sbe_sum("RES_SBE_SUM"),
        res_dbe_sum("RES_DBE_SUM"),
        lat_sbe_sum("LAT_SBE_SUM"),
        res_mbe_sum("RES_MBE_SUM"),
        res_tbe_pass("RES_TBE_PASS"),
        lat_dbe_pass("LAT_DBE_PASS"),
        res_wd_pass("res_wd_pass"),
        s1("s1"),
        s2("s2"),
        s3("s3"),
        s4("s4"),
        s5("s5"),
        s6("s6"),
        s7("s7"),
        s8("s8"),
        s9("s9"),
        s10("s10"),
        dq_upset("DQ_UPSET", 0.001 * DRAM_FIT),
        all_zero("ALL_ZERO", 0.0748 * DRAM_FIT)
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

        dq_upset.output(s10);

        res_mbe_sum.inputs(I_RES_MBE);
        res_mbe_sum.inputs(s10);
        res_mbe_sum.output(O_RES_MBE);

        res_wd_pass.input(I_RES_WD);
        res_wd_pass.output(O_RES_WD);

        all_zero.output(O_RES_AZ);
    }
};

SC_MODULE(DRAM_SEC_DED)
{
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_LAT_SBE, I_LAT_DBE, I_RES_MBE, I_RES_WD,
        I_RES_AZ;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_RES_MBE, O_LAT_SBE, O_LAT_DBE, O_LAT_TBE,
        O_LAT_MBE, O_LAT_SEC_DED_BROKEN, O_RES_WD, O_RES_AZ;

    coverage res_sbe_cov, res_dbe_cov, res_tbe_cov, lat_sbe_cov, lat_dbe_cov, res_mbe_cov;
    split res_tbe_split;
    sum lat_sbe_sum, lat_dbe_sum, res_mbe_sum;
    pass lat_tbe_pass, res_wd_pass, res_az_pass;
    basic_event sec_ded_broken;
    sc_signal<double> s1, s2, s3, s4, s5, s6, s7;

    SC_CTOR(DRAM_SEC_DED) :
        I_RES_SBE("I_RES_SBE"),
        I_RES_DBE("I_RES_DBE"),
        I_RES_TBE("I_RES_TBE"),
        I_LAT_SBE("I_LAT_SBE"),
        I_LAT_DBE("I_LAT_DBE"),
        I_RES_MBE("I_RES_MBE"),
        I_RES_WD("I_RES_WD"),
        I_RES_AZ("I_RES_AZ"),
        O_RES_SBE("O_RES_SBE"),
        O_RES_DBE("O_RES_DBE"),
        O_RES_TBE("O_RES_TBE"),
        O_RES_MBE("O_RES_MBE"),
        O_LAT_SBE("O_LAT_SBE"),
        O_LAT_DBE("O_LAT_DBE"),
        O_LAT_TBE("O_LAT_TBE"),
        O_LAT_MBE("O_LAT_MBE"),
        O_LAT_SEC_DED_BROKEN("O_LAT_SEC_DED_BROKEN"),
        O_RES_WD("O_RES_WD"),
        O_RES_AZ("O_RES_AZ"),
        res_sbe_cov("RES_SBE_COV", 1.0),
        res_dbe_cov("RES_DBE_COV", 1.0),
        res_tbe_cov("RES_TBE_COV", 1.0),
        res_mbe_cov("RES_MBE_COV", 0.5),
        lat_sbe_cov("LAT_SBE_COV", 1.0),
        lat_dbe_cov("LAT_DBE_COV", 1.0),
        res_tbe_split("RES_TBE_SPLIT"),
        lat_sbe_sum("LAT_SBE_SUM"),
        lat_dbe_sum("LAT_DBE_SUM"),
        res_mbe_sum("RES_MBE_SUM"),
        lat_tbe_pass("LAT_TBE_PASS"),
        res_wd_pass("RES_WD_PASS"),
        res_az_pass("RES_AZ_PASS"),
        s1("s1"),
        s2("s2"),
        s3("s3"),
        s4("s4"),
        s5("s5"),
        s6("s6"),
        s7("s7"),
        sec_ded_broken("SEC_DED_BROKEN", 0.1)
    {
        res_sbe_cov.input(I_RES_SBE);
        res_dbe_cov.input(I_RES_DBE);
        res_tbe_split.input(I_RES_TBE);
        res_mbe_cov.input(I_RES_MBE);

        lat_sbe_cov.input(I_LAT_SBE);
        lat_dbe_cov.input(I_LAT_DBE);

        res_sbe_cov.output(O_RES_SBE);
        res_dbe_cov.output(O_RES_DBE);
        res_tbe_cov.output(O_RES_TBE);
        res_mbe_cov.output(s7);
        res_mbe_cov.latent(O_LAT_MBE);

        res_tbe_split.outputs.bind(s1, 0.44);
        res_tbe_split.outputs.bind(s6, 0.56);

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

        sec_ded_broken.output(O_LAT_SEC_DED_BROKEN);

        res_mbe_sum.inputs(s6);
        res_mbe_sum.inputs(s7);
        res_mbe_sum.output(O_RES_MBE);

        res_wd_pass.input(I_RES_WD);
        res_wd_pass.output(O_RES_WD);

        res_az_pass.input(I_RES_AZ);
        res_az_pass.output(O_RES_AZ);
    }
};

SC_MODULE(DRAM_SEC_DED_TRIM)
{
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_RES_MBE, I_LAT_SBE, I_LAT_DBE, I_LAT_TBE,
        I_LAT_MBE, I_RES_WD, I_RES_AZ;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_RES_MBE, O_LAT_SBE, O_LAT_DBE, O_LAT_TBE,
        O_LAT_MBE, O_RES_WD, O_RES_AZ;

    split res_sbe_split, res_dbe_split, res_tbe_split, lat_sbe_split, lat_dbe_split, lat_tbe_split;
    sum res_sbe_sum, res_dbe_sum, lat_sbe_sum, lat_dbe_sum;
    pass res_tbe_pass, res_mbe_pass, lat_tbe_pass, res_wd_pass, res_az_pass, lat_mbe_pass;

    sc_signal<double> s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;

    SC_CTOR(DRAM_SEC_DED_TRIM) :
        I_RES_SBE("I_RES_SBE"),
        I_RES_DBE("I_RES_DBE"),
        I_RES_TBE("I_RES_TBE"),
        I_RES_MBE("I_RES_MBE"),
        I_LAT_SBE("I_LAT_SBE"),
        I_LAT_DBE("I_LAT_DBE"),
        I_LAT_TBE("I_LAT_TBE"),
        I_LAT_MBE("I_LAT_MBE"),
        I_RES_WD("I_RES_WD"),
        I_RES_AZ("I_RES_AZ"),
        O_RES_SBE("O_RES_SBE"),
        O_RES_DBE("O_RES_DBE"),
        O_RES_TBE("O_RES_TBE"),
        O_RES_MBE("O_RES_MBE"),
        O_LAT_SBE("O_LAT_SBE"),
        O_LAT_DBE("O_LAT_DBE"),
        O_LAT_TBE("O_LAT_TBE"),
        O_LAT_MBE("O_LAT_MBE"),
        O_RES_WD("O_RES_WD"),
        O_RES_AZ("O_RES_AZ"),
        res_sbe_split("RES_SBE_SPLIT"),
        res_dbe_split("RES_DBE_SPLIT"),
        res_tbe_split("RES_TBE_SPLIT"),
        lat_sbe_split("LAT_SBE_SPLIT"),
        lat_dbe_split("LAT_DBE_SPLIT"),
        lat_tbe_split("LAT_TBE_SPLIT"),
        res_sbe_sum("RES_SBE_SUM"),
        res_dbe_sum("RES_DBE_SUM"),
        lat_sbe_sum("LAT_SBE_SUM"),
        lat_dbe_sum("LAT_DBE_SUM"),
        res_tbe_pass("RES_TBE_PASS"),
        res_mbe_pass("RES_MBE_PASS"),
        lat_tbe_pass("LAT_TBE_PASS"),
        res_wd_pass("RES_WD_PASS"),
        res_az_pass("RES_AZ_PASS"),
        lat_mbe_pass("LAT_MBE_PASS"),
        s0("s0"),
        s1("s1"),
        s2("s2"),
        s3("s3"),
        s4("s4"),
        s5("s5"),
        s6("s6"),
        s7("s7"),
        s8("s8"),
        s9("s9"),
        s10("s10"),
        s11("s11")
    {
        res_sbe_split.input(I_RES_SBE);
        res_dbe_split.input(I_RES_DBE);
        res_tbe_split.input(I_RES_TBE);
        lat_sbe_split.input(I_LAT_SBE);
        lat_dbe_split.input(I_LAT_DBE);
        lat_tbe_split.input(I_LAT_TBE);

        res_sbe_split.outputs.bind(s0, 0.89);

        res_dbe_split.outputs.bind(s1, 0.20);
        res_dbe_split.outputs.bind(s2, 0.79);

        res_tbe_split.outputs.bind(s3, 0.03);
        res_tbe_split.outputs.bind(s4, 0.27);
        res_tbe_split.outputs.bind(s5, 0.70);

        lat_sbe_split.outputs.bind(s6, 0.89);

        lat_dbe_split.outputs.bind(s7, 0.20);
        lat_dbe_split.outputs.bind(s8, 0.79);

        lat_tbe_split.outputs.bind(s9, 0.03);
        lat_tbe_split.outputs.bind(s10, 0.27);
        lat_tbe_split.outputs.bind(s11, 0.70);

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

        res_wd_pass.input(I_RES_WD);
        res_wd_pass.output(O_RES_WD);

        res_az_pass.input(I_RES_AZ);
        res_az_pass.output(O_RES_AZ);

        lat_mbe_pass.input(I_LAT_MBE);
        lat_mbe_pass.output(O_LAT_MBE);
    }
};

SC_MODULE(ALL_OTHER_COMPONENTS)
{
    sc_out<double> OTHER_RES, OTHER_LAT;
    basic_event all_other;
    sc_signal<double> s0, s1, s2;
    split other_split;
    coverage other_cov;
    coverage other_cov_lat;

    SC_HAS_PROCESS(ALL_OTHER_COMPONENTS);
    ALL_OTHER_COMPONENTS(sc_module_name name, double OTHER_COMPONENTS) :
        s0("s0"),
        s1("s1"),
        s2("s2"),
        OTHER_RES("OTHER_RES"),
        OTHER_LAT("OTHER_LAT"),
        other_split("OTHER_SPLIT"),
        other_cov("OTHER_COV", 0.9),
        other_cov_lat("OTHER_COV_LAT", 1.0),
        all_other("ALL_OTHER", OTHER_COMPONENTS)
    {
        all_other.output.bind(s0);
        other_split.input(s0);
        other_cov.input.bind(s1);
        other_split.outputs.bind(s1, 0.5);
        other_cov.output(OTHER_RES);
        other_cov.latent(s2);
        other_cov_lat.input.bind(s2);
        other_cov_lat.output(OTHER_LAT);
    }
};

int sc_main(int __attribute__((unused)) sc_argc, char __attribute__((unused)) * sc_argv[])
{
    double DRAM_FIT = 2300.0;
    double OTHER_COMPONENTS = 1920.0;
    DRAM dram("DRAM", DRAM_FIT);
    DRAM_SEC_ECC sec_ecc("DRAM_SEC_ECC");
    DRAM_SEC_TRIM sec_trim("DRAM_SEC_TRIM");
    DRAM_BUS_TRIM bus_trim("DRAM_BUS_TRIM", DRAM_FIT);
    DRAM_SEC_DED sec_ded("DRAM_SEC_DED");
    DRAM_SEC_DED_TRIM sec_ded_trim("DRAM_SEC_DED_TRIM");
    ALL_OTHER_COMPONENTS all_other_components("ALL_OTHER_COMPONENTS", OTHER_COMPONENTS);
    sum residual("RESIDUAL"), latent("LATENT");
    asil calculate_asil("ASIL", DRAM_FIT + OTHER_COMPONENTS);

    sc_signal<double> a1("a1"), a2("a2"), a3("a3"), a4("a4");

    dram.SBE.bind(a1);
    dram.DBE.bind(a2);
    dram.MBE.bind(a3);
    dram.WD.bind(a4);

    sec_ecc.I_SBE.bind(a1);
    sec_ecc.I_DBE.bind(a2);
    sec_ecc.I_MBE.bind(a3);
    sec_ecc.I_WD.bind(a4);

    sc_signal<double> b1("b1"), b2("b2"), b3("b3"), b4("b4"), b5("b5"), b6("b6"), b7("b7"),
        b8("b8");

    sec_ecc.O_RES_SBE.bind(b1);
    sec_ecc.O_RES_DBE.bind(b2);
    sec_ecc.O_RES_TBE.bind(b3);
    sec_ecc.O_LAT_SBE.bind(b4);
    sec_ecc.O_LAT_DBE.bind(b5);
    sec_ecc.O_LAT_SEC_BROKEN.bind(b6);
    sec_ecc.O_RES_MBE.bind(b7);
    sec_ecc.O_RES_WD.bind(b8);

    sec_trim.I_RES_SBE.bind(b1);
    sec_trim.I_RES_DBE.bind(b2);
    sec_trim.I_RES_TBE.bind(b3);
    sec_trim.I_LAT_SBE.bind(b4);
    sec_trim.I_LAT_DBE.bind(b5);
    sec_trim.I_RES_MBE.bind(b7);
    sec_trim.I_RES_WD.bind(b8);

    sc_signal<double> c1("c1"), c2("c2"), c3("c3"), c4("c4"), c5("c5"), c6("c6"), c7("c7");

    sec_trim.O_RES_SBE.bind(c1);
    sec_trim.O_RES_DBE.bind(c2);
    sec_trim.O_RES_TBE.bind(c3);
    sec_trim.O_LAT_SBE.bind(c4);
    sec_trim.O_LAT_DBE.bind(c5);
    sec_trim.O_RES_MBE.bind(c6);
    sec_trim.O_RES_WD.bind(c7);

    bus_trim.I_RES_SBE.bind(c1);
    bus_trim.I_RES_DBE.bind(c2);
    bus_trim.I_RES_TBE.bind(c3);
    bus_trim.I_LAT_SBE.bind(c4);
    bus_trim.I_LAT_DBE.bind(c5);
    bus_trim.I_RES_MBE.bind(c6);
    bus_trim.I_RES_WD.bind(c7);

    sc_signal<double> d1("d1"), d2("d2"), d3("d3"), d4("d4"), d5("d5"), d6("d6"), d7("d7"),
        d8("d8"), d9("d9");

    bus_trim.O_RES_SBE.bind(d1);
    bus_trim.O_RES_DBE.bind(d2);
    bus_trim.O_RES_TBE.bind(d3);
    bus_trim.O_LAT_SBE.bind(d4);
    bus_trim.O_LAT_DBE.bind(d5);
    bus_trim.O_RES_MBE.bind(d7);
    bus_trim.O_RES_WD.bind(d8);
    bus_trim.O_RES_AZ.bind(d9);

    sec_ded.I_RES_SBE.bind(d1);
    sec_ded.I_RES_DBE.bind(d2);
    sec_ded.I_RES_TBE.bind(d3);
    sec_ded.I_LAT_SBE.bind(d4);
    sec_ded.I_LAT_DBE.bind(d5);
    sec_ded.O_LAT_SEC_DED_BROKEN.bind(d6);
    sec_ded.I_RES_MBE.bind(d7);
    sec_ded.I_RES_WD.bind(d8);
    sec_ded.I_RES_AZ.bind(d9);

    sc_signal<double> e1("e1"), e2("e2"), e3("e3"), e4("e4"), e5("e5"), e6("e6"), e7("e7"),
        e8("e8"), e9("e9"), e10("e10");

    sec_ded.O_RES_SBE.bind(e1);
    sec_ded.O_RES_DBE.bind(e2);
    sec_ded.O_RES_TBE.bind(e3);
    sec_ded.O_RES_MBE.bind(e4);
    sec_ded.O_LAT_SBE.bind(e5);
    sec_ded.O_LAT_DBE.bind(e6);
    sec_ded.O_LAT_TBE.bind(e7);
    sec_ded.O_RES_WD.bind(e8);
    sec_ded.O_RES_AZ.bind(e9);
    sec_ded.O_LAT_MBE.bind(e10);

    sec_ded_trim.I_RES_SBE.bind(e1);
    sec_ded_trim.I_RES_DBE.bind(e2);
    sec_ded_trim.I_RES_TBE.bind(e3);
    sec_ded_trim.I_RES_MBE.bind(e4);
    sec_ded_trim.I_LAT_SBE.bind(e5);
    sec_ded_trim.I_LAT_DBE.bind(e6);
    sec_ded_trim.I_LAT_TBE.bind(e7);
    sec_ded_trim.I_RES_WD.bind(e8);
    sec_ded_trim.I_RES_AZ.bind(e9);
    sec_ded_trim.I_LAT_MBE.bind(e10);

    sc_signal<double> f1("f1"), f2("f2"), f3("f3"), f4("f4"), f5("f5"), f6("f6"), f7("f7"),
        f8("f8"), f9("f9"), f10("f10"), f11("f11"), f12("f12");

    sec_ded_trim.O_RES_SBE.bind(f1);
    sec_ded_trim.O_RES_DBE.bind(f2);
    sec_ded_trim.O_RES_TBE.bind(f3);
    sec_ded_trim.O_RES_MBE.bind(f4);
    sec_ded_trim.O_LAT_SBE.bind(f5);
    sec_ded_trim.O_LAT_DBE.bind(f6);
    sec_ded_trim.O_LAT_TBE.bind(f7);
    sec_ded_trim.O_RES_WD(f8);
    sec_ded_trim.O_RES_AZ(f9);
    sec_ded_trim.O_LAT_MBE(f10);

    all_other_components.OTHER_RES.bind(f11);
    all_other_components.OTHER_LAT.bind(f12);

    residual.inputs.bind(f1);
    residual.inputs.bind(f2);
    residual.inputs.bind(f3);
    residual.inputs.bind(f4);
    residual.inputs.bind(f8);
    residual.inputs.bind(f9);
    residual.inputs.bind(f11);

    latent.inputs.bind(f5);
    latent.inputs.bind(f6);
    latent.inputs.bind(f7);
    latent.inputs.bind(b6); // SEC Broken
    latent.inputs.bind(d6); // SEC-DED Broken
    latent.inputs.bind(f10);
    latent.inputs.bind(f12);

    sc_signal<double> latent_result("latent_result"), residual_result("residual_result");

    residual.output.bind(residual_result);
    latent.output.bind(latent_result);

    calculate_asil.residual.bind(residual_result);
    calculate_asil.latent.bind(latent_result);

    sc_start();

    std::cout << "A: RES_SBE: " << a1 << std::endl;
    std::cout << "A: RES_DBE: " << a2 << std::endl;
    std::cout << "A: RES_MBE: " << a3 << std::endl;
    std::cout << "A: RES_WD:  " << a4 << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "B: RES_SBE: " << b1 << std::endl;
    std::cout << "B: RES_DBE: " << b2 << std::endl;
    std::cout << "B: RES_TBE: " << b3 << std::endl;
    std::cout << "B: RES_MBE: " << b7 << std::endl;
    std::cout << "B: RES_WD:  " << b8 << std::endl;
    std::cout << "B: LAT_SBE: " << b4 << std::endl;
    std::cout << "B: LAT_DBE: " << b5 << std::endl;
    std::cout << "B: LAT_SEC: " << b6 << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "C: RES_SBE: " << c1 << std::endl;
    std::cout << "C: RES_DBE: " << c2 << std::endl;
    std::cout << "C: RES_TBE: " << c3 << std::endl;
    std::cout << "C: RES_MBE: " << c6 << std::endl;
    std::cout << "C: RES_WD:  " << c7 << std::endl;
    std::cout << "C: LAT_SBE: " << c4 << std::endl;
    std::cout << "C: LAT_DBE: " << c5 << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "D: RES_SBE: " << d1 << std::endl;
    std::cout << "D: RES_DBE: " << d2 << std::endl;
    std::cout << "D: RES_TBE: " << d3 << std::endl;
    std::cout << "D: RES_MBE: " << d7 << std::endl;
    std::cout << "D: RES_WD:  " << d8 << std::endl;
    std::cout << "D: RES_AZ:  " << d9 << std::endl;
    std::cout << "D: LAT_SBE: " << d4 << std::endl;
    std::cout << "D: LAT_DBE: " << d5 << std::endl;
    std::cout << "D: LAT_DED: " << d6 << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "E: RES_SBE: " << e1 << std::endl;
    std::cout << "E: RES_DBE: " << e2 << std::endl;
    std::cout << "E: RES_TBE: " << e3 << std::endl;
    std::cout << "E: RES_MBE: " << e4 << std::endl;
    std::cout << "E: RES_WD:  " << e8 << std::endl;
    std::cout << "E: RES_AZ:  " << e9 << std::endl;
    std::cout << "E: LAT_SBE: " << e5 << std::endl;
    std::cout << "E: LAT_DBE: " << e6 << std::endl;
    std::cout << "E: LAT_TBE: " << e7 << std::endl;
    std::cout << "E: LAT_MBE: " << e10 << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "F: RES_SBE: " << f1 << std::endl;
    std::cout << "F: RES_DBE: " << f2 << std::endl;
    std::cout << "F: RES_TBE: " << f3 << std::endl;
    std::cout << "F: RES_MBE: " << f4 << std::endl;
    std::cout << "F: RES_WD:  " << f8 << std::endl;
    std::cout << "F: RES_AZ:  " << f9 << std::endl;
    std::cout << "F: LAT_SBE: " << f5 << std::endl;
    std::cout << "F: LAT_DBE: " << f6 << std::endl;
    std::cout << "F: LAT_TBE: " << f7 << std::endl;
    std::cout << "F: LAT_MBE: " << f10 << std::endl;
    std::cout << "------------------------------ " << std::endl;
    sc_stop();

    return 0;
}
