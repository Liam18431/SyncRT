#include "SpectrumData.hh"

const std::map<double, double> SRT::SpectrumData::GetSpectrum()
{
	std::map<double, double> result{};

	double min_energy = std::numeric_limits<double>::max();
	double max_energy = 0;
	double max_flux = 0;
	double peak_energy = 0;
	double probability = 0;
	double sum_product = 0;
	for (unsigned int i = 0; i < size_; i++)
	{
		double energy = spectrum_[i][0];
		double flux = spectrum_[i][1] / flux_;

		if (energy < min_energy) min_energy = energy;
		if (energy > max_energy) max_energy = energy;
		
		if (flux > max_flux)
		{
			max_flux = flux;
			peak_energy = energy;
		}
		
		probability += flux;
		sum_product += energy * flux;
		
		result[probability] = energy;
	}
	double weighted_mean_energy = sum_product / probability;

	std::cout << "Min spectrum energy (MeV): " << min_energy << std::endl;
	std::cout << "Max spectrum energy (MeV): " << max_energy << std::endl;
	std::cout << "Peak spectrum energy (MeV): " << peak_energy << std::endl;
	std::cout << "Mean spectrum energy (MeV): " << weighted_mean_energy << std::endl;

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