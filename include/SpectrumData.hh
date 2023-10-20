#pragma once

#include <string>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <numeric>

#include "globals.hh"

#include "G4SystemOfUnits.hh"

namespace SRT
{
	class SpectrumData
	{
	public:
		SpectrumData(const std::string& spectrum_file);
		~SpectrumData() {};

		const std::map<double, double>* GetSpectrumCDF() const { return &this->spectrum_cdf; }

	private:
		std::map<double, double> CalculatePDF(std::map<double, double> spectrum);
		std::map<double, double> CalculateCDF(std::map<double, double> spectrum_pdf);
		void CheckSpectra(std::map<double, double> spectrum_pdf, std::map<double, double> spectrum_cdf);

		std::map<double, double> spectrum;
		std::map<double, double> spectrum_pdf;
		std::map<double, double> spectrum_cdf;
	};
}