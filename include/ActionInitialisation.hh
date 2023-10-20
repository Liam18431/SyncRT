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
		ActionInitialisation(const SRT::SpectrumData* spectrum_data, SRT::VoxelScorer* voxel_scorer);

		void BuildForMaster() const override;
		void Build() const override;

	private:
		const SRT::SpectrumData* spectrum_data_;
		SRT::VoxelScorer* voxel_scorer_;
	};
}