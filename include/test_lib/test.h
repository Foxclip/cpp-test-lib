#pragma once

#include <vector>
#include <functional>
#include <string>
#include <memory>
#include <filesystem>
#include <stack>

namespace test {

class Test;
using TestFuncType = std::function<void(Test& test)>;

// prefixes in macros needed to allow calling from free functions

#define T_MESSAGE(message) \
	test::testMessage(test, __FILE__, __LINE__, message)

#define T_CHECK(value, ...) \
	test::testCheck(test, __FILE__, __LINE__, value, #value, __VA_ARGS__)

#define T_COMPARE(actual, expected, ...) \
	test::testCompare(test, __FILE__, __LINE__, #actual, actual, expected, __VA_ARGS__)

#define T_APPROX_COMPARE(actual, expected, ...) \
	test::testApproxCompare(test, __FILE__, __LINE__, #actual, actual, expected, __VA_ARGS__)

#define T_VEC2_COMPARE(actual, expected, ...) \
	test::testVec2Compare(test, __FILE__, __LINE__, #actual, actual, expected, __VA_ARGS__)

#define T_VEC2_APPROX_COMPARE(actual, expected, ...) \
	test::testVec2ApproxCompare(test, __FILE__, __LINE__, #actual, actual, expected, __VA_ARGS__)

#define T_ASSERT(expr) \
	if (!expr) { \
		return; \
	} \

#define T_ASSERT_NO_ERRORS() \
	if (!test.result) { \
		return; \
	} \

#define T_CONTAINER(message) \
	test::ErrorContainer error_container(test, __FILE__, __LINE__, message);

#define T_WRAP_CONTAINER(expr, ...) \
	{ \
		T_CONTAINER(#expr); \
		expr; \
	}

	class TestModule;

	// Free function declarations for test macros
	void testMessage(Test& test, const std::string& file, size_t line, const std::string& message);
	bool testCheck(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message);
	bool testCheck(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message, const std::string message);
	template<typename T1, typename T2>
	bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected);
	template<typename T1, typename T2, typename TStr>
	bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str);
	template<typename T1, typename T2, typename TStr, typename TCmp>
	bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str, TCmp cmp);
	template<typename T>
	bool testApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, T epsilon = 0.0001f);
	template<typename T>
	bool testVec2Compare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected);
	template<typename T>
	bool testVec2ApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, double epsilon = 0.0001);
	template<typename T, typename TEps>
	bool equals(T left, T right, TEps epsilon = 0.0001f);

	class TestNode {
	public:
		std::string name = "<unnamed>";
		TestModule* parent = nullptr;
		std::vector<TestNode*> required_nodes;
		bool is_run = false;
		bool result = false;
		bool cancelled = false;
		bool isRoot() const;
		virtual bool run() = 0;
	private:
	};

	struct TestError {
		enum class Type {
			Root,
			Container,
			Normal,
		};
		Type type;
		std::string str;
		std::vector<std::unique_ptr<TestError>> subentries;
		explicit TestError(const std::string& str, Type type);
		TestError* add(const std::string& message, Type type = Type::Normal);
		void log() const;
	};

	class Test : public TestNode {
	public:
		std::unique_ptr<TestError> root_error;

		Test(std::string name, TestFuncType func);
		Test(std::string name, std::vector<TestNode*> required, TestFuncType func);
		bool run() override;
		TestError* getCurrentError() const;
		static std::string char_to_str(char c);
		static std::string char_to_esc(std::string str, bool convert_quotes = true);

	private:
		friend class ErrorContainer;
		TestFuncType func;
		std::stack<TestError*> error_stack;
	};

	class ErrorContainer {
	public:
		ErrorContainer(Test& test, const std::string& file, size_t line, const std::string& message = "");
		~ErrorContainer();

	private:
		Test& test;

		void close();
	};

	class TestModule : public TestNode {
	public:

		std::vector<std::unique_ptr<TestNode>> children;
		std::vector<std::string> passed_list;
		std::vector<std::string> cancelled_list;
		std::vector<std::string> failed_list;
		std::vector<std::string> empty_module_list;
		size_t max_test_name = 0;
		std::function<void(void)> OnBeforeRun = []() { };
		std::function<void(void)> OnAfterRun = []() { };
		std::function<void(void)> OnBeforeRunTest = []() { };
		std::function<void(void)> OnAfterRunTest = []() { };

		TestModule(const std::string& name, TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
		Test* addTest(const std::string& name, TestFuncType func);
		Test* addTest(const std::string& name, const std::vector<TestNode*>& required, TestFuncType func);
		TestModule* addModule(const std::string& name, const std::vector<TestNode*>& required = { });
		template<typename T>
		requires std::derived_from<T, TestModule>
		T* addModule(const std::string& name, const std::vector<TestNode*>& required = { });
		TestModule* getRoot();
		std::vector<TestNode*> getChildren() const;
		std::vector<Test*> getChildTests() const;
		std::vector<TestModule*> getChildModules() const;
		std::vector<Test*> getAllTests() const;
		bool run() override;
		void printSummary();

	protected:

		virtual void beforeRunModule();
		virtual void afterRunModule();

	private:
		// Deleted - converted to free functions
		friend void testMessage(Test& test, const std::string& file, size_t line, const std::string& message);
		friend bool testCheck(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message);
		friend bool testCheck(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message, const std::string message);
		template<typename T1, typename T2, typename TStr>
		friend bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str);
		template<typename T1, typename T2, typename TStr, typename TCmp>
		friend bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str, TCmp cmp);
		template<typename T>
		friend bool testApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, T epsilon);
		template<typename T>
		friend bool testVec2Compare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected);
		template<typename T>
		friend bool testVec2ApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, double epsilon);
		template<typename T, typename TEps>
		friend bool equals(T left, T right, TEps epsilon);
		template<typename T1, typename T2, typename TStr>
		friend void compareFail(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str);

	};

	template<typename T>
	requires std::derived_from<T, TestModule>
	T* TestModule::addModule(const std::string& name, const std::vector<TestNode*>& required) {
		std::unique_ptr<T> uptr = std::make_unique<T>(name, this, required);
		T* ptr = uptr.get();
		children.push_back(std::move(uptr));
		return ptr;
	}

	template<typename T1, typename T2>
	bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected) {
		if constexpr (std::convertible_to<T1, std::string> || std::same_as<T1, const char*>) {
			auto func = [](const T1& val) { return val; };
			return testCompare(test, file, line, name, actual, expected, func);
		} else {
			auto func = [](const T1& val) { return std::to_string(val); };
			return testCompare(test, file, line, name, actual, expected, func);
		}
	}

	template<typename T1, typename T2, typename TStr>
	bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str) {
		if (actual != expected) {
			compareFail(test, file, line, name, actual, expected, to_str);
			return false;
		}
		return true;
	}

	template<typename T1, typename T2, typename TStr, typename TCmp>
	bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str, TCmp cmp) {
		if (!cmp(actual, expected)) {
			compareFail(test, file, line, name, actual, expected, to_str);
			return false;
		}
		return true;
	}

	template<typename T>
	bool testApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, T epsilon) {
		if (!equals(actual, expected, epsilon)) {
			auto func = [](const T& val) { return std::to_string(val); };
			compareFail(test, file, line, name, actual, expected, func);
			return false;
		}
		return true;
	}

	template<typename T>
	bool testVec2Compare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected) {
		if (actual.x != expected.x || actual.y != expected.y) {
			auto to_str = [](const T& vec) {
				return "(" + std::to_string(vec.x) + " " + std::to_string(vec.y) + ")";
			};
			compareFail(test, file, line, name, actual, expected, to_str);
			return false;
		}
		return true;
	}

	template<typename T>
	bool testVec2ApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, double epsilon) {
		if (!equals(actual.x, expected.x, epsilon) || !equals(actual.y, expected.y, epsilon)) {
			auto to_str = [](const T& vec) {
				return "(" + std::to_string(vec.x) + " " + std::to_string(vec.y) + ")";
			};
			compareFail(test, file, line, name, actual, expected, to_str);
			return false;
		}
		return true;
	}

	template<typename T, typename TEps>
	bool equals(T left, T right, TEps epsilon) {
		return abs(left - right) < epsilon;
	}

	template<typename T1, typename T2, typename TStr>
	void compareFail(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str) {
		std::string filename = std::filesystem::path(file).filename().string();
		std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
		TestError* error = test.getCurrentError()->add(name + " " + location_str);
		error->add("Expected value: " + to_str(expected));
		error->add("Actual value:   " + to_str(actual));
		test.result = false;
	}

}
