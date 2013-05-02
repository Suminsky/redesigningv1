#include "ConstantBuffer.h"

// private

#include "../namespace gen/gen_macros.h"

using namespace render;

render::ConstantBuffer::ConstantBuffer( const UINT size_p, const UINT nVariables_p, const UINT * pVariablesSizes_p )
	:
	m_variablesBuffer( new BYTE[size_p]()),
	m_variablesOffsets( new byteoffset[nVariables_p]),
	m_variablesSizes( new UINT[nVariables_p])
{
	m_nBytes = size_p;
	m_nVariables = nVariables_p;
	m_bNeedUpdate = false; // first time needs update, but theres no way to set it if not by the methods

	UINT iOffsetCache = 0;
	for( UINT it = 0; it < m_nVariables; ++it ){

		m_variablesOffsets[it] = iOffsetCache;
		m_variablesSizes[it] = pVariablesSizes_p[it];

		iOffsetCache += m_variablesSizes[it];
	}
}

void render::ConstantBuffer::Init( const UINT size_p, const UINT nVariables_p, const UINT * pVariablesSizes_p )
{
	m_variablesBuffer.reset(new BYTE[size_p]());
	m_variablesOffsets.reset(new byteoffset[nVariables_p]);
	m_variablesSizes.reset(new UINT[nVariables_p]);

	m_nBytes = size_p;
	m_nVariables = nVariables_p;
	m_bNeedUpdate = false;

	UINT iOffsetCache = 0;
	for( UINT it = 0; it < m_nVariables; ++it ){

		m_variablesOffsets[it] = iOffsetCache;
		m_variablesSizes[it] = pVariablesSizes_p[it];

		iOffsetCache += m_variablesSizes[it];
	}
}