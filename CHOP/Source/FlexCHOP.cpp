#include "FlexCHOP.h"

#include <stdio.h>
#include <string.h>


typedef unsigned int       uint32_t;

// These functions are basic C function, which the DLL loader can find
// much easier than finding a C++ Class.
// The DLLEXPORT prefix is needed so the compile exports these functions from the .dll
// you are creating
extern "C" {

DLLEXPORT void FillCHOPPluginInfo(CHOP_PluginInfo *info)
	{
		// Always set this to CHOPCPlusPlusAPIVersion.
		info->apiVersion = CHOPCPlusPlusAPIVersion;

		// The opType is the unique name for this CHOP. It must start with a 
		// capital A-Z character, and all the following characters must lower case
		// or numbers (a-z, 0-9)
		info->customOPInfo.opType->setString("Flex");

		// The opLabel is the text that will show up in the OP Create Dialog
		info->customOPInfo.opLabel->setString("Flex");

		// Information about the author of this OP
		info->customOPInfo.authorName->setString("Vincent Houze");
		//info->customOPInfo.authorEmail->setString("email@email.com");

		// This CHOP can work with 0 inputs
		info->customOPInfo.minInputs = 0;

		// It can accept up to 1 input though, which changes it's behavior
		info->customOPInfo.maxInputs = 0;
	}

DLLEXPORT CHOP_CPlusPlusBase* CreateCHOPInstance(const OP_NodeInfo *info)
{
	// Return a new instance of your class every time this is called.
	// It will be called once per CHOP that is using the .dll
	return new FlexCHOP(info);
}

DLLEXPORT void DestroyCHOPInstance(CHOP_CPlusPlusBase *instance)
{
	// Delete the instance here, this will be called when
	// Touch is shutting down, when the CHOP using that instance is deleted, or
	// if the CHOP loads a different DLL
	delete (FlexCHOP*)instance;
}

};



FlexCHOP::FlexCHOP(const OP_NodeInfo *info) : myNodeInfo(info)
{
	myExecuteCount = 0;

	printf("\n\n*************************INITCHOP**************************\n");


	FlexSys = new FlexSystem();

	FlexSys->initSystem();

	maxVel = 1;

	maxParticles = 0;
	activeIndicesSize = 0;
	inactiveIndicesSize = 0;

}

FlexCHOP::~FlexCHOP()
{
	
	delete FlexSys;

}

void FlexCHOP::getGeneralInfo(CHOP_GeneralInfo* ginfo, const OP_Inputs *inputs, void* reserved1)
{
	// This will cause the node to cook every frame
	ginfo->cookEveryFrameIfAsked = true;
	ginfo->timeslice = false;
	ginfo->inputMatchIndex = 0;
}

bool FlexCHOP::getOutputInfo(CHOP_OutputInfo* info, const OP_Inputs *inputs, void *reserved1)
{
	
	if (FlexSys->g_solver){
		info->numSamples = FlexSys->g_buffers->positions.size();
	}else{
		info->numSamples = 1;
	}
	
	
	info->numChannels = 6;

	info->sampleRate = 60;

	return true;

}

void FlexCHOP::updateParams(const OP_Inputs* inputs) {

	FlexSys->g_profile = inputs->getParInt("Profile");

	FlexSys->g_params.maxSpeed = inputs->getParDouble("Maxspeed");
	FlexSys->g_numSubsteps = inputs->getParInt("Numsubsteps");
	FlexSys->g_params.numIterations = inputs->getParInt("Numiterations");

	float fps = inputs->getParDouble("Fps");
	fps = max(1.0f, fps);

	FlexSys->g_dt = 1.0 / fps;
	maxVel = 0.5f*FlexSys->g_params.radius*FlexSys->g_numSubsteps / FlexSys->g_dt;


	double gravity[3];
	inputs->getParDouble3("Gravity", gravity[0], gravity[1], gravity[2]);

	FlexSys->g_params.gravity[0] = gravity[0];
	FlexSys->g_params.gravity[1] = gravity[1];
	FlexSys->g_params.gravity[2] = gravity[2];

	FlexSys->g_params.dynamicFriction = inputs->getParDouble("Dynamicfriction");
	FlexSys->g_params.restitution = inputs->getParDouble("Restitution");
	FlexSys->g_params.adhesion = inputs->getParDouble("Adhesion");
	FlexSys->g_params.dissipation = inputs->getParDouble("Dissipation");

	FlexSys->g_params.cohesion = inputs->getParDouble("Cohesion");
	FlexSys->g_params.surfaceTension = inputs->getParDouble("Surfacetension");
	FlexSys->g_params.viscosity = inputs->getParDouble("Viscosity");
	FlexSys->g_params.vorticityConfinement = inputs->getParDouble("Vorticityconfinement");

	FlexSys->g_params.damping = inputs->getParDouble("Damping");

	FlexSys->g_params.collisionDistance = inputs->getParDouble("Collisiondistance");
	FlexSys->g_params.shapeCollisionMargin = inputs->getParDouble("Shapecollisionmargin");

	// set collision distance automatically based on rest distance if not alraedy set
	if (FlexSys->g_params.collisionDistance == 0.0f)
		FlexSys->g_params.collisionDistance = FlexSys->g_params.fluidRestDistance*0.5f;

	// add a margin for detecting contacts between particles and shapes
	if (FlexSys->g_params.shapeCollisionMargin == 0.0f)
		FlexSys->g_params.shapeCollisionMargin = FlexSys->g_params.collisionDistance*0.5f;

}

void FlexCHOP::getChannelName(int32_t index, OP_String *name, const OP_Inputs *inputs, void* reserved1)
{
	switch(index) {
		case 0:
			name->setString("tx");
			break;
		case 1:
			name->setString("ty");
			break;
		case 2:
			name->setString("tz");
			break;
		case 3:
			name->setString("vx");
			break;
		case 4:
			name->setString("vy");
			break;
		case 5:
			name->setString("vz");
			break;
			}
}

void FlexCHOP::updateTriangleMesh(const OP_Inputs* inputs) {
	const OP_SOPInput* sopInput = inputs->getParSOP("Trianglespolysop");
	if (FlexSys->deformingMesh && sopInput && sopInput->getNumPrimitives()>0 && FlexSys->g_triangleCollisionMesh->GetNumVertices() == sopInput->getNumPoints()) {

		Point3 minExt(FLT_MAX);
		Point3 maxExt(-FLT_MAX);

		for (int i = 0; i < sopInput->getNumPoints(); i++) {
			Position curPos = sopInput->getPointPositions()[i];
			FlexSys->g_triangleCollisionMesh->m_positions[i] = Vec4(curPos.x, curPos.y, curPos.z, 0.0);

			const Point3& a = Point3(curPos.x, curPos.y, curPos.z);

			minExt = Min(a, minExt);
			maxExt = Max(a, maxExt);
		}

		FlexSys->g_triangleCollisionMesh->minExtents = Vector3(minExt);
		FlexSys->g_triangleCollisionMesh->maxExtents = Vector3(maxExt);

		FlexSys->UpdateTriangleMesh(FlexSys->g_triangleCollisionMesh, FlexSys->triangleCollisionMeshId);
	}
}

void FlexCHOP::initTriangleMesh(const OP_Inputs* inputs) {
	const OP_SOPInput* sopInput = inputs->getParSOP("Trianglespolysop");
	if (sopInput && sopInput->getNumPrimitives()>0) {

		if (FlexSys->g_triangleCollisionMesh)
			delete FlexSys->g_triangleCollisionMesh;

		FlexSys->g_triangleCollisionMesh = new VMesh();

		FlexSys->deformingMesh = inputs->getParInt("Deformingmesh");

		Point3 minExt(FLT_MAX);
		Point3 maxExt(-FLT_MAX);

		for (int i = 0; i < sopInput->getNumPoints(); i++) {
			Position curPos = sopInput->getPointPositions()[i];
			FlexSys->g_triangleCollisionMesh->m_positions.push_back(Vec4(curPos.x, curPos.y, curPos.z, 0.0));

			const Point3& a = Point3(curPos.x, curPos.y, curPos.z);

			minExt = Min(a, minExt);
			maxExt = Max(a, maxExt);

		}

		FlexSys->g_triangleCollisionMesh->minExtents = Vector3(minExt);
		FlexSys->g_triangleCollisionMesh->maxExtents = Vector3(maxExt);


		for (int i = 0; i < sopInput->getNumPrimitives(); i++) {
			SOP_PrimitiveInfo curPrim = sopInput->getPrimitive(i);
			FlexSys->g_triangleCollisionMesh->m_indices.push_back(curPrim.pointIndices[2]);
			FlexSys->g_triangleCollisionMesh->m_indices.push_back(curPrim.pointIndices[1]);
			FlexSys->g_triangleCollisionMesh->m_indices.push_back(curPrim.pointIndices[0]);

		}

		double meshTrans[3];
		inputs->getParDouble3("Meshtranslation", meshTrans[0], meshTrans[1], meshTrans[2]);

		double meshRot[3];
		inputs->getParDouble3("Meshrotation", meshRot[0], meshRot[1], meshRot[2]);

		FlexSys->curMeshTrans = Vec3(meshTrans[0], meshTrans[1], meshTrans[2]);
		Vec3 rot = Vec3(meshRot[0], meshRot[1], meshRot[2]);

		Quat qx = QuatFromAxisAngle(Vec3(1, 0, 0), DegToRad(rot.x));
		Quat qy = QuatFromAxisAngle(Vec3(0, 1, 0), DegToRad(rot.y));
		Quat qz = QuatFromAxisAngle(Vec3(0, 0, 1), DegToRad(rot.z));

		FlexSys->curMeshRot = qz*qy*qx;

		FlexSys->previousMeshTrans = FlexSys->curMeshTrans;
		FlexSys->previousMeshRot = FlexSys->curMeshRot;

	}

}

void FlexCHOP::initVolumeBoxes(const OP_Inputs* inputs) {
	const OP_CHOPInput* volumeBoxesInput = inputs->getParCHOP("Boxesemitterschop");
	if (volumeBoxesInput && volumeBoxesInput->numChannels == 9) {
		FlexSys->nVolumeBoxes = inputs->getParCHOP("Boxesemitterschop")->numSamples;
		VolumeBox vb1;

		for (int i = 0; i < FlexSys->nVolumeBoxes; i++) {
			vb1.mPos = Point3(volumeBoxesInput->getChannelData(0)[i],
				volumeBoxesInput->getChannelData(1)[i],
				volumeBoxesInput->getChannelData(2)[i]);

			vb1.mRot = Vec3(volumeBoxesInput->getChannelData(3)[i],
				volumeBoxesInput->getChannelData(4)[i],
				volumeBoxesInput->getChannelData(5)[i]);

			vb1.mSize = Point3(volumeBoxesInput->getChannelData(6)[i],
				volumeBoxesInput->getChannelData(7)[i],
				volumeBoxesInput->getChannelData(8)[i]);


			FlexSys->g_volumeBoxes.push_back(vb1);
		}
	}
	else
		FlexSys->nVolumeBoxes = 0;
}

void FlexCHOP::initEmitters(const OP_Inputs* inputs) {
	const OP_CHOPInput* emittersInput = inputs->getParCHOP("Emitterschop");
	if (emittersInput && emittersInput->numChannels == 15) {

		FlexSys->nEmitter = emittersInput->numSamples;
		RectEmitter re1;
		for (int i = 0; i < FlexSys->nEmitter; i++) {
			FlexSys->g_rectEmitters.push_back(re1);
		}
	}
	else
		FlexSys->nEmitter = 0;
}

void FlexCHOP::updatePlanes(const OP_Inputs* inputs) {

	const OP_CHOPInput*	colPlanesInput = inputs->getParCHOP("Colplaneschop");
	if (colPlanesInput) {
		if (colPlanesInput->numChannels == 4) {

			int nPlanes = colPlanesInput->numSamples;
			FlexSys->g_params.numPlanes = nPlanes;

			for (int i = 0; i < nPlanes; i++) {
				(Vec4&)FlexSys->g_params.planes[i] = Vec4(colPlanesInput->getChannelData(0)[i],
					colPlanesInput->getChannelData(1)[i],
					colPlanesInput->getChannelData(2)[i],
					colPlanesInput->getChannelData(3)[i]);
			}
		}
	}
}

void FlexCHOP::updateEmitters(const OP_Inputs* inputs) {
	const OP_CHOPInput* emitterInput = inputs->getParCHOP("Emitterschop");
	if (emitterInput && FlexSys->nEmitter>0 && emitterInput->numChannels == 15) {

		int length = min(FlexSys->nEmitter, emitterInput->numSamples);

		for (int i = 0; i < length; i++) {

			FlexSys->g_rectEmitters[i].mPos = Point3(emitterInput->getChannelData(0)[i],
				emitterInput->getChannelData(1)[i],
				emitterInput->getChannelData(2)[i]);

			FlexSys->g_rectEmitters[i].mRot = Vec3(emitterInput->getChannelData(3)[i],
				emitterInput->getChannelData(4)[i],
				emitterInput->getChannelData(5)[i]);

			FlexSys->g_rectEmitters[i].mSize = Point3(emitterInput->getChannelData(6)[i],
				emitterInput->getChannelData(7)[i],
				0);

			FlexSys->g_rectEmitters[i].mSpeed = emitterInput->getChannelData(8)[i];
			FlexSys->g_rectEmitters[i].mDisc = emitterInput->getChannelData(9)[i];

			FlexSys->g_rectEmitters[i].mEnabled = emitterInput->getChannelData(10)[i];

			FlexSys->g_rectEmitters[i].mNoise = emitterInput->getChannelData(11)[i];
			FlexSys->g_rectEmitters[i].mNoiseThreshold = emitterInput->getChannelData(12)[i];
			FlexSys->g_rectEmitters[i].mNoiseFreq = emitterInput->getChannelData(13)[i];
			FlexSys->g_rectEmitters[i].mNoiseOffset = emitterInput->getChannelData(14)[i];
		}
	}
}

void FlexCHOP::updateSpheresCols(const OP_Inputs* inputs) {
	const OP_CHOPInput* spheresInput = inputs->getParCHOP("Colsphereschop");
	if (spheresInput && spheresInput->numChannels == 4) {
		for (int i = 0; i < spheresInput->numSamples; i++) {

			Vec3 spherePos = Vec3(spheresInput->getChannelData(0)[i],
				spheresInput->getChannelData(1)[i],
				spheresInput->getChannelData(2)[i]);

			float sphereRadius = spheresInput->getChannelData(3)[i];

			FlexSys->AddSphere(sphereRadius, spherePos, Quat());
		}
	}
}

void FlexCHOP::updateBoxesCols(const OP_Inputs* inputs) {

	const OP_CHOPInput* boxesInput = inputs->getParCHOP("Colboxeschop");
	if (boxesInput && boxesInput->numChannels == 9) {
		for (int i = 0; i < boxesInput->numSamples; i++) {

			Vec3 boxPos = Vec3(boxesInput->getChannelData(0)[i],
				boxesInput->getChannelData(1)[i],
				boxesInput->getChannelData(2)[i]);

			Vec3 boxSize = Vec3(boxesInput->getChannelData(3)[i],
				boxesInput->getChannelData(4)[i],
				boxesInput->getChannelData(5)[i]);

			Vec3 boxRot = Vec3(boxesInput->getChannelData(6)[i],
				boxesInput->getChannelData(7)[i],
				boxesInput->getChannelData(8)[i]);

			Quat qx = QuatFromAxisAngle(Vec3(1, 0, 0), DegToRad(boxRot.x));
			Quat qy = QuatFromAxisAngle(Vec3(0, 1, 0), DegToRad(boxRot.y));
			Quat qz = QuatFromAxisAngle(Vec3(0, 0, 1), DegToRad(boxRot.z));

			FlexSys->AddBox(boxSize, boxPos, qz*qy*qx);
		}
	}
}

void FlexCHOP::execute(CHOP_Output* output, const OP_Inputs* inputs, void* reserved1)
{
	myExecuteCount++;
	//double t1 = GetSeconds();

	int posRender = 0;

	int reset = inputs->getParInt("Reset");

	if (reset == 1) {

		printf("\n********INITFLEX***********\n");

		FlexSys->initScene();
		FlexSys->g_params.radius = inputs->getParDouble("Radius");

		initVolumeBoxes(inputs);
		initEmitters(inputs);

		FlexSys->maxParticles = inputs->getParInt("Maxparticles");

		initTriangleMesh(inputs);

	} //reset End

	updateParams(inputs);
	

	if (FlexSys->g_solver) {

		FlexSys->g_buffers->MapBuffers();
		FlexSys->getSimTimers();

		//planes collision
		updatePlanes(inputs);

		//emission
		updateEmitters(inputs);

		FlexSys->ClearShapes();

		updateSpheresCols(inputs);
		updateBoxesCols(inputs);

		if (FlexSys->g_triangleCollisionMesh) {
			updateTriangleMesh(inputs);

			FlexSys->previousMeshTrans = FlexSys->curMeshTrans;
			FlexSys->previousMeshRot = FlexSys->curMeshRot;

			double meshTrans[3];
			inputs->getParDouble3("Meshtranslation", meshTrans[0], meshTrans[1], meshTrans[2]);

			double meshRot[3];
			inputs->getParDouble3("Meshrotation", meshRot[0], meshRot[1], meshRot[2]);

			FlexSys->curMeshTrans = Vec3(meshTrans[0], meshTrans[1], meshTrans[2]);
			Vec3 rot = Vec3(meshRot[0], meshRot[1], meshRot[2]);

			Quat qx = QuatFromAxisAngle(Vec3(1, 0, 0), DegToRad(rot.x));
			Quat qy = QuatFromAxisAngle(Vec3(0, 1, 0), DegToRad(rot.y));
			Quat qz = QuatFromAxisAngle(Vec3(0, 0, 1), DegToRad(rot.z));

			FlexSys->curMeshRot = qz*qy*qx;

			FlexSys->AddTriangleMesh(FlexSys->triangleCollisionMeshId, FlexSys->curMeshTrans, FlexSys->curMeshRot, FlexSys->previousMeshTrans, FlexSys->previousMeshRot, 1.0f);

		}

	}

	int simulate = inputs->getParInt("Simulate");

	if (reset == 1) {

		FlexSys->postInitScene();
		FlexSys->update();

	}
	else if (FlexSys->g_solver) {

		double t1 = GetSeconds();
		for (int i = 0; i < output->numSamples; i++) {
			
			//memcpy(output->channels[i], &FlexSys->g_buffers->positions[0], FlexSys->g_buffers->positions.size() * sizeof(Vec4));
			//memcpy(output->channels[i], &FlexSys->g_buffers->velocities[0], FlexSys->g_buffers->positions.size() * sizeof(Vec3));


			output->channels[TX][i] = FlexSys->g_buffers->positions[i].x;
			output->channels[TY][i] = FlexSys->g_buffers->positions[i].y;
			output->channels[TZ][i] = FlexSys->g_buffers->positions[i].z;

			output->channels[VX][i] = FlexSys->g_buffers->velocities[i].x;
			output->channels[VY][i] = FlexSys->g_buffers->velocities[i].y;
			output->channels[VZ][i] = FlexSys->g_buffers->velocities[i].z;

		}

		double t2 = GetSeconds();

		timer = 1000*(t2 - t1);

		if (simulate == 1)
			FlexSys->update();


		//activeCount = FlexSys->activeParticles;
		maxParticles = FlexSys->maxParticles;

		//positionsSize = FlexSys->g_buffers->positions.size();
		activeIndicesSize = FlexSys->g_buffers->activeIndices.size();
		inactiveIndicesSize = FlexSys->g_inactiveIndices.size();

	}

}

int32_t FlexCHOP::getNumInfoCHOPChans(void *reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 3;
}

void FlexCHOP::getInfoCHOPChan(int32_t index, OP_InfoCHOPChan* chan, void* reserved1)
{
	// This function will be called once for each channel we said we'd want to return
	// In this example it'll only be called once.

	switch (index) {

	case 0:
		chan->name->setString("executeCount");
		chan->value = myExecuteCount;
		break;

	case 1:
		chan->name->setString("flexTime");
		chan->value = FlexSys->simLatency;
		break;

	case 2:
		chan->name->setString("solveVelocities");
		chan->value = FlexSys->g_timers.solveVelocities;
		break;


	}
}

bool FlexCHOP::getInfoDATSize(OP_InfoDATSize* infoSize, void *reserved1)
{
	infoSize->rows = 7;
	infoSize->cols = 2;
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void FlexCHOP::getInfoDATEntries(int32_t index, int32_t nEntries, OP_InfoDATEntries* entries, void *reserved1)
{
	
	static char tempBuffer1[4096];
	static char tempBuffer2[4096];

	switch (index) {

	case 0:
		strcpy(tempBuffer1, "maxParticles");
		sprintf(tempBuffer2, "%d", maxParticles);
		break;

	case 1:
		strcpy(tempBuffer1, "activeIndicesSize");
		sprintf(tempBuffer2, "%d", activeIndicesSize);
		break;

	case 2:
		strcpy(tempBuffer1, "inactiveIndicesSize");
		sprintf(tempBuffer2, "%d", inactiveIndicesSize);
		break;

	case 3:
		strcpy(tempBuffer1, "maxVel");
		sprintf(tempBuffer2, "%f", maxVel);
		break;

	case 4:
		strcpy(tempBuffer1, "cursor");
		sprintf(tempBuffer2, "%d", FlexSys->cursor);
		break;

	case 5:
		strcpy(tempBuffer1, "collisionDistance");
		sprintf(tempBuffer2, "%f", FlexSys->g_params.collisionDistance);
		break;

	case 6:
		strcpy(tempBuffer1, "shapeCollisionMargin");
		sprintf(tempBuffer2, "%f", FlexSys->g_params.shapeCollisionMargin);
		break;

	}

	entries->values[0]->setString(tempBuffer1);
	entries->values[1]->setString(tempBuffer2);
}

void FlexCHOP::setupParamsCustom(OP_ParameterManager* manager) {
	// reset
	{
		OP_NumericParameter	np;

		np.name = "Reset";
		np.label = "Reset";
		np.defaultValues[0] = 0.0;

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Simulate
	{
		OP_NumericParameter	np;

		np.name = "Simulate";
		np.label = "Simulate";
		np.defaultValues[0] = 1.0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Radius
	{
		OP_NumericParameter	np;

		np.name = "Radius";
		np.label = "Radius";
		np.defaultValues[0] = 0.05;

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}


	// Profile
	{
		OP_NumericParameter	np;

		np.name = "Profile";
		np.label = "Profile";
		np.defaultValues[0] = 0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}


	// Maxspeed
	{
		OP_NumericParameter	np;

		np.name = "Maxspeed";
		np.label = "Max Speed";
		np.defaultValues[0] = 100;

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}


	//Gravity
	{
		OP_NumericParameter	np;

		np.name = "Gravity";
		np.label = "Gravity";

		np.defaultValues[0] = 0.0;
		np.defaultValues[1] = -3.0;
		np.defaultValues[2] = 0.0;


		OP_ParAppendResult res = manager->appendXYZ(np);
		assert(res == OP_ParAppendResult::Success);
	}
}

void FlexCHOP::setupParamsSolver(OP_ParameterManager* manager) {
	// Fps
	{
		OP_NumericParameter	np;

		np.name = "Fps";
		np.label = "FPS";
		np.page = "Solver";
		np.defaultValues[0] = 30;
		np.clampMins[0] = true;
		np.minValues[0] = 1.0;

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Numsubsteps
	{
		OP_NumericParameter	np;

		np.name = "Numsubsteps";
		np.label = "Num Substeps";
		np.page = "Solver";
		np.defaultValues[0] = 3;

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Numiterations
	{
		OP_NumericParameter	np;

		np.name = "Numiterations";
		np.label = "Num Iterations";
		np.page = "Solver";
		np.defaultValues[0] = 3;

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Maxparticles
	{
		OP_NumericParameter	np;

		np.name = "Maxparticles";
		np.label = "Max Number of Particles";
		np.page = "Solver";
		np.defaultValues[0] = 160000;

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}
}

void FlexCHOP::setupParamsParts(OP_ParameterManager* manager) {
	//Damping
	{
		OP_NumericParameter	np;

		np.name = "Damping";
		np.label = "Damping";
		np.defaultValues[0] = 0;
		np.page = "PartsParams";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Dynamicfriction
	{
		OP_NumericParameter	np;

		np.name = "Dynamicfriction";
		np.label = "Dynamic Friction";
		np.defaultValues[0] = 0;
		np.page = "PartsParams";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Restitution
	{
		OP_NumericParameter	np;

		np.name = "Restitution";
		np.label = "Restitution";
		np.defaultValues[0] = 0.001f;
		np.page = "PartsParams";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Adhesion
	{
		OP_NumericParameter	np;

		np.name = "Adhesion";
		np.label = "Adhesion";
		np.defaultValues[0] = 0.0f;
		np.page = "PartsParams";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Dissipation
	{
		OP_NumericParameter	np;

		np.name = "Dissipation";
		np.label = "Dissipation";
		np.defaultValues[0] = 0.0f;
		np.page = "PartsParams";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Cohesion
	{
		OP_NumericParameter	np;

		np.name = "Cohesion";
		np.label = "Cohesion";
		np.defaultValues[0] = 0.1f;
		np.page = "PartsParams";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Surfacetension
	{
		OP_NumericParameter	np;

		np.name = "Surfacetension";
		np.label = "Surface Tension";
		np.defaultValues[0] = 0.0f;
		np.page = "PartsParams";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Viscosity
	{
		OP_NumericParameter	np;

		np.name = "Viscosity";
		np.label = "Viscosity";
		np.defaultValues[0] = 0.0f;
		np.page = "PartsParams";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Vorticityconfinement
	{
		OP_NumericParameter	np;

		np.name = "Vorticityconfinement";
		np.label = "Vorticity Confinement";
		np.defaultValues[0] = 80.0f;
		np.page = "PartsParams";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}
}

void FlexCHOP::setupParamsEmission(OP_ParameterManager* manager) {
	//Boxesemitterschop
	{
		OP_StringParameter sp;

		sp.name = "Boxesemitterschop";
		sp.label = "Boxes Emitters CHOP";
		sp.page = "Emission";

		OP_ParAppendResult res = manager->appendCHOP(sp);
		assert(res == OP_ParAppendResult::Success);
	}


	//Emitterschop
	{
		OP_StringParameter sp;

		sp.name = "Emitterschop";
		sp.label = "Emitters CHOP";
		sp.page = "Emission";

		OP_ParAppendResult res = manager->appendCHOP(sp);
		assert(res == OP_ParAppendResult::Success);
	}
}

void FlexCHOP::setupParamsCollisions(OP_ParameterManager* manager) {
	//Colplaneschop
	{
		OP_StringParameter sp;

		sp.name = "Colplaneschop";
		sp.label = "Collision Planes CHOP";
		sp.page = "Collisions";

		OP_ParAppendResult res = manager->appendCHOP(sp);
		assert(res == OP_ParAppendResult::Success);
	}

	//Colspheres
	{
		OP_StringParameter sp;

		sp.name = "Colsphereschop";
		sp.label = "Collision Spheres CHOP";
		sp.page = "Collisions";

		OP_ParAppendResult res = manager->appendCHOP(sp);
		assert(res == OP_ParAppendResult::Success);
	}

	//Colboxes
	{
		OP_StringParameter sp;

		sp.name = "Colboxeschop";
		sp.label = "Collision Boxes CHOP";
		sp.page = "Collisions";

		OP_ParAppendResult res = manager->appendCHOP(sp);
		assert(res == OP_ParAppendResult::Success);
	}

	//Trianglespolysop
	{
		OP_StringParameter	sp;

		sp.name = "Trianglespolysop";
		sp.label = "Triangles Polygons SOP";
		sp.page = "Collisions";

		OP_ParAppendResult res = manager->appendSOP(sp);
		assert(res == OP_ParAppendResult::Success);
	}

	// Deformingmesh
	{
		OP_NumericParameter	np;

		np.name = "Deformingmesh";
		np.label = "Deforming Mesh";
		np.page = "Collisions";
		np.defaultValues[0] = 0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Meshtranslation
	{
		OP_NumericParameter	np;

		np.name = "Meshtranslation";
		np.label = "Mesh Translation";
		np.page = "Collisions";
		np.defaultValues[0] = 0;
		np.defaultValues[1] = 0;
		np.defaultValues[2] = 0;

		OP_ParAppendResult res = manager->appendXYZ(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Meshrotation
	{
		OP_NumericParameter	np;

		np.name = "Meshrotation";
		np.label = "Mesh Rotation";
		np.page = "Collisions";
		np.defaultValues[0] = 0;
		np.defaultValues[1] = 0;
		np.defaultValues[2] = 0;

		OP_ParAppendResult res = manager->appendXYZ(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Collisiondistance
	{
		OP_NumericParameter	np;

		np.name = "Collisiondistance";
		np.label = "Collision Distance";
		np.defaultValues[0] = 0.0f; // 0.05f;
		np.page = "Collisions";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}

	//Shapecollisionmargin
	{
		OP_NumericParameter	np;

		np.name = "Shapecollisionmargin";
		np.label = "Shape Collision Margin";
		np.defaultValues[0] = 0.0f; //0.00001f
		np.page = "Collisions";

		OP_ParAppendResult res = manager->appendFloat(np);
		assert(res == OP_ParAppendResult::Success);
	}
}

void FlexCHOP::setupParameters(OP_ParameterManager* manager, void* reserved1)
{
	setupParamsCustom(manager);
	setupParamsSolver(manager);
	setupParamsParts(manager);
	setupParamsEmission(manager);
	setupParamsCollisions(manager);
	
}