#include "PrimaryGeneratorAction.hh"

SRT::PrimaryGeneratorAction::PrimaryGeneratorAction(const std::map<double, double>* spectrum_cdf) : 
	spectrum_cdf_(spectrum_cdf)
{
	this->particle_gun = new G4ParticleGun();

	G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particle_table->FindParticle("gamma");

	this->particle_gun->SetParticleDefinition(particle);
}

SRT::PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete this->particle_gun;
}

static inline G4ThreeVector SampleSource(double sigma_width, double sigma_height)
{
	double x = 0;
	double y = G4RandGauss::shoot(0, sigma_width);
	double z = G4RandGauss::shoot(0, sigma_height);

	return G4ThreeVector(x, y, z);
}

static inline G4ThreeVector SampleRectangularBBField(double field_width, double field_height)
{
	double x = 0;
	double y = G4UniformRand() * field_width - 0.5 * field_width;
	double z = G4UniformRand() * field_height - 0.5 * field_height;

	return G4ThreeVector(x, y, z);
}

static inline G4ThreeVector SampleEllipticalBBField(double field_width, double field_height)
{
	double x = 0;

	double r = std::sqrt(G4UniformRand());
	double phi = G4UniformRand() * SRT::tau_;

	double y = r * std::cos(phi) * 0.5 * field_width;
	double z = r * std::sin(phi) * 0.5 * field_height;

	return G4ThreeVector(x, y, z);
}

static inline G4ThreeVector SampleRectangularMRTField(double field_width, double field_height, double microbeam_width, double ctc)
{
	double x = 0;

	int n_microbeams = static_cast<int>(field_width / ctc);

	if (n_microbeams % 2 == 0) n_microbeams += 1;

	double new_field_width = (n_microbeams - 1) * ctc + microbeam_width;

	int microbeam = static_cast<int>(G4UniformRand() * n_microbeams);

	double microbeam_centre_coord = microbeam * ctc + 0.5 * microbeam_width - 0.5 * new_field_width;

	double y = (microbeam_centre_coord - 0.5 * microbeam_width) + G4UniformRand() * microbeam_width;

	double z = G4UniformRand() * field_height - 0.5 * field_height;

	return G4ThreeVector(x, y, z);
}

static inline double Square(double val)
{
	double result = val * val;

	return result;
}

static inline bool CoordInsideEllipse(double x, double y, double width, double height)
{
	bool result = Square(x) / Square(0.5 * width) + Square(y) / Square(0.5 * height) <= 1.0;

	return result;
}

/* NOTE: This uses acceptance/rejection and is inefficient. There is probably a better way to do this.*/
static inline G4ThreeVector SampleEllipticalMRTField(double field_width, double field_height, double microbeam_width, double ctc)
{
	double x = 0, y, z;

	int n_microbeams = static_cast<int>(field_width / ctc);

	if (n_microbeams % 2 == 0) n_microbeams += 1;

	double new_field_width = (n_microbeams - 1) * ctc + microbeam_width;

	do
	{
		int microbeam = static_cast<int>(G4UniformRand() * n_microbeams);

		double microbeam_centre_coord = microbeam * ctc + 0.5 * microbeam_width - 0.5 * new_field_width;

		y = (microbeam_centre_coord - 0.5 * microbeam_width) + G4UniformRand() * microbeam_width;

		z = G4UniformRand() * field_height - 0.5 * field_height;
	} while (!CoordInsideEllipse(y, z, new_field_width, field_height));

	return G4ThreeVector(x, y, z);
}

/* NOTE: Beam coord sys:
	+x = direction
	+y = horizontal
	+z = vertical
*/
void SRT::PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	double sad = 318.7 * mm;
	double scd = 200.0 * mm;
	double field_width = 50.0 *mm;
	double field_height = 50.0 *mm;
	double sigma_width = 1.0 * mm;
	double sigma_height = 1.0 * mm;
	double microbeam_width = 0.5 * mm;
	double ctc = 1.0 * mm;
	double radius = 5.0f *mm;

	/* Sample the source*/
	G4ThreeVector source = SampleSource(sigma_width, sigma_height);
	source.setX(-sad);

	/* Set particle position*/
	G4ThreeVector position = SampleRectangularMRTField(field_width, field_height, microbeam_width, ctc);
	position.setX(-(sad - scd));
	this->particle_gun->SetParticlePosition(position);

	/* Set particle direction*/
	G4ThreeVector direction = position - source;
	this->particle_gun->SetParticleMomentumDirection(direction);

	/* Set particle energy*/
	double energy = this->spectrum_cdf_->lower_bound(G4UniformRand())->second *keV;
	this->particle_gun->SetParticleEnergy(energy);

	this->particle_gun->GeneratePrimaryVertex(event);
}
