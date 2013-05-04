#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/04
	created:	4:5:2013   13:53
	file:		Timer.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	a timer. can be pause. updates from a system gathered clock

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes

namespace game{

	class State;

template< typename ClockType >
class Timer{

	friend class State;

	float m_fSpeed;
	ClockType m_deltaTime;
	ClockType m_accumTime;

	bool m_bPaused;

public:

	//------------------------------------------------------------------------
	// ctor
	//------------------------------------------------------------------------
	Timer():m_fSpeed(1.0f), m_bPaused(false){}
	//------------------------------------------------------------------------
	// dctor
	//------------------------------------------------------------------------
	virtual ~Timer(){}

	//------------------------------------------------------------------------
	// call this to update time
	//------------------------------------------------------------------------
	void Update( ClockType delta_p ){

		if( !m_bPaused ){

			delta_p *= m_fSpeed;

			m_deltaTime = delta_p;
			m_accumTime += delta_p;
		}
	}

	//------------------------------------------------------------------------
	// pause funcs
	//------------------------------------------------------------------------
	void Pause(){

		m_bPaused = true;
	}
	void Unpause(){

		m_bPaused = false;
	}
	void PauseToogle(){

		if( m_bPaused ) m_bPaused = false;
		else m_bPaused = true;
	}

	//------------------------------------------------------------------------
	// getters
	//------------------------------------------------------------------------
	ClockType GetDelta(){ return m_deltaTime;}
	ClockType GetTime(){ return m_accumTime; }
};
}