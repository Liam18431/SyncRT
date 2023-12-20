#include "SpectrumData.hh"

SRT::SpectrumData::SpectrumData(const std::string& spectrum_file)
{
	std::ifstream spectrum_filestream(spectrum_file, std::ios::in);

	std::stringstream err_msg;
	err_msg << "Could not open spectrum file: " << spectrum_file << std::endl;
	if (!spectrum_filestream) throw std::runtime_error(err_msg.str().c_str());

	G4double energy;
	G4double flux;

	std::string line;
	while (std::getline(spectrum_filestream, line))
	{
		if (line.at(0) == '#') continue;
		std::vector<std::string> values;

		size_t pos = 0;
		size_t n_pos = 0;
		while (n_pos != std::string::npos)
		{
			n_pos = line.find_first_of(" \t,;", pos);
			std::string entry = line.substr(pos, n_pos - pos);
			entry.erase(std::remove_if(entry.begin(), entry.end(), isspace), entry.end());
			if (!entry.empty())
			{
				values.push_back(entry);
			}
			pos = n_pos + 1;
		}

		err_msg.clear();
		err_msg << "two entries are required for spectrum file on line: " << line << std::endl;
		if (values.size() != 2) throw std::runtime_error(err_msg.str().c_str());

		energy = std::stod(values.at(0));
		flux = std::stod(values.at(1));

		this->spectrum[energy] = flux;
	}

	this->spectrum_pdf = CalculatePDF(this->spectrum);
	this->spectrum_cdf = CalculateCDF(this->spectrum_pdf);

	CheckSpectra(this->spectrum_pdf, this->spectrum_cdf);
}

std::map<double, double> SRT::SpectrumData::CalculatePDF(std::map<double, double> spectrum)
{
	std::cout << "Calculate spectrum pdf." << std::endl;

	double init = 0;
	double flux_sum = std::accumulate(spectrum.begin(), spectrum.end(), init, [](double value, const auto& p) { return value + p.second; });
	for (auto itr = spectrum.begin(); itr != spectrum.end(); itr++)
	{
		itr->second /= flux_sum;
	}

	return spectrum;
}

std::map<double, double> SRT::SpectrumData::CalculateCDF(std::map<double, double> spectrum_pdf)
{
	std::cout << "Calculate spectrum cdf." << std::endl;

	double probability = 0;
	std::map<double, double> spectrum_cdf;
	for (auto& bin : spectrum_pdf)
	{
		probability += bin.second;
		spectrum_cdf[probability] = bin.first;
	}

	return spectrum_cdf;
}

void SRT::SpectrumData::CheckSpectra(std::map<double, double> spectrum_pdf, std::map<double, double> spectrum_cdf)
{
	/* sanity checks*/
	auto min_energy = std::min_element(spectrum.begin(), spectrum.end(), [](const auto& p1, const auto& p2) { return p1.first < p2.first; });
	std::cout << "Min spectrum energy (keV): " << std::to_string(min_energy->first) << std::endl;

	auto max_energy = std::max_element(spectrum.begin(), spectrum.end(), [](const auto& p1, const auto& p2) { return p1.first < p2.first; });
	std::cout << "Max spectrum energy (keV): " << std::to_string(max_energy->first) << std::endl;

	auto max_flux = std::max_element(spectrum.begin(), spectrum.end(), [](const auto& p1, const auto& p2) { return p1.second < p2.second; });
	std::cout << "Peak spectrum energy (keV): " << std::to_string(max_flux->first) << std::endl;

	double init = 0.0;
	double probability_sum = std::accumulate(spectrum.begin(), spectrum.end(), init, [](double value, const auto& p) { return value + p.second; });
	double sum_product = std::accumulate(spectrum.begin(), spectrum.end(), init, [](double value, const auto& p) { return value + (p.first * p.second); });
	double weighted_mean_energy = sum_product / probability_sum;
	std::cout << "Mean spectrum energy (keV): " + std::to_string(weighted_mean_energy) << std::endl;

	auto min_cdf = spectrum_cdf.begin();
	auto max_cdf = spectrum_cdf.rbegin();

	double epsilon = 1e-4;
	std::stringstream err_msg;
	err_msg << "First probability sample in cdf is not 0, is: " << min_cdf->first << std::endl;
	if (!(std::fabs(min_cdf->first - 0.0) < epsilon)) throw std::runtime_error(err_msg.str().c_str());
	else
	{
		auto value = spectrum_cdf.at(min_cdf->first);
		spectrum_cdf.erase(min_cdf->first);
		spectrum_cdf.insert(spectrum_cdf.begin(), std::make_pair(0.0, value));
	}

	err_msg.clear();
	err_msg << "Last probability sample in cdf is not 1, is: " << max_cdf->first << std::endl;
	if (!(std::fabs(max_cdf->first - 1.0) < epsilon)) throw std::runtime_error(err_msg.str().c_str());
	else
	{
		auto value = spectrum_cdf.at(max_cdf->first);
		spectrum_cdf.erase(max_cdf->first);
		spectrum_cdf.insert(spectrum_cdf.end(), std::make_pair(1.0, value));
	}

	std::cout << "Min spectrum CDF (energy, probability sample): " << std::to_string(spectrum_cdf.begin()->second) << ", " << std::to_string(spectrum_cdf.begin()->first) << std::endl;
	std::cout << "Max spectrum CDF (energy, probability sample): " << std::to_string(spectrum_cdf.rbegin()->second) << ", " << std::to_string(spectrum_cdf.rbegin()->first) << std::endl;

	return;
}


