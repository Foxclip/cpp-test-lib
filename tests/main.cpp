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

void test_dependency_execution() {
    TestModule* test_module = new TestModule("DependencyTestModule", nullptr);
    test::Test* passing_test = test_module->addTest("DependencyTest", [](test::Test& test) { });
    test::Test* dependent_test = test_module->addTest("DependentTest", { passing_test }, [](test::Test& test) { } );
    test_module->print_summary_enabled = true;
    test_module->run();
    assert(passing_test->is_run);
    assert(passing_test->result);
    assert(dependent_test->is_run);
    assert(dependent_test->result);
}

void test_dependency_cancellation() {
    TestModule* test_module = new TestModule("CancellationTestModule", nullptr);
    test::Test* failing_test = test_module->addTest("FailingDependencyTest", [&](test::Test& test) {
        test_module->failingTest(test);
    });
    test::Test* dependent_test = test_module->addTest("DependentTest", { failing_test }, [](test::Test& test) { });
    test_module->print_summary_enabled = true;
    test_module->run();
    assert(failing_test->is_run);
    assert(!failing_test->result);
    assert(!dependent_test->is_run);
    assert(!dependent_test->result);
    assert(dependent_test->cancelled);
}

void test_module_dependency_execution() {
    TestModule* root_module = new TestModule("RootModule", nullptr);
    TestModule* dependency_module = root_module->addModule<TestModule>("DependencyModule");
    test::Test* dependency_test = dependency_module->addTest("DependencyTest", [](test::Test& test) { });
    TestModule* dependent_module = root_module->addModule<TestModule>("DependentModule", {dependency_module});
    test::Test* dependent_test = dependent_module->addTest("DependentTest", [](test::Test& test) { });
    root_module->print_summary_enabled = true;
    root_module->run();
    assert(dependency_test->is_run);
    assert(dependency_test->result);
    assert(dependent_test->is_run);
    assert(dependent_test->result);
}

void test_module_dependency_cancellation() {
    TestModule* root_module = new TestModule("RootModule", nullptr);
    TestModule* failing_module = root_module->addModule<TestModule>("FailingDependencyModule");
    test::Test* failing_test = failing_module->addTest("FailingDependencyTest", [&](test::Test& test) {
        failing_module->failingTest(test);
    });
    TestModule* dependent_module = root_module->addModule<TestModule>("DependentModule", { failing_module });
    test::Test* dependent_test = dependent_module->addTest("DependentTest", [](test::Test& test) { });
    root_module->print_summary_enabled = true;
    root_module->run();
    assert(failing_test->is_run);
    assert(!failing_test->result);
    assert(!dependent_test->is_run);
    assert(!dependent_test->result);
    assert(dependent_test->cancelled);
}

int main() {
    basic_test();
    add_test();
    run_test();
    failing_test();
    test_dependency_execution();
    test_dependency_cancellation();
    test_module_dependency_execution();
    test_module_dependency_cancellation();

    std::cout << "ALL PASSED" << std::endl;
    return 0;
}
