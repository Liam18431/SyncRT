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
	G4ThreeVector voxel_scorer_dim = this->voxel_scorer_->GetDimensions();

	G4ThreeVector voxel_scorer_pos = this->voxel_scorer_->GetPosition();

	G4Box* voxel_scorer_solid = new G4Box("Voxel Scorer (solid)", voxel_scorer_dim.x() / 2., voxel_scorer_dim.y() / 2., voxel_scorer_dim.z() / 2.);

	this->voxel_scorer_logical_ = new G4LogicalVolume(voxel_scorer_solid, 0, "Voxel Scorer (logical)");

	G4VPhysicalVolume* voxel_scorer_physical = new G4PVPlacement(0, voxel_scorer_pos, this->voxel_scorer_logical_, "Voxel Scorer (physical)", parallel_world_logical, false, 0, check_overlaps);

	G4VisAttributes* voxel_scorer_vis = new G4VisAttributes(G4Colour(1, 0, 0, 0.8));
	this->voxel_scorer_logical_->SetVisAttributes(voxel_scorer_vis);
}

void SRT::ParallelDetectorConstruction::ConstructSD()
{
	G4AutoLock l(&mutex);
	{
		SRT::SensitiveDetector* sensitive_detector = new SRT::SensitiveDetector("SensitiveDetector", this->voxel_scorer_);

		G4SDManager::GetSDMpointer()->AddNewDetector(sensitive_detector);

		SetSensitiveDetector(this->voxel_scorer_logical_, sensitive_detector);
	}

	return;
}
