#include "test.h"

class ExampleTestModule3 : public test::TestModule {
public:
	ExampleTestModule3(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });

private:
	void test1_1(test::Test& test);
	void test1_2(test::Test& test);
	void test1_3(test::Test& test);

};
