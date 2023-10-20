#pragma once


#include "G4VModularPhysicsList.hh"
#include "G4EmLivermorePolarizedPhysics.hh"
#include "G4DecayPhysics.hh"

#include "G4SystemOfUnits.hh"

#include "G4ParallelWorldProcess.hh"

#include "G4ProcessManager.hh"
#include "G4EmParameters.hh"

#include "DetectorConstruction.hh"
#include "ParallelDetectorConstruction.hh"

namespace SRT
{
	class PhysicsList : public G4VModularPhysicsList
	{
	public:
		PhysicsList(const DetectorConstruction* detector_construction, const ParallelDetectorConstruction* parallel_detector_construction);

		void ConstructProcess() override;

		void SetCuts() override;

	private:
		void SetRegionCuts();
		void AddParallelWorldProcess();

		G4EmLivermorePolarizedPhysics* em_physics_list = nullptr;
		G4DecayPhysics* em_decay_list = nullptr;

		const DetectorConstruction* detector_construction_;
		const ParallelDetectorConstruction* parallel_detector_construction_;
	};
}
