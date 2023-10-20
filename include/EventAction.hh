#pragma once

#include <chrono>

#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4UserEventAction.hh"
#include "G4Event.hh"

namespace SRT
{
	class EventAction : public G4UserEventAction
	{
	public:
		EventAction();
		virtual ~EventAction();

		virtual void BeginOfEventAction(const G4Event*);
		virtual void EndOfEventAction(const G4Event*);

		static void BeginRun(G4int histories_to_process);

	private:
		static G4long total_events_processed_in_run_;
		static G4long total_events_to_process_;
		static G4int progress_;
		static std::chrono::steady_clock::time_point run_start_tp;
	};
}