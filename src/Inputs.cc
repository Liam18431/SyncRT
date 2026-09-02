#include "Inputs.hh"

SRT::strmap SRT::Inputs::inputs_;

void SRT::Inputs::ParseInputs(int argc, char* argv[])
{
	inputs_.clear();

	std::vector<std::string> args;
	for (unsigned int i = 0; i < argc; i++) /* Process command line */
	{
		std::string arg = argv[i];
		if (arg.find("--") == 0) args.push_back(arg.substr(2)); /* Assume new argument if arg starts with '--'. */
		else if (!args.empty()) args.back() += arg;
	}

	for (unsigned int i = 0; i < args.size(); i++)
	{
		const std::string& arg = args[i];
		std::string key;
		std::string val;

		std::size_t delim_pos = arg.find("=");

		if (delim_pos == std::string::npos)
		{
			key = arg;
			val = "true";
		}
		else
		{
			key = arg.substr(0, delim_pos);
			val = arg.substr(delim_pos + 1, arg.length());
		}

		inputs_[key] = val;
	}

	return;
}

void SRT::Inputs::PrintHelp()
{
	std::stringstream arg_ss;
	arg_ss << "\t--help [boolean]:    \tShow help menu (default: false)" << std::endl;
	arg_ss << "\t--ncores [int]:      \tThe number of cores to use for a multithreaded simulation. (default: all)" << std::endl;
	arg_ss << "\t--macro [string]:    \tThe path to the visualisation macro file. (default: none)" << std::endl;
	arg_ss << "\t--ui [string]:       \tThe UI session type to use in visualisation mode. (default: Qt)" << std::endl;
	arg_ss << "\t--histories [int]:   \tThe number of histories to simulate in batch mode. Setting this option runs the simulation in batch mode. (default: none)" << std::endl;
	arg_ss << "\t--output [string]:   \tThe output file for storing the simulation dose data. (default: ./dose.dos)" << std::endl;
	std::cout << arg_ss.str();
}

bool SRT::Inputs::InputsContains(const std::string& key)
{
	return inputs_.find(key) != inputs_.end();
}

std::string SRT::Inputs::GetInputValueAsString(const std::string& key)
{
	if (!InputsContains(key))
	{
		std::stringstream err_msg;
		err_msg << "Argument: " << key << ", is required but has not been provided." << std::endl;
		throw std::runtime_error(err_msg.str().c_str());
	}
	return inputs_[key];
}

int SRT::Inputs::GetInputValueAsInt(const std::string& key)
{
	std::string str_val = GetInputValueAsString(key);
	int val = std::stoi(str_val);
	return val;
}

double SRT::Inputs::GetInputValueAsDouble(const std::string& key)
{
	std::string str_val = GetInputValueAsString(key);
	double val = std::stod(str_val);
	return val;
}

bool SRT::Inputs::GetInputValueAsBool(const std::string& key)
{
	std::string str_val = GetInputValueAsString(key);

	std::transform(str_val.begin(), str_val.end(), str_val.begin(),
		[](unsigned char c) { return std::tolower(c); });

	return str_val == "true" || str_val == "yes" || str_val == "on" || str_val == "y" || str_val == "1";
}
