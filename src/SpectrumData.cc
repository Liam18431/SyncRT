#include "SpectrumData.hh"

SRT::SpectrumData::SpectrumData(const std::string& spectrum_file)
{
	std::ifstream spectrum_filestream(spectrum_file, std::ios::in);

	if (!spectrum_filestream)
	{
		std::stringstream err_msg;
		err_msg << "Could not open spectrum file: " << spectrum_file << std::endl;
		throw std::runtime_error(err_msg.str().c_str());
	}

	G4double energy;
	G4double flux;

	std::map<double, double> spectrum;
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
				if (entry.at(0) == '#') break;
				values.push_back(entry);
			}
			pos = n_pos + 1;
		}

		if (values.size() != 2)
		{
			std::stringstream err_msg;
			err_msg << "Two entries are required for spectrum file on line: " << line << std::endl;
			throw std::runtime_error(err_msg.str().c_str());
		}

		energy = std::stod(values.at(0));
		flux = std::stod(values.at(1));

		spectrum[energy] = flux;
	}

	if (spectrum.size() <= 1) throw std::runtime_error("At least two energy bins are required for spectrum file.");

	std::map<double, double> spectrum_pdf = CalculatePDF(spectrum);
	this->spectrum_cdf_ = CalculateCDF(spectrum_pdf);

	CanonicaliseSpectra(spectrum_pdf, spectrum_cdf_);
}

std::map<double, double> SRT::SpectrumData::CalculatePDF(const std::map<double, double>& spectrum_)
{
	double init = 0;
	double flux_sum = 0;
	std::map<double, double> result = spectrum_;
	for (auto& elm : result)
	{
		flux_sum += elm.second;
	}

	for (auto& elm : result)
	{
		elm.second /= flux_sum;
	}

	return result;
}

std::map<double, double> SRT::SpectrumData::CalculateCDF(const std::map<double, double>& spectrum_pdf)
{
	double probability = 0;
	std::map<double, double> spectrum_cdf;
	for (auto& bin : spectrum_pdf)
	{
		probability += bin.second;
		spectrum_cdf[probability] = bin.first;
	}

	return spectrum_cdf;
}

static inline bool ApproxEquals(double v1, double v2, double eps = std::numeric_limits<double>::epsilon())
{
	bool result = std::fabs(v1 - v2) < eps;

	return result;
}

void SRT::SpectrumData::CanonicaliseSpectra(std::map<double, double>& spectrum_pdf, std::map<double, double>& spectrum_cdf)
{
	/* sanity checks*/
	double min_energy = std::numeric_limits<double>::max();
	double max_energy = 0;
	double max_flux = 0;
	double peak_energy = 0;
	double probability_sum = 0;
	double sum_product = 0;
	for (auto& elm : spectrum_pdf)
	{
		if (elm.first < min_energy) min_energy = elm.first;
		if (elm.first > max_energy) max_energy = elm.first;
		if (elm.second > max_flux)
		{
			max_flux = elm.second;
			peak_energy = elm.first;
		}

		probability_sum += elm.second;
		sum_product += elm.first * elm.second;
	}
	double weighted_mean_energy = sum_product / probability_sum;

	std::cout << "Min spectrum energy (keV): " << std::to_string(min_energy) << std::endl;
	std::cout << "Max spectrum energy (keV): " << std::to_string(max_energy) << std::endl;
	std::cout << "Peak spectrum energy (keV): " << std::to_string(peak_energy) << std::endl;
	std::cout << "Mean spectrum energy (keV): " + std::to_string(weighted_mean_energy) << std::endl;

	double min_cdf = spectrum_cdf.begin()->first;
	double max_cdf = spectrum_cdf.rbegin()->first;
	if (!ApproxEquals(min_cdf, 0.0, 1e-9))
	{
		std::stringstream err_msg;
		err_msg << "First probability sample in CDF must be 0, is: " << min_cdf << std::endl;
		throw std::runtime_error(err_msg.str().c_str());
	}
	else
	{
		double value = spectrum_cdf.at(min_cdf);
		spectrum_cdf.erase(min_cdf);
		spectrum_cdf.insert(spectrum_cdf.begin(), std::make_pair(0.0, value));
	}

	if (!ApproxEquals(max_cdf, 1.0, 1e-9))
	{
		std::stringstream err_msg;
		err_msg << "Last probability sample in CDF must be 1, is: " << max_cdf << std::endl;
		throw std::runtime_error(err_msg.str().c_str());
	}
	else
	{
		double value = spectrum_cdf.at(max_cdf);
		spectrum_cdf.erase(max_cdf);
		spectrum_cdf.insert(spectrum_cdf.end(), std::make_pair(1.0, value));
	}

	return;
}


