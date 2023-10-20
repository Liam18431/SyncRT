#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include "globals.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

namespace SRT
{
	class VoxelScorer
	{
	public:
		VoxelScorer(const std::string& output_filename);
		~VoxelScorer();

		int GetBinFromLocalCoords(const G4ThreeVector& coords);

		void AddDose(int bin, double dose);

		double CalculateUncertainties(unsigned long long int total_n_events);

		void WriteDose();

		G4ThreeVector GetDimensions() { return this->dim_; }
		G4ThreeVector GetPosition() { return this->pos_; }

		std::string GetOutputFilename() { return this->output_filename_; }

	private:
		double CalculateAverageUncertainty(double dose_max);

		struct v3i
		{
			int x;
			int y;
			int z;
		};

		G4ThreeVector pos_ = {};
		G4ThreeVector dim_ = {};
		G4ThreeVector half_dim_ = {};
		G4ThreeVector res_ = {};
		G4ThreeVector half_res_ = {};
		v3i size_;
		int size_xy_;
		int size_n_;

		std::vector<double> dose_;
		std::vector<double> dose2_;
		std::vector<double> unc_;

		G4String output_filename_;
		std::ofstream output_filestream_;
	};
}
