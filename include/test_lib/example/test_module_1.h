#include "test.h"

class ExampleTestModule1 : public test::TestModule {
public:
	ExampleTestModule1(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });

private:
	void test1_1(test::Test& test);
	void test1_2(test::Test& test);
	void test1_3(test::Test& test);

	void test2_1(test::Test& test);
	void test2_2(test::Test& test);
	void test2_3(test::Test& test);

};
