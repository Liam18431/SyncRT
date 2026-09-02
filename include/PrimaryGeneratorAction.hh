#pragma once

#include <cmath>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"

#include "G4SystemOfUnits.hh"

#include "Randomize.hh"

#include "SpectrumData.hh"

#include "Common.hh"
#include "Utilities.hh"

namespace SRT
{
	class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
	{
	public:
		PrimaryGeneratorAction(const std::map<double, double>* spectrum);
		~PrimaryGeneratorAction();

		void GeneratePrimaries(G4Event*) override;

	private:
		G4ParticleGun* particle_gun = nullptr;
		const std::map<double, double>* spectrum_;
	};
}