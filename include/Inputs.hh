#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include <filesystem>

#include "Common.hh"

namespace SRT 
{
	class Inputs
	{
	public:
		Inputs() =delete;
		~Inputs() =delete;

		static void ParseInputs(int argc, char* argv[]);
		static void PrintHelp();

		static bool InputsContains(const std::string& key);

		static std::string GetInputValueAsString(const std::string& key);
		static int GetInputValueAsInt(const std::string& key);
		static double GetInputValueAsDouble(const std::string& key);
		static bool GetInputValueAsBool(const std::string& key);

	private:
		static strmap inputs_;
	};
}