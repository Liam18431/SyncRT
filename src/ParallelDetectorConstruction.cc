#include "ParallelDetectorConstruction.hh"

namespace { G4Mutex mutex = G4MUTEX_INITIALIZER; }

void SRT::ParallelDetectorConstruction::Construct()
{
	bool check_overlaps = true;

	G4VPhysicalVolume* parallel_world_physical = GetWorld();
	G4LogicalVolume* parallel_world_logical = parallel_world_physical->GetLogicalVolume();

	G4VisAttributes* world_vis = new G4VisAttributes(G4Colour(0, 1, 0, 0.0));
	parallel_world_logical->SetVisAttributes(world_vis);

	/* Voxel scorer construction*/
	G4Box* voxel_scorer_solid = new G4Box("Voxel Scorer (solid)", VoxelScorer::half_dim_.x, VoxelScorer::half_dim_.y, VoxelScorer::half_dim_.z);

	this->voxel_scorer_logical_ = new G4LogicalVolume(voxel_scorer_solid, 0, "Voxel Scorer (logical)");

	G4VPhysicalVolume* voxel_scorer_physical = new G4PVPlacement(0, G4ThreeVector(VoxelScorer::pos_.x, VoxelScorer::pos_.y, VoxelScorer::pos_.z), this->voxel_scorer_logical_, "Voxel Scorer (physical)", parallel_world_logical, false, 0, check_overlaps);

	G4VisAttributes* voxel_scorer_vis = new G4VisAttributes(G4Colour(1, 0, 0, 0.8));
	this->voxel_scorer_logical_->SetVisAttributes(voxel_scorer_vis);
}

void SRT::ParallelDetectorConstruction::ConstructSD()
{
	G4AutoLock l(&mutex);
	{
		SRT::SensitiveDetector* sensitive_detector = new SRT::SensitiveDetector("SensitiveDetector");

		G4SDManager::GetSDMpointer()->AddNewDetector(sensitive_detector);

		SetSensitiveDetector(this->voxel_scorer_logical_, sensitive_detector);
	}

	return;
}
