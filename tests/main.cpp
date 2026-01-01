#pragma once

#include "test_lib/test.h"
#include <assert.h>
#include <iostream>

class TestModule : public test::TestModule {
public:
    TestModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { })
        : test::TestModule(name, parent, required_nodes) { };

    void failingTest(test::Test& test) {
        T_CHECK(false, "This test is expected to fail");
    }
};

void basic_test() {
    TestModule* test_module = new TestModule("TestModule", nullptr);
}

void add_test() {
    TestModule* test_module = new TestModule("TestModule", nullptr);
    test::Test* test = test_module->addTest("Test", [](test::Test& test) { });
}

void run_test() {
    TestModule* test_module = new TestModule("TestModule", nullptr);
    test::Test* test = test_module->addTest("Test", [](test::Test& test) { });
    test_module->print_summary_enabled = true;
    test_module->run();
    assert(test->is_run);
    assert(test->result);
}

void failing_test() {
    TestModule* test_module = new TestModule("FailingTestModule", nullptr);
    test::Test* test = test_module->addTest("FailingTest", [&](test::Test& test) {
        test_module->failingTest(test);
    });
    test_module->print_summary_enabled = true;
    test_module->run();
    assert(test->is_run);
    assert(!test->result);
}

int main() {
    basic_test();
    add_test();
    run_test();
    failing_test();

    std::cout << "ALL PASSED" << std::endl;
    return 0;
}
