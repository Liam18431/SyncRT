#pragma once

#include "G4VUserParallelWorld.hh"

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4ThreeVector.hh"

#include "G4SystemOfUnits.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"

#include "G4Box.hh"

#include "G4VisAttributes.hh"

#include "G4SDManager.hh"

#include "SensitiveDetector.hh"

#include "VoxelScorer.hh"

namespace SRT
{
	class ParallelDetectorConstruction : public G4VUserParallelWorld
	{
	public:
		ParallelDetectorConstruction(const G4String& name, VoxelScorer* voxel_scorer) : G4VUserParallelWorld(name), voxel_scorer_(voxel_scorer) {};

		void Construct() override;

		void ConstructSD() override;

		G4String GetName() const { return this->fWorldName; }

	private:
		G4LogicalVolume* voxel_scorer_logical_ = nullptr;
		VoxelScorer* voxel_scorer_ = nullptr;
	};
}
