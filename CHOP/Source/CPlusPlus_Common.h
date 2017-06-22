/* Shared Use License: This file is owned by Derivative Inc. (Derivative) and
 * can only be used, and/or modified for use, in conjunction with 
 * Derivative's TouchDesigner software, and only if you are a licensee who has
 * accepted Derivative's TouchDesigner license or assignment agreement (which
 * also govern the use of this file).  You may share a modified version of this
 * file with another authorized licensee of Derivative's TouchDesigner software.
 * Otherwise, no redistribution or sharing of this file, with or without
 * modification, is permitted.
 */

/*
 * Produced by:
 *
 * 				Derivative Inc
 *				401 Richmond Street West, Unit 386
 *				Toronto, Ontario
 *				Canada   M5V 3A8
 *				416-591-3555
 *
 * NAME:				CPlusPlus_Common.h 
 *
 */

/*******
	Derivative Developers:: Make sure the virtual function order
	stays the same, otherwise changes won't be backwards compatible
********/


#ifndef __CPlusPlus_Common
#define __CPlusPlus_Common

#define NOMINMAX

#ifdef WIN32
	#include <windows.h>
	#include <stdint.h>
	#include "GL_Extensions.h"
	#define DLLEXPORT __declspec (dllexport)
#else
	#include <OpenGL/gltypes.h>
	#define DLLEXPORT
#endif

struct cudaArray;

enum class OP_CPUMemPixelType : int32_t
{
	// 8-bit per color, BGRA pixels. This is preferred for 4 channel 8-bit data
	BGRA8Fixed = 0,
	// 8-bit per color, RGBA pixels. Only use this one if absolutely nesseary.
	RGBA8Fixed,
	// 32-bit float per color, RGBA pixels
	RGBA32Float,

	// Single and double channel options
	// Fixed
	R8Fixed,
	RG8Fixed,
	// Float
	R32Float,
	RG32Float,
};


class OP_NodeInfo
{
public:
	// The full path to the operator

	const char*		opPath;

	// A unique ID representing the operator, no two operators will ever
	// have the same ID in a single TouchDesigner instance.

	uint32_t		opID;

	// This is the handle to the main TouchDesigner window.
	// It's possible this will be 0 the first few times the operator cooks,
	// incase it cooks while TouchDesigner is still loading up

#ifdef WIN32
	HWND			mainWindowHandle;
#endif

private:
	int32_t			reserved[19];
};


class OP_DATInput
{
public:
	const char*		opPath;
	uint32_t		opId;

	int32_t         numRows;
	int32_t         numCols;
	bool            isTable;

	// data, referenced by (row,col), which will be a const char* for the
	// contents of the cell
	// E.g getCell(1,2) will be the contents of the cell located at (1,2)

	const char*		getCell(int32_t row, int32_t col) const
					{
						return cellData[row * numCols + col];
					}

protected:
	const char**	cellData;

private:
	int32_t         reserved[20];
};

class OP_TOPInput
{
public:
	const char*		opPath;
	uint32_t		opId;

	int32_t			width;
	int32_t			height;


	// The OpenGL Texture index for this TOP.
	// This is only valid when accessed from C++ TOPs.
	// Other C++ OPs will have this value set to 0 (invalid).
	GLuint			textureIndex;

	// The OpenGL Texture target for this TOP.
	// E.g GL_TEXTURE_2D, GL_TEXTURE_CUBE,
	// GL_TEXTURE_2D_ARRAY
	GLenum			textureType;

	// Depth for 3D and 2D_ARRAY textures, undefined
	// for other texture types
	uint32_t		depth;

	// contains the internalFormat for the texture
	// such as GL_RGBA8, GL_RGBA32F, GL_R16
	GLint			pixelFormat;

	// reserved
	int32_t			reserved1;

	// When the TOP_ExecuteMode is CUDA, this will be filled in
	cudaArray*		cudaInput;

private:
	int32_t			reserved[16];
};


class OP_CHOPInput
{
public:

	const char*		opPath;
	uint32_t		opId;

	int32_t			numChannels;
	int32_t			numSamples;
	double			sampleRate;
	double			startIndex;



	// Retrieve a float array for a specific channel.
	// 'i' ranges from 0 to numChannels-1
	// The returned arrray contains 'numSamples' samples.
	// e.g: getChannelData(1)[10] will refer to the 11th sample in the 2nd channel 

	const float*	getChannelData(int32_t i) const
					{
						return channelData[i];
					}


	// Retrieve the name of a specific channel.
	// 'i' ranges from 0 to numChannels-1
	// For example getChannelName(1) is the name of the 2nd channel

	const char*		getChannelName(int32_t i) const
					{
						return nameData[i];
					}

protected:

	const float**	channelData;
	const char**	nameData;


private:

	int32_t			reserved[20];
};

class OP_ObjectInput
{
public:

	const char*		opPath;
	uint32_t		opId;

	// Use these methods to calculate object transforms
	double			worldTransform[4][4];
	double			localTransform[4][4];


private:

	int32_t			reserved[20];
};


enum class OP_TOPInputDownloadType : int32_t
{
	// The texture data will be downloaded and and available on the next frame.
	// Except for the first time this is used, getTOPDataInCPUMemory()
	// will return the texture data on the CPU from the previous frame.
	// The first getTOPDataInCPUMemory() is called it will be nullptr.
	// ** This mode should be used is most cases for performance reasons **
	Delayed = 0,

	// The texture data will be downloaded immediately and be available
	// this frame. This can cause a large stall though and should be avoided
	// in most cases
	Instant,
};

class OP_TOPInputDownloadOptions
{
public:
	OP_TOPInputDownloadOptions()
	{
		downloadType = OP_TOPInputDownloadType::Delayed;
		verticalFlip = false;
		cpuMemPixelType = OP_CPUMemPixelType::BGRA8Fixed;
	}

	OP_TOPInputDownloadType	downloadType;

	// Set this to true if you want the image vertically flipped in the
	// downloaded data
	bool					verticalFlip;

	// Set this to how you want the pixel data to be give to you in CPU
	// memory. BGRA8Fixed should be used for 4 channel 8-bit data if possible
	OP_CPUMemPixelType		cpuMemPixelType;

};


class OP_Inputs
{
public:
	// NOTE: When writting a TOP, none of these functions should
	// be called inside a beginGLCommands()/endGLCommands() section
	// as they may require GL themselves to complete execution.

	// these are wired into the node
	virtual int32_t		getNumInputs() = 0;

	// may return nullptr when invalid input
	virtual const OP_TOPInput*		getInputTOP(int32_t index) = 0;	// only valid for TOP operators
	virtual const OP_CHOPInput*		getInputCHOP(int32_t index) = 0;	// only valid for CHOP operators

	// these are defined by parameters.
	// may return nullptr when invalid input
	// this value is valid until the parameters are rebuilt or it is called with the same parameter name.
	virtual const OP_DATInput*		getParDAT(const char *name) = 0;
	virtual const OP_TOPInput*		getParTOP(const char *name) = 0;
	virtual const OP_CHOPInput*		getParCHOP(const char *name) = 0;
	virtual const OP_ObjectInput*	getParObject(const char *name) = 0;

	// these work on any type of parameter and can be interchanged
	// for menu types, int returns the menu selection index, string returns the item

	// returns the requested value, index may be 0 to 4.
	virtual double		getParDouble(const char* name, int32_t index=0) = 0;

	// for multiple values: returns True on success/false otherwise
	virtual bool        getParDouble2(const char* name, double &v0, double &v1) = 0;
	virtual bool        getParDouble3(const char* name, double &v0, double &v1, double &v2) = 0;
	virtual bool        getParDouble4(const char* name, double &v0, double &v1, double &v2, double &v3) = 0;


	// returns the requested value
	virtual int32_t		getParInt(const char* name, int32_t index=0) = 0;

	// for multiple values: returns True on success/false otherwise
	virtual bool        getParInt2(const char* name, int32_t &v0, int32_t &v1) = 0;
	virtual bool        getParInt3(const char* name, int32_t &v0, int32_t &v1, int32_t &v2) = 0;
	virtual bool        getParInt4(const char* name, int32_t &v0, int32_t &v1, int32_t &v2, int32_t &v3) = 0;

	// returns the requested value
	// this value is valid until the parameters are rebuilt or it is called with the same parameter name.
	virtual const char*	getParString(const char* name) = 0;	// return value usable for life of parameter

	// this is similar to getParString, but will return an absolute path if it exists, with
	// slash direction consistent with O/S requirements.
	// to get the original parameter value, use getParString
	virtual const char*	getParFilePath(const char* name) = 0;	// return value usable for life of parameter

	// returns true on success
	// from_name and to_name must be Object parameters
	virtual bool	getRelativeTransform(const char* from_name, const char* to_name, double matrix[4][4]) = 0;


	// disable or enable updating of the parameter
	virtual void		 enablePar(const char* name, bool onoff) = 0;


	// these are defined by paths.
	// may return nullptr when invalid input
	// this value is valid until the parameters are rebuilt or it is called with the same parameter name.
	virtual const OP_DATInput*		getDAT(const char *path) = 0;
	virtual const OP_TOPInput*		getTOP(const char *path) = 0;
	virtual const OP_CHOPInput*		getCHOP(const char *path) = 0;
	virtual const OP_ObjectInput*	getObject(const char *path) = 0;


	// This function can be used to retrieve the TOPs texture data in CPU
	// memory. You must pass the OP_TOPInput object you get from 
	// getParTOP/getInputTOP into this, not a copy you've made
	//
	// Fill in a OP_TOPIputDownloadOptions class with the desired options set
	//
	// Returns the data, which will be valid until the end of execute()
	// Returned value may be nullptr in some cases, such as the first call
	// to this with options->downloadType == OP_TOP_DOWNLOAD_DELAYED.
	virtual void* 					getTOPDataInCPUMemory(const OP_TOPInput *top,
													const OP_TOPInputDownloadOptions *options) = 0;
													
};

class OP_InfoCHOPChan
{
public:
	const char*		name;
	float			value;
private:
	int32_t			reserved[10];
};

class OP_InfoDATSize
{
public:
	
	// Set this to the size you want the table to be

	int32_t			rows;
	int32_t			cols;

	// Set this to true if you want to return DAT entries on a column
	// by column basis.
	// Otherwise set to false, and you'll be expected to set them on
	// a row by row basis.
	// DEFAULT : false

	bool			byColumn;


private:
	int32_t			reserved[10];
};

class OP_InfoDATEntries
{
public:

	// This is an array of char* pointers which you are expected to assign
	// The start off as NULL, you need to allocate or assign constant/statis
	// strings to them
	// e.g values[1] = "myColumnName";

	char**			values;

private:
	int32_t			reserved[10];
};



class OP_NumericParameter
{
public:

	OP_NumericParameter(const char* iname=nullptr)
	{
		name = iname;
		label = page = nullptr;

		for (int i=0; i<4; i++)
		{
			defaultValues[i] = 0.0;

			minSliders[i] = 0.0;
			maxSliders[i] = 1.0;

			minValues[i] = 0.0;
			maxValues[i] = 1.0;

			clampMins[i] = false;
			clampMaxes[i] = false;
		}
	}

	// Any char* values passed are copied immediately by the append parameter functions,
	// and do not need to be retained by the calling function.
	const char*	name;		// Must begin with capital letter, and contain no spaces
	const char*	label;
	const char*	page;

	double		defaultValues[4];
	double		minValues[4];
	double		maxValues[4];

	bool		clampMins[4];
	bool		clampMaxes[4];

	double		minSliders[4];
	double		maxSliders[4];

private:

	int32_t		reserved[20];

};

class OP_StringParameter
{
public:

	OP_StringParameter(const char* iname=nullptr)
	{
		name = iname;
		label = page = nullptr;
		defaultValue = nullptr;
	}

	// Any char* values passed are copied immediately by the append parameter functions,
	// and do not need to be retained by the calling function.

	const char*	name;		// Must begin with capital letter, and contain no spaces
	const char*	label;
	const char*	page;

	const char*	defaultValue;

private:

	int32_t		reserved[20];
};


enum class OP_ParAppendResult : int32_t
{
	Success = 0,
	InvalidName,	// invalid or duplicate name
	InvalidSize,	// size out of range
};


class OP_ParameterManager
{

public:

	// Returns PARAMETER_APPEND_SUCCESS on succesful

	virtual OP_ParAppendResult		appendFloat(const OP_NumericParameter &np, int32_t size=1) = 0;
	virtual OP_ParAppendResult		appendInt(const OP_NumericParameter &np, int32_t size=1) = 0;

	virtual OP_ParAppendResult		appendXY(const OP_NumericParameter &np) = 0;
	virtual OP_ParAppendResult		appendXYZ(const OP_NumericParameter &np) = 0;

	virtual OP_ParAppendResult		appendUV(const OP_NumericParameter &np) = 0;
	virtual OP_ParAppendResult		appendUVW(const OP_NumericParameter &np) = 0;

	virtual OP_ParAppendResult		appendRGB(const OP_NumericParameter &np) = 0;
	virtual OP_ParAppendResult		appendRGBA(const OP_NumericParameter &np) = 0;

	virtual OP_ParAppendResult		appendToggle(const OP_NumericParameter &np) = 0;
	virtual OP_ParAppendResult		appendPulse(const OP_NumericParameter &np) = 0;

	virtual OP_ParAppendResult		appendString(const OP_StringParameter &sp) = 0;
	virtual OP_ParAppendResult		appendFile(const OP_StringParameter &sp) = 0;
	virtual OP_ParAppendResult		appendFolder(const OP_StringParameter &sp) = 0;

	virtual OP_ParAppendResult		appendDAT(const OP_StringParameter &sp) = 0;
	virtual OP_ParAppendResult		appendCHOP(const OP_StringParameter &sp) = 0;
	virtual OP_ParAppendResult		appendTOP(const OP_StringParameter &sp) = 0;
	virtual OP_ParAppendResult		appendObject(const OP_StringParameter &sp) = 0;

	// Any char* values passed are copied immediately by the append parameter functions,
	// and do not need to be retained by the calling function.
	virtual OP_ParAppendResult		appendMenu(const OP_StringParameter &sp,
									int32_t nitems, const char **names,
									const char **labels) = 0;

	// Any char* values passed are copied immediately by the append parameter functions,
	// and do not need to be retained by the calling function.
	virtual OP_ParAppendResult		appendStringMenu(const OP_StringParameter &sp,
									int32_t nitems, const char **names,
									const char **labels) = 0;

private:


};

#endif
