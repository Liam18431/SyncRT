#include "RunAction.hh"

void SRT::RunAction::BeginOfRunAction(const G4Run* run)
{
	if (IsMaster())
	{
		G4cout << "Begin simulation for master thread: " << G4Threading::G4GetThreadId() << G4endl;
		SRT::EventAction::BeginRun(run->GetNumberOfEventToBeProcessed());
	}
	else
	{
		G4cout << "Begin simulation for thread: " << G4Threading::G4GetThreadId() << G4endl;
	}
}

void SRT::RunAction::EndOfRunAction(const G4Run* run)
{
	if (IsMaster())
	{
		G4cout << "End simulation for master thread: " << G4Threading::G4GetThreadId() << G4endl;
		G4RunManager* run_manager = G4RunManager::GetRunManager();
		int n_events_in_run = run_manager->GetCurrentRun()->GetNumberOfEvent();
		this->total_n_events_ += static_cast<unsigned long long int>(n_events_in_run);
		double avg_unc = this->voxel_scorer_->CalculateUncertainties(this->total_n_events_);
		G4cout << "Average uncertainty: " << avg_unc * 100 << " %" << G4endl;
	}
	else
	{
		G4cout << "End simulation for thread: " << G4Threading::G4GetThreadId() << G4endl;
	}
}
