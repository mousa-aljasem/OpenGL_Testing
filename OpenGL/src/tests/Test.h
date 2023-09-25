#pragma once
#include <vector>
#include <string>
#include <functional>
#include <iostream>

namespace test {
	class Test {
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};

	class TestMenu : public Test {
	public:
		TestMenu(Test*& currentTestPointer);

		void OnImGuiRender() override;

		template<typename T>
		void RegisterTest(const std::string& name) {
			std::cout << "Registering test " << name << std::endl;

			// Pushes a new test into the m_Tests vector of pairs.
			// return new T() lambda returns a Test function that can be accessed
			m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
		}
	private:
		// A reference to the pointer, so it can write to that object
		Test*& m_CurrentTest;
		// A vector of pairs. Each pair contains a string and function which returns a test pointer.
		std::vector<std::pair<std::string, std::function<Test* ()>>> m_Tests;
	};
}