#include "example/test_module_3.h"

ExampleTestModule3::ExampleTestModule3(
	const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes
) : TestModule(name, parent, required_nodes) {
	test::TestModule* list_1 = addModule("List1");
	test::Test* test_1_1 = list_1->addTest("first", [&](test::Test& test) { test1_1(test); });
	test::Test* test_1_2 = list_1->addTest("second", [&](test::Test& test) { test1_2(test); });
	test::Test* test_1_3 = list_1->addTest("third", { test_1_2 }, [&](test::Test& test) { test1_3(test); });
	test::TestModule* empty_module = addModule("EmptyModule");
}

void ExampleTestModule3::test1_1(test::Test& test) { }

void ExampleTestModule3::test1_2(test::Test& test) {
	T_CHECK(false);
}

void ExampleTestModule3::test1_3(test::Test& test) { }
