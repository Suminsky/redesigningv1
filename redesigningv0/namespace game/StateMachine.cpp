#include "StateMachine.h"
#include "State.h"

using namespace game;


game::StateMachine::StateMachine() :m_pCurrentState(nullptr), m_pToBeChangedTo_Tmp(nullptr)
{

}

game::StateMachine::~StateMachine()
{
	if( m_pCurrentState ){

		m_pCurrentState->Destroy();
	}
}

void game::StateMachine::Update( const double dDeltaTime_p )
{
	if( m_pCurrentState ){

		m_pCurrentState->Update( dDeltaTime_p );
	}
}

void game::StateMachine::Draw( const double dInterpolation_p )
{
	if( m_pCurrentState ){

		m_pCurrentState->Draw( dInterpolation_p );
	}
}

void game::StateMachine::ChangeState( const shared_State_ptr & pNewState_p )
{
	// TODO: warning if already set to change

	m_pToBeChangedTo_Tmp = pNewState_p;
}

void game::StateMachine::ChangeState( shared_State_ptr && pNewState_p )
{
	m_pToBeChangedTo_Tmp = std::move(pNewState_p);
}

void game::StateMachine::OnResize( int W_p, int H_p )
{
	if( m_pCurrentState ){

		m_pCurrentState->OnResize( W_p, H_p );
	}
}

void game::StateMachine::ResolveStateChange()
{
	if( !m_pToBeChangedTo_Tmp ) return;

	// destroy current state, if any

	if( m_pCurrentState ){

		m_pCurrentState->Destroy();
	}

	// assign new state

	m_pCurrentState = std::move( m_pToBeChangedTo_Tmp );
	m_pToBeChangedTo_Tmp.reset();

	// init new state, if any

	if( m_pCurrentState ){

		m_pCurrentState->VOnInit(); // TODO: give old state to new state
	}
}

void game::StateMachine::Destroy()
{
	if( m_pCurrentState ){

		m_pCurrentState->Destroy();
		m_pCurrentState = nullptr;
	}
}
