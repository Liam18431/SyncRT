#include "DetectorConstruction.hh"

G4VPhysicalVolume* SRT::DetectorConstruction::Construct()
{
	bool check_overlaps = true;

	auto nist = G4NistManager::Instance();

	/* World construction*/
	G4ThreeVector world_dim =
	{
		30 *cm,
		30 *cm,
		30 *cm
	};

	G4ThreeVector world_pos =
	{
		0 * cm,
		0 * cm,
		0 * cm
	};

	G4Material* world_mat = nist->FindOrBuildMaterial(G4String("G4_AIR"));

	G4Box* world_solid = new G4Box("World (solid)", world_dim.x() / 2., world_dim.y() / 2., world_dim.z() / 2.);

	G4LogicalVolume* world_logical = new G4LogicalVolume(world_solid, world_mat, "World (logical)");

	G4VPhysicalVolume* world_physical = new G4PVPlacement(0, world_pos, world_logical, "World (physical)", 0, false, 0, check_overlaps);

	G4VisAttributes* world_vis = new G4VisAttributes(G4Colour(1, 1, 1, 0.0));
	world_logical->SetVisAttributes(world_vis);

	/* Water cube construction*/
	G4ThreeVector cube_dim =
	{
		20 *cm,
		20 *cm,
		20 *cm
	};

	G4ThreeVector cube_pos =
	{
		0 * cm,
		0 * cm,
		0 * cm
	};

	G4Material* cube_mat = nist->FindOrBuildMaterial("G4_WATER");

	G4Box* cube_solid = new G4Box("Cube (solid)", cube_dim.x() / 2., cube_dim.y() / 2., cube_dim.z() / 2.);

	G4LogicalVolume* cube_logical = new G4LogicalVolume(cube_solid, cube_mat, "Cube (logical)");

	G4VPhysicalVolume* cube_physical = new G4PVPlacement(0, cube_pos, cube_logical, "Cube (physical)", world_logical, false, 0, check_overlaps);

	G4VisAttributes* cube_vis = new G4VisAttributes(G4Colour(0, 0, 1, 0.5));
	cube_logical->SetVisAttributes(cube_vis);

	/* Fine-tracking region */
	this->fine_tracking_region_ = new G4Region("FineTrackingRegion");

	cube_logical->SetRegion(this->fine_tracking_region_);
	this->fine_tracking_region_->AddRootLogicalVolume(cube_logical);
	
	return world_physical;
}
