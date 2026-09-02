#pragma once

#include <string>
#include <filesystem>
#include <sstream>
#include <fstream>

#include "globals.hh"

#include "G4SystemOfUnits.hh"

#include "Common.hh"
#include "Utilities.hh"

namespace SRT
{
	class SpectrumData
	{
	public:
		SpectrumData() = delete;
		~SpectrumData() = delete;
		SpectrumData(const SpectrumData&) = delete;
		SpectrumData& operator = (const SpectrumData&) = delete;

		static const std::map<double, double> GetSpectrum();

	private:
		template <unsigned int M, unsigned int N>
		static constexpr double TotalFlux(const double(&spectrum)[M][N])
		{
			double result{};
			for (unsigned int i = 0; i < M; i++)
			{
				result += spectrum[i][1];
			}

			return result;
		};

		static constexpr double spectrum_[2][2] = /* 100 keV mono energy. */
		{
			/* Energy (MeV), Probability/flux */
			{ 0, 0   },
			{ 0.1, 1 }
		};
		static constexpr unsigned int size_ = sizeof(spectrum_) / sizeof(spectrum_[0]);
		static constexpr double flux_ = TotalFlux(spectrum_);
	};
}