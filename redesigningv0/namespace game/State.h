#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/12
	created:	12:4:2013   20:21
	file:		State.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	A game state.
				I.e. menu, level 1, level 2, map screen, game over, etc.
				NOTE: I changed from iterators to int it, so that I can add layers
				any time I want without worrying about invalid iterators.
				This way I can add and new layers will only be updated next loop
				automatically.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#include <memory>
#include <vector>

#include "Timer.h"
#include "Layer.h"

namespace game{

	class StateMachine;

	typedef std::vector<shared_Layer_ptr> StateLayers;
	typedef std::vector<LAYERINDEX> LayerIndexes;

	//========================================================================
	// 
	//========================================================================
	class State{

		friend StateMachine;

	public:

		Timer<double> m_timer;

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~State();

		//------------------------------------------------------------------------
		// layer stuff
		//------------------------------------------------------------------------
		void AddLayer( shared_Layer_ptr && pNewLayer_p );
		void AddLayer( const shared_Layer_ptr & pNewLayer_p );
		void RemoveLayer( LAYERINDEX layerCurrentIndex_p );
		void RemoveLayer(  const shared_Layer_ptr & pNewLayer_p  );
		void RemoveLayer(  Layer * pNewLayer_p  );

	private:

		//------------------------------------------------------------------------
		// updates state timer and call updates with timer time, for each layer
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void OnResize( int W_p, int H_p);

		//------------------------------------------------------------------------
		// calls all layers VOnDestroy than state VOnDestroy
		//------------------------------------------------------------------------
		void Destroy();

		//------------------------------------------------------------------------
		// traverse layer and call draw
		//------------------------------------------------------------------------
		void Draw( const double dInterpolation_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void CleanRemovedLayers();


		StateLayers m_layers;
		//LayerIndexes m_removedLayers;
		std::vector<Layer*> m_removedLayers;

		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VOnInit();
		virtual void VOnUpdate(double, double);	// called before layers update
		virtual void VLateUpdate(double, double);	// called after layers update
		virtual void VOnDraw();					// called after layers draw (why I took interp param off?)
		virtual void VOnDestroy();

		virtual void VOnResize();
	};

	typedef std::shared_ptr<State> shared_State_ptr;
	typedef std::weak_ptr<State> weak_State_ptr;
}
