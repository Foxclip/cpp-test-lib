#include "example/test_module_1.h"
#include "example/test_list_3.h"

ExampleTestModule1::ExampleTestModule1(
	const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes
) : TestModule(name, parent, required_nodes) {
	test::TestModule* list_1 = addModule("List1");
	test::Test* test_1_1 = list_1->addTest("first", [&](test::Test& test) { test1_1(test); });
	test::Test* test_1_2 = list_1->addTest("second", { test_1_1 }, [&](test::Test& test) { test1_2(test); });
	test::Test* test_1_3 = list_1->addTest("third", { test_1_2 }, [&](test::Test& test) { test1_3(test); });

	test::TestModule* list_2 = addModule("List2");
	test::Test* test_2_1 = list_2->addTest("first", [&](test::Test& test) { test2_1(test); });
	test::Test* test_2_2 = list_2->addTest("second", [&](test::Test& test) { test2_2(test); });
	test::Test* test_2_3 = list_2->addTest("third", { test_2_1, test_2_2 }, [&](test::Test& test) { test2_3(test); });

	TestList3* list_3 = addModule<TestList3>("List3", { list_2 });
}

void ExampleTestModule1::test1_1(test::Test& test) {
	int four = 2 + 2;
	T_CHECK(four > 0);
	T_COMPARE(four, 4);
}

void ExampleTestModule1::test1_2(test::Test& test) { }

void ExampleTestModule1::test1_3(test::Test& test) { }

void ExampleTestModule1::test2_1(test::Test& test) { }

void ExampleTestModule1::test2_2(test::Test& test) {
	T_CHECK(false);
}

void ExampleTestModule1::test2_3(test::Test& test) { }
