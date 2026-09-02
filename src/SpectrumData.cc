#include "SpectrumData.hh"

const std::map<double, double> SRT::SpectrumData::GetSpectrum()
{
	std::map<double, double> result{};

	double init = 0;
	double flux_sum = 0;

	result = {};
	double probability = 0;
	for (unsigned int i = 0; i < size_; i++)
	{
		double energy = spectrum_[i][0];
		double flux = spectrum_[i][1];

		probability += flux / flux_;
		result[probability] = energy;
	}

	/* sanity checks*/
	double min_energy = std::numeric_limits<double>::max();
	double max_energy = 0;
	double max_flux = 0;
	double peak_energy = 0;
	double probability_sum = 0;
	double sum_product = 0;
	for (auto& elm : result)
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

	double min_cdf = result.begin()->first;
	double max_cdf = result.rbegin()->first;
	if (!ApproxEquals(min_cdf, 0.0, 1e-9))
	{
		std::stringstream err_msg;
		err_msg << "First probability sample in CDF must be 0, is: " << min_cdf << std::endl;
		throw std::runtime_error(err_msg.str().c_str());
	}
	else
	{
		double value = result.at(min_cdf);
		result.erase(min_cdf);
		result.insert(result.begin(), std::make_pair(0.0, value));
	}

	if (!ApproxEquals(max_cdf, 1.0, 1e-9))
	{
		std::stringstream err_msg;
		err_msg << "Last probability sample in CDF must be 1, is: " << max_cdf << std::endl;
		throw std::runtime_error(err_msg.str().c_str());
	}
	else
	{
		double value = result.at(max_cdf);
		result.erase(max_cdf);
		result.insert(result.end(), std::make_pair(1.0, value));
	}

	return result;
}