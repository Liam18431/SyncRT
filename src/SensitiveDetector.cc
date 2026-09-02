#include "SensitiveDetector.hh"

namespace { G4Mutex mutex = G4MUTEX_INITIALIZER; }

SRT::SensitiveDetector::SensitiveDetector(const G4String& name) : G4VSensitiveDetector(name)
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
				G4double mass = density * VoxelScorer::voxel_volume_;
				G4double dose = (energy_deposit / mass) / gray;

				G4ThreeVector global_position = post_step_point->GetPosition();

				G4ThreeVector local_position = post_step_point->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(global_position);

				if (VoxelScorer::LocalCoordInsideVoxelScorer(local_position))
				{
					int bin = VoxelScorer::GetBinFromLocalCoords(local_position);
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
		VoxelScorer::AddDose(element.first, element.second);
	}
	this->dose_map_.clear();
}
