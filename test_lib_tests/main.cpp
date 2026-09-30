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
    test_module->run();
    test_module->printSummary();
    assert(test->is_run);
    assert(test->result);
}

void failing_test() {
    TestModule* test_module = new TestModule("FailingTestModule", nullptr);
    test::Test* test = test_module->addTest("FailingTest", [&](test::Test& test) {
        test_module->failingTest(test);
    });
    test_module->run();
    test_module->printSummary();
    assert(test->is_run);
    assert(!test->result);
}

void test_dependency_execution() {
    TestModule* test_module = new TestModule("DependencyTestModule", nullptr);
    test::Test* passing_test = test_module->addTest("DependencyTest", [](test::Test& test) { });
    test::Test* dependent_test = test_module->addTest("DependentTest", { passing_test }, [](test::Test& test) { } );
    test_module->run();
    test_module->printSummary();
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
    test_module->run();
    test_module->printSummary();
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
    root_module->run();
    root_module->printSummary();
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
    root_module->run();
    root_module->printSummary();
    assert(failing_test->is_run);
    assert(!failing_test->result);
    assert(!dependent_test->is_run);
    assert(!dependent_test->result);
    assert(dependent_test->cancelled);
}

void test_run_single_test() {
    TestModule* root_module = new TestModule("SingleTestModule", nullptr);
    TestModule* module_a = root_module->addModule<TestModule>("ModuleA");
    TestModule* module_b = module_a->addModule<TestModule>("ModuleB");
    test::Test* test_a = module_a->addTest("TestA", [](test::Test& test) { });
    test::Test* test_b = module_b->addTest("TestB", [](test::Test& test) { });
    test::Test* test_c = module_b->addTest("TestC", [](test::Test& test) { });
    bool result = root_module->run("ModuleA/ModuleB/TestB");
    root_module->printSummary();
    assert(result);
    assert(!test_a->is_run);
    assert(test_b->is_run);
    assert(test_b->result);
    assert(!test_c->is_run);
}

void test_run_single_test_with_dependencies() {
    TestModule* root_module = new TestModule("SingleTestDependencyModule", nullptr);
    TestModule* module = root_module->addModule<TestModule>("Module");
    test::Test* first_test = module->addTest("FirstTest", [](test::Test& test) { });
    test::Test* second_test = module->addTest("SecondTest", { first_test }, [](test::Test& test) { });
    test::Test* third_test = module->addTest("ThirdTest", { second_test }, [](test::Test& test) { });
    bool result = root_module->run("Module/ThirdTest");
    root_module->printSummary();
    assert(result);
    assert(first_test->is_run);
    assert(first_test->result);
    assert(second_test->is_run);
    assert(second_test->result);
    assert(third_test->is_run);
    assert(third_test->result);
}

void test_run_single_test_with_module_dependency() {
    TestModule* root_module = new TestModule("SingleTestModuleDependencyModule", nullptr);
    TestModule* dependency_module = root_module->addModule<TestModule>("DependencyModule");
    test::Test* dependency_test = dependency_module->addTest("DependencyTest", [](test::Test& test) { });
    TestModule* target_module = root_module->addModule<TestModule>("TargetModule", { dependency_module });
    test::Test* target_test = target_module->addTest("TargetTest", [](test::Test& test) { });
    test::Test* unrelated_test = root_module->addTest("UnrelatedTest", [](test::Test& test) { });
    bool result = root_module->run("TargetModule/TargetTest");
    root_module->printSummary();
    assert(result);
    assert(dependency_test->is_run);
    assert(dependency_test->result);
    assert(target_test->is_run);
    assert(target_test->result);
    assert(!unrelated_test->is_run);
}

void test_run_single_test_failing_dependency() {
    TestModule* test_module = new TestModule("SingleTestFailingDependencyModule", nullptr);
    TestModule* module = test_module->addModule<TestModule>("Module");
    test::Test* failing_test = module->addTest("FailingDependencyTest", [&](test::Test& test) {
        test_module->failingTest(test);
    });
    test::Test* target_test = module->addTest("TargetTest", { failing_test }, [](test::Test& test) { });
    bool result = test_module->run("Module/TargetTest");
    test_module->printSummary();
    assert(!result);
    assert(failing_test->is_run);
    assert(!failing_test->result);
    assert(!target_test->is_run);
    assert(!target_test->result);
    assert(target_test->cancelled);
}

void test_run_single_test_not_found() {
    TestModule* root_module = new TestModule("SingleTestNotFoundModule", nullptr);
    TestModule* module = root_module->addModule<TestModule>("Module");
    test::Test* test = module->addTest("Test", [](test::Test& test) { });
    bool result = root_module->run("Module/NonexistentTest");
    assert(!result);
    assert(!test->is_run);
}

int main() {
    basic_test();
    add_test();
    run_test();
    std::cout << std::endl;
    failing_test();
    std::cout << std::endl;
    test_dependency_execution();
    std::cout << std::endl;
    test_dependency_cancellation();
    std::cout << std::endl;
    test_module_dependency_execution();
    std::cout << std::endl;
    test_module_dependency_cancellation();
    std::cout << std::endl;
    test_run_single_test();
    std::cout << std::endl;
    test_run_single_test_with_dependencies();
    std::cout << std::endl;
    test_run_single_test_with_module_dependency();
    std::cout << std::endl;
    test_run_single_test_failing_dependency();
    std::cout << std::endl;
    test_run_single_test_not_found();
    std::cout << std::endl;
    std::cout << "ALL PASSED" << std::endl;

    // TODO: add T_FAIL macro that outputs message and returns
    // TODO: rename T_WRAP_CONTAINER to T_CALL

    return 0;
}
