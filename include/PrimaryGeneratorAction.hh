#pragma once

#include <cmath>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"

#include "G4SystemOfUnits.hh"

#include "Randomize.hh"

#include "SpectrumData.hh"

namespace SRT
{
	constexpr double pi_ = 3.14159265358979323846264338328;
	constexpr double tau_ = 6.283185307179586476925286766559;

	class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
	{
	public:
		PrimaryGeneratorAction(const std::map<double, double>* spectrum_cdf);
		~PrimaryGeneratorAction();

		void GeneratePrimaries(G4Event*) override;

	private:
		G4ParticleGun* particle_gun = nullptr;

		const std::map<double, double>* spectrum_cdf_;
	};
}