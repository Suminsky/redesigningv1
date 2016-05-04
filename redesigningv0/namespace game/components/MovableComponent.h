#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2014/10/04
	created:	4:10:2014   19:13
	file:		MovableComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	Requires a TransformComponent attached to the object.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <DirectXMath.h>

// private includes
#include "../../namespace gen/gen_data.h"

#include "../Component.h"
#include "../ComponentFactory.h"
#include "../Event.h"

#include "TransformComponent.h"

namespace game{

	class MovableComponent: public game::Component{

		friend class MovableSystem;
		friend class MovableFactory;

	public:

		struct config{

			float fAccel;
			float fMaxVel;
			float fDeccel;
			float fVelStopThreshold;
			float fGravity;
			float fJumpSpeed;
			float fMaxFallVel;
			float fAirDeccel;
		};

		struct state{

			bool bGrounded;
			DirectX::XMFLOAT3 vVel;
			DirectX::XMFLOAT3 vFaceDir;
			DirectX::XMFLOAT3 vMoveDir;
		};

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		MovableComponent(){

			m_type = COMPONENT_TYPE(MovableComponent);
		}

		void Initialize( config && config_p, DirectX::XMFLOAT3 && vDefFaceDir_p ){

			m_config = config_p;

			memset( &m_state, 0, sizeof(state));
			m_state.vFaceDir = vDefFaceDir_p;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Jump(){

			m_state.vVel.y += m_config.fJumpSpeed;

			m_state.bGrounded = false;
		}
		void JumpRelease(){

			if( m_state.vVel.y >= m_config.fJumpSpeed*0.5 )
				m_state.vVel.y = m_config.fJumpSpeed*0.5f;
		}

		//------------------------------------------------------------------------
		// TODO:move to sys
		//------------------------------------------------------------------------
		void Update(float delta){

			if(delta == 0.0f) return;

			// horz handling

			float currMaxVel = m_config.fMaxVel;
			float velDir = m_state.vVel.x > 0 ? 1.0f : -1.0f;
			float currDeccel = m_state.bGrounded ? m_config.fDeccel : m_config.fAirDeccel;

			if( m_state.vMoveDir.x == 0.0f ){

				// not moving, deccelerate, watch out to no go to the other dir (trough zero)

				m_state.vVel.x -= gen::Min( currDeccel * delta, abs(m_state.vVel.x) )*velDir;

				if( abs(m_state.vVel.x) < m_config.fVelStopThreshold )
					m_state.vVel.x = 0.0f;
			}
			else{

				// moving, accelerate

				currMaxVel *= abs(m_state.vMoveDir.x);

				// if moving against current vel
				if( (m_state.vVel.x > 0) != (m_state.vMoveDir.x > 0) ){

					m_state.vVel.x += m_config.fAccel * delta * m_state.vMoveDir.x;

					// do not clamp, cause its accelerating on the other direction, it will naturally
					// "break" (deccel till changes direction), but need to avoid changing dir and go straight
					// to trhough the max
				
					if(  (m_state.vVel.x > 0) != (velDir > 0) ){

						// changed dir (this is REALLY unlike to happen,
						// except in the transition from ~0 to the other dir), wich means it rarely will
						// trigger the clamp bellow

						if( abs(m_state.vVel.x) > abs(currMaxVel) )
							m_state.vVel.x = currMaxVel*-velDir;
					}
				}
				// if not at max
				else if( abs(m_state.vVel.x) <= currMaxVel	){

					m_state.vVel.x += m_config.fAccel * delta * m_state.vMoveDir.x;
				
					// clamp:

					if( abs(m_state.vVel.x) > abs(currMaxVel) )m_state.vVel.x = currMaxVel*velDir;

				}
				else{
				
					// moving at the same dir but beyond max vel

					// deccel towards max

					m_state.vVel.x -= currDeccel * delta *velDir;
					
					// clamp:
				
					if( abs(m_state.vVel.x) <= currMaxVel )	m_state.vVel.x = currMaxVel * velDir;
				}
			}


			// vert handling

			// update gravity effects

			float fGravityStep = m_config.fGravity * delta;

			m_state.vVel.y -= fGravityStep;

			if( m_state.vVel.y < -m_config.fMaxFallVel ) m_state.vVel.y = -m_config.fMaxFallVel;

			//

			pTrafoRef->m_local.position.x += m_state.vVel.x * delta;
			pTrafoRef->m_local.position.y += m_state.vVel.y * delta;
			pTrafoRef->m_local.position.z += m_state.vVel.z * delta;

			m_state.bGrounded = false; // should be set to true on collision solving
		}
		void Update3(float delta){

			if(delta == 0.0f) return;

			// horz handling

		
			DirectX::XMFLOAT3 xzVel = m_state.vVel;
			xzVel.y = 0.0f;
			DirectX::XMVECTOR vVel = DirectX::XMLoadFloat3(&xzVel);
			DirectX::XMVECTOR vvelDir =  DirectX::XMVector3Normalize( vVel );
			DirectX::XMFLOAT3 velDir; DirectX::XMStoreFloat3(&velDir,vvelDir);
			float currDeccel = m_state.bGrounded ? m_config.fDeccel : m_config.fAirDeccel;

			if( m_state.vMoveDir.x == 0.0f
				&&
				m_state.vMoveDir.z == 0.0f ){

				// not moving, deccelerate, watch out to no go to the other dir (trough zero)

				float dirX = (DirectX::XMVectorGetX(vvelDir));
				float dirZ = (DirectX::XMVectorGetZ(vvelDir));
				m_state.vVel.x -= gen::Min( currDeccel * delta * dirX, abs(m_state.vVel.x) );
				m_state.vVel.z -= gen::Min( currDeccel * delta * dirZ, abs(m_state.vVel.z) );

				float speed = DirectX::XMVectorGetX(DirectX::XMVector3Length(vVel));
				if( speed < m_config.fVelStopThreshold ){
					m_state.vVel.x = 0.0f;
					m_state.vVel.x = 0.0f;	
				}
			}
			else{

				// moving, accelerate

				auto aa = []( const DirectX::XMFLOAT3 & s, unsigned int it )-> float &{
					// aa stands for as array
					return ((float*)&s)[it];
				};

				for( int it = 0; it < 3; it+=2 ){
			
					float currMaxVel = m_config.fMaxVel;
					currMaxVel *= abs(aa(m_state.vMoveDir, it));

					// if moving against current vel
					if( (aa(m_state.vVel, it) > 0) != (aa(m_state.vMoveDir, it) > 0) ){

						aa(m_state.vVel, it) += m_config.fAccel * delta * aa(m_state.vMoveDir, it);

						// do not clamp, cause its accelerating on the other direction, it will naturally
						// "break" (deccel till changes direction), but need to avoid changing dir and go straight
						// to trhough the max

						if(  (aa(m_state.vVel, it) > 0) != (aa(velDir, it) > 0) ){

							// changed dir (this is REALLY unlike to happen,
							// except in the transition from ~0 to the other dir), wich means it rarely will
							// trigger the clamp bellow

							if( abs(aa(m_state.vVel, it)) > abs(currMaxVel) )
								aa(m_state.vVel, it) = currMaxVel*-aa(m_state.vMoveDir, it);
						}
					}
					// if not at max
					else if( abs(aa(m_state.vVel, it)) <= currMaxVel	){

						aa(m_state.vVel, it) += m_config.fAccel * delta * aa(m_state.vMoveDir, it);

						// clamp:

						if( abs(aa(m_state.vVel, it)) > abs(currMaxVel) )
							aa(m_state.vVel, it) = currMaxVel*aa(m_state.vMoveDir, it);

					}
					else{

						// moving at the same dir but beyond max vel

						// deccel towards max

						aa(m_state.vVel, it) -= currDeccel * delta *aa(velDir, it);

						// clamp:

						if( abs(aa(m_state.vVel, it)) <= currMaxVel )
							aa(m_state.vVel, it) = currMaxVel * aa(m_state.vMoveDir, it);
					}
				}
			}


			// vert handling

			// update gravity effects

			float fGravityStep = m_config.fGravity * delta;

			m_state.vVel.y -= fGravityStep;

			if( m_state.vVel.y < -m_config.fMaxFallVel ) m_state.vVel.y = -m_config.fMaxFallVel;

			//

			pTrafoRef->m_local.position.x += m_state.vVel.x * delta;
			pTrafoRef->m_local.position.y += m_state.vVel.y * delta;
			pTrafoRef->m_local.position.z += m_state.vVel.z * delta;

			m_state.bGrounded = false; // should be set to true on collision solving
		}

		//------------------------------------------------------------------------
		// setters
		//------------------------------------------------------------------------
		void SetFaceDir( const DirectX::XMFLOAT3 & vFaceDir_p ){
			m_state.vFaceDir = vFaceDir_p;
			//pTrafoRef->m_local.scale.x = vFaceDir_p.x;
			//DirectX::XMVECTOR qRot = DirectX::XMLoadFloat4(&pTrafoRef->m_local.qRotation);
			//qRot = DirectX::XMQuaternionMultiply(qRot, DirectX::XMQuaternionRotationAxis(DirectX::g_XMIdentityR1, 180.0f * (float)gen_PI/180.0f));
			//DirectX::XMStoreFloat4(&pTrafoRef->m_local.qRotation, qRot);
		}
		void SetMoveDir( const DirectX::XMFLOAT3 & vMoveDir_p ){m_state.vMoveDir = vMoveDir_p;}
		void ZeroMoveDirAndVel(){
			memset( &m_state.vMoveDir, 0, sizeof(  DirectX::XMFLOAT3 ) );
			memset( &m_state.vVel, 0, sizeof(  DirectX::XMFLOAT3 ) );
		}
		void ZeroXMoveDirAndVel(){
			m_state.vMoveDir.x = 0.0f;
			m_state.vVel.x = 0.0f;
		}

		void SetXMoveDir( const float xMoveDir_p){ m_state.vMoveDir.x = xMoveDir_p; }
		void SetXVel( const float xVel_p){ m_state.vVel.x = xVel_p; }
		void SetYMoveDir( const float yMoveDir_p){ m_state.vMoveDir.y = yMoveDir_p; }
		void SetYVel( const float yVel_p){ m_state.vVel.y = yVel_p; }
		void SetZMoveDir( const float zMoveDir_p){ m_state.vMoveDir.z = zMoveDir_p; }
		void SetZVel( const float zVel_p){ m_state.vVel.z = zVel_p; }
		void SetGravity( float fGrav_p );
		void SetMaxVel( const float param1 );
		void SetAccel( const float accel){ m_config.fAccel = accel;}
		void SetDeccel(const float deccel_p){ m_config.fDeccel = deccel_p;}
		void SetAirDeccel(const float deccel_p){ m_config.fAirDeccel = deccel_p;}

		void SetGrounded( const bool bGrounded_p = true ){ m_state.bGrounded = bGrounded_p; }

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		const DirectX::XMFLOAT3& GetFaceDir() const{ return m_state.vFaceDir; }
		const DirectX::XMFLOAT3& GetMoveDir() const{ return m_state.vMoveDir; }
		const DirectX::XMFLOAT3& GetVel() const{ return m_state.vVel; }
		const float GetMaxVel() const { return m_config.fMaxVel; }
		const float GetStopVel() const{ return m_config.fVelStopThreshold;}
		const float GetDeccel() const{ return m_config.fDeccel;}

		bool IsGrounded() const { return m_state.bGrounded; }

		const DirectX::XMFLOAT4& GetPos() const{ return pTrafoRef->m_local.position;}

	private:

		config	m_config;
		state	m_state;

		game::TransformComponent * pTrafoRef;

		//------------------------------------------------------------------------
		// grabs ref to trafo compo
		//------------------------------------------------------------------------
		void VOnAttach();
	
	};

	//template<> const unsigned int   ComponentType<MovableComponent>::s_value = E_USERTYPE+3;
	//template<> const char *			ComponentType<MovableComponent>::s_szName = "MovableComponent";

	typedef gen::pool_ptr<MovableComponent> pool_MovableComponent_ptr;

	//========================================================================
	//========================================================================
	//========================================================================
	//========================================================================
	//========================================================================
	//========================================================================
	//======================================================================== 
	//========================================================================
	//========================================================================
	//========================================================================

	class MovableFactory: public game::IComponentFactory{

		friend class MovableSystem;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		MovableFactory( unsigned int maxComponents_p )
			:
		m_pool(maxComponents_p){}

	private:

		gen::Pool<MovableComponent> m_pool;

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual pool_Component_ptr VCreateComponent(){ return pool_Component_ptr(m_pool); }
		virtual pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );
		virtual pool_Component_ptr VCloneComponent( const Component * pCompo_p );
		virtual void VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p );

		DirectX::XMFLOAT3 GetXYZFromGfig( text::GfigElementA * pGFig_p );

		virtual void VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p );

		static void SerlializeXYZ( const DirectX::XMFLOAT3 & xyz_p, text::GfigElementA & gfig_p );
	};

	typedef std::shared_ptr<MovableFactory>	shared_MovableFactory_ptr;
	typedef std::weak_ptr<MovableFactory>	weak_MovableFactory_ptr;

}