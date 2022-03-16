#include <gtest/gtest.h>
#include <systemc.h>
#include "../sc_fta.h"
#include "../sc_hw_metrics.h"

TEST(prob, and) {
    sc_fta::prob a(0.5);
    sc_fta::prob b(0.5);
    EXPECT_EQ(a&&b, 0.25);
}

TEST(prob, or) {
    sc_fta::prob a(0.5);
    sc_fta::prob b(0.5);
    EXPECT_EQ(a||b, 0.75);
}

TEST(prob, not) {
    sc_fta::prob a(0.75);
    EXPECT_EQ(!a, 0.25);
}

// Component Fault Trees:

SC_MODULE(component_and) {

    sc_in<sc_fta::prob>  i_prob_1;
    sc_in<sc_fta::prob>  i_prob_2;
    sc_out<sc_fta::prob> o_prob_1;

    SC_CTOR(component_and) : i_prob_1("i_prob_1"), i_prob_2("i_prob_2"), o_prob_1("o_prob_1") {
        SC_METHOD(compute_prob)
        sensitive << i_prob_1 << i_prob_2;
    }

    void compute_prob() {
        o_prob_1.write(i_prob_1.read() && i_prob_2.read());
    }

};

TEST(cft, and) {
    component_and a("component_and");
    sc_signal<sc_fta::prob> s1("s1",0.5);
    sc_signal<sc_fta::prob> s2("s2",0.5);
    sc_signal<sc_fta::prob> s3("s3",0.0);

    a.i_prob_1.bind(s1);
    a.i_prob_2.bind(s2);
    a.o_prob_1.bind(s3);

    sc_start();

    EXPECT_EQ(s3.read(), 0.25);
}

TEST(cft, concat) {
    sc_signal<sc_fta::prob> s1("s1",0.5);
    sc_signal<sc_fta::prob> s2("s2",0.5);
    sc_signal<sc_fta::prob> s3("s3");
    sc_signal<sc_fta::prob> s4("s4",0.5);
    sc_signal<sc_fta::prob> s5("s5",0.5);
    sc_signal<sc_fta::prob> s6("s6");
    sc_signal<sc_fta::prob> s9("s9");

    component_and t_level1_a("t1a");
    component_and t_level1_b("t1b");
    component_and t_level2("t1c");

    t_level1_a.i_prob_1.bind(s1);
    t_level1_a.i_prob_2.bind(s2);
    t_level1_a.o_prob_1.bind(s3);
    t_level1_b.i_prob_1.bind(s4);
    t_level1_b.i_prob_2.bind(s5);
    t_level1_b.o_prob_1.bind(s6);
    t_level2.i_prob_1.bind(s3);
    t_level2.i_prob_2.bind(s6);
    t_level2.o_prob_1.bind(s9);

    sc_start();

    EXPECT_EQ(s9.read(), 0.0625);
}

SC_MODULE(component_not)
{
    sc_in<sc_fta::prob>  in;
    sc_out<sc_fta::prob> out;

    SC_CTOR(component_not) {
        SC_METHOD(compute_prob);
        sensitive << in;
    }

    void compute_prob() {
        out.write(!in.read());
    }
};

SC_MODULE(component_hierarchy)
{
    component_not* n;
    component_and* a;

    sc_in<sc_fta::prob>  in1; 
    sc_in<sc_fta::prob>  in2;
    sc_out<sc_fta::prob> out;

    sc_signal<sc_fta::prob> s1;

    SC_CTOR(component_hierarchy) : in1("in1"), in2("in2"), out("out"), s1("s1") {
        n = new component_not("component_not");
        a = new component_and("component_and");

        a->i_prob_1.bind(in1);
        a->i_prob_2.bind(in2);
        a->o_prob_1.bind(s1);
        n->in(s1);
        n->out.bind(out);
    }

    ~component_hierarchy() {
        delete n;
        delete a;
    }

};

TEST(cft, hierarchy) {
    sc_signal<sc_fta::prob> s1("s1",0.5);
    sc_signal<sc_fta::prob> s2("s2",0.5);
    sc_signal<sc_fta::prob> s3("s3");

    component_hierarchy h("h");

    h.in1.bind(s1);
    h.in2.bind(s2);
    h.out.bind(s3);

    sc_start();

    EXPECT_EQ(s3.read(), 0.75);
}

// Hardware Metrics:

TEST(hw_metric, coverage) {
    sc_signal<double> i("i",100.0);
    sc_signal<double> o1("o1");

    sc_hw_metrics::coverage m("m", 0.83);

    m.input.bind(i);
    m.output.bind(o1);

    sc_start();

    EXPECT_DOUBLE_EQ(o1.read(), 17.0);
}

TEST(hw_metric, split) {
    sc_signal<double> i("i",100.0);
    sc_signal<double> o1("o1");
    sc_signal<double> o2("o2");

    sc_hw_metrics::split s("s");

    s.input.bind(i);
    s.outputs.bind(o1, 0.87);
    s.outputs.bind(o2, 0.13);

    sc_start();

    EXPECT_DOUBLE_EQ(o1.read(), 87.0);
    EXPECT_DOUBLE_EQ(o2.read(), 13.0);
}

TEST(hw_metric, sum) {
    sc_signal<double> i1("i1", 10.0);
    sc_signal<double> i2("i2", 10.0);
    sc_signal<double> o("o");

    sc_hw_metrics::sum s("sum");

    s.inputs.bind(i1);
    s.inputs.bind(i2);
    s.output.bind(o);

    sc_start();

    EXPECT_DOUBLE_EQ(o.read(), 20.0);
}

int sc_main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int status = RUN_ALL_TESTS();
    return status;
}