#include "SensitiveDetector.hh"

namespace { G4Mutex mutex = G4MUTEX_INITIALIZER; }

SRT::SensitiveDetector::SensitiveDetector(const G4String& name, VoxelScorer* voxel_scorer) : G4VSensitiveDetector(name), voxel_scorer_(voxel_scorer), voxel_volume_(voxel_scorer->GetVoxelVolume())
{
}

SRT::SensitiveDetector::~SensitiveDetector()
{
}

void SRT::SensitiveDetector::Initialize(G4HCofThisEvent* hit_collection)
{
}

G4bool SRT::SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* histroy)
{
	G4StepPoint* post_step_point = step->GetPostStepPoint();

	if (post_step_point->GetStepStatus() != fGeomBoundary)
	{
		G4double energy_deposit = step->GetTotalEnergyDeposit();
		if (energy_deposit > 0)
		{
			G4double density = post_step_point->GetMaterial()->GetDensity();
			if (density > 0)
			{
				G4double mass = density * this->voxel_volume_;
				G4double dose = (energy_deposit / mass) * 1e12; /* Gy */

				G4ThreeVector global_position = post_step_point->GetPosition();

				G4ThreeVector local_position = post_step_point->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(global_position);

				if (this->voxel_scorer_->LocalCoordInsideVoxelScorer(local_position))
				{
					int bin = this->voxel_scorer_->GetBinFromLocalCoords(local_position);
					this->dose_map_[bin] += dose;
				}
			}
		}
	}

	return true;
}

void SRT::SensitiveDetector::EndOfEvent(G4HCofThisEvent* hit_collection)
{
	G4AutoLock l(&mutex);
	for (auto& element : this->dose_map_)
	{
		this->voxel_scorer_->AddDose(element.first, element.second);
	}
	this->dose_map_.clear();
}
