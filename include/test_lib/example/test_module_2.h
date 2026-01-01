#include "test.h"

class ExampleTestModule2 : public test::TestModule {
public:
	ExampleTestModule2(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });

private:
	void test1_1(test::Test& test);

};
