#include "EventAction.hh"
namespace { G4Mutex mutex = G4MUTEX_INITIALIZER; }

G4long SRT::EventAction::total_events_processed_in_run_ = 0;
G4long SRT::EventAction::total_events_to_process_ = 0;
G4int SRT::EventAction::progress_ = 0;
std::chrono::steady_clock::time_point SRT::EventAction::run_start_tp = std::chrono::steady_clock::time_point();

SRT::EventAction::EventAction() : G4UserEventAction()
{
}

SRT::EventAction::~EventAction()
{
}

void SRT::EventAction::BeginRun(G4int events_to_process)
{
	SRT::EventAction::total_events_processed_in_run_ = 0;
	SRT::EventAction::total_events_to_process_ = events_to_process;
	SRT::EventAction::progress_ = 0;
	SRT::EventAction::run_start_tp = std::chrono::steady_clock::now();
}

void SRT::EventAction::BeginOfEventAction(const G4Event*)
{
	G4AutoLock l(&mutex);
	{
		SRT::EventAction::total_events_processed_in_run_++;
	}
}

void SRT::EventAction::EndOfEventAction(const G4Event* event)
{
	if (SRT::EventAction::total_events_to_process_ > 0)
	{
		G4int progress = G4int(SRT::EventAction::total_events_processed_in_run_ / G4double(SRT::EventAction::total_events_to_process_) * 100);
		if (progress % 10 == 0)
		{
			G4AutoLock l(&mutex);
			{
				if (progress > SRT::EventAction::progress_)
				{
					SRT::EventAction::progress_ = progress;

					long long run_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - SRT::EventAction::run_start_tp).count();

					G4cout << "Progress: " << progress << " %, completed in: " << run_time << " s" << std::endl;
				}
			}
		}
	}
}
