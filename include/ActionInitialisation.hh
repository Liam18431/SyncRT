#pragma once

#include "G4VUserActionInitialization.hh"
#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"

#include "SpectrumData.hh"

#include "VoxelScorer.hh"

namespace SRT
{
	class ActionInitialisation : public G4VUserActionInitialization
	{
	public:
		ActionInitialisation();

		void BuildForMaster() const override;
		void Build() const override;

	private:
		const std::map<double, double> spectrum_;
	};
}