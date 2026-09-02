#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include "globals.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include "Common.hh"

namespace SRT
{
	class VoxelScorer
	{
	public:
		VoxelScorer() = delete;
		~VoxelScorer() = delete;
		VoxelScorer(const VoxelScorer&) = delete;
		VoxelScorer& operator = (const VoxelScorer&) = delete;
		
		static inline bool LocalCoordInsideVoxelScorer(const G4ThreeVector& coord, double epsilon = 1 * nm)
		{
			bool outside =
				((coord.x() < -half_dim_.x + epsilon) || (coord.x() > half_dim_.x - epsilon)) ||
				((coord.y() < -half_dim_.y + epsilon) || (coord.y() > half_dim_.y - epsilon)) ||
				((coord.z() < -half_dim_.z + epsilon) || (coord.z() > half_dim_.z - epsilon));

			return !outside;
		}
		static inline int GetBinFromLocalCoords(const G4ThreeVector& coords)
		{
			int index_x = static_cast<int>((coords.x() + half_dim_.x) / res_.x);
			int index_y = static_cast<int>((coords.y() + half_dim_.y) / res_.y);
			int index_z = static_cast<int>((coords.z() + half_dim_.z) / res_.z);

			int bin = index_x + index_y * size_.x + index_z * size_xy_;

			return bin;
		}
		static inline void AddDose(int bin, double dose)
		{
			if ((bin >= 0 && bin < size_n_))
			{
				dose_[bin] += dose;
				dose2_[bin] += dose * dose;
			}
#if _DEBUG
			else
			{
				throw std::runtime_error("Bin exceeds dose dimensions.");
			}
#endif
			return;
		}
		static double CalculateUncertainties(unsigned long long int total_n_events);
		static void WriteDose(const std::string& output_filename);

		static constexpr vec3d pos_ = { 0 * cm, 0 * cm, 0 * cm };
	private:
		static constexpr vec3d tdim_ = { 20 * cm, 30 * mm, 30 * mm };
	public:
		static constexpr vec3d res_ = { 1 * mm, 5 * um, 1 * mm };
		static constexpr vec3i size_ = {
			static_cast<int>(tdim_.x / res_.x),
			static_cast<int>(tdim_.y / res_.y),
			static_cast<int>(tdim_.z / res_.z)
		};

		/* Recompute the dimensions to account for the size of the voxel scorer*/
		static constexpr vec3d dim_ =
		{
			size_.x * res_.x,
			size_.y * res_.y,
			size_.z * res_.z
		};
		static constexpr double voxel_volume_ = res_.x * res_.y * res_.z;

		static constexpr vec3d half_dim_ = { 0.5 * dim_.x, 0.5 * dim_.y, 0.5 * dim_.z };
		static constexpr vec3d half_res_ = { 0.5 * res_.x, 0.5 * res_.y, 0.5 * res_.z };
		static constexpr int size_xy_ = size_.x * size_.y;
		static constexpr int size_n_ = size_xy_ * size_.z;

	private:
		static double CalculateAverageUncertainty(double dose_max);

		static double dose_[size_n_];
		static double dose2_[size_n_];
		static double unc_[size_n_];
	};
}
