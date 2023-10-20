#include "PhysicsList.hh"

SRT::PhysicsList::PhysicsList(const DetectorConstruction* detector_construction, const ParallelDetectorConstruction* parallel_detector_construction) : 
	G4VModularPhysicsList(), 
	detector_construction_(detector_construction),
	parallel_detector_construction_(parallel_detector_construction)
{
	this->em_physics_list = new G4EmLivermorePolarizedPhysics();

	RegisterPhysics(this->em_physics_list);

	this->em_decay_list = new G4DecayPhysics();

	RegisterPhysics(this->em_decay_list);

	/* Optional physics processes*/
	G4EmParameters* parameters = G4EmParameters::Instance();
	parameters->SetFluo(true);
	parameters->SetAuger(false);
	parameters->SetPixe(false);
	parameters->SetDeexcitationIgnoreCut(false);
}

void SRT::PhysicsList::ConstructProcess()
{
	AddTransportation();

	AddParallelWorldProcess();

	this->em_physics_list->ConstructProcess();
	this->em_decay_list->ConstructProcess();
}

void SRT::PhysicsList::SetCuts()
{
	SetCutsWithDefault();

	SetCutValue(1 *mm, "gamma");
	SetCutValue(1 *mm, "e-");
	SetCutValue(1 *mm, "e+");
	SetCutValue(1 *mm, "proton");

	SetRegionCuts();

	DumpCutValuesTable();
}

void SRT::PhysicsList::SetRegionCuts()
{
	G4Region* fine_tracking_region_ = this->detector_construction_->GetFineTrackingRegion();

	G4ProductionCuts* fine_tracking_cuts = new G4ProductionCuts;

	fine_tracking_cuts->SetProductionCut(1 *um, "gamma");
	fine_tracking_cuts->SetProductionCut(1 *um, "e-");
	fine_tracking_cuts->SetProductionCut(1 *um, "e+");
	fine_tracking_cuts->SetProductionCut(1 *um, "proton");

	fine_tracking_region_->SetProductionCuts(fine_tracking_cuts);

	return;
}

void SRT::PhysicsList::AddParallelWorldProcess()
{
	G4ParallelWorldProcess* parallel_world_process = new G4ParallelWorldProcess(this->parallel_detector_construction_->GetName());
	parallel_world_process->SetParallelWorld(this->parallel_detector_construction_->GetName());

	parallel_world_process->SetLayeredMaterialFlag(false);

	auto particleIterator = GetParticleIterator();

	particleIterator->reset();
	while ((*particleIterator)())
	{
		G4ParticleDefinition* particle = particleIterator->value();
		G4ProcessManager* process_manager = particle->GetProcessManager();
		process_manager->AddProcess(parallel_world_process);

		if (parallel_world_process->IsAtRestRequired(particle))
		{
			process_manager->SetProcessOrderingToLast(parallel_world_process, idxAtRest);
		}

		process_manager->SetProcessOrdering(parallel_world_process, idxAlongStep, 1);
		process_manager->SetProcessOrderingToLast(parallel_world_process, idxPostStep);
	}
}
