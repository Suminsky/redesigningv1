#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/01
	created:	1:11:2013   17:58
	file:		ComponentTypes.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	All components must add a template specialization with a unique value to this file.
				This helps by having type information at a unique place.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

namespace game{

	//------------------------------------------------------------------------
	// the template
	//------------------------------------------------------------------------
	template<typename Compo>
	struct ComponentType {

		static const unsigned int	s_value;
		static const char *			s_szName;
	};

	//------------------------------------------------------------------------
	// macro to make specialization easier
	//------------------------------------------------------------------------
#define NEW_COMPONENT_TYPE( type ) \
	class type;\
	template<> const unsigned int   ComponentType<type>::s_value = __LINE__;\
	template<> const char *			ComponentType<type>::s_szName = #type

	//------------------------------------------------------------------------
	// macro to make getting type int from type easier
	//------------------------------------------------------------------------
#define COMPONENT_TYPE( type ) \
	ComponentType<type>::s_value

	//------------------------------------------------------------------------
	// macro to read type as string
	//------------------------------------------------------------------------
#define COMPONENT_NAME( type ) \
	ComponentType<type>::s_szName

	//------------------------------------------------------------------------
	// specializations
	//------------------------------------------------------------------------
	NEW_COMPONENT_TYPE(ColorComponent);
	NEW_COMPONENT_TYPE(TransformComponent);
	NEW_COMPONENT_TYPE(SpriteComponent_);
	NEW_COMPONENT_TYPE(SpriteAnimationComponent);
	

	template< typename DELEGATE_PARAM > class ButtonComponent;
	template<> const unsigned int   ComponentType<ButtonComponent<int>>::s_value = __LINE__;
	template<> const char *			ComponentType<ButtonComponent<int>>::s_szName = "ButtonComponent<int>";

	enum E_COMPONENTTYPE{
		E_USERTYPE = __LINE__
	};
}