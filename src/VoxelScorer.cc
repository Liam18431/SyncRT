#include "VoxelScorer.hh"

double SRT::VoxelScorer::dose_[size_n_]{};
double SRT::VoxelScorer::dose2_[size_n_]{};
double SRT::VoxelScorer::unc_[size_n_]{};

double SRT::VoxelScorer::CalculateAverageUncertainty(double dose_max)
{
	std::size_t n = 0;
	double unc_sum = 0;
	double half_dose_max = 0.5 * dose_max;

	for (std::size_t i = 0; i < size_n_; i++)
	{
		double X = dose_[i];
		double unc = unc_[i];
		if (X > (half_dose_max))
		{
			unc_sum += unc * unc;
			n++;
		}
	}

	double avg_unc = std::sqrt((1.0 / static_cast<double>(n)) * unc_sum);

	return avg_unc;
}

double SRT::VoxelScorer::CalculateUncertainties(unsigned long long int total_n_events)
{
	double dose_max = 0;
	double n_events = static_cast<double>(total_n_events);
	for (std::size_t i = 0; i < size_n_; i++)
	{
		unc_[i] = 1;
		if (n_events > 0)
		{
			double X = dose_[i];
			double X2 = dose2_[i];

			if (X > dose_max) dose_max = X;

			double unc_1 = (X2 / n_events) - std::pow((X / n_events), 2);
			double unc_2 = (1 / static_cast<double>(n_events - 1));

			if (unc_1 > 0)
			{
				double SX_ = std::sqrt(unc_2 * unc_1);
				double X_ = X / n_events;
				unc_[i] = SX_ / X_;
			}
		}
	}

	return (dose_max > 0) ? CalculateAverageUncertainty(dose_max) : 1;
}

void SRT::VoxelScorer::WriteDose(const std::string& output_filename)
{
	std::ofstream output_filestream = std::ofstream(output_filename, std::ios::binary | std::ios::out);
	if (output_filestream)
	{
		/* Write header info*/
		uint64_t size_n = size_n_;

		output_filestream.write(reinterpret_cast<const char*>(&size_n), sizeof(size_n));

		uint64_t size_x = size_.x;
		uint64_t size_y = size_.y;
		uint64_t size_z = size_.z;

		output_filestream.write(reinterpret_cast<const char*>(&size_x), sizeof(size_x));
		output_filestream.write(reinterpret_cast<const char*>(&size_y), sizeof(size_y));
		output_filestream.write(reinterpret_cast<const char*>(&size_z), sizeof(size_z));

		double dim_x = dim_.x;
		double dim_y = dim_.y;
		double dim_z = dim_.z;

		output_filestream.write(reinterpret_cast<const char*>(&dim_x), sizeof(dim_x));
		output_filestream.write(reinterpret_cast<const char*>(&dim_y), sizeof(dim_y));
		output_filestream.write(reinterpret_cast<const char*>(&dim_z), sizeof(dim_z));

		double res_x = res_.x;
		double res_y = res_.y;
		double res_z = res_.z;

		output_filestream.write(reinterpret_cast<const char*>(&res_x), sizeof(res_x));
		output_filestream.write(reinterpret_cast<const char*>(&res_y), sizeof(res_y));
		output_filestream.write(reinterpret_cast<const char*>(&res_z), sizeof(res_z));

		/* Write dose data*/
		output_filestream.write(reinterpret_cast<const char*>(&dose_[0]), size_n_ * sizeof(double));

		/* Write uncertainty data*/
		output_filestream.write(reinterpret_cast<const char*>(&unc_[0]), size_n_ * sizeof(double));

	}
	else
	{
		throw std::runtime_error("Failed to open dose file for output");
	}

	return;
}