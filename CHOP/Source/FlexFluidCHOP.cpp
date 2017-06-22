#include "FlexFluidCHOP.h"

#include <stdio.h>
#include <string.h>
//#include <math.h>

#define FLT_MAX         3.402823466e+38F        /* max value */
typedef unsigned int       uint32_t;

// These functions are basic C function, which the DLL loader can find
// much easier than finding a C++ Class.
// The DLLEXPORT prefix is needed so the compile exports these functions from the .dll
// you are creating
extern "C"
{

DLLEXPORT
int
GetCHOPAPIVersion(void)
{
	// Always return CHOP_CPLUSPLUS_API_VERSION in this function.
	return CHOP_CPLUSPLUS_API_VERSION;
}

DLLEXPORT
CHOP_CPlusPlusBase*
CreateCHOPInstance(const CHOP_NodeInfo *info)
{
	// Return a new instance of your class every time this is called.
	// It will be called once per CHOP that is using the .dll
	return new FlexFluidCHOP(info);
}

DLLEXPORT
void
DestroyCHOPInstance(CHOP_CPlusPlusBase *instance)
{
	// Delete the instance here, this will be called when
	// Touch is shutting down, when the CHOP using that instance is deleted, or
	// if the CHOP loads a different DLL
	delete (FlexFluidCHOP*)instance;
}

};



FlexFluidCHOP::FlexFluidCHOP(const CHOP_NodeInfo *info) : myNodeInfo(info)
{
	myExecuteCount = 0;

	activeCount =0;
	maxParticles=0;

	positionsSize=0;
	activeIndicesSize=0;

	flexInit();

	FlexSys = new FlexSystem();

	//sourcePos.push_back(Vec3(0,0,0));
	//sourceDir.push_back(Vec3(1,0,0));

	//initFlexScene();

    //solver = flexCreateSolver(maxParticles, maxDiffuse);
}

FlexFluidCHOP::~FlexFluidCHOP()
{
	
	delete FlexSys;
    

}

void
FlexFluidCHOP::getGeneralInfo(CHOP_GeneralInfo *ginfo)
{
	// This will cause the node to cook every frame
	ginfo->cookEveryFrameIfAsked = true;
	ginfo->timeslice = false;
	ginfo->inputMatchIndex = 0;
}

bool
FlexFluidCHOP::getOutputInfo(CHOP_OutputInfo *info)
{
	
	//info->length = FlexSys->g_positions.size();

	info->length = activeCount;

	/*if (FlexSys->g_flex){
		info->length = FlexSys->g_positions.size();
	}else{
		info->length = 1;
	}*/
		info->numChannels = 7;
		info->sampleRate = 60;

	return true;

}

const char*
FlexFluidCHOP::getChannelName(int index, void* reserved)
{
	const char* name = "";

	switch(index) {
		case 0:
			name = "tx";
			break;
		case 1:
			name = "ty";
			break;
		case 2:
			name = "tz";
			break;
		case 3:
			name = "vx";
			break;
		case 4:
			name = "vy";
			break;
		case 5:
			name = "vz";
			break;
		case 6:
			name = "speed";
			break;

			}
	return name;
}

void
FlexFluidCHOP::execute(const CHOP_Output* output,
								const CHOP_InputArrays* inputs,
								void* reserved)
{
	myExecuteCount++;

	int reset = inputs->floatInputs[0].values[0];
	//FlexSys->substeps = inputs->floatInputs[0].values[1];  2

	if(reset==1)
		FlexSys->initDam();

	FlexSys->gravity.x = inputs->floatInputs[1].values[0];
	FlexSys->gravity.y = inputs->floatInputs[1].values[1];
	FlexSys->gravity.z = inputs->floatInputs[1].values[2];

	FlexSys->radius = inputs->floatInputs[2].values[0];

	FlexSys->boxWidth.x = inputs->floatInputs[3].values[0];
	FlexSys->boxWidth.y = inputs->floatInputs[3].values[1];
	FlexSys->boxWidth.z = inputs->floatInputs[3].values[2];

	FlexSys->boxPos.x = inputs->floatInputs[4].values[0];
	FlexSys->boxPos.y = inputs->floatInputs[4].values[1];
	FlexSys->boxPos.z = inputs->floatInputs[4].values[2];


	//for (int i = 0; i < FlexSys->activeParticles; i++){


	//for (int i = 0; i < g_positions.size(); i++) {

	if (FlexSys->g_flex){

		FlexSys->updateDam();

		//int osize = output->length;
		activeCount = FlexSys->activeParticles;
		maxParticles = FlexSys->maxParticles;

		positionsSize = FlexSys->g_positions.size();
		activeIndicesSize = FlexSys->g_activeIndices.size();

		for (int i = 0; i < output->length; i++) {
		//for (int i = 0; i < activeCount; i++) {

					//currSys->particles[i].update(i, maxParts);


			/*output->channels[TX][i] = FlexSys->g_positions[i].x;
						output->channels[TY][i] = FlexSys->g_positions[i].y;
						output->channels[TZ][i] = FlexSys->g_positions[i].z;

						output->channels[VX][i] = FlexSys-> g_velocities[i].x;
						output->channels[VY][i] = FlexSys->g_velocities[i].y;
						output->channels[VZ][i] = FlexSys->g_velocities[i].z;

						output->channels[SPEED][i] = FlexSys->g_velocities[i].x*FlexSys->g_velocities[i].x
													+ FlexSys->g_velocities[i].y*FlexSys->g_velocities[i].y
													+ FlexSys->g_velocities[i].z*FlexSys->g_velocities[i].z;*/

			output->channels[TX][i] = FlexSys->g_positions[FlexSys->g_activeIndices[i]].x;
			output->channels[TY][i] = FlexSys->g_positions[FlexSys->g_activeIndices[i]].y;
			output->channels[TZ][i] = FlexSys->g_positions[FlexSys->g_activeIndices[i]].z;

			output->channels[VX][i] = FlexSys-> g_velocities[FlexSys->g_activeIndices[i]].x;
			output->channels[VY][i] = FlexSys->g_velocities[FlexSys->g_activeIndices[i]].y;
			output->channels[VZ][i] = FlexSys->g_velocities[FlexSys->g_activeIndices[i]].z;

			output->channels[SPEED][i] = FlexSys->g_velocities[FlexSys->g_activeIndices[i]].x*FlexSys->g_velocities[FlexSys->g_activeIndices[i]].x
										+ FlexSys->g_velocities[FlexSys->g_activeIndices[i]].y*FlexSys->g_velocities[FlexSys->g_activeIndices[i]].y
										+ FlexSys->g_velocities[FlexSys->g_activeIndices[i]].z*FlexSys->g_velocities[FlexSys->g_activeIndices[i]].z;

		}

	}

}

int
FlexFluidCHOP::getNumInfoCHOPChans()
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 5;
}

void
FlexFluidCHOP::getInfoCHOPChan(int index,
										CHOP_InfoCHOPChan *chan)
{
	// This function will be called once for each channel we said we'd want to return
	// In this example it'll only be called once.

	/*if (index == 0)
	{
		chan->name = "executeCount";
		chan->value = myExecuteCount;
	}*/

	switch(index){

	case 0:
		chan->name = "executeCount";
		chan->value = myExecuteCount;
		break;

	case 1:
		chan->name = "maxParticles";
		chan->value = maxParticles;
		break;

	case 2:
		chan->name = "activeCount";
		chan->value = activeCount;
		break;

	case 3:
		chan->name = "positionsSize";
		chan->value = positionsSize;
		break;

	case 4:
		chan->name = "activeIndicesSize";
		chan->value = activeIndicesSize;
		break;

	}
}

bool		
FlexFluidCHOP::getInfoDATSize(CHOP_InfoDATSize *infoSize)
{
	infoSize->rows = 1;
	infoSize->cols = 2;
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void
FlexFluidCHOP::getInfoDATEntries(int index,
										int nEntries,
										CHOP_InfoDATEntries *entries)
{
	if (index == 0)
	{
		// It's safe to use static buffers here because Touch will make it's own
		// copies of the strings immediately after this call returns
		// (so the buffers can be reuse for each column/row)
		static char tempBuffer1[4096];
		static char tempBuffer2[4096];
		// Set the value for the first column
		strcpy(tempBuffer1, "executeCount");
		entries->values[0] = tempBuffer1;

		// Set the value for the second column
		sprintf(tempBuffer2, "%d", myExecuteCount);
		entries->values[1] = tempBuffer2;
	}
}
