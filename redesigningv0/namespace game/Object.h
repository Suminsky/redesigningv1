#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/14
	created:	14:4:2013   18:54
	file:		Object.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	The game object, the rice of the game.
				NOTE:
				Cache friendly idea:
				store all components (derived from Component) instances in a contiguous block (std::vector)
				and make objects point to then based on a pointer to type plus a pointer to the right instance.
				To update the actors, instead of iterating actors, one would update all components of the same
				type, till all types.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <assert.h>
#include <memory>
#include <unordered_map>

// private includes

#include "Component.h"
//#include "Message.h"

namespace game{

	class Layer;
	class Component;

	//typedef unsigned int ObjectID;

	typedef unsigned int OBJECTINDEX;
	static const unsigned int INVALID_OBJECTINDEX = (unsigned int)-1;
	
	typedef std::vector<shared_Component_ptr> ObjectComponents;
	typedef std::vector<COMPONENTINDEX> ComponentIndexes;

	//========================================================================
	// 
	//========================================================================
	class Object{

		friend class Layer;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Object()
			:
			m_currentObjectIndex(INVALID_OBJECTINDEX),
			m_pLayerOwner(nullptr),
			m_bDead(true){}

		virtual ~Object(){}

		//------------------------------------------------------------------------
		// component stuff
		//------------------------------------------------------------------------
		void AddComponent( shared_Component_ptr && pComponent_p ){

			assert( pComponent_p->m_bDead );

			pComponent_p->m_bDead = false;

			if( pComponent_p->m_currentComponentIndex == INVALID_COMPONENTINDEX 
				||
				pComponent_p->m_pObjectOwner != this ){

					pComponent_p->m_currentComponentIndex = (COMPONENTINDEX)m_components.size();
					pComponent_p->m_pObjectOwner = this;

					//pComponent_p->VOnInit();

					m_components.push_back( std::move(pComponent_p) );
			}
			else{
			
				//pComponent_p->VOnInit();
			}
		}
		void AddComponent( const shared_Component_ptr & pComponent_p ){

			assert( pComponent_p->m_bDead );

			pComponent_p->m_bDead = false;

			if( pComponent_p->m_currentComponentIndex == INVALID_COMPONENTINDEX 
				||
				pComponent_p->m_pObjectOwner != this ){

					pComponent_p->m_currentComponentIndex = (COMPONENTINDEX)m_components.size();
					pComponent_p->m_pObjectOwner = this;

					m_components.push_back( pComponent_p );	
			}

			//pComponent_p->VOnInit();
		}
		void RemoveComponent( COMPONENTINDEX componentCurrentIndex_p ){

			assert( componentCurrentIndex_p != INVALID_COMPONENTINDEX );
			assert( !m_components[componentCurrentIndex_p]->m_bDead );

			m_components[componentCurrentIndex_p]->m_bDead = true;

			m_removedComponents.push_back(componentCurrentIndex_p);
		}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		Layer * GetLayerOwner() const { return m_pLayerOwner; }

	protected:

		Layer * m_pLayerOwner;

	private:

		OBJECTINDEX m_currentObjectIndex;
		bool m_bDead;

		ObjectComponents m_components;
		ComponentIndexes m_removedComponents;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void CleanRemovedComponents(){

			unsigned int nRemoved =	(unsigned int)m_removedComponents.size(); // cache
			unsigned int nComponents =		(unsigned int)m_components.size();

			for( unsigned int itRemoved = 0, itLast = nComponents - 1;
				 itRemoved < nRemoved;
				 ++itRemoved, --itLast ){

				if( !m_components[m_removedComponents[itRemoved]]->m_bDead ){

					--nRemoved;
					continue; // untested
				}

				//m_components[m_removedComponents[itRemoved]]->Abort();
				m_components[m_removedComponents[itRemoved]]->m_currentComponentIndex = INVALID_COMPONENTINDEX;
				m_components[m_removedComponents[itRemoved]]->m_pObjectOwner = nullptr;

				if( m_removedComponents[itRemoved] == itLast ){

					continue;
				}

				std::swap( m_components[m_removedComponents[itRemoved]], m_components[itLast] );

				if( m_components[m_removedComponents[itRemoved]]->m_bDead ){

					// find the swaped task on the list to be destroyed, update the index

					for( unsigned int itToBeDestroyed = itRemoved; itToBeDestroyed < nRemoved; ++itToBeDestroyed ){

						if( m_removedComponents[itToBeDestroyed] == m_components[m_removedComponents[itRemoved]]->m_currentComponentIndex ){

							m_removedComponents[itToBeDestroyed] = m_removedComponents[itRemoved];
						}
					}
				}
				else{

					m_components[m_removedComponents[itRemoved]]->m_currentComponentIndex = m_removedComponents[itRemoved];
				}
			}

			// "trim"

			m_components.resize(nComponents - nRemoved);

			m_removedComponents.clear();
		}

	};

	typedef std::shared_ptr<Object> shared_Object_ptr;
	typedef std::weak_ptr<Object> weak_Object_ptr;
}