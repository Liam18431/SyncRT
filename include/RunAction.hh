#pragma once

#include <iostream>

#include "G4UserRunAction.hh"
#include "G4Threading.hh"

#include "G4Run.hh"

#include "G4RunManager.hh"

#include "EventAction.hh"
#include "VoxelScorer.hh"

namespace SRT
{
	class RunAction : public G4UserRunAction
	{
	public:
		RunAction() {}
		void BeginOfRunAction(const G4Run*) override;
		void EndOfRunAction(const G4Run*) override;

		unsigned long long int GetTotalNumberOfEvents() { return this->total_n_events_; }

	private:
		unsigned long long int total_n_events_ = 0;
	};
}