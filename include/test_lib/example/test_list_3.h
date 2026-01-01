#include "test.h"

class TestList3 : public test::TestModule {
public:
	TestList3(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void test3_1(test::Test& test);
	void test3_2(test::Test& test);
	void test3_3(test::Test& test);
};
