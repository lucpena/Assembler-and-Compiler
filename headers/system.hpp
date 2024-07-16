#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <cstring>
#include <map>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <cstdlib>

using std::endl;
using std::cout;
using std::cin;
using std::getline;
using std::string;
using std::map;
using std::vector;
using std::pair;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::toupper;
using std::stoi;
using std::to_string;

/********************************
    Configuracoes
*********************************/
map<string, int> opcodes = 
{
    {"ADD", 1}, {"SUB", 2}, {"MUL", 3}, 
    {"DIV", 4}, {"JMP", 5}, {"JMPN", 6}, 
    {"JMPP", 7}, {"JMPZ", 8}, {"COPY", 9}, 
    {"LOAD", 10}, {"STORE", 11}, {"INPUT", 12}, 
    {"OUTPUT", 13}, {"STOP", 14}
};

map<int, string> instructions = 
{
	{1, "ADD"}, {2, "SUB"}, {3, "MUL"},
	{4, "DIV"}, {5, "JMP"}, {6, "JMPN"},
	{7, "JMPP"}, {8, "JMPZ"}, {9, "COPY"},
	{10, "LOAD"}, {11, "STORE"}, {12, "INPUT"},
	{13, "OUTPUT"}, {14, "STOP"}
};

vector<string> validInstructions = 
    {"ADD", "SUB", "MUL", "DIV", "JMP", 
    "JMPN",	"JMPP", "JMPZ", "COPY",
    "LOAD",  "STORE",  "INPUT",
    "OUTPUT",  "STOP", "SPACE" };

vector<string> directives = 
    {"BEGIN", "SECTION", "CONST", 
    "END", "EXTERN", "PUBLIC", "SPACE"};

vector<string> sessions = {"NONE", "TEXT", "DATA"};

string validChars = {" ETAOINSHRDLCUMWFGYPBVKJXQZ0123456789:,_+"};
string validNumber = {"123456789x"};

string currentSession = sessions.at(0); // Define sessao inicial para NONE

// Codigo e Codigo Intermediario

// Montador
//      linha   , valores
map<unsigned int, string> 			program;
map<unsigned int, vector<string>> 	tokens;
map<unsigned int, int> 				codeObject;
map<unsigned int, int> 				address;

// Ligador
// ID do programa, Vetor de dados do programa, dados
vector<vector<pair<unsigned int, string>>>   vec_AllLinkerUseTables = {};
vector<vector<pair<unsigned int, string>>>   vec_AllLinkerDefinitionTables = {};

vector<vector<unsigned int>>    vec_RelativeTable = {};
vector<vector<unsigned int>>    vec_CodeObject = {};

vector<unsigned int>    AllProgramSizes = {};

// Declarando as funcoes ajudantes 
bool isStringInVector( const string& target, const vector<string>& vec );
bool isSessionValid( string session );
int getMemorySpace( string instruction );
void pressEnter();
bool isThisDecimal( string value );
bool isThisHex( string value );

/*******************************************************
    Para retornar o status do processamento da linha 
********************************************************/
class StatusReturn
{
public:

    string status;
    bool hasErrors;
    string errorType;

    StatusReturn() 
    : status(""), hasErrors(false), errorType("") {}

};

/********************************
    Simbolos
*********************************/
class Symbol
{
private:

    string m_Symbol;
    unsigned int m_Value;
    bool m_Defined;
    vector<int> m_PendenciesList;
    bool m_Extern;

public:

    Symbol( string symbol ) 
    : m_Symbol(symbol), m_Value(0), m_Defined(false), m_PendenciesList({}), m_Extern(false) {}

    Symbol( string symbol, unsigned int value, bool isExtern )
    : m_Symbol(symbol), m_Value(value), m_Defined(false), m_PendenciesList({}), m_Extern(isExtern) {}

    Symbol( string symbol, unsigned int value, bool isDefined, bool isExtern )
    : m_Symbol(symbol), m_Value(value), m_Defined(isDefined), m_PendenciesList({}), m_Extern(isExtern) {}

    string getSymbol() { return m_Symbol; }
    unsigned int getValue() { return m_Value; }
    vector<int> getPendanciesList() { return m_PendenciesList; }
    bool getIsDefined() { return m_Defined; }
    bool getIsExtern() { return m_Extern; }

    void setDefined( bool value ) { m_Defined = value; }
    void setValue( unsigned int value ) { m_Value = value; }
    void setExtern( bool value ) { m_Extern = value; }
    void setNewPendency( int value ) { m_PendenciesList.push_back(value); }

};
vector<Symbol> symbolTable = {};

class UseSymbol
{
private:
    
    string m_Symbol;
    int m_Value;

public:

    UseSymbol( string symbol, int value)
    : m_Symbol(symbol), m_Value(value) {}

    void setSymbol( string value ) { m_Symbol = value; }
    void setValue( int value ) { m_Value = value; }

    string getSymbol() { return m_Symbol; }
    int getValue() { return m_Value; }

};
vector<UseSymbol> useTable = {};
vector<UseSymbol> definitionTable = {};



/********************************
    Montador
*********************************/
class Assembler
{
private:
    
    bool m_HasSectionText = false;
    bool m_HasSectionData = false;
    bool m_HasBegin = false;
    string m_CurrentSession;
    vector<string> m_Errors = {};
    vector<string> m_ErrorsType = {};

public:

    Assembler() 
    : m_CurrentSession( sessions.at(0) ) {}

    string getCurrentSession() { return m_CurrentSession; }
    vector<string> getErrors() { return m_Errors; }
    vector<string> getErrorType() { return m_ErrorsType; }

    bool getHasSectionText() { return m_HasSectionText; }
    bool getHasSectionData() { return m_HasSectionData; }
    bool getHasBegin() { return m_HasBegin; }

    void setHasSectionText( bool value) { m_HasSectionText = value; }    
    void setHasSectionData( bool value) { m_HasSectionData = value; }    
    void setCurrentSession( string session ) { m_CurrentSession = session; }
    void setError( string value ) { m_Errors.push_back(value); }
    void setHasBegin( bool value ) { m_HasBegin = value; }

    StatusReturn processLineOfTokens( vector<string> lineTokens, int memorySpace, int lineNumber, string currentSession, bool hasBegin );

};

StatusReturn Assembler::processLineOfTokens( vector<string> lineTokens, int memorySpace, int lineNumber, string currentSession, bool hasBegin )
{

    StatusReturn status;

    // Processa STOP se estiver sozinho
    if( lineTokens[0] == "STOP" )
    {
        status.status += "End of program execution.";
        return status;
    }

    // Define o BEGIN se estiver sozinho
    if( lineTokens[0] == "BEGIN")
    {
        setHasBegin(true);
        status.status += "Start of the code set.";
        return status;
    }

    // Checa se eh um token normal e nao eh COPY
    if( isStringInVector( lineTokens[0], validInstructions ) && lineTokens[0] != "COPY" )
    {
        // Checar se o operando esta na tabela de simbolo
        bool tokenInTable = false;
        for( Symbol& sym : symbolTable ) {
            if( lineTokens[1] == sym.getSymbol() )
            {
                tokenInTable = true;
                sym.setNewPendency(memorySpace + 1);
                status.status += "Pendency updated on " + sym.getSymbol() + ".";
                return status;
            }
        }

        // Se nao tiver cria e adiciona pendencia
        if( !tokenInTable )
        {
            Symbol sym(lineTokens[1]);
            sym.setNewPendency(memorySpace + 1);
            symbolTable.push_back(sym);
            status.status += "Pendency created and added to " + sym.getSymbol() + ".";
            return status;
        }

        status.status += "Token valido nao processado na linha " + to_string(lineNumber) + " .";
        status.hasErrors = true;

        return status;
    }

    // Checa se eh um token normal e eh COPY
    if( isStringInVector( lineTokens[0], validInstructions ) && lineTokens[0] == "COPY" )
    {
        status.status += "COPY: ";
        // Checar se o primeiro operando esta na tabela de simbolo
        bool tokenInTable = false;
        for( Symbol& sym : symbolTable ) {
            if( lineTokens[1] == sym.getSymbol() )
            {
                tokenInTable = true;
                sym.setNewPendency(memorySpace + 1);
                status.status += "Pendency updated on " + sym.getSymbol() + ". ";
            }
        }

        // Se nao tiver cria e adiciona pendencia
        if( !tokenInTable )
        {
            Symbol sym(lineTokens[1]);
            sym.setNewPendency(memorySpace + 1);
            symbolTable.push_back(sym);
            status.status += "Pendency created and added to " + sym.getSymbol() + ". ";
        }

        // Checar se o segundo operando esta na tabela de simbolo
        tokenInTable = false;
        for( Symbol& sym : symbolTable ) {
            if( lineTokens[2] == sym.getSymbol() )
            {
                tokenInTable = true;
                sym.setNewPendency(memorySpace + 2);
                status.status += "Pendency updated on " + sym.getSymbol() + ". ";
            }
        }

        // Se nao tiver cria e adiciona pendencia
        if( !tokenInTable )
        {
            Symbol sym(lineTokens[2]);
            sym.setNewPendency(memorySpace + 2);
            symbolTable.push_back(sym);
            status.status += "Pendency created and added to " + sym.getSymbol() + ". ";
        }

        return status;
    }

    // Checa se eh um Label
    if( !isStringInVector(lineTokens[0], validInstructions) && !isStringInVector(lineTokens[0], directives) && lineTokens[0] != "CONST" ) 
    {
        string label = lineTokens[0];

        // Define o BEGIN com um Label
        if( lineTokens[1] == "BEGIN" )
        {
            setHasBegin(true);
            setCurrentSession("BEGIN");
            Symbol labelSym(label, memorySpace, true, false);
            symbolTable.push_back(labelSym);
            status.status += "Start of the code set with Label. ";
            return status;
        }

        // Define o STOP com um Label
        if( lineTokens[1] == "STOP" )
        {
            for( Symbol& sym : symbolTable )
            {
                if( sym.getSymbol() == label )
                {
                    sym.setDefined(true);
                    sym.setValue(memorySpace);
                }
            }

            status.status += "End of the code set with Label. ";
            return status;
        }


        // Checa se o uma variavel esta sendo definida fora do lugar certo
        for( Symbol& sym : symbolTable )
        {
            if( sym.getSymbol() != label && currentSession != "TEXT" && lineTokens[1] != "SPACE" && lineTokens[1] != "CONST" && lineTokens[1] != "EXTERN" )
            {
                status.status += "Warning: Label " + sym.getSymbol() + " defined outside the TEXT section. ";
                status.hasErrors = true;
            }

            if( sym.getSymbol() == label && currentSession != "DATA" && lineTokens[1] != "EXTERN" && ( lineTokens[1] == "SPACE" || lineTokens[1] == "CONST") )
            {
                status.status += "Warning: Variable " + sym.getSymbol() + " defined outside the DATA section. ";
                status.hasErrors = true;
            }

            if( sym.getSymbol() == label && lineTokens[1] == "EXTERN" && !getHasBegin())
            {
                status.status += "Warning: BEGIN not found. ";
                status.status += "Warning: EXTERN variable " + sym.getSymbol() + " defined outside the BEGIN section. ";
                status.hasErrors = true;
            }
        }

        // Label para definicao de variavel. Checa se o operando eh CONST ou SPACE
        if( lineTokens[1] == "SPACE" || lineTokens[1] == "CONST" )
        {
            for( Symbol& sym : symbolTable )
            {
                if( sym.getSymbol() == label )
                {
                    sym.setDefined(true);
                    sym.setValue(memorySpace);

                    // Analisa novamente para checar o operando
                    if( lineTokens[1] == "CONST" )
                    {
                        lineTokens.erase(lineTokens.begin());

                        // Checa se o valor de CONST eh valido (Decimal ou Hex)
                        for( auto theChar : lineTokens[1] )
                        {
                            if( validNumber.find(theChar) == string::npos )
                            {
                                if( lineTokens[1].find("X") != string::npos && lineTokens[1].find("X") != 1 )
                                {
                                status.status += "Line " + to_string(lineNumber) + ": Invalid character in CONST value (" + theChar + ").";
                                status.hasErrors = true;
                                return status;
                                }
                            }					
                        }

                        if( isThisDecimal(lineTokens[1]) || isThisHex(lineTokens[1]) )
                        {
                            status.status += "CONST is valid";
                            return status;
                        }
                        else
                        {
                            status.status += "Invalid CONST value.";
                            status.hasErrors = true;
                            return status;    
                        }
                    }

                    status.status += "Symbol " + sym.getSymbol() + " defined.";
                    return status;
                }
            }
        }

        // Cria um Label externo
        if( lineTokens[1] == "EXTERN")
        {
            Symbol labelSym(label, memorySpace, false, true);
            symbolTable.push_back(labelSym);
        }

        // Label para funcoes
        bool onTheTable = false;
        for( Symbol& sym : symbolTable )
        {
            if( sym.getSymbol() == label )
            {
                onTheTable = true;
                sym.setDefined(true);
                sym.setValue(memorySpace);
            }
        }

        if(!onTheTable)
        {
            Symbol labelSym(label, memorySpace, true, false);
            symbolTable.push_back(labelSym);
        }

        // Chama a funcao de novo mas sem o label para proximos tokens
        status.status += "Variable " + lineTokens[0] + " added and ";
        lineTokens.erase(lineTokens.begin());
        processLineOfTokens(lineTokens, memorySpace, lineNumber, currentSession, hasBegin);

        status.status += "Token " + lineTokens[0] +  " processed.";
        return status;
    }

    // Checa se o Label eh uma diretiva
    if ( isStringInVector(lineTokens[0], directives) )
    {        
        // Checa se eh Section
        if( lineTokens[0] == "SECTION" && isSessionValid(lineTokens[1]) && lineTokens.size() == 2 ) 
        {
            if( isSessionValid(lineTokens[1]) )
            {
                setCurrentSession(lineTokens[1]);
                status.status += "Section changed.";
                return status;
            }

            status.status += "Invalid section.";
            status.hasErrors = true;
            return status;        
        } 

        // Checa se eh PUBLIC
        if( lineTokens[0] == "PUBLIC" ) 
        {
            Symbol labelSym(lineTokens[1]);
            symbolTable.push_back(labelSym);

            status.status += "Public variable " + lineTokens[1] + " defined.";
            return status;        
        } 

        // Checa se eh END
        if( lineTokens[0] == "END" ) 
        {
            status.status += "End of file.";
            return status;        
        } 

        status.status += "Error executing the directive " + lineTokens[0] + ".";
        status.hasErrors = true;
        return status;
    }    

    status.status += "Token " + lineTokens[0] + " not found.";
    status.hasErrors = true;
    return status;
}

/********************************
    Ligador
*********************************/
class Linker
{
private:
    
public:
    Linker() {};
    ~Linker() {};
};


/********************************
    Helper Functions
*********************************/
bool isThisDecimal( string value )
{
    try {
        size_t pos = stoi(value, &pos);
        return true; 
    } catch (...) {
        return false;
    }
    return false;
}

bool isThisHex( string value )
{
    try {
        size_t pos = stoi(value, &pos, 16);
        return true; 
    } catch (...) {
        return false;
    }
    return false;
}


bool throwError() 
{
    return false;
}

bool isStringInVector(const string& target, const vector<string>& vec) 
{
    return find(vec.begin(), vec.end(), target) != vec.end();
}

bool isSessionValid( string session ) 
{
    for( const string& str : sessions )
    {
        if( str == session )
            return true;        
    }

    return false;
}

int getMemorySpace( string instruction ) 
{
    if( instruction == "COPY" )
        return 3;

    if( instruction == "STOP" || instruction == "CONST" )
        return 1;

    if( instruction == "SPACE" )
        return 1;

    if( instruction == "BEGIN" || instruction == "SECTION" || instruction == "END" || instruction == "EXTERN" || instruction == "PUBLIC" )
        return 0;

    return 2;    
}

int getMemorySpace( unsigned int instruction )
{
    if (instruction == 9)
        return 3;

    if (instruction == 14 )
        return 1;

    if (instruction == 0)
        return 0;

    return 2;
}

void pressEnter()
{
    cout << "Press enter to continue...\n" << endl;
    cin.get();
}

void pressEnterNoMsg()
{
    cin.get();
}

void log( string log )
{
    cout << "Log: " << log;
    cin.get();
}

void log( int log )
{
    cout << "Log: " << log;
    cin.get();
}

#endif