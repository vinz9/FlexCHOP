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
class FlexFluidCHOP : public CHOP_CPlusPlusBase
{
public:
	FlexFluidCHOP(const CHOP_NodeInfo *info);
	virtual ~FlexFluidCHOP();

	virtual void		getGeneralInfo(CHOP_GeneralInfo *);
	virtual bool		getOutputInfo(CHOP_OutputInfo*);
	virtual const char*	getChannelName(int index, void* reserved);

	virtual void		execute(const CHOP_Output*,
								const CHOP_InputArrays*,
								void* reserved);


	virtual int			getNumInfoCHOPChans();
	virtual void		getInfoCHOPChan(int index,
										CHOP_InfoCHOPChan *chan);

	virtual bool		getInfoDATSize(CHOP_InfoDATSize *infoSize);
	virtual void		getInfoDATEntries(int index,
											int nEntries,
											CHOP_InfoDATEntries *entries);
private:

	// We don't need to store this pointer, but we do for the example.
	// The CHOP_NodeInfo class store information about the node that's using
	// this instance of the class (like its name).
	const CHOP_NodeInfo		*myNodeInfo;

	// In this example this value will be incremented each time the execute()
	// function is called, then passes back to the CHOP 
	int						 myExecuteCount;


	FlexSystem* FlexSys;

	int activeCount;
	int maxParticles;

	int positionsSize;
	int activeIndicesSize;


	vector<Vec3> sourcePos;
	vector<Vec3> sourceDir;

	enum { TX, TY, TZ, VX, VY, VZ, SPEED };

};
