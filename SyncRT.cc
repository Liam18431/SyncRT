#include <iostream>
#include <random>

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#include "G4WorkerRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4ParallelWorldPhysics.hh"

#include "G4UImanager.hh"
#include "Randomize.hh"

#include "Inputs.hh"
#include "SpectrumData.hh"

#include "DetectorConstruction.hh"
#include "ParallelDetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialisation.hh"

#include "VoxelScorer.hh"

int main(int argc, char* argv[])
{
	std::cout << "SyncRT" << std::endl;
	
	try
	{
		SRT::Inputs::ParseInputs(argc, argv);

		if (SRT::Inputs::InputsContains("--help") && SRT::Inputs::GetInputValueAsBool("--help"))
		{
			SRT::Inputs::PrintHelp();
			return 0;
		}
	}
	catch (std::exception &e)
	{
		std::cout << "Failed to parse inputs: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "Failed to parse inputs: unhandled exception" << std::endl;
		return 2;
	}

	std::string spectrum_file = "spectrum.dat";
	try
	{
		if (SRT::Inputs::InputsContains("--spectrum"))
		{
			spectrum_file = SRT::Inputs::GetInputValueAsString("--spectrum");
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to set data files: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "Failed to set data files: unhandled exception" << std::endl;
		return 2;
	}

	int job_id =0;
	try
	{
		G4Random::setTheEngine(new CLHEP::RanecuEngine);

		G4long seeds[2]{};
		std::random_device rd;
		if (SRT::Inputs::InputsContains("--job"))
		{
			job_id = SRT::Inputs::GetInputValueAsInt("--job");
		}

		std::cout << "Job id: " << job_id << std::endl;

		G4double rand = (G4double(rd()) / rd.max());
		seeds[0] = (G4long)rd();
		seeds[1] = (G4long)(rd() * rand);
		G4Random::setTheSeed((G4long)job_id);
		G4Random::setTheSeeds(seeds);
		G4Random::showEngineStatus();
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to set random engine: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "Failed to set random engine: unhandled exception" << std::endl;
		return 2;
	}

	SRT::SpectrumData* spectrum_data;
	try
	{
		std::cout << "Reading spectrum file: " << spectrum_file << std::endl;

		if (!std::filesystem::exists(spectrum_file))
		{
			std::stringstream err_msg;
			err_msg << "Spectrum file: " << spectrum_file << ", does not exist." << std::endl;
			throw std::runtime_error(err_msg.str().c_str());
		}

		spectrum_data = new SRT::SpectrumData(spectrum_file);
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to initialise spectrum data: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "Failed to initialise spectrum data: unhandled exception" << std::endl;
		return 2;
	}

	SRT::VoxelScorer* voxel_scorer;
	try
	{
		std::string output_dir = "output/";
		if (SRT::Inputs::InputsContains("--output"))
		{
			output_dir = SRT::Inputs::GetInputValueAsString("--output");
		}
		output_dir += "/" + std::to_string(job_id) + "/";

		if (!(std::filesystem::exists(output_dir)))
		{
			try { std::filesystem::create_directories(output_dir); }
			catch (...) { throw std::runtime_error("Could not create output directory"); }
		}

		std::string filename = "/dose.bin";
		std::string output_filename = output_dir + filename;

		std::cout << "Voxel scorer output file: " << output_filename << std::endl;

		voxel_scorer = new SRT::VoxelScorer(output_filename);
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to initialise voxel scorer: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "Failed to initialise voxel scorer: unhandled exception" << std::endl;
		return 2;
	}

	G4MTRunManager* run_manager;
	try
	{
#ifdef G4MULTITHREADED
		run_manager = new G4MTRunManager();
		run_manager->SetVerboseLevel(2);
		int n_cores = G4Threading::G4GetNumberOfCores();

		if (SRT::Inputs::InputsContains("--ncores"))
		{
			n_cores = SRT::Inputs::GetInputValueAsInt("--ncores");

			if (n_cores > G4Threading::G4GetNumberOfCores())
			{
				std::cout << "Warning: number of cores specified: " << n_cores << ", exceeds the total number of available system cores: " << G4Threading::G4GetNumberOfCores() << "\nSetting the number of cores to the number of available system cores." << std::endl;
				n_cores = G4Threading::G4GetNumberOfCores();
			}
		}

		std::cout << "Number of cores: " << n_cores << std::endl;
		if (n_cores <= 0) throw std::runtime_error("Invalid number of cores.");

		run_manager->SetNumberOfThreads(n_cores);
#else
		G4RunManager* run_manager = new G4RunManager();
		run_manager->SetVerboseLevel(2);
#endif
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to initialise run manager: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "Failed to initialise run manager: unhandled exception" << std::endl;
		return 2;
	}

	try
	{
		SRT::DetectorConstruction* detector_construction = new SRT::DetectorConstruction();
		SRT::ParallelDetectorConstruction* parallel_detector_construction = new SRT::ParallelDetectorConstruction("ParallelWorld", voxel_scorer);

		detector_construction->RegisterParallelWorld(parallel_detector_construction);

		run_manager->SetUserInitialization(detector_construction);

		SRT::PhysicsList* physics_list = new SRT::PhysicsList(detector_construction, parallel_detector_construction);
		run_manager->SetUserInitialization(physics_list);
		
		run_manager->SetUserInitialization(new SRT::ActionInitialisation(spectrum_data, voxel_scorer));
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to initialise Geant4 classes: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "Failed to initialise Geant4 classes: unhandled exception" << std::endl;
		return 2;
	}

	try
	{
		G4int n_histories = 0;
		if (SRT::Inputs::InputsContains("--histories")) /* Batch mode simulation. */
		{
			n_histories = SRT::Inputs::GetInputValueAsInt("--histories");
			std::cout << "Batch mode using: " << n_histories << " histories." << std::endl;
			run_manager->Initialize();
			run_manager->BeamOn(n_histories);
		}
		else  /* UI mode simulation. */
		{
			std::cout << "Visualisation mode." << std::endl;

			std::string ui_mode = "Qt"; /* Try Qt.*/
			if (SRT::Inputs::InputsContains("--ui"))
			{
				ui_mode = SRT::Inputs::GetInputValueAsString("--ui");
			}
			G4UIExecutive* ui = new G4UIExecutive(argc, argv, ui_mode);
			G4VisManager* vis_manager = new G4VisExecutive;
			G4UImanager* ui_manager = G4UImanager::GetUIpointer();

			vis_manager->Initialize();

			if (SRT::Inputs::InputsContains("--macro"))
			{
				std::string macro_file = SRT::Inputs::GetInputValueAsString("--macro");

				std::cout << "Using macro file: " << macro_file << std::endl;
				if (!std::filesystem::exists(macro_file)) throw std::runtime_error("Macro file does not exist.");

				ui_manager->ApplyCommand("/control/execute " + macro_file);
			}
			else
			{
				ui_manager->ApplyCommand("/control/verbose 2");
				ui_manager->ApplyCommand("/run/verbose 2");
				ui_manager->ApplyCommand("/vis/verbose errors");

				ui_manager->ApplyCommand("/run/initialize");

				ui_manager->ApplyCommand("/vis/open OGL"); /* Try OpenGL.*/

				ui_manager->ApplyCommand("/vis/drawVolume worlds");

				ui_manager->ApplyCommand("/vis/viewer/set/upVector 0 0 1");
				ui_manager->ApplyCommand("/vis/viewer/set/targetPoint 0 0 0 mm");
				ui_manager->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 70 220");
				ui_manager->ApplyCommand("/vis/viewer/set/lightsVector 0.5 0.3 0.2");

				ui_manager->ApplyCommand("/vis/viewer/set/style surface");
				ui_manager->ApplyCommand("/vis/viewer/set/auxiliaryEdge true");
				ui_manager->ApplyCommand("/vis/viewer/set/lineSegmentsPerCircle 100");

				ui_manager->ApplyCommand("/vis/scene/add/trajectories");
				ui_manager->ApplyCommand("/vis/modeling/trajectories/create/drawByCharge");
				ui_manager->ApplyCommand("/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true");
				ui_manager->ApplyCommand("/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2");

				ui_manager->ApplyCommand("/vis/scene/endOfEventAction accumulate");

				ui_manager->ApplyCommand("/vis/viewer/set/autoRefresh true");
			}

			ui->SessionStart();

			delete vis_manager;
			delete ui;
		}

		delete run_manager;
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to run simulation: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "Failed to run simulation: unhandled exception" << std::endl;
		return 2;
	}

	try
	{
		std::cout << "Writing dose output to: " << voxel_scorer->GetOutputFilename() << std::endl;
		voxel_scorer->WriteDose();
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to write data: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "Failed to write data: unhandled exception" << std::endl;
		return 2;
	}

	std::cout << "Simulation has finished." << std::endl;
	return 0;
}