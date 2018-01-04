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
    size_t Data;
	IntOptVar() { Type = INT; }
	IntOptVar(size_t data) { Type = INT; Data = data; }
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
	OpcodeItem(vm_inst opCode) : OpCode(opCode) { }
	OpcodeItem(vm_inst opCode, OptVar* opt) : OpCode(opCode), Opt(opt) { }
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

void CodeGenerator::Generate(std::vector<size_t>& codes)
{
	for (int i = 0; i < this->impl->opcodes.size(); ++i) {
        codes.push_back(this->impl->opcodes[i]->OpCode);

		if (this->impl->opcodes[i]->Opt != nullptr)
			switch(this->impl->opcodes[i]->Opt->Type)
			{
				case OptVar::VARIABLE:
                    codes.push_back(this->impl->variables[((VariableOptVar*)this->impl->opcodes[i]->Opt)->Data]);
					break;

				case OptVar::METHOD:
                    codes.push_back(this->impl->methods[((MethodOptVar*)this->impl->opcodes[i]->Opt)->Data]);
					break;

				case OptVar::INT:
                    codes.push_back(((IntOptVar*)this->impl->opcodes[i]->Opt)->Data);
					break;
			}
	}
}

void CodeGenerator::visit(AssignmentAst* ast) 
{
	this->impl->variables[ast->Name] = this->impl->variables.size();
    this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iSTORE, new IntOptVar(static_cast<int>(this->impl->variables[ast->Name]))));
    this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iLOAD, new IntOptVar(static_cast<int>(this->impl->variables[ast->Name]))));
}

void CodeGenerator::visit(BlockAst* ast) { }
void CodeGenerator::visit(IfStatementAst* ast) { }
void CodeGenerator::visit(FunctionDefinetionAst* ast) 
{
	this->impl->methods[ast->Name] = this->impl->methods.size();
}

void CodeGenerator::visit(ForStatementAst* ast) { }
void CodeGenerator::visit(VariableAst* ast) { }

void CodeGenerator::visit(PrimativeAst* ast) {
    this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iPUSH, new IntOptVar(ast->Value->Integer)));
}

void CodeGenerator::visit(ControlAst* ast) { }
void CodeGenerator::visit(BinaryAst* ast)
{

    switch (ast->Op)
    {
        case OPERATOR_NONE:break;
        case OPERATION:break;
        case PLUS:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iADD));
            break;

        case MINUS:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iSUB));
            break;

        case MULTIPLICATION:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iMUL));
            break;

        case DIVISION:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iDIV));
            break;

        case EQUAL:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iEQ));
            break;

        case NOT_EQUAL:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iSUB));
            break;

        case GREATOR:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iGT));
            break;

        case LOWER:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iLT));
            break;

        case GREATOR_EQUAL:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iGTE));
            break;

        case LOWER_EQUAL:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iLTE));
            break;

        case SINGLE_QUOTES:break;
        case DOUBLE_QUOTES:break;
        case LEFT_PARENTHESES:break;
        case RIGHT_PARENTHESES:break;
        case SQUARE_BRACKET_START:break;
        case SQUARE_BRACKET_END:break;
        case BLOCK_START:break;
        case BLOCK_END:break;
        case OR:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iOR));
            break;

        case AND:
            this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iAND));
            break;

        case COMMA:break;
        case ASSIGN:break;
        case SINGLE_COLON:break;
        case DOUBLE_COLON:break;
        case UNDERLINE:break;
        case APPEND:break;
        case INDEXER:break;
    }
}

void CodeGenerator::visit(StructAst* ast) { }
void CodeGenerator::visit(ReturnAst* ast) { }
void CodeGenerator::visit(ParenthesesGroupAst* ast) { }
void CodeGenerator::visit(FunctionCallAst* ast) { }
void CodeGenerator::visit(UnaryAst* ast) { }
void CodeGenerator::visit(ExprStatementAst* ast) { }