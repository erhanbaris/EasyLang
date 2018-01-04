#include "CodeGenerator.h"
#include "Vm.h"

class OptVar
{
public:
	enum {
		VARIABLE,
		METHOD,
		INT
	} Type;
};

class IntOptVar : public OptVar
{
public:
	int Data;
	IntOptVar() { Type = INT; }
	IntOptVar(int data) { Type = INT; Data = data; }
};

class MethodOptVar : public OptVar
{
public:
	string_type Data;
	MethodOptVar() { Type = METHOD; }
	MethodOptVar(string_type data) { Type = METHOD; Data = data; }
};

class VariableOptVar : public OptVar
{
public:
	string_type Data;
	VariableOptVar() { Type = VARIABLE; }
	VariableOptVar(string_type data) { Type = VARIABLE; Data = data; }
};

class OpcodeItem {
public:
	vm_inst OpCode;
	OptVar* Opt{ nullptr };

	OpcodeItem() {}
	OpcodeItem(vm_inst opCode) : OpCode(opCode), Opt(nullptr) { }
	OpcodeItem(vm_inst opCode, OptVar* opt) : OpCode(opCode) { }
};

class CodeGeneratorImpl
{
public:
	std::unordered_map<string_type, size_t> variables;
	std::unordered_map<string_type, size_t> methods;

	std::vector<OpcodeItem*> opcodes;
};

CodeGenerator::CodeGenerator()
{
	impl = new CodeGeneratorImpl;
}

CodeGenerator::~CodeGenerator()
{
	delete impl;
}

void CodeGenerator::visit(AssignmentAst* ast) 
{
	this->impl->variables[ast->Name] = this->impl->variables.size();

	//this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iSTORE, this->impl->variables[ast->Name]));
}

void CodeGenerator::visit(BlockAst* ast) { }
void CodeGenerator::visit(IfStatementAst* ast) { }
void CodeGenerator::visit(FunctionDefinetionAst* ast) 
{
	this->impl->methods[ast->Name] = this->impl->methods.size();
}

void CodeGenerator::visit(ForStatementAst* ast) { }
void CodeGenerator::visit(VariableAst* ast) { }
void CodeGenerator::visit(PrimativeAst* ast) { }
void CodeGenerator::visit(ControlAst* ast) { }
void CodeGenerator::visit(BinaryAst* ast) { }
void CodeGenerator::visit(StructAst* ast) { }
void CodeGenerator::visit(ReturnAst* ast) { }
void CodeGenerator::visit(ParenthesesGroupAst* ast) { }
void CodeGenerator::visit(FunctionCallAst* ast) { }
void CodeGenerator::visit(UnaryAst* ast) { }
void CodeGenerator::visit(ExprStatementAst* ast) { }