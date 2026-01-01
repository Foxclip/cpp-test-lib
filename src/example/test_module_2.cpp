#include "example/test_module_2.h"

ExampleTestModule2::ExampleTestModule2(
	const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes
) : TestModule(name, parent, required_nodes) {
	test::TestModule* list_1 = addModule("List1");
	test::Test* test_1_1 = list_1->addTest("first", [&](test::Test& test) { test1_1(test); });
}

void ExampleTestModule2::test1_1(test::Test& test) { }
