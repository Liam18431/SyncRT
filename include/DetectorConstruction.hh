#pragma once

#include "G4VUserDetectorConstruction.hh"

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4ThreeVector.hh"

#include "G4SystemOfUnits.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Orb.hh"

#include "G4VisAttributes.hh"

#include "G4SDManager.hh"

#include "G4ProductionCuts.hh"

#include "SensitiveDetector.hh"

#include "VoxelScorer.hh"

namespace SRT
{
	class DetectorConstruction : public G4VUserDetectorConstruction
	{
	public:
		G4VPhysicalVolume* Construct() override;
	};
}
