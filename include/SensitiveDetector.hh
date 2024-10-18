#pragma once

#include "G4VSensitiveDetector.hh"

#include "G4SystemOfUnits.hh"

#include "G4Threading.hh"

#include "VoxelScorer.hh"

namespace SRT
{
	class SensitiveDetector : public G4VSensitiveDetector
	{
	public:
		SensitiveDetector(const G4String& name, VoxelScorer* voxel_scorer);
		~SensitiveDetector();

		void Initialize(G4HCofThisEvent* hit_collection) override;
		G4bool ProcessHits(G4Step* step, G4TouchableHistory* histroy) override;
		void EndOfEvent(G4HCofThisEvent* hit_collection) override;

	private:
		VoxelScorer* voxel_scorer_ = nullptr;
		std::map<int, double> dose_map_;
		G4double voxel_volume_;
	};
}