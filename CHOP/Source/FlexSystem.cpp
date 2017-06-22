#include "FlexSystem.h"


void ErrorCallback(NvFlexErrorSeverity, const char* msg, const char* file, int line)
{
	printf("Flex: %s - %s:%d\n", msg, file, line);
	//g_Error = true;
	//assert(0); asserts are bad for TeamCity
}


SimBuffers::SimBuffers(NvFlexLibrary* l) :
	positions(l), restPositions(l), velocities(l), phases(l), densities(l),
	anisotropy1(l), anisotropy2(l), anisotropy3(l), normals(l), smoothPositions(l),
	diffusePositions(l), diffuseVelocities(l), diffuseIndices(l), activeIndices(l),
	shapeGeometry(l), shapePositions(l), shapeRotations(l), shapePrevPositions(l),
	shapePrevRotations(l), shapeFlags(l), rigidOffsets(l), rigidIndices(l), rigidMeshSize(l),
	rigidCoefficients(l), rigidRotations(l), rigidTranslations(l),
	rigidLocalPositions(l), rigidLocalNormals(l), inflatableTriOffsets(l),
	inflatableTriCounts(l), inflatableVolumes(l), inflatableCoefficients(l),
	inflatablePressures(l), springIndices(l), springLengths(l),
	springStiffness(l), triangles(l), triangleNormals(l), uvs(l){


}

SimBuffers::~SimBuffers() {

	positions.destroy();
	restPositions.destroy();
	velocities.destroy();
	phases.destroy();
	densities.destroy();
	anisotropy1.destroy();
	anisotropy2.destroy();
	anisotropy3.destroy();
	normals.destroy();
	diffusePositions.destroy();
	diffuseVelocities.destroy();
	diffuseIndices.destroy();
	smoothPositions.destroy();
	activeIndices.destroy();

	// convexes
	shapeGeometry.destroy();
	shapePositions.destroy();
	shapeRotations.destroy();
	shapePrevPositions.destroy();
	shapePrevRotations.destroy();
	shapeFlags.destroy();

	// rigids
	rigidOffsets.destroy();
	rigidIndices.destroy();
	rigidMeshSize.destroy();
	rigidCoefficients.destroy();
	rigidRotations.destroy();
	rigidTranslations.destroy();
	rigidLocalPositions.destroy();
	rigidLocalNormals.destroy();

	// springs
	springIndices.destroy();
	springLengths.destroy();
	springStiffness.destroy();

	// inflatables
	inflatableTriOffsets.destroy();
	inflatableTriCounts.destroy();
	inflatableVolumes.destroy();
	inflatableCoefficients.destroy();
	inflatablePressures.destroy();

	// triangles
	triangles.destroy();
	triangleNormals.destroy();
	uvs.destroy();


}

void SimBuffers::MapBuffers() {

	positions.map();
	restPositions.map();
	velocities.map();
	phases.map();
	densities.map();
	anisotropy1.map();
	anisotropy2.map();
	anisotropy3.map();
	normals.map();
	diffusePositions.map();
	diffuseVelocities.map();
	diffuseIndices.map();
	smoothPositions.map();
	activeIndices.map();

	// convexes
	shapeGeometry.map();
	shapePositions.map();
	shapeRotations.map();
	shapePrevPositions.map();
	shapePrevRotations.map();
	shapeFlags.map();

	rigidOffsets.map();
	rigidIndices.map();
	rigidMeshSize.map();
	rigidCoefficients.map();
	rigidRotations.map();
	rigidTranslations.map();
	rigidLocalPositions.map();
	rigidLocalNormals.map();

	springIndices.map();
	springLengths.map();
	springStiffness.map();

	// inflatables
	inflatableTriOffsets.map();
	inflatableTriCounts.map();
	inflatableVolumes.map();
	inflatableCoefficients.map();
	inflatablePressures.map();

	triangles.map();
	triangleNormals.map();
	uvs.map();


}

void SimBuffers::UnmapBuffers() {

	// particles
	positions.unmap();
	restPositions.unmap();
	velocities.unmap();
	phases.unmap();
	densities.unmap();
	anisotropy1.unmap();
	anisotropy2.unmap();
	anisotropy3.unmap();
	normals.unmap();
	diffusePositions.unmap();
	diffuseVelocities.unmap();
	diffuseIndices.unmap();
	smoothPositions.unmap();
	activeIndices.unmap();

	// convexes
	shapeGeometry.unmap();
	shapePositions.unmap();
	shapeRotations.unmap();
	shapePrevPositions.unmap();
	shapePrevRotations.unmap();
	shapeFlags.unmap();

	// rigids
	rigidOffsets.unmap();
	rigidIndices.unmap();
	rigidMeshSize.unmap();
	rigidCoefficients.unmap();
	rigidRotations.unmap();
	rigidTranslations.unmap();
	rigidLocalPositions.unmap();
	rigidLocalNormals.unmap();

	// springs
	springIndices.unmap();
	springLengths.unmap();
	springStiffness.unmap();

	// inflatables
	inflatableTriOffsets.unmap();
	inflatableTriCounts.unmap();
	inflatableVolumes.unmap();
	inflatableCoefficients.unmap();
	inflatablePressures.unmap();

	// triangles
	triangles.unmap();
	triangleNormals.unmap();
	uvs.unmap();

}

void SimBuffers::InitBuffers() {

	positions.resize(0);
	velocities.resize(0);
	phases.resize(0);

	/*rigidOffsets.resize(0);
	rigidIndices.resize(0);
	rigidMeshSize.resize(0);
	rigidRotations.resize(0);
	rigidTranslations.resize(0);
	rigidCoefficients.resize(0);
	rigidLocalPositions.resize(0);
	rigidLocalNormals.resize(0);

	springIndices.resize(0);
	springLengths.resize(0);
	springStiffness.resize(0);
	triangles.resize(0);
	triangleNormals.resize(0);
	uvs.resize(0);*/

	shapeGeometry.resize(0);
	shapePositions.resize(0);
	shapeRotations.resize(0);
	shapePrevPositions.resize(0);
	shapePrevRotations.resize(0);
	shapeFlags.resize(0);

}

FlexSystem::FlexSystem()
{	

	g_profile = false;
	
	nEmitter = 0;
	nVolumeBoxes = 0;

	maxParticles = 9;
	g_maxDiffuseParticles=0;
	numDiffuse = 0;

	g_flex = NULL;


	time1 = 0;
	time2 = 0;
	time3 = 0;
	time4 = 0;

	memset(&g_timers, 0, sizeof(g_timers));

	cursor = 0;

}

FlexSystem::~FlexSystem(){


	if (g_flex)
	{
		if(g_buffers)
			delete g_buffers;

		NvFlexDestroySolver(g_flex);
		NvFlexShutdown(g_flexLib);
	}

	

	NvFlexDeviceDestroyCudaContext();

}

void FlexSystem::getSimTimers() {

	if (g_profile) {
		memset(&g_timers, 0, sizeof(g_timers));
		NvFlexGetTimers(g_flex, &g_timers);
		simLatency = g_timers.total;
	}
	else
		simLatency = NvFlexGetDeviceLatency(g_flex);

}

void FlexSystem::initSystem() {

	int g_device = -1;
	g_device = NvFlexDeviceGetSuggestedOrdinal();

	// Create an optimized CUDA context for Flex and set it on the 
	// calling thread. This is an optional call, it is fine to use 
	// a regular CUDA context, although creating one through this API
	// is recommended for best performance.
	bool success = NvFlexDeviceCreateCudaContext(g_device);

	if (!success)
	{
		printf("Error creating CUDA context.\n");
	}

	NvFlexInitDesc desc;
	desc.deviceIndex = g_device;
	desc.enableExtensions = true;
	desc.renderDevice = 0;
	desc.renderContext = 0;
	desc.computeType = eNvFlexCUDA;

	g_flexLib = NvFlexInit(NV_FLEX_VERSION, ErrorCallback, &desc);

}


void FlexSystem::initParams() {

	// sim params
	g_params.gravity[0] = 0.0f;
	g_params.gravity[1] = -9.8f;
	g_params.gravity[2] = 0.0f;

	g_params.wind[0] = 0.0f;
	g_params.wind[1] = 0.0f;
	g_params.wind[2] = 0.0f;

	g_params.radius = 0.15f;
	g_params.viscosity = 0.0f;
	g_params.dynamicFriction = 0.0f;
	g_params.staticFriction = 0.0f;
	g_params.particleFriction = 0.0f; // scale friction between particles by default
	g_params.freeSurfaceDrag = 0.0f;
	g_params.drag = 0.0f;
	g_params.lift = 0.0f;
	g_params.numIterations = 3;
	g_params.fluidRestDistance = 0.0f;
	g_params.solidRestDistance = 0.0f;

	g_params.anisotropyScale = 1.0f;
	g_params.anisotropyMin = 0.1f;
	g_params.anisotropyMax = 2.0f;
	g_params.smoothing = 1.0f;

	g_params.dissipation = 0.0f;
	g_params.damping = 0.0f;
	g_params.particleCollisionMargin = 0.0f;
	g_params.shapeCollisionMargin = 0.0f;
	g_params.collisionDistance = 0.0f;
	g_params.plasticThreshold = 0.0f;
	g_params.plasticCreep = 0.0f;
	g_params.fluid = true;
	g_params.sleepThreshold = 0.0f;
	g_params.shockPropagation = 0.0f;
	g_params.restitution = 0.001f;

	g_params.maxSpeed = FLT_MAX;
	g_params.maxAcceleration = 100.0f;	// approximately 10x gravity

	g_params.smoothing = 1.0f;

	g_params.relaxationMode = eNvFlexRelaxationLocal;
	g_params.relaxationFactor = 1.0f;
	g_params.solidPressure = 1.0f;
	g_params.adhesion = 0.0f;
	g_params.cohesion = 0.1f;
	g_params.surfaceTension = 0.0f;
	g_params.vorticityConfinement = 80.0f;
	g_params.buoyancy = 1.0f;
	g_params.diffuseThreshold = 100.0f;
	g_params.diffuseBuoyancy = 1.0f;
	g_params.diffuseDrag = 0.8f;
	g_params.diffuseBallistic = 16;
	g_params.diffuseSortAxis[0] = 0.0f;
	g_params.diffuseSortAxis[1] = 0.0f;
	g_params.diffuseSortAxis[2] = 0.0f;
	g_params.diffuseLifetime = 2.0f;

	g_params.numPlanes = 0;

}

void FlexSystem::initScene(){


	RandInit();

	cursor = 0;


	if (g_flex)
	{
		
		if (g_buffers)
			delete g_buffers;

		NvFlexDestroySolver(g_flex);
		g_flex = NULL;

	}

	// alloc buffers
	g_buffers = new SimBuffers(g_flexLib);

	// map during initialization
	g_buffers->MapBuffers();
	g_buffers->InitBuffers();

	g_rectEmitters.resize(0);
	g_volumeBoxes.resize(0);

	initParams();

	g_numSubsteps = 2;

	g_sceneLower = FLT_MAX;
	g_sceneUpper = -FLT_MAX;

	ClearShapes();


	maxParticles = 64;

	g_maxDiffuseParticles = 0;
	g_maxNeighborsPerParticle = 96;

}

void FlexSystem::postInitScene(){


	if (!g_params.fluid) {
		g_params.particleCollisionMargin = g_params.radius*0.5f;
		g_params.shapeCollisionMargin = g_params.radius*0.5f;
	}

	

	if(g_params.fluid)
		g_params.fluidRestDistance = g_params.radius*0.65f;

	// by default solid particles use the maximum radius
	if (g_params.fluid && g_params.solidRestDistance == 0.0f)
		g_params.solidRestDistance = g_params.fluidRestDistance;
	else
		g_params.solidRestDistance = g_params.radius;

	// collision distance with shapes half the radius
	if (g_params.collisionDistance == 0.0f)
	{
		g_params.collisionDistance = g_params.radius*0.5f;

		if (g_params.fluid)
			g_params.collisionDistance = g_params.fluidRestDistance*0.5f;
	}

	// default particle friction to 10% of shape friction
	if (g_params.particleFriction == 0.0f)
		g_params.particleFriction = g_params.dynamicFriction*0.1f;

	// add a margin for detecting contacts between particles and shapes
	if (g_params.shapeCollisionMargin == 0.0f)
		g_params.shapeCollisionMargin = g_params.collisionDistance*0.5f;


	g_maxDiffuseParticles = 0;

	if (g_params.fluid) {

		for (int i = 0; i < nVolumeBoxes; i++) {

			CreateCenteredParticleGrid(Point3(g_volumeBoxes[i].mPos.x, g_volumeBoxes[i].mPos.y, g_volumeBoxes[i].mPos.z), g_volumeBoxes[i].mRot, Point3(g_volumeBoxes[i].mSize.x, g_volumeBoxes[i].mSize.y, g_volumeBoxes[i].mSize.z), g_params.fluidRestDistance, Vec3(0.0f), 1, false, NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid), g_params.fluidRestDistance*0.01f);
		}
	}
	else {

		for (int i = 0; i < nVolumeBoxes; i++) {

			CreateCenteredParticleGrid(Point3(g_volumeBoxes[i].mPos.x, g_volumeBoxes[i].mPos.y, g_volumeBoxes[i].mPos.z), g_volumeBoxes[i].mRot, Point3(g_volumeBoxes[i].mSize.x, g_volumeBoxes[i].mSize.y, g_volumeBoxes[i].mSize.z), g_params.radius, Vec3(0.0f), 1, false, NvFlexMakePhase(0, eNvFlexPhaseSelfCollide), g_params.radius*0.01f);
		}

	}

	g_params.anisotropyScale = 3.0f/g_params.radius;


	uint32_t numParticles = g_buffers->positions.size(); //non zero if init volume boxes

	//**** IF PARTICLE GRID

	if (g_buffers->positions.size()) {
		g_buffers->activeIndices.resize(numParticles);
		for (size_t i = 0; i < g_buffers->activeIndices.size(); ++i)
			g_buffers->activeIndices[i] = i;
	}
	g_inactiveIndices.resize(maxParticles - numParticles);

	for (size_t i = 0; i < g_inactiveIndices.size(); ++i)
		g_inactiveIndices[i] = i + numParticles;


	g_buffers->diffusePositions.resize(g_maxDiffuseParticles);
	g_buffers->diffuseVelocities.resize(g_maxDiffuseParticles);
	g_buffers->diffuseIndices.resize(g_maxDiffuseParticles);
	
	// for fluid rendering these are the Laplacian smoothed positions
	g_buffers->smoothPositions.resize(maxParticles);

	g_buffers->normals.resize(0);
	g_buffers->normals.resize(maxParticles);


	// resize particle buffers to fit
	g_buffers->positions.resize(maxParticles);
	g_buffers->velocities.resize(maxParticles);
	g_buffers->phases.resize(maxParticles);

	g_buffers->densities.resize(maxParticles);
	g_buffers->anisotropy1.resize(maxParticles);
	g_buffers->anisotropy2.resize(maxParticles);
	g_buffers->anisotropy3.resize(maxParticles);

	// save rest positions
	g_buffers->restPositions.resize(g_buffers->positions.size());
	for (int i = 0; i < g_buffers->positions.size(); ++i)
		g_buffers->restPositions[i] = g_buffers->positions[i];


	g_flex = NvFlexCreateSolver(g_flexLib, maxParticles, g_maxDiffuseParticles, g_maxNeighborsPerParticle);

	g_buffers->MapBuffers();

}


void FlexSystem::AddBox(Vec3 halfEdge, Vec3 center, Quat quat, bool dynamic)
{
	// transform
	g_buffers->shapePositions.push_back(Vec4(center.x, center.y, center.z, 0.0f));
	g_buffers->shapeRotations.push_back(quat);

	g_buffers->shapePrevPositions.push_back(g_buffers->shapePositions.back());
	g_buffers->shapePrevRotations.push_back(g_buffers->shapeRotations.back());

	NvFlexCollisionGeometry geo;
	geo.box.halfExtents[0] = halfEdge.x;
	geo.box.halfExtents[1] = halfEdge.y;
	geo.box.halfExtents[2] = halfEdge.z;

	g_buffers->shapeGeometry.push_back(geo);
	g_buffers->shapeFlags.push_back(NvFlexMakeShapeFlags(eNvFlexShapeBox, dynamic));
}

void FlexSystem::AddSphere(float radius, Vec3 position, Quat rotation)
{
	NvFlexCollisionGeometry geo;
	geo.sphere.radius = radius;
	g_buffers->shapeGeometry.push_back(geo);

	g_buffers->shapePositions.push_back(Vec4(position, 0.0f));
	g_buffers->shapeRotations.push_back(rotation);

	g_buffers->shapePrevPositions.push_back(g_buffers->shapePositions.back());
	g_buffers->shapePrevRotations.push_back(g_buffers->shapeRotations.back());

	int flags = NvFlexMakeShapeFlags(eNvFlexShapeSphere, false);
	g_buffers->shapeFlags.push_back(flags);
}

void FlexSystem::GetParticleBounds(Vec3& lower, Vec3& upper)
{
	lower = Vec3(FLT_MAX);
	upper = Vec3(-FLT_MAX);

	for (size_t i=0; i < g_buffers->positions.size(); ++i)
	{
		lower = Min(Vec3(g_buffers->positions[i]), lower);
		upper = Max(Vec3(g_buffers->positions[i]), upper);
	}
}

void FlexSystem::CreateParticleGrid(Vec3 lower, int dimx, int dimy, int dimz, float radius, Vec3 velocity, float invMass, bool rigid, float rigidStiffness, int phase, float jitter=0.005f)
{


	for (int x=0; x < dimx; ++x)
	{
		for (int y=0; y < dimy; ++y)
		{
			for (int z=0; z < dimz; ++z)
			{

				Vec3 position = lower + Vec3(float(x), float(y), float(z))*radius + RandomUnitVector()*jitter;

				g_buffers->positions.push_back(Vec4(position.x, position.y, position.z, invMass));
				g_buffers->velocities.push_back(velocity);
				g_buffers->phases.push_back(phase);
			}
		}
	}


}

void FlexSystem::CreateCenteredParticleGrid(Point3 center, Vec3 rotation, Point3 size, float restDistance, Vec3 velocity, float invMass, bool rigid, int phase, float jitter)
{

	int dx = int(ceilf(size.x / restDistance));
	int dy = int(ceilf(size.y / restDistance));
	int dz = int(ceilf(size.z / restDistance));

	for (int x=0; x < dx; ++x)
	{
		for (int y=0; y < dy; ++y)
		{
			for (int z=0; z < dz; ++z)
			{
				Point3 position = restDistance*Point3(float(x) - 0.5*(dx-1), float(y) - 0.5*(dy-1), float(z) - 0.5*(dz-1)) + RandomUnitVector()*jitter;
				position = TranslationMatrix(center) * TransformMatrix(Rotation(rotation.y, rotation.z, rotation.x), Point3(0.f))*position;

				if(cursor<maxParticles-1) {

					g_buffers->positions.push_back(Vec4(position.x, position.y, position.z, invMass));
					g_buffers->velocities.push_back(velocity);
					g_buffers->phases.push_back(phase);

					cursor++;
				}

				
				
			}
		}
	}
}

void FlexSystem::ClearShapes()
{
	g_buffers->shapeGeometry.resize(0);
	g_buffers->shapePositions.resize(0);
	g_buffers->shapeRotations.resize(0);
	g_buffers->shapePrevPositions.resize(0);
	g_buffers->shapePrevRotations.resize(0);
	g_buffers->shapeFlags.resize(0);
}

void FlexSystem::setShapes(){

	if(g_buffers->shapeFlags.size()){
	NvFlexSetShapes(
				g_flex,
				g_buffers->shapeGeometry.buffer,
				g_buffers->shapePositions.buffer,
				g_buffers->shapeRotations.buffer,
				g_buffers->shapePrevPositions.buffer,
				g_buffers->shapePrevRotations.buffer,
				g_buffers->shapeFlags.buffer,
				g_buffers->shapeFlags.size());
		}
		

}


void FlexSystem::emission(){

	size_t e=0;


	for (; e < nEmitter; ++e)
	{
		if (!g_rectEmitters[e].mEnabled)
			continue;

		Vec3 emitterRot = g_rectEmitters[e].mRot;
		Point3 emitterSize = g_rectEmitters[e].mSize;
		Point3 emitterPos = g_rectEmitters[e].mPos;

		float r;
		int phase;

		if (g_params.fluid)
		{
			r = g_params.fluidRestDistance;
			phase = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid);
		}
		else
		{
			r = g_params.solidRestDistance;
			phase = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide);
		}

		float numSlices = (g_rectEmitters[e].mSpeed / r)*g_dt;

		// whole number to emit
		int n = int(numSlices + g_rectEmitters[e].mLeftOver);
				
		if (n)
			g_rectEmitters[e].mLeftOver = (numSlices + g_rectEmitters[e].mLeftOver)-n;
		else
			g_rectEmitters[e].mLeftOver += numSlices;

		//int circle = 1;

		int disc = g_rectEmitters[e].mDisc;

		int dy = 0;

				
		int dx = int(ceilf(emitterSize.x / g_params.fluidRestDistance));
		if(disc)
			dy = int(ceilf(emitterSize.x / g_params.fluidRestDistance));
		else
			dy = int(ceilf(emitterSize.y / g_params.fluidRestDistance));
		Mat44	tMat = TransformMatrix(Rotation(emitterRot.y, emitterRot.z, emitterRot.x), emitterPos);


		for (int z=0; z < n; ++z)
				{
			for (int x=0; x < dx; ++x)
			{
				for (int y=0; y < dy; ++y)
				{
						
					Point3 position = g_params.fluidRestDistance*Point3(float(x) - 0.5*(dx-1), float(y) - 0.5*(dy-1), float(z)) + RandomUnitVector()*g_params.fluidRestDistance*0.01f;

					int keep = 1;

					if(disc){
						if(position.x*position.x + position.y*position.y>0.25*emitterSize.x*emitterSize.x)
							keep=0;
					}

					if(g_rectEmitters[e].mNoise){
						Point3 scaledP = position*g_rectEmitters[e].mNoiseFreq + Point3(0,0,g_rectEmitters[e].mNoiseOffset);
						const float kNoise = Perlin3D(scaledP.x, scaledP.y, scaledP.z, 1, 0.25f);

						if(kNoise<g_rectEmitters[e].mNoiseThreshold)
							keep=0;

					}

					if(keep) {

						position = tMat*position;

						Vec3 vel = Vec3(0,0,g_rectEmitters[e].mSpeed);
						vel = tMat*vel;

						g_buffers->positions[cursor] = Vec4(Vec3(position), 1.0f);
						g_buffers->velocities[cursor] = vel;
						g_buffers->phases[cursor] = phase;

						if(g_buffers->activeIndices.size()<maxParticles)
							g_buffers->activeIndices.push_back(cursor);

						if(cursor<maxParticles-1)
							cursor++;
						else
							cursor = 0;

						
					}//end dist

				}
			}
		}

	}

	

}


void FlexSystem::update(){


		activeParticles = NvFlexGetActiveCount(g_flex);


		time1 = GetSeconds();

		emission();

		time2 = GetSeconds();


		g_buffers->UnmapBuffers();


		if (g_buffers->activeIndices.size()) {
			NvFlexSetActive(g_flex, g_buffers->activeIndices.buffer, g_buffers->activeIndices.size());
		}

		if (g_buffers->positions.size()) {


			NvFlexSetParticles(g_flex, g_buffers->positions.buffer, g_buffers->positions.size());
			NvFlexSetVelocities(g_flex, g_buffers->velocities.buffer, g_buffers->velocities.size());
			NvFlexSetPhases(g_flex, g_buffers->phases.buffer, g_buffers->phases.size());

		}

		setShapes();

		NvFlexSetParams(g_flex, &g_params);

		NvFlexUpdateSolver(g_flex, g_dt, g_numSubsteps, g_profile);


		if(g_buffers->positions.size()){


			NvFlexGetParticles(g_flex, g_buffers->positions.buffer, g_buffers->positions.size());
			NvFlexGetVelocities(g_flex, g_buffers->velocities.buffer, g_buffers->velocities.size());

		}

		activeParticles = NvFlexGetActiveCount(g_flex);

}