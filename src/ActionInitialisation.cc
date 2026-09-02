#include "ActionInitialisation.hh"

SRT::ActionInitialisation::ActionInitialisation() : 
	spectrum_(SpectrumData::GetSpectrum())
{
}

void SRT::ActionInitialisation::BuildForMaster() const
{
	RunAction* run_action_master = new RunAction();
	SetUserAction(run_action_master);
}

void SRT::ActionInitialisation::Build() const
{
	RunAction* run_action_worker = new RunAction();
	SetUserAction(run_action_worker);

	EventAction* event_action = new EventAction();
	SetUserAction(event_action);

	PrimaryGeneratorAction* primary_generator_action = new PrimaryGeneratorAction(&this->spectrum_);
	SetUserAction(primary_generator_action);
}
