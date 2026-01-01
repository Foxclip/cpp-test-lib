#include "test_lib/test.h"
#include <assert.h>
#include <iostream>

class TestTestModule : public test::TestModule {
public:
    TestTestModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { })
        : test::TestModule(name, parent, required_nodes) { };
};

void basic_test() {
    TestTestModule* test_module = new TestTestModule("TestModule", nullptr);
};

void add_test() {
    TestTestModule* test_module = new TestTestModule("TestModule", nullptr);
    test::Test* test = test_module->addTest("Test", [](test::Test& test) { });
};

void run_test() {
    TestTestModule* test_module = new TestTestModule("TestModule", nullptr);
    test::Test* test = test_module->addTest("Test", [](test::Test& test) { });
    test_module->print_summary_enabled = true;
    test_module->run();
    assert(test->is_run);
    assert(test->result);
}

int main() {
    basic_test();
    add_test();
    run_test();

    std::cout << "ALL PASSED" << std::endl;
    return 0;
}
