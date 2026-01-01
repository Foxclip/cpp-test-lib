#include "test.h"
#include "common/logger.h"
#include <cassert>
#include <algorithm>

namespace test {

	bool TestNode::isRoot() const {
		return parent == nullptr;
	}

	Test::Test(std::string name, TestFuncType func) {
		this->name = name;
		this->func = func;
		root_error = std::make_unique<TestError>("root", TestError::Type::Root);
		error_stack.push(root_error.get());
	}

	Test::Test(std::string name, std::vector<TestNode*> required, TestFuncType func)
	: Test(name, func) {
		this->required_nodes = required;
	}

	bool Test::run() {
		if (!std::all_of(required_nodes.begin(), required_nodes.end(), [](TestNode* test) {
			return test->result;
		})) {
			cancelled = true;
			return false;
		}
		result = true;
		try {
			func(*this);
		} catch (std::exception exc) {
			getCurrentError()->add("EXCEPTION: " + std::string(exc.what()));
			result = false;
		}
		return result;
	}

	TestError* Test::getCurrentError() const {
		return error_stack.top();
	}

	std::string Test::char_to_str(char c) {
		if (c < -1) {
			return "(" + std::to_string(c) + ")";
		} else if (c == '\n') {
			return "\\n";
		} else if (c == '\r') {
			return "\\r";
		} else if (c == '\t') {
			return "\\t";
		} else if (c == '\0') {
			return "\\0";
		} else if (c == '\\') {
			return "\\\\";
		} else if (c == '"') {
			return "\\\"";
		} else if (c == EOF) {
			return "\\(EOF)";
		} else {
			return std::string(1, c);
		}
	}

	std::string Test::char_to_esc(std::string str, bool convert_quotes) {
		std::string result;
		for (size_t i = 0; i < str.size(); i++) {
			char current_char = str[i];
			if (!convert_quotes && current_char == '"') {
				result += current_char;
			} else {
				result += char_to_str(current_char);
			}
		}
		return result;
	}

	TestError::TestError(const std::string& str, Type type) {
		this->str = str;
		this->type = type;
	}

	TestError* TestError::add(const std::string& message, Type type) {
		std::unique_ptr<TestError> uptr = std::make_unique<TestError>(message, type);
		TestError* ptr = uptr.get();
		subentries.push_back(std::move(uptr));
		return ptr;
	}

	void TestError::log() const {
		if (type == Type::Container) {
			std::function<bool(const TestError& error)> has_non_container_subentries = [&](const TestError& error) {
				for (auto& subentry : error.subentries) {
					if (subentry->type != Type::Container) {
						return true;
					} else {
						if (has_non_container_subentries(*subentry)) {
							return true;
						}
					}
				}
				return false;
			};
			if (!has_non_container_subentries(*this)) {
				return;
			}
		}
		if (type != Type::Root) {
			std::string esc_str = Test::char_to_esc(str, false);
			logger << esc_str << "\n";
		}
		if (subentries.size() > 0) {
			LoggerIndent subentries_indent(type != Type::Root);
			for (auto& subentry : subentries) {
				subentry->log();
			}
		}
	}

	ErrorContainer::ErrorContainer(Test& test, const std::string& file, size_t line, const std::string& message) : test(test) {
		std::string filename = std::filesystem::path(file).filename().string();
		std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
		std::string space_str = message.size() > 0 ? " " : "";
		TestError* error = test.getCurrentError()->add(message + space_str + location_str, TestError::Type::Container);
		test.error_stack.push(error);
	}

	ErrorContainer::~ErrorContainer() {
		close();
	}

	void ErrorContainer::close() {
		test.error_stack.pop();
	}

	TestModule::TestModule(const std::string& name, TestModule* parent, const std::vector<TestNode*>& required_nodes) {
		this->name = name;
		this->parent = parent;
		this->required_nodes = required_nodes;
	}

	Test* TestModule::addTest(const std::string& name, TestFuncType func) {
		return addTest(name, { }, func);
	}

	Test* TestModule::addTest(const std::string& name, const std::vector<TestNode*>& required, TestFuncType func) {
		std::unique_ptr<Test> uptr = std::make_unique<Test>(name, required, func);
		Test* ptr = uptr.get();
		children.push_back(std::move(uptr));
		return ptr;
	}

	TestModule* TestModule::addModule(const std::string& name, const std::vector<TestNode*>& required) {
		std::unique_ptr<TestModule> uptr = std::make_unique<TestModule>(name, this, required);
		TestModule* ptr = uptr.get();
		children.push_back(std::move(uptr));
		return ptr;
	}

	TestModule* TestModule::getRoot() {
		TestModule* currentModule = this;
		while (currentModule) {
			if (currentModule->isRoot()) {
				return currentModule;
			}
			currentModule = currentModule->parent;
		}
		return currentModule;
	}

	std::vector<TestNode*> TestModule::getChildren() const {
		std::vector<TestNode*> result;
		for (size_t i = 0; i < children.size(); i++) {
			result.push_back(children[i].get());
		}
		return result;
	}

	std::vector<Test*> TestModule::getChildTests() const {
		std::vector<Test*> result;
		for (size_t i = 0; i < children.size(); i++) {
			if (Test* test = dynamic_cast<Test*>(children[i].get())) {
				result.push_back(test);
			}
		}
		return result;
	}

	std::vector<TestModule*> TestModule::getChildModules() const {
		std::vector<TestModule*> result;
		for (size_t i = 0; i < children.size(); i++) {
			if (TestModule* module = dynamic_cast<TestModule*>(children[i].get())) {
				result.push_back(module);
			}
		}
		return result;
	}

	std::vector<Test*> TestModule::getAllTests() const {
		std::vector<Test*> result;
		for (size_t i = 0; i < children.size(); i++) {
			TestNode* node = children[i].get();
			if (Test* test = dynamic_cast<Test*>(node)) {
				result.push_back(test);
			} else if (TestModule* module = dynamic_cast<TestModule*>(node)) {
				std::vector<Test*> module_tests = module->getAllTests();
				result.insert(result.end(), module_tests.begin(), module_tests.end());
			}
		}
		return result;
	}

	bool TestModule::run() {
		if (isRoot()) {
			std::vector<Test*> all_tests = getAllTests();
			for (Test* test : all_tests) {
				if (test->name.size() > max_test_name) {
					max_test_name = test->name.size();
				}
			}
			logger << name << "\n";
		}
		LoggerIndent test_list_indent(1, isRoot());
		beforeRunModule();
		OnBeforeRun();
		for (auto& node : children) {
			if (Test* test = dynamic_cast<Test*>(node.get())) {
				std::string spacing_str;
				size_t spacing_size = getRoot()->max_test_name - test->name.size();
				for (size_t i = 0; i < spacing_size; i++) {
					spacing_str += "-";
				}
				logger << test->name << spacing_str << "|" << LoggerFlush();
				Logger::disableStdWrite();
				logger.manualDeactivate();
				OnBeforeRunTest();
				bool result = test->run();
				OnAfterRunTest();
				logger.manualActivate();
				Logger::enableStdWrite();
				if (result) {
					logger << "passed" << "\n";
					passed_list.push_back(test->name);
				} else {
					if (test->cancelled) {
						logger << "cancelled" << "\n";
						cancelled_list.push_back(test->name);
					} else {
						logger << "FAILED" << "\n";
						LoggerIndent errors_indent;
						test->root_error->log();
						failed_list.push_back(test->name);
					}
				}
			} else if (TestModule* module = dynamic_cast<TestModule*>(node.get())) {
				logger << module->name << "\n";
				LoggerIndent test_list_indent;
				bool cancelled = false;
				if (module->children.empty()) {
					empty_module_list.push_back(module->name);
				}
				for (TestNode* req_node : module->required_nodes) {
					if (!req_node->result) {
						cancelled = true;
						break;
					}
				}
				if (cancelled) {
					std::vector<Test*> tests = module->getAllTests();
					for (Test* test : tests) {
						test->cancelled = true;
						module->cancelled_list.push_back(test->name);
					}
					logger << "Cancelled " << tests.size() << " tests\n";
				} else {
					module->run();
				}
				for (const std::string& name : module->passed_list) {
					passed_list.push_back(module->name + "/" + name);
				}
				for (const std::string& name : module->cancelled_list) {
					cancelled_list.push_back(module->name + "/" + name);
				}
				for (const std::string& name : module->failed_list) {
					failed_list.push_back(module->name + "/" + name);
				}
				for (const std::string& name : module->empty_module_list) {
					empty_module_list.push_back(module->name + "/" + name);
				}
			}
		}
		afterRunModule();
		OnAfterRun();
		if (print_summary_enabled) {
			printSummary();
		}
		is_run = true;
		result = cancelled_list.empty() && failed_list.empty();
		return result;
	}

	void TestModule::printSummary() {
		logger << "Passed " << passed_list.size() << " tests, "
			<< "cancelled " << cancelled_list.size() << " tests, "
			<< "failed " << failed_list.size() << " tests";
		if (failed_list.size() > 0) {
			logger << ":\n";
			LoggerIndent failed_list_indent;
			for (const std::string& name : failed_list) {
				logger << name << "\n";
			}
		} else {
			logger << "\n";
		}
		if (empty_module_list.size() > 0) {
			logger << "WARNING: " << empty_module_list.size() << " empty modules:\n";
			LoggerIndent empty_modules_list_indent;
			for (const std::string& module_name : empty_module_list) {
				logger << module_name << "\n";
			}
		}
		if (isRoot()) {
			if (passed_list.size() > 0 && cancelled_list.empty() && failed_list.empty()) {
				logger << "ALL PASSED\n";
			}
		}
	}

	void TestModule::beforeRunModule() { }

	void TestModule::afterRunModule() { }

	void TestModule::testMessage(Test& test, const std::string& file, size_t line, const std::string& message) {
		std::string filename = std::filesystem::path(file).filename().string();
		std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
		test.getCurrentError()->add(message + " " + location_str);
	}

	bool TestModule::testCheck(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message) {
		if (!value) {
			std::string filename = std::filesystem::path(file).filename().string();
			std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
			test.getCurrentError()->add("Failed condition: " + value_message + " " + location_str);
			test.result = false;
		}
		return value;
	}

	bool TestModule::testCheck(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message, const std::string message) {
		if (!value) {
			std::string filename = std::filesystem::path(file).filename().string();
			std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
			test.getCurrentError()->add(message + ": " + value_message + " " + location_str);
			test.result = false;
		}
		return value;
	}

}
