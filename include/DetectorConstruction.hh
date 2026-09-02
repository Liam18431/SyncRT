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
	private:
		static constexpr vec3d world_dim_ =
		{
			30 * cm,
			30 * cm,
			30 * cm
		};
		
		static constexpr vec3d half_world_dim_ =
		{
			0.5 * world_dim_.x,
			0.5 * world_dim_.y,
			0.5 * world_dim_.z
		};

		static constexpr vec3d world_pos_ =
		{
			0 * cm,
			0 * cm,
			0 * cm
		};

		static constexpr vec3d  cube_dim_ =
		{
			20 * cm,
			20 * cm,
			20 * cm
		};

		static constexpr vec3d half_cube_dim_ =
		{
			0.5 * cube_dim_.x,
			0.5 * cube_dim_.y,
			0.5 * cube_dim_.z
		};

		static constexpr vec3d  cube_pos_ =
		{
			0 * cm,
			0 * cm,
			0 * cm
		};
	};
}
