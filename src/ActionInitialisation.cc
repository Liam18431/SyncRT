#include "ActionInitialisation.hh"

SRT::ActionInitialisation::ActionInitialisation(const SRT::SpectrumData* spectrum_data, SRT::VoxelScorer* voxel_scorer) : 
	spectrum_data_(spectrum_data), voxel_scorer_(voxel_scorer)
{
}

void SRT::ActionInitialisation::BuildForMaster() const
{
	RunAction* run_action_master = new RunAction(this->voxel_scorer_);
	SetUserAction(run_action_master);
}

void SRT::ActionInitialisation::Build() const
{
	RunAction* run_action_worker = new RunAction(this->voxel_scorer_);
	SetUserAction(run_action_worker);

	EventAction* event_action = new EventAction();
	SetUserAction(event_action);

	PrimaryGeneratorAction* primary_generator_action = new PrimaryGeneratorAction(spectrum_data_->GetSpectrumCDF());
	SetUserAction(primary_generator_action);
}
