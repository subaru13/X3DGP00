#pragma once
#include "Arithmetic.h"
#include <Windows.h>

class CameraControl
{
private:
	FLOAT3 pos;
	FLOAT3 traget;
	FLOAT3 up_vector;

	float fov;
	float zfar;
	float znear;
	float width;
	float height;

	POINT l_fulcrum;
	POINT r_fulcrum;
	POINT c_fulcrum;
	FLOAT3 attitude;
public:
	CameraControl();

	FLOAT3* getPos()	{ return &pos; }
	FLOAT3* getTraget()	{ return &traget; }
	FLOAT3* getUp()		{ return &up_vector; }

	float* getFov()		{ return &fov; }
	float* getFar()		{ return &zfar; }
	float* getNear()	{ return &znear; }
	float* getWidth()	{ return &width; }
	float* getHeight()	{ return &height; }

	FLOAT4X4 getView()const;
	FLOAT4X4 getProjection()const;
	FLOAT4X4 getOrthographic()const;

	/// <summary>
	/// カメラの更新をします。
	/// </summary>
	/// <param name="move_speed">移動速度</param>
	/// <param name="rotation_speed">角速度</param>
	/// <param name="elapsed_time">経過時間</param>
	void update(float move_speed, float rotation_speed, float elapsed_time);

};