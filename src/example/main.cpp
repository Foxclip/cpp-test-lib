#include "common/logger.h"
#include "example/test_module_1.h"
#include "example/test_module_2.h"
#include "example/test_module_3.h"

void run_tests() {
    test::TestModule root_module("Example tests", nullptr);
    root_module.print_summary_enabled = true;
    ExampleTestModule1* module_1 = root_module.addModule<ExampleTestModule1>("ExampleModule1");
    ExampleTestModule2* module_2 = root_module.addModule<ExampleTestModule2>("ExampleModule2", { module_1 });
    ExampleTestModule3* module_3 = root_module.addModule<ExampleTestModule3>("ExampleModule3");
    test::TestModule* module_4 = root_module.addModule("EmptyModule");
    for (test::TestModule* module : root_module.getChildModules()) {
        module->print_summary_enabled = true;
    }
    root_module.run();

    test::TestModule empty_root_module("Empty root module", nullptr);
    empty_root_module.print_summary_enabled = true;
    empty_root_module.run();

    test::TestModule passing_module("Passing module", nullptr);
    passing_module.print_summary_enabled = true;
    passing_module.addTest("test1", [](test::Test& test) { });
    passing_module.run();
}

int main() {
    run_tests();
    return 0;
}
