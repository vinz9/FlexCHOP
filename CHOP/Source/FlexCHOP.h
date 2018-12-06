#include "CHOP_CPlusPlusBase.h"

#include "FlexSystem.h"

using namespace std;

/*

This example file implements a class that does 2 different things depending on
if a CHOP is connected to the CPlusPlus CHOPs input or not.
The example is timesliced, which is the more complex way of working.

If an input is connected the node will output the same number of channels as the
input and divide the first 'N' samples in the input channel by 2. 'N' being the current
timeslice size. This is noteworthy because if the input isn't changing then the output
will look wierd since depending on the timeslice size some number of the first samples
of the input will get used.

If no input is connected then the node will output a smooth sine wave at 120hz.
*/


// To get more help about these functions, look at CHOP_CPlusPlusBase.h
class FlexCHOP : public CHOP_CPlusPlusBase
{
public:
	FlexCHOP(const OP_NodeInfo *info);
	virtual ~FlexCHOP();

	virtual void		getGeneralInfo(CHOP_GeneralInfo*, const OP_Inputs *inputs, void* reserved1) override;
	virtual bool		getOutputInfo(CHOP_OutputInfo*, const OP_Inputs *inputs, void *reserved1)override;
	virtual void		getChannelName(int32_t index, OP_String *name,
									const OP_Inputs *inputs, void* reserved1) override;

	virtual void		execute(CHOP_Output* outputs,
		const OP_Inputs* inputs,
		void* reserved1) override;


	virtual int32_t		getNumInfoCHOPChans(void *reserved1) override;
	virtual void		getInfoCHOPChan(int32_t index, OP_InfoCHOPChan* chan, void* reserved1) override;

	virtual bool		getInfoDATSize(OP_InfoDATSize* infoSize, void *reserved1) override;
	virtual void		getInfoDATEntries(int32_t index, int32_t nEntries,
											OP_InfoDATEntries* entries,
											void *reserved1) override;

	void updateParams(const OP_Inputs* inputs);

	virtual void		setupParameters(OP_ParameterManager* manager, void* reserved1) override;
	//virtual void		pulsePressed(const char* name) override;

	float maxVel;

	int maxParticles;
	int activeIndicesSize;
	int inactiveIndicesSize;

	float timer;

private:

	// We don't need to store this pointer, but we do for the example.
	// The CHOP_NodeInfo class store information about the node that's using
	// this instance of the class (like its name).
	const OP_NodeInfo		*myNodeInfo;

	// In this example this value will be incremented each time the execute()
	// function is called, then passes back to the CHOP 
	int						 myExecuteCount;


	FlexSystem* FlexSys;



	//enum { TX, TY, TZ, VX, VY, VZ };
	enum { TX, TY, TZ, VX, VY, VZ, Q1X, Q1Y, Q1Z, Q1W, Q2X, Q2Y, Q2Z, Q2W, Q3X, Q3Y, Q3Z, Q3W };

};
