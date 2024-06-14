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
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_RES_MBE, O_RES_WD;
    sc_out<double> O_LAT_SBE, O_LAT_SEC_BROKEN;
    coverage sec_coverage;
    split sec_split;
    pass mbe_pass, wd_pass;
    basic_event sec_broken;

    DRAM_SEC_ECC(const sc_module_name& name) :
        I_SBE("I_SBE"),
        I_DBE("I_DBE"),
        I_MBE("I_MBE"),
        I_WD("I_WD"),
        O_RES_SBE("O_RES_SBE"),
        O_RES_DBE("O_RES_DBE"),
        O_RES_TBE("O_RES_TBE"),
        O_LAT_SBE("O_LAT_SBE"),
        O_LAT_SEC_BROKEN("O_LAT_SEC_BROKEN"),
        O_RES_MBE("O_RES_MBE"),
        O_RES_WD("O_RES_WD"),
        sec_coverage("SEC_Coverage", 1.0, 0.0),
        sec_split("SEC_split"),
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

        sec_broken.output(O_LAT_SEC_BROKEN);

        mbe_pass.input(I_MBE);
        mbe_pass.output(O_RES_MBE);

        wd_pass.input(I_WD);
        wd_pass.output(O_RES_WD);
    }
};

SC_MODULE(DRAM_SEC_TRIM)
{
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_RES_MBE, I_RES_WD;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_RES_MBE, O_RES_WD;

    split res_sbe_split, res_dbe_split, res_tbe_split;
    sum res_sbe_sum, res_dbe_sum;
    pass mbe_pass, wd_pass;

    sc_signal<double> s1, s2, s3, s4, s5, s7, s8;

    DRAM_SEC_TRIM(const sc_module_name& name) :
        I_RES_SBE("I_RES_SBE"),
        I_RES_DBE("I_RES_DBE"),
        I_RES_TBE("I_RES_TBE"),
        I_RES_MBE("I_RES_MBE"),
        I_RES_WD("I_RES_WD"),
        O_RES_SBE("O_RES_SBE"),
        O_RES_DBE("O_RES_DBE"),
        O_RES_TBE("O_RES_TBE"),
        O_RES_MBE("O_RES_MBE"),
        O_RES_WD("O_RES_WD"),
        res_sbe_split("RES_SBE_SPLIT"),
        res_dbe_split("RES_DBE_SPLIT"),
        res_tbe_split("RES_TBE_SPLIT"),
        res_sbe_sum("RES_SBE_SUM"),
        res_dbe_sum("RES_DBE_SUM"),
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

        res_sbe_split.outputs.bind(s1, 0.94);

        res_dbe_split.outputs.bind(s2, 0.11);
        res_dbe_split.outputs.bind(s3, 0.89);

        res_tbe_split.outputs.bind(s4, 0.009); // TODO does not add up to one
        res_tbe_split.outputs.bind(s5, 0.15);
        res_tbe_split.outputs.bind(O_RES_TBE, 0.83);

        res_sbe_sum.inputs.bind(s1);
        res_sbe_sum.inputs.bind(s2);
        res_sbe_sum.inputs.bind(s4);
        res_sbe_sum.output(O_RES_SBE);

        res_dbe_sum.inputs.bind(s3);
        res_dbe_sum.inputs.bind(s5);
        res_dbe_sum.output(O_RES_DBE);

        mbe_pass.input(I_RES_MBE);
        mbe_pass.output(O_RES_MBE);

        wd_pass.input(I_RES_WD);
        wd_pass.output(O_RES_WD);
    }
};

SC_MODULE(DRAM_BUS_TRIM)
{
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_RES_MBE, I_RES_WD;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_RES_MBE, O_RES_WD, O_RES_AZ, O_LAT_IF, O_LAT_LB;

    split res_sbe_split, res_dbe_split, res_tbe_split;
    sum res_sbe_sum, res_dbe_sum, res_mbe_sum;
    coverage if_sbe_coverage;
    pass res_tbe_pass, res_wd_pass;
    basic_event if_sbe, link_ecc_broken, all_zero;

    sc_signal<double> s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;

    DRAM_BUS_TRIM(const sc_module_name& name, double DRAM_FIT) :
        I_RES_SBE("I_RES_SBE"),
        I_RES_DBE("I_RES_DBE"),
        I_RES_TBE("I_RES_TBE"),
        I_RES_MBE("I_RES_MBE"),
        I_RES_WD("I_RES_WD"),
        O_RES_SBE("O_RES_SBE"),
        O_RES_DBE("O_RES_DBE"),
        O_RES_TBE("O_RES_TBE"),
        O_RES_MBE("O_RES_MBE"),
        O_RES_WD("O_RES_WD"),
        O_RES_AZ("O_RES_AZ"),
        O_LAT_IF("O_LAT_IF"),
        O_LAT_LB("O_LAT_LB"),
        res_sbe_split("RES_SBE_SPLIT"),
        res_dbe_split("RES_DBE_SPLIT"),
        res_tbe_split("RES_TBE_SPLIT"),
        res_sbe_sum("RES_SBE_SUM"),
        res_dbe_sum("RES_DBE_SUM"),
        res_mbe_sum("RES_MBE_SUM"),
        res_tbe_pass("RES_TBE_PASS"),
        res_wd_pass("res_wd_pass"),
        if_sbe_coverage("IF_SBE_COVERAGE", 1.0, 1.0),
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
        s11("s11"),
        if_sbe("IF_SBE", 5e9),
        link_ecc_broken("LINK_ECC_BROKEN", 0.1),
        all_zero("ALL_ZERO", 0.0748 * DRAM_FIT)
    {
        res_sbe_split.input(I_RES_SBE);
        res_dbe_split.input(I_RES_DBE);
        res_tbe_split.input(I_RES_TBE);

        res_sbe_split.outputs.bind(s1, 0.438);

        res_dbe_split.outputs.bind(s2, 0.496);
        res_dbe_split.outputs.bind(s3, 0.314);

        res_tbe_split.outputs.bind(s4, 0.325);
        res_tbe_split.outputs.bind(s5, 0.419);
        res_tbe_split.outputs.bind(s6, 0.175);

        res_sbe_sum.inputs.bind(s1);
        res_sbe_sum.inputs.bind(s2);
        res_sbe_sum.inputs.bind(s4);
        res_sbe_sum.output.bind(O_RES_SBE);

        res_dbe_sum.inputs.bind(s3);
        res_dbe_sum.inputs.bind(s5);
        res_dbe_sum.output.bind(O_RES_DBE);

        res_tbe_pass.input(s6);
        res_tbe_pass.output(O_RES_TBE);

        if_sbe.output(s10);
        if_sbe_coverage.input(s10);
        if_sbe_coverage.output(s11);
        if_sbe_coverage.latent(O_LAT_IF);

        link_ecc_broken.output(O_LAT_LB);

        res_mbe_sum.inputs(I_RES_MBE);
        res_mbe_sum.inputs(s11);
        res_mbe_sum.output(O_RES_MBE);

        res_wd_pass.input(I_RES_WD);
        res_wd_pass.output(O_RES_WD);

        all_zero.output(O_RES_AZ);
    }
};

SC_MODULE(DRAM_SEC_DED)
{
    sc_in<double> I_RES_SBE{"I_RES_SBE"};
    sc_in<double> I_RES_DBE{"I_RES_DBE"};
    sc_in<double> I_RES_TBE{"I_RES_TBE"};
    sc_in<double> I_RES_MBE{"I_RES_MBE"};
    sc_in<double> I_RES_WD{"I_RES_WD"};
    sc_in<double> I_RES_AZ{"I_RES_AZ"};
    sc_out<double> O_RES_SBE{"O_RES_SBE"};
    sc_out<double> O_RES_DBE{"O_RES_DBE"};
    sc_out<double> O_RES_TBE{"O_RES_TBE"};
    sc_out<double> O_RES_MBE{"O_RES_MBE"};
    sc_out<double> O_LAT_SBE{"O_LAT_SBE"};
    sc_out<double> O_LAT_DBE{"O_LAT_DBE"};
    sc_out<double> O_LAT_TBE{"O_LAT_TBE"};
    sc_out<double> O_LAT_MBE{"O_LAT_MBE"};
    sc_out<double> O_LAT_SEC_DED_BROKEN{"O_LAT_SEC_DED_BROKEN"};
    sc_out<double> O_RES_WD{"O_RES_WD"};
    sc_out<double> O_RES_AZ{"O_RES_AZ"};

    coverage res_sbe_cov, res_dbe_cov, res_tbe_cov, res_mbe_cov;
    split res_tbe_split;
    sum res_mbe_sum;
    pass res_wd_pass, res_az_pass;
    basic_event sec_ded_broken;
    sc_signal<double> s1, s2, s3, s6, s7;

    DRAM_SEC_DED(const sc_core::sc_module_name& name) :
        res_sbe_cov("RES_SBE_COV", 1.0, 1.0),
        res_dbe_cov("RES_DBE_COV", 1.0, 1.0),
        res_tbe_cov("RES_TBE_COV", 1.0, 1.0), // TODO: unklar ob dieser coverage block korrekt ist oder nicht eher ein split hierhin muss
        res_mbe_cov("RES_MBE_COV", 0.5, 1.0), // TODO: unklar welcher wert
        res_tbe_split("RES_TBE_SPLIT"),
        res_mbe_sum("RES_MBE_SUM"),
        res_wd_pass("RES_WD_PASS"),
        res_az_pass("RES_AZ_PASS"),
        s1("s1"),
        s2("s2"),
        s3("s3"),
        s6("s6"),
        s7("s7"),
        sec_ded_broken("SEC_DED_BROKEN", 0.1)
    {
        res_sbe_cov.input(I_RES_SBE);
        res_dbe_cov.input(I_RES_DBE);
        res_tbe_split.input(I_RES_TBE);
        res_mbe_cov.input(I_RES_MBE);

        res_sbe_cov.output(O_RES_SBE);
        res_dbe_cov.output(O_RES_DBE);
        res_tbe_cov.output(O_RES_TBE);
        res_mbe_cov.output(s7);
        res_mbe_cov.latent(O_LAT_MBE);

        res_tbe_split.outputs.bind(s1, 0.44);
        res_tbe_split.outputs.bind(s6, 0.56);

        res_tbe_cov.input(s1);

        res_sbe_cov.latent.bind(O_LAT_SBE);
        res_dbe_cov.latent.bind(O_LAT_DBE);
        res_tbe_cov.latent.bind(O_LAT_TBE);

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
    sc_in<double> I_RES_SBE, I_RES_DBE, I_RES_TBE, I_RES_MBE, I_RES_WD, I_RES_AZ;
    sc_out<double> O_RES_SBE, O_RES_DBE, O_RES_TBE, O_RES_MBE, O_RES_WD, O_RES_AZ;

    split res_sbe_split, res_dbe_split, res_tbe_split;
    sum res_sbe_sum, res_dbe_sum;
    pass res_tbe_pass, res_mbe_pass, res_wd_pass, res_az_pass;

    sc_signal<double> s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;

    DRAM_SEC_DED_TRIM(const sc_module_name& name) :
        I_RES_SBE("I_RES_SBE"),
        I_RES_DBE("I_RES_DBE"),
        I_RES_TBE("I_RES_TBE"),
        I_RES_MBE("I_RES_MBE"),
        I_RES_WD("I_RES_WD"),
        I_RES_AZ("I_RES_AZ"),
        O_RES_SBE("O_RES_SBE"),
        O_RES_DBE("O_RES_DBE"),
        O_RES_TBE("O_RES_TBE"),
        O_RES_MBE("O_RES_MBE"),
        O_RES_WD("O_RES_WD"),
        O_RES_AZ("O_RES_AZ"),
        res_sbe_split("RES_SBE_SPLIT"),
        res_dbe_split("RES_DBE_SPLIT"),
        res_tbe_split("RES_TBE_SPLIT"),
        res_sbe_sum("RES_SBE_SUM"),
        res_dbe_sum("RES_DBE_SUM"),
        res_tbe_pass("RES_TBE_PASS"),
        res_mbe_pass("RES_MBE_PASS"),
        res_wd_pass("RES_WD_PASS"),
        res_az_pass("RES_AZ_PASS"),
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

        res_sbe_split.outputs.bind(s0, 0.89);

        res_dbe_split.outputs.bind(s1, 0.20);
        res_dbe_split.outputs.bind(s2, 0.79);

        res_tbe_split.outputs.bind(s3, 0.03);
        res_tbe_split.outputs.bind(s4, 0.27);
        res_tbe_split.outputs.bind(s5, 0.70);

        res_sbe_sum.inputs.bind(s0);
        res_sbe_sum.inputs.bind(s1);
        res_sbe_sum.inputs.bind(s3);
        res_sbe_sum.output.bind(O_RES_SBE);

        res_dbe_sum.inputs.bind(s2);
        res_dbe_sum.inputs.bind(s4);
        res_dbe_sum.output.bind(O_RES_DBE);

        res_tbe_pass.input(s5);
        res_tbe_pass.output(O_RES_TBE);

        res_mbe_pass.input(I_RES_MBE);
        res_mbe_pass.output(O_RES_MBE);

        res_wd_pass.input(I_RES_WD);
        res_wd_pass.output(O_RES_WD);

        res_az_pass.input(I_RES_AZ);
        res_az_pass.output(O_RES_AZ);
    }
};

SC_MODULE(ALL_OTHER_COMPONENTS)
{
    sc_out<double> OTHER_RES, OTHER_LAT;
    basic_event all_other;
    sc_signal<double> s0, s1, s2;
    split other_split;
    coverage other_cov;

    ALL_OTHER_COMPONENTS(const sc_module_name& name, double OTHER_COMPONENTS) :
        s0("s0"),
        s1("s1"),
        s2("s2"),
        OTHER_RES("OTHER_RES"),
        OTHER_LAT("OTHER_LAT"),
        other_split("OTHER_SPLIT"),
        other_cov("OTHER_COV", 0.9, 1.0), // Internal coverages of other components
        all_other("ALL_OTHER", OTHER_COMPONENTS)
    {
        all_other.output.bind(s0);
        other_split.input(s0);
        other_cov.input.bind(s1);
        other_split.outputs.bind(s1, 0.5); // safe faults?
        other_cov.output(OTHER_RES);
        other_cov.latent(OTHER_LAT);
    }
};

int sc_main(int argc, char *argv[])
{
    double DRAM_FIT = (argc == 1) ? 2300.0 : std::stod(argv[1]);
    double OTHER_COMPONENTS = 1920.0;
    double TOTAL = DRAM_FIT + OTHER_COMPONENTS;

    DRAM dram("DRAM", DRAM_FIT);
    DRAM_SEC_ECC sec_ecc("DRAM_SEC_ECC");
    DRAM_SEC_TRIM sec_trim("DRAM_SEC_TRIM");
    DRAM_BUS_TRIM bus_trim("DRAM_BUS_TRIM", DRAM_FIT);
    DRAM_SEC_DED sec_ded("DRAM_SEC_DED");
    DRAM_SEC_DED_TRIM sec_ded_trim("DRAM_SEC_DED_TRIM");
    ALL_OTHER_COMPONENTS all_other_components("ALL_OTHER_COMPONENTS", OTHER_COMPONENTS);
    sum residual("RESIDUAL");
    sum latent("LATENT");
    asil calculate_asil("ASIL", TOTAL);

    // DRAM
    sc_signal<double> dram_res_sbe("dram_res_sbe");
    sc_signal<double> dram_res_dbe("dram_res_dbe");
    sc_signal<double> dram_res_mbe("dram_res_mbe");
    sc_signal<double> dram_res_wd("dram_res_wd");

    dram.SBE.bind(dram_res_sbe);
    dram.DBE.bind(dram_res_dbe);
    dram.MBE.bind(dram_res_mbe);
    dram.WD.bind(dram_res_wd);

    // SEC-ECC
    sec_ecc.I_SBE.bind(dram_res_sbe);
    sec_ecc.I_DBE.bind(dram_res_dbe);
    sec_ecc.I_MBE.bind(dram_res_mbe);
    sec_ecc.I_WD.bind(dram_res_wd);

    sc_signal<double> sec_ecc_res_sbe("sec_ecc_res_sbe");
    sc_signal<double> sec_ecc_res_dbe("sec_ecc_res_dbe");
    sc_signal<double> sec_ecc_res_tbe("sec_ecc_res_tbe");
    sc_signal<double> sec_ecc_res_mbe("sec_ecc_res_mbe");
    sc_signal<double> sec_ecc_res_wd("sec_ecc_res_wd");
    sc_signal<double> sec_ecc_lat_sbe("sec_ecc_lat_sbe");
    sc_signal<double> sec_ecc_lat_sec_broken("sec_ecc_lat_sec_broken");

    sec_ecc.O_RES_SBE.bind(sec_ecc_res_sbe);
    sec_ecc.O_RES_DBE.bind(sec_ecc_res_dbe);
    sec_ecc.O_RES_TBE.bind(sec_ecc_res_tbe);
    sec_ecc.O_RES_MBE.bind(sec_ecc_res_mbe);
    sec_ecc.O_RES_WD.bind(sec_ecc_res_wd);
    sec_ecc.O_LAT_SBE.bind(sec_ecc_lat_sbe);
    sec_ecc.O_LAT_SEC_BROKEN.bind(sec_ecc_lat_sec_broken);

    // DRAM-TRIM
    sec_trim.I_RES_SBE.bind(sec_ecc_res_sbe);
    sec_trim.I_RES_DBE.bind(sec_ecc_res_dbe);
    sec_trim.I_RES_TBE.bind(sec_ecc_res_tbe);
    sec_trim.I_RES_MBE.bind(sec_ecc_res_mbe);
    sec_trim.I_RES_WD.bind(sec_ecc_res_wd);

    sc_signal<double> sec_trim_res_sbe("sec_trim_res_sbe");
    sc_signal<double> sec_trim_res_dbe("sec_trim_res_dbe");
    sc_signal<double> sec_trim_res_tbe("sec_trim_res_tbe");
    sc_signal<double> sec_trim_res_mbe("sec_trim_res_mbe");
    sc_signal<double> sec_trim_res_wd("sec_trim_res_wd");

    sec_trim.O_RES_SBE.bind(sec_trim_res_sbe);
    sec_trim.O_RES_DBE.bind(sec_trim_res_dbe);
    sec_trim.O_RES_TBE.bind(sec_trim_res_tbe);
    sec_trim.O_RES_MBE.bind(sec_trim_res_mbe);
    sec_trim.O_RES_WD.bind(sec_trim_res_wd);

    // BUS-TRIM
    bus_trim.I_RES_SBE.bind(sec_trim_res_sbe);
    bus_trim.I_RES_DBE.bind(sec_trim_res_dbe);
    bus_trim.I_RES_TBE.bind(sec_trim_res_tbe);
    bus_trim.I_RES_MBE.bind(sec_trim_res_mbe);
    bus_trim.I_RES_WD.bind(sec_trim_res_wd);

    sc_signal<double> bus_trim_res_sbe("bus_trim_res_sbe");
    sc_signal<double> bus_trim_res_dbe("bus_trim_res_dbe");
    sc_signal<double> bus_trim_res_tbe("bus_trim_res_tbe");
    sc_signal<double> bus_trim_res_mbe("bus_trim_res_mbe");
    sc_signal<double> bus_trim_res_wd("bus_trim_res_wd");
    sc_signal<double> bus_trim_res_az("bus_trim_res_az");
    sc_signal<double> bus_trim_lat_if("bus_trim_lat_if");
    sc_signal<double> bus_trim_lat_lb("bus_trim_lat_lb");

    bus_trim.O_RES_SBE.bind(bus_trim_res_sbe);
    bus_trim.O_RES_DBE.bind(bus_trim_res_dbe);
    bus_trim.O_RES_TBE.bind(bus_trim_res_tbe);
    bus_trim.O_RES_MBE.bind(bus_trim_res_mbe);
    bus_trim.O_RES_WD.bind(bus_trim_res_wd);
    bus_trim.O_RES_AZ.bind(bus_trim_res_az);
    bus_trim.O_LAT_IF.bind(bus_trim_lat_if);
    bus_trim.O_LAT_LB.bind(bus_trim_lat_lb);

    // SEC-DED
    sec_ded.I_RES_SBE.bind(bus_trim_res_sbe);
    sec_ded.I_RES_DBE.bind(bus_trim_res_dbe);
    sec_ded.I_RES_TBE.bind(bus_trim_res_tbe);
    sec_ded.I_RES_MBE.bind(bus_trim_res_mbe);
    sec_ded.I_RES_WD.bind(bus_trim_res_wd);
    sec_ded.I_RES_AZ.bind(bus_trim_res_az);

    sc_signal<double> sec_ded_res_sbe("sec_ded_res_sbe");
    sc_signal<double> sec_ded_res_dbe("sec_ded_res_dbe");
    sc_signal<double> sec_ded_res_tbe("sec_ded_res_tbe");
    sc_signal<double> sec_ded_res_mbe("sec_ded_res_mbe");
    sc_signal<double> sec_ded_res_wd("sec_ded_res_wd");
    sc_signal<double> sec_ded_res_az("sec_ded_res_az");
    sc_signal<double> sec_ded_lat_sbe("sec_ded_lat_sbe");
    sc_signal<double> sec_ded_lat_dbe("sec_ded_lat_dbe");
    sc_signal<double> sec_ded_lat_tbe("sec_ded_lat_tbe");
    sc_signal<double> sec_ded_lat_mbe("sec_ded_lat_mbe");
    sc_signal<double> sec_ded_lat_sec_ded_broken("sec_ded_lat_sec_ded_broken");

    sec_ded.O_RES_SBE.bind(sec_ded_res_sbe);
    sec_ded.O_RES_DBE.bind(sec_ded_res_dbe);
    sec_ded.O_RES_TBE.bind(sec_ded_res_tbe);
    sec_ded.O_RES_MBE.bind(sec_ded_res_mbe);
    sec_ded.O_RES_WD.bind(sec_ded_res_wd);
    sec_ded.O_RES_AZ.bind(sec_ded_res_az);
    sec_ded.O_LAT_SBE.bind(sec_ded_lat_sbe);
    sec_ded.O_LAT_DBE.bind(sec_ded_lat_dbe);
    sec_ded.O_LAT_TBE.bind(sec_ded_lat_tbe);
    sec_ded.O_LAT_MBE.bind(sec_ded_lat_mbe);
    sec_ded.O_LAT_SEC_DED_BROKEN.bind(sec_ded_lat_sec_ded_broken);

    // SEC-DED-TRIM
    sec_ded_trim.I_RES_SBE.bind(sec_ded_res_sbe);
    sec_ded_trim.I_RES_DBE.bind(sec_ded_res_dbe);
    sec_ded_trim.I_RES_TBE.bind(sec_ded_res_tbe);
    sec_ded_trim.I_RES_MBE.bind(sec_ded_res_mbe);
    sec_ded_trim.I_RES_WD.bind(sec_ded_res_wd);
    sec_ded_trim.I_RES_AZ.bind(sec_ded_res_az);

    sc_signal<double> sec_ded_trim_res_sbe("sec_ded_trim_res_sbe");
    sc_signal<double> sec_ded_trim_res_dbe("sec_ded_trim_res_dbe");
    sc_signal<double> sec_ded_trim_res_tbe("sec_ded_trim_res_tbe");
    sc_signal<double> sec_ded_trim_res_mbe("sec_ded_trim_res_mbe");
    sc_signal<double> sec_ded_trim_res_wd("sec_ded_trim_res_wd");
    sc_signal<double> sec_ded_trim_res_az("sec_ded_trim_res_az");

    sec_ded_trim.O_RES_SBE.bind(sec_ded_trim_res_sbe);
    sec_ded_trim.O_RES_DBE.bind(sec_ded_trim_res_dbe);
    sec_ded_trim.O_RES_TBE.bind(sec_ded_trim_res_tbe);
    sec_ded_trim.O_RES_MBE.bind(sec_ded_trim_res_mbe);
    sec_ded_trim.O_RES_WD(sec_ded_trim_res_wd);
    sec_ded_trim.O_RES_AZ(sec_ded_trim_res_az);

    // Other
    sc_signal<double> all_other_components_res("all_other_components_res");
    sc_signal<double> all_other_components_lat("all_other_components_lat");

    all_other_components.OTHER_RES.bind(all_other_components_res);
    all_other_components.OTHER_LAT.bind(all_other_components_lat);

    // ASIL
    residual.inputs.bind(sec_ded_trim_res_sbe);
    residual.inputs.bind(sec_ded_trim_res_dbe);
    residual.inputs.bind(sec_ded_trim_res_tbe);
    residual.inputs.bind(sec_ded_trim_res_mbe);
    residual.inputs.bind(sec_ded_trim_res_wd);
    residual.inputs.bind(sec_ded_trim_res_az);
    residual.inputs.bind(all_other_components_res);

    latent.inputs.bind(sec_ecc_lat_sbe);
    latent.inputs.bind(sec_ecc_lat_sec_broken);
    latent.inputs.bind(sec_ded_lat_sbe);
    latent.inputs.bind(sec_ded_lat_dbe);
    latent.inputs.bind(sec_ded_lat_tbe);
    latent.inputs.bind(sec_ded_lat_mbe);
    latent.inputs.bind(sec_ded_lat_sec_ded_broken);
    latent.inputs.bind(bus_trim_lat_if);
    latent.inputs.bind(bus_trim_lat_lb);
    latent.inputs.bind(all_other_components_lat);

    sc_signal<double> latent_result("latent_result");
    sc_signal<double> residual_result("residual_result");

    residual.output.bind(residual_result);
    latent.output.bind(latent_result);

    calculate_asil.residual.bind(residual_result);
    calculate_asil.latent.bind(latent_result);

    sc_start();

    std::cout << "DRAM: RES_SBE: " << dram_res_sbe << std::endl;
    std::cout << "DRAM: RES_DBE: " << dram_res_dbe << std::endl;
    std::cout << "DRAM: RES_MBE: " << dram_res_mbe << std::endl;
    std::cout << "DRAM: RES_WD:  " << dram_res_wd << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "SEC: RES_SBE: " << sec_ecc_res_sbe << std::endl;
    std::cout << "SEC: RES_DBE: " << sec_ecc_res_dbe << std::endl;
    std::cout << "SEC: RES_TBE: " << sec_ecc_res_tbe << std::endl;
    std::cout << "SEC: RES_MBE: " << sec_ecc_res_mbe << std::endl;
    std::cout << "SEC: RES_WD:  " << sec_ecc_res_wd << std::endl;
    std::cout << "SEC: LAT_SBE: " << sec_ecc_lat_sbe << std::endl;
    std::cout << "SEC: LAT_SEC: " << sec_ecc_lat_sec_broken << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "DRAM-TRIM: RES_SBE: " << sec_trim_res_sbe << std::endl;
    std::cout << "DRAM-TRIM: RES_DBE: " << sec_trim_res_dbe << std::endl;
    std::cout << "DRAM-TRIM: RES_TBE: " << sec_trim_res_tbe << std::endl;
    std::cout << "DRAM-TRIM: RES_MBE: " << sec_trim_res_mbe << std::endl;
    std::cout << "DRAM-TRIM: RES_WD:  " << sec_trim_res_wd << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "BUS-TRIM: RES_SBE: " << bus_trim_res_sbe << std::endl;
    std::cout << "BUS-TRIM: RES_DBE: " << bus_trim_res_dbe << std::endl;
    std::cout << "BUS-TRIM: RES_TBE: " << bus_trim_res_tbe << std::endl;
    std::cout << "BUS-TRIM: RES_MBE: " << bus_trim_res_mbe << std::endl;
    std::cout << "BUS-TRIM: RES_WD:  " << bus_trim_res_wd << std::endl;
    std::cout << "BUS-TRIM: RES_AZ:  " << bus_trim_res_az << std::endl;
    std::cout << "BUS-TRIM: LAT_IF:  " << bus_trim_lat_if << std::endl;
    std::cout << "BUS-TRIM: LAT_LB:  " << bus_trim_lat_lb << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "SEC-DED: RES_SBE: " << sec_ded_res_sbe << std::endl;
    std::cout << "SEC-DED: RES_DBE: " << sec_ded_res_dbe << std::endl;
    std::cout << "SEC-DED: RES_TBE: " << sec_ded_res_tbe << std::endl;
    std::cout << "SEC-DED: RES_MBE: " << sec_ded_res_mbe << std::endl;
    std::cout << "SEC-DED: RES_WD:  " << sec_ded_res_wd << std::endl;
    std::cout << "SEC-DED: RES_AZ:  " << sec_ded_res_az << std::endl;
    std::cout << "SEC-DED: LAT_SBE: " << sec_ded_lat_sbe << std::endl;
    std::cout << "SEC-DED: LAT_DBE: " << sec_ded_lat_dbe << std::endl;
    std::cout << "SEC-DED: LAT_TBE: " << sec_ded_lat_tbe << std::endl;
    std::cout << "SEC-DED: LAT_MBE: " << sec_ded_lat_mbe << std::endl;
    std::cout << "SEC-DED: LAT_SDB: " << sec_ded_lat_sec_ded_broken << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "SEC-DED-TRIM: RES_SBE: " << sec_ded_trim_res_sbe << std::endl;
    std::cout << "SEC-DED-TRIM: RES_DBE: " << sec_ded_trim_res_dbe << std::endl;
    std::cout << "SEC-DED-TRIM: RES_TBE: " << sec_ded_trim_res_tbe << std::endl;
    std::cout << "SEC-DED-TRIM: RES_MBE: " << sec_ded_trim_res_mbe << std::endl;
    std::cout << "SEC-DED-TRIM: RES_WD:  " << sec_ded_trim_res_wd << std::endl;
    std::cout << "SEC-DED-TRIM: RES_AZ:  " << sec_ded_trim_res_az << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "OTHER: RES: " << all_other_components_res << std::endl;
    std::cout << "OTHER: LAT: " << all_other_components_lat << std::endl;
    std::cout << "------------------------------ " << std::endl;
    std::cout << "TOTAL: RES_SUM: " << residual_result << std::endl;
    std::cout << "TOTAL: LAT_SUM: " << latent_result << std::endl;
    std::cout << "------------------------------ " << std::endl;
    sc_stop();

    return 0;
}
