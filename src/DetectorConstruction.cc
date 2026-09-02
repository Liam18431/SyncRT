#include "DetectorConstruction.hh"

G4VPhysicalVolume* SRT::DetectorConstruction::Construct()
{
	bool check_overlaps = true;

	auto nist = G4NistManager::Instance();

	/* World construction*/

	G4Material* world_mat = nist->FindOrBuildMaterial(G4String("G4_AIR"));

	G4Box* world_solid = new G4Box("World (solid)", half_world_dim_.x, half_world_dim_.y, half_world_dim_.z);

	G4LogicalVolume* world_logical = new G4LogicalVolume(world_solid, world_mat, "World (logical)");

	G4VPhysicalVolume* world_physical = new G4PVPlacement(0, G4ThreeVector(world_pos_.x, world_pos_.y, world_pos_.z), world_logical, "World (physical)", 0, false, 0, check_overlaps);

	G4VisAttributes* world_vis = new G4VisAttributes(G4Colour(1, 1, 1, 0.0));
	world_logical->SetVisAttributes(world_vis);

	/* Water cube construction*/

	G4Material* cube_mat = nist->FindOrBuildMaterial("G4_WATER");

	G4Box* cube_solid = new G4Box("Cube (solid)", half_cube_dim_.x, half_cube_dim_.y, half_cube_dim_.z);

	G4LogicalVolume* cube_logical = new G4LogicalVolume(cube_solid, cube_mat, "Cube (logical)");

	G4VPhysicalVolume* cube_physical = new G4PVPlacement(0, G4ThreeVector(cube_pos_.x, cube_pos_.y, cube_pos_.z), cube_logical, "Cube (physical)", world_logical, false, 0, check_overlaps);

	G4VisAttributes* cube_vis = new G4VisAttributes(G4Colour(0, 0, 1, 0.5));
	cube_logical->SetVisAttributes(cube_vis);

	/* Fine-tracking region */
	G4Region* fine_tracking_region = new G4Region("FineTrackingRegion");
	cube_logical->SetRegion(fine_tracking_region);
	fine_tracking_region->AddRootLogicalVolume(cube_logical);

	G4ProductionCuts* fine_tracking_cuts = new G4ProductionCuts;

	fine_tracking_cuts->SetProductionCut(1 * um, "gamma");
	fine_tracking_cuts->SetProductionCut(1 * um, "e-");
	fine_tracking_cuts->SetProductionCut(1 * um, "e+");
	fine_tracking_cuts->SetProductionCut(1 * um, "proton");

	fine_tracking_region->SetProductionCuts(fine_tracking_cuts);
	
	return world_physical;
}
