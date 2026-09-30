#include "logger/logger.h"
#include "example/test_module_1.h"
#include "example/test_module_2.h"
#include "example/test_module_3.h"

void run_tests() {
    test::TestModule root_module("Example tests", nullptr);
    ExampleTestModule1* module_1 = root_module.addModule<ExampleTestModule1>("ExampleModule1");
    ExampleTestModule2* module_2 = root_module.addModule<ExampleTestModule2>("ExampleModule2", { module_1 });
    ExampleTestModule3* module_3 = root_module.addModule<ExampleTestModule3>("ExampleModule3");
    test::TestModule* module_4 = root_module.addModule("EmptyModule");
    root_module.run();
    root_module.printSummary();

    test::TestModule empty_root_module("Empty root module", nullptr);
    empty_root_module.run();
    empty_root_module.printSummary();

    test::TestModule passing_module("Passing module", nullptr);
    passing_module.addTest("test1", [](test::Test& test) { });
    passing_module.run();
    passing_module.printSummary();
}

void run_single_test() {
    // Runs only the test at the given path, along with the tests it depends on
    test::TestModule root_module("Single test example", nullptr);
    ExampleTestModule1* module_1 = root_module.addModule<ExampleTestModule1>("ExampleModule1");
    root_module.run("ExampleModule1/List1/third");
    root_module.printSummary();
}

int main() {
    run_tests();
    std::cout << std::endl;
    run_single_test();
    return 0;
}
