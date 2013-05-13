#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/06
	created:	6:5:2013   9:31
	file:		Mapper.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	maps raw input to actions/states/ranges
				I dunno from where to start..
				goals:
				- user register device he wants handle input from (keyboard, mouse, game pad..)
				- user register his actions, states and ranges to the corresponding raw input
				- user can remap raw to actions/states/ranges any time
				- user can assign multiple raws to the same actions/states/ranges
				- user can create different sets of mappings, turning multiple on if he wants
				some details I believe be the best way:
				- raw input is provided under my engine side as enums..a value for each of them all ._.
				- actions, states and ranges are provided as...NO IDEA
				- user pool for actions, ranges and states..callback is for pussies

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes

namespace input{

	class Mapper{

	};
}