#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2015/07/12
	created:	12:7:2015   2:27
	file:		SortMask.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes

namespace sprite{

	union SortMask{

		unsigned __int64 intRepresentation;

		struct{

			// least sig bit
			unsigned __int64 textureID		: 15;
			unsigned __int64 shaderID		: 10;
			unsigned __int64 blending		: 2;	// 4 modes: opaque, blended, additive, subtractive...
			unsigned __int64 Zdepth			: 24;	// 0 - 16 777 216 drawables depth range
			unsigned __int64 viewportLayer	: 8;	// 256 viewport layers: skybox, world, fx, HUD, parallax...
			unsigned __int64 viewport		: 3;	// 8 viewports: split screens, portals, mirrors...
			unsigned __int64 layer			: 2;	// 4 layers: game, HUD, full screen effect...
			// most sig bit

		}bitfield;
	};

	//TODO: add one for samplerID
	//TODO: depth test/write for opaques, depth test/no write for alpha blended and additive
	//TODO: if not ~transparent, the sorting should be by shader and texture before Zdepth
	//TODO: current, bigger values are drawn first, so the zdepth is contrary from the actual
	// viewport z culling (near 0 far 1)
	// TODO: on the draw(), update the key from the camera, (viewport and layer stuff should be info holded
	// by the camera).
}