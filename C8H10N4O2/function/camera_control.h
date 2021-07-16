#pragma once
#include "Arithmetic.h"
#include "SceneConstantBuffer.h"


class CameraControl
{
private:
	FLOAT3 pos;
	FLOAT3 traget;
	FLOAT3 up_vector;

	FLOAT fov;
	FLOAT zfar;
	FLOAT znear;
	FLOAT width;
	FLOAT height;
public:
	CameraControl();

	FLOAT3* getPos()	{ return &pos; }
	FLOAT3* getTraget()	{ return &traget; }
	FLOAT3* getUp()		{ return &up_vector; }

	FLOAT* getFov()		{ return &fov; }
	FLOAT* getFar()		{ return &zfar; }
	FLOAT* getNear()	{ return &znear; }
	FLOAT* getWidth()	{ return &width; }
	FLOAT* getHeight()	{ return &height; }

	FLOAT4X4 getView()const;
	FLOAT4X4 getProjection()const;
	FLOAT4X4 getOrthographic()const;



};

