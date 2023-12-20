#include "VoxelScorer.hh"

SRT::VoxelScorer::VoxelScorer(const std::string& output_filename)
{
	this->output_filename_ = output_filename;
	this->output_filestream_ = std::ofstream(this->output_filename_, std::ios::binary | std::ios::out);
	if (!this->output_filestream_) throw std::runtime_error("Failed to open dose file for output");

	this->pos_ = { 0 *cm, 0 *cm, 0 *cm };
	this->dim_ = { 20 * cm, 30 * mm, 30 * mm };
	this->res_ = { 1 * mm, 5 * um, 1 * mm };

	this->size_ = 
	{
		static_cast<int>(this->dim_.x() / this->res_.x()),
		static_cast<int>(this->dim_.y() / this->res_.y()),
		static_cast<int>(this->dim_.z() / this->res_.z())
	};

	/* Recompute the dimensions to account for the size of the voxel scorer*/
	this->dim_ =
	{
		this->size_.x * this->res_.x(),
		this->size_.y * this->res_.y(),
		this->size_.z * this->res_.z()
	};

	this->half_dim_ = dim_ / 2.0;
	this->half_res_ = res_ / 2.0;

	this->size_xy_ = this->size_.x * this->size_.y;
	this->size_n_ = this->size_xy_ * this->size_.z;

	this->dose_ = std::vector<double>(this->size_n_, 0);
	this->dose2_ = std::vector<double>(this->size_n_, 0);
	this->unc_ = std::vector<double>(this->size_n_, 1);
}

SRT::VoxelScorer::~VoxelScorer()
{
}

int SRT::VoxelScorer::GetBinFromLocalCoords(const G4ThreeVector& coords)
{
	int index_x = static_cast<int>((coords.x() + this->half_dim_.x()) / this->res_.x());
	int index_y = static_cast<int>((coords.y() + this->half_dim_.y()) / this->res_.y());
	int index_z = static_cast<int>((coords.z() + this->half_dim_.z()) / this->res_.z());

	int bin = index_x + index_y * this->size_.x + index_z * this->size_xy_;

	return bin;
}

void SRT::VoxelScorer::AddDose(int bin, double dose)
{
	if ((bin >= 0 && bin < size_n_))
	{
		this->dose_[bin] += dose;
		this->dose2_[bin] += dose * dose;
	}
#if _DEBUG
	else
	{
		throw std::runtime_error("Bin exceeds dose dimensions.");
	}
#endif

	return;
}

double SRT::VoxelScorer::CalculateAverageUncertainty(double dose_max)
{
	size_t n = 0;
	double unc_sum = 0;
	double half_dose_max = dose_max / 2.;

	for (size_t i = 0; i < this->size_n_; i++)
	{
		double X = this->dose_[i];
		double unc = this->unc_[i];
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
	std::fill(this->unc_.begin(), this->unc_.end(), 1);

	double dose_max = 0;
	double n_events = static_cast<double>(total_n_events);
	if (n_events > 1)
	{
		for (size_t i = 0; i < this->size_n_; i++)
		{
			double X = this->dose_[i];
			double X2 = this->dose2_[i];

			if (X > dose_max) dose_max = X;

			double unc_1 = (X2 / n_events) - std::pow((X / n_events), 2);
			double unc_2 = (1 / static_cast<double>(n_events - 1));

			if (unc_1 > 0)
			{
				double SX_ = std::sqrt(unc_2 * unc_1);
				double X_ = X / n_events;
				this->unc_[i] = SX_ / X_;
			}
		}
	}

	return (dose_max > 0) ? CalculateAverageUncertainty(dose_max) : 1;
}

void SRT::VoxelScorer::WriteDose()
{
	std::vector out_dose(this->dose_);

	double volume = this->res_.x() * this->res_.y() * this->res_.z();
	for (int i = 0; i < out_dose.size(); i++)
	{
		out_dose[i] = this->dose_[i] / volume; /* Gives dose in 1e-12 Gy.*/
	}

	/* Write header info*/
	uint64_t size_n = this->size_n_;

	this->output_filestream_.write(reinterpret_cast<const char*>(&size_n), sizeof(size_n));

	uint64_t size_x = this->size_.x;
	uint64_t size_y = this->size_.y;
	uint64_t size_z = this->size_.z;

	this->output_filestream_.write(reinterpret_cast<const char*>(&size_x), sizeof(size_x));
	this->output_filestream_.write(reinterpret_cast<const char*>(&size_y), sizeof(size_y));
	this->output_filestream_.write(reinterpret_cast<const char*>(&size_z), sizeof(size_z));

	double dim_x = this->dim_.x();
	double dim_y = this->dim_.y();
	double dim_z = this->dim_.z();

	this->output_filestream_.write(reinterpret_cast<const char*>(&dim_x), sizeof(dim_x));
	this->output_filestream_.write(reinterpret_cast<const char*>(&dim_y), sizeof(dim_y));
	this->output_filestream_.write(reinterpret_cast<const char*>(&dim_z), sizeof(dim_z));

	double res_x = this->res_.x();
	double res_y = this->res_.y();
	double res_z = this->res_.z();

	this->output_filestream_.write(reinterpret_cast<const char*>(&res_x), sizeof(res_x));
	this->output_filestream_.write(reinterpret_cast<const char*>(&res_y), sizeof(res_y));
	this->output_filestream_.write(reinterpret_cast<const char*>(&res_z), sizeof(res_z));

	/* Write dose data*/
	this->output_filestream_.write(reinterpret_cast<const char*>(&out_dose[0]), this->size_n_ * sizeof(double));

	/* Write uncertainty data*/
	this->output_filestream_.write(reinterpret_cast<const char*>(&this->unc_[0]), this->size_n_ * sizeof(double));

	return;
}