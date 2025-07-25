#include "nuanceur/builder/ShaderBuilder.h"

using namespace Nuanceur;

bool Nuanceur::IsIdentitySwizzle(SWIZZLE_TYPE swizzle)
{
	return (swizzle == SWIZZLE_X) ||
	       (swizzle == SWIZZLE_XY) ||
	       (swizzle == SWIZZLE_XYZ) ||
	       (swizzle == SWIZZLE_XYZW);
}

bool Nuanceur::IsMaskSwizzle(SWIZZLE_TYPE swizzle)
{
	switch(swizzle)
	{
	case SWIZZLE_X:
	case SWIZZLE_Y:
	case SWIZZLE_Z:
	case SWIZZLE_W:
	case SWIZZLE_XY:
	case SWIZZLE_YZ:
	case SWIZZLE_YW:
	case SWIZZLE_ZW:
	case SWIZZLE_XYZ:
	case SWIZZLE_XYW:
	case SWIZZLE_XZW:
	case SWIZZLE_YZW:
	case SWIZZLE_XYZW:
		return true;
	default:
		assert(false);
		return false;
	}
}

SWIZZLE_TYPE Nuanceur::TransformSwizzle(SWIZZLE_TYPE a, SWIZZLE_TYPE b)
{
	switch(a)
	{
	default:
		assert(false);
		[[fallthrough]];
	case SWIZZLE_X:
	case SWIZZLE_XY:
	case SWIZZLE_XYZ:
	case SWIZZLE_XYZW:
		//Return b if identity
		return b;
	case SWIZZLE_Y:
		switch(b)
		{
		default:
			assert(false);
			[[fallthrough]];
		case SWIZZLE_XX:
			return SWIZZLE_YY;
		case SWIZZLE_XXX:
			return SWIZZLE_YYY;
		}
		break;
	case SWIZZLE_ZW:
		switch(b)
		{
		default:
			assert(false);
			[[fallthrough]];
		case SWIZZLE_X:
			return SWIZZLE_Z;
		case SWIZZLE_Y:
			return SWIZZLE_W;
		case SWIZZLE_XY:
			return SWIZZLE_ZW;
		}
		break;
	}
}

uint32 CShaderBuilder::GetMetadata(METADATA_TYPE type, uint32 defaultValue) const
{
	auto iterator = m_metadata.find(type);
	if(iterator == std::end(m_metadata)) return defaultValue;
	return iterator->second;
}

void CShaderBuilder::SetMetadata(METADATA_TYPE type, uint32 value)
{
	m_metadata[type] = value;
}

const CShaderBuilder::SymbolArray& CShaderBuilder::GetSymbols() const
{
	return m_symbols;
}

CShaderBuilder::SEMANTIC_INFO CShaderBuilder::GetInputSemantic(const SYMBOL& sym) const
{
	assert(sym.location == SYMBOL_LOCATION_INPUT);
	return m_inputSemantics.find(sym.index)->second;
}

CShaderBuilder::SEMANTIC_INFO CShaderBuilder::GetOutputSemantic(const SYMBOL& sym) const
{
	assert(sym.location == SYMBOL_LOCATION_OUTPUT);
	return m_outputSemantics.find(sym.index)->second;
}

std::string CShaderBuilder::GetVariableName(const SYMBOL& sym) const
{
	assert(sym.location == SYMBOL_LOCATION_VARIABLE);
	return m_variableNames.find(sym.index)->second;
}

std::string CShaderBuilder::GetUniformName(const SYMBOL& sym) const
{
	assert(sym.location == SYMBOL_LOCATION_UNIFORM);
	return m_uniformNames.find(sym.index)->second;
}

CVector4 CShaderBuilder::GetTemporaryValue(const SYMBOL& sym) const
{
	CVector4 result(0, 0, 0, 0);
	assert(sym.location == SYMBOL_LOCATION_TEMPORARY);
	assert(sym.type == SYMBOL_TYPE_FLOAT4);
	auto temporaryValueIterator = m_temporaryValues.find(sym.index);
	if(temporaryValueIterator != std::end(m_temporaryValues))
	{
		result = temporaryValueIterator->second;
	}
	return result;
}

CShaderBuilder::CIntVector4 CShaderBuilder::GetTemporaryValueInt(const SYMBOL& sym) const
{
	CIntVector4 result(0, 0, 0, 0);
	assert(sym.location == SYMBOL_LOCATION_TEMPORARY);
	assert((sym.type == SYMBOL_TYPE_INT4) ||
	       (sym.type == SYMBOL_TYPE_UINT4) ||
	       (sym.type == SYMBOL_TYPE_USHORT4) ||
	       (sym.type == SYMBOL_TYPE_UCHAR4));
	auto temporaryValueIterator = m_temporaryValuesInt.find(sym.index);
	if(temporaryValueIterator != std::end(m_temporaryValuesInt))
	{
		result = temporaryValueIterator->second;
	}
	return result;
}

CShaderBuilder::CBoolVector4 CShaderBuilder::GetTemporaryValueBool(const SYMBOL& sym) const
{
	CBoolVector4 result(false, false, false, false);
	assert(sym.location == SYMBOL_LOCATION_TEMPORARY);
	assert(sym.type == SYMBOL_TYPE_BOOL4);
	auto temporaryValueIterator = m_temporaryValuesBool.find(sym.index);
	if(temporaryValueIterator != std::end(m_temporaryValuesBool))
	{
		result = temporaryValueIterator->second;
	}
	return result;
}

const CShaderBuilder::StatementList& CShaderBuilder::GetStatements() const
{
	return m_statements;
}

void CShaderBuilder::InsertStatement(const STATEMENT& statement)
{
	m_statements.push_back(statement);
}

const std::string& CShaderBuilder::GetSource() const
{
	return m_source;
}

void CShaderBuilder::SetSource(std::string source)
{
	m_source = std::move(source);
}

void CShaderBuilder::SetSourceLine(uint32 sourceLine)
{
	STATEMENT statement;
	statement.op = STATEMENT_OP_SOURCE_LINE;
	statement.src1Ref.symbol.index = sourceLine;
	InsertStatement(statement);
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateInput(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentInputIndex++;
	sym.type = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_INPUT;
	m_symbols.push_back(sym);

	m_inputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateInputInt(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentInputIndex++;
	sym.type = SYMBOL_TYPE_INT4;
	sym.location = SYMBOL_LOCATION_INPUT;
	m_symbols.push_back(sym);

	m_inputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateInputUint(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentInputIndex++;
	sym.type = SYMBOL_TYPE_UINT4;
	sym.location = SYMBOL_LOCATION_INPUT;
	m_symbols.push_back(sym);

	m_inputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateOutput(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentOutputIndex++;
	sym.type = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_OUTPUT;
	m_symbols.push_back(sym);

	m_outputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateOutputUint(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentOutputIndex++;
	sym.type = SYMBOL_TYPE_UINT4;
	sym.location = SYMBOL_LOCATION_OUTPUT;
	m_symbols.push_back(sym);

	m_outputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateVariableFloat(const std::string& name)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentVariableIndex++;
	sym.type = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_VARIABLE;
	m_symbols.push_back(sym);

	m_variableNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateVariableInt(const std::string& name)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentVariableIndex++;
	sym.type = SYMBOL_TYPE_INT4;
	sym.location = SYMBOL_LOCATION_VARIABLE;
	m_symbols.push_back(sym);

	m_variableNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateVariableUint(const std::string& name)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentVariableIndex++;
	sym.type = SYMBOL_TYPE_UINT4;
	sym.location = SYMBOL_LOCATION_VARIABLE;
	m_symbols.push_back(sym);

	m_variableNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateVariableBool(const std::string& name)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentVariableIndex++;
	sym.type = SYMBOL_TYPE_BOOL4;
	sym.location = SYMBOL_LOCATION_VARIABLE;
	m_symbols.push_back(sym);

	m_variableNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateTemporary()
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateTemporaryBool()
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_BOOL4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateTemporaryInt()
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_INT4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateTemporaryUint()
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_UINT4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateTemporaryUshort()
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_USHORT4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateTemporaryUchar()
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_UCHAR4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateConstant(float v1, float v2, float v3, float v4)
{
	//TODO: Check if constant already exists

	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	auto tempValue = CVector4(v1, v2, v3, v4);
	m_temporaryValues.insert(std::make_pair(sym.index, tempValue));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateConstantInt(int32 v1, int32 v2, int32 v3, int32 v4)
{
	//TODO: Check if constant already exists

	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_INT4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	auto tempValue = CIntVector4(v1, v2, v3, v4);
	m_temporaryValuesInt.insert(std::make_pair(sym.index, tempValue));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateConstantUint(uint32 v1, uint32 v2, uint32 v3, uint32 v4)
{
	//TODO: Check if constant already exists

	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_UINT4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	auto tempValue = CIntVector4(v1, v2, v3, v4);
	m_temporaryValuesInt.insert(std::make_pair(sym.index, tempValue));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateConstantBool(bool v1, bool v2, bool v3, bool v4)
{
	//TODO: Check if constant already exists

	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_BOOL4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	auto tempValue = CBoolVector4(v1, v2, v3, v4);
	m_temporaryValuesBool.insert(std::make_pair(sym.index, tempValue));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateUniformFloat4(const std::string& name, unsigned int unit)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_UNIFORM;
	sym.unit = unit;
	m_symbols.push_back(sym);

	m_uniformNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateUniformInt4(const std::string& name, unsigned int unit)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_INT4;
	sym.location = SYMBOL_LOCATION_UNIFORM;
	sym.unit = unit;
	m_symbols.push_back(sym);

	m_uniformNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateUniformMatrix(const std::string& name, unsigned int unit)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_MATRIX;
	sym.location = SYMBOL_LOCATION_UNIFORM;
	sym.unit = unit;
	m_symbols.push_back(sym);

	m_uniformNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateUniformArrayUint(const std::string& name, unsigned int unit, uint32 attributes)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_ARRAYUINT;
	sym.location = SYMBOL_LOCATION_UNIFORM;
	sym.unit = unit;
	sym.attributes = attributes;
	m_symbols.push_back(sym);

	m_uniformNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateUniformArrayUchar(const std::string& name, unsigned int unit, uint32 attributes)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_ARRAYUCHAR;
	sym.location = SYMBOL_LOCATION_UNIFORM;
	sym.unit = unit;
	sym.attributes = attributes;
	m_symbols.push_back(sym);

	m_uniformNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateUniformArrayUshort(const std::string& name, unsigned int unit, uint32 attributes)
{
	SYMBOL sym;
	sym.owner = this;
	sym.index = m_currentTempIndex++;
	sym.type = SYMBOL_TYPE_ARRAYUSHORT;
	sym.location = SYMBOL_LOCATION_UNIFORM;
	sym.unit = unit;
	sym.attributes = attributes;
	m_symbols.push_back(sym);

	m_uniformNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateTexture2D(unsigned int unit)
{
	SYMBOL sym;
	sym.owner = this;
	sym.type = SYMBOL_TYPE_TEXTURE2D;
	sym.location = SYMBOL_LOCATION_TEXTURE;
	sym.unit = unit;
	sym.index = -1;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateImage2DUint(unsigned int unit)
{
	SYMBOL sym;
	sym.owner = this;
	sym.type = SYMBOL_TYPE_IMAGE2DUINT;
	sym.location = SYMBOL_LOCATION_TEXTURE;
	sym.unit = unit;
	sym.index = -1;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateSubpassInput(unsigned int unit, unsigned int index)
{
	SYMBOL sym;
	sym.owner = this;
	sym.type = SYMBOL_TYPE_SUBPASSINPUT;
	sym.location = SYMBOL_LOCATION_TEXTURE;
	sym.unit = unit;
	sym.index = index;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateSubpassInputUint(unsigned int unit, unsigned int index)
{
	SYMBOL sym;
	sym.owner = this;
	sym.type = SYMBOL_TYPE_SUBPASSINPUTUINT;
	sym.location = SYMBOL_LOCATION_TEXTURE;
	sym.unit = unit;
	sym.index = index;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateOptionalInput(bool available, SEMANTIC semantic, unsigned int semanticIndex)
{
	return available ? CreateInput(semantic, semanticIndex) : SYMBOL();
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateOptionalOutput(bool available, SEMANTIC semantic, unsigned int semanticIndex)
{
	return available ? CreateOutput(semantic, semanticIndex) : SYMBOL();
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateOptionalUniformMatrix(bool available, const std::string& name)
{
	return available ? CreateUniformMatrix(name) : SYMBOL();
}
