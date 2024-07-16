/*
	Propriedade intelectual de Lucas Araújo Pena.

*/


#include "../headers/system.hpp"

int main( int argc, char *argv[] )
{
	/********************************
		Abrindo o arquivo
	*********************************/

	if( argc < 2) 
	{
		cout << "\n\tUsando o montador: ./montador arquivo [-enter|-fast]" << endl;
		return -1;
	}

	// // Tipo de execucao do programa
	string mode = "-default";
	for (int i = 1; i < argc; ++i)
	{
		string arg = argv[i];
		if (arg.size() > 1 && arg[0] == '-')
		{
			mode = arg;
		}
	}

	// Inicio da execucao
	for (unsigned int i = 1; (int)i < argc; i++)
	{

		string arg = argv[i];

		if (arg.size() > 1 && arg[0] == '-')
			break;

		unsigned int localAddress = 0;
		unsigned int programIndex = 1; // Marcador de linha no promt
		bool fatalError = false;

		Assembler assembler;

		string fileName = argv[i];
		fileName.append(".asm");
		ifstream input(fileName);

		// Ressetando tudo para o próximo arquivo
		symbolTable = {};
		useTable = {};
		definitionTable = {};

		if (input.is_open())
		{
 
			unsigned int lineNumber = 1;
			string line = "", temp = "", singleLineToken = "";
			vector<string> lineTokens = {};
			string lonelyLabel = "";

			while (getline(input, line))
			{

				lineTokens = {};
				temp = "";

				// Caso encontre um label sozinho, mantem os mesmos dados e volta pra linha anterior
				if (lonelyLabel != "")
				{
					temp += lonelyLabel + ": ";
					lonelyLabel = "";
				}

				// Remove comentarios
				if (line.find(";") != string::npos)
					line = line.substr(0, line.find(";"));

				// Remove tabulacoes
				line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

				// Desconsidera linhas vazias
				if (!line.empty() || line != "\0")
				{
					// Transforma a string para maiusculo
					for (auto theChar : line)
					{
						temp += toupper(theChar);
					}

					// Salva a linha do programa
					program[lineNumber] = temp;

					// Checa caracteres invalidos
					for (auto theChar : temp)
					{
						if (validChars.find(theChar) == string::npos)
						{
							assembler.setError("Line " + to_string(lineNumber) + ": Invalid character (" + theChar + ").");
							fatalError = true;
						}
					}

					// Checa mais de uma label na mesma linha
					if (temp.find(":") != string::npos)
					{
						string checkError = temp.substr(temp.find(":") + 1, temp.size());
						if (checkError.find(":") != string::npos)
						{
							assembler.setError("Line " + to_string(lineNumber) + ": More than one label at the same line.");
							fatalError = true;
						}
					}

					// Adquirindo os tokens
					stringstream ss(temp);
					while (ss >> singleLineToken)
					{
						// Remove a virgula do COPY
						if (singleLineToken != "," && singleLineToken.find(":") == string::npos)
						{
							lineTokens.push_back(singleLineToken);
						}

						// Ve se eh label e resolve espacos
						if (singleLineToken.find(":") != string::npos)
						{
							string temp1 = "", temp2 = "";
							temp1 = singleLineToken.substr(0, singleLineToken.find(":"));
							temp2 = singleLineToken.substr(singleLineToken.find(":") + 1, singleLineToken.size());

							if (!temp1.empty())
								lineTokens.push_back(temp1);
							if (!temp2.empty())
								lineTokens.push_back(temp2);
						}
					}

					tokens[lineNumber] = lineTokens;

					// Checa se as sessoes foram definidas
					if (lineTokens[0] == "SECTION")
					{
						if (isSessionValid(lineTokens[1]) && lineTokens.size() == 2)
						{
							if (isSessionValid(lineTokens[1]) && lineTokens[1] == "TEXT")
							{
								assembler.setHasSectionText(true);
							}
							if (isSessionValid(lineTokens[1]) && lineTokens[1] == "DATA")
							{
								assembler.setHasSectionData(true);
							}
						}
					}

					// Mantem na mesma linha se for um token sozinho
					if (program[lineNumber].find(":") != string::npos && lineTokens.size() < 2)
					{
						lonelyLabel = lineTokens[0];
					}
					else
					{
						lineNumber++;
					}
				}
			}

			input.close();
		}
		else
		{
			cout << "\n\n"
				 << "Erro: Nao foi possivel abrir o arquivo " << argv[i] << "." << endl;
		}

		// Lanca o aviso caso nao exista as sessoes TEXT ou DATA
		if (!assembler.getHasSectionText())
		{
			assembler.setError("Warning: Section TEXT not found.");
		}
		if (!assembler.getHasSectionData())
		{
			assembler.setError("Warning: Section DATA not found.");
		}

		// for( unsigned int i = 1; i < program.size(); i++ )
		// {
		// 	if(    program[i].find("DATA")  != string::npos
		// 	    || program[i].find("SPACE") != string::npos
		// 		|| program[i].find("CONST") != string::npos)
		// 	{
		// 		auto& programElement = program;
		// 		pair<unsigned int, string> element;
		// 		element.first = i;
		// 		element.second = program[i];

		// 		assembler.setError(element.first + " " + element.second);

		// 		program.erase(i);
		// 		program.insert(program.end(), element);
		// 	}
		// }

		/********************************
		 *	Processamento
		 *********************************/
		for (const auto &par : program)
		{
			/*********************************
			 * 	Adquirindo os OPCODES
			 *********************************/
			system("clear");

			string instruction;
			int lineNumber = par.first;

			if (par.second.find(":") == string::npos)
			{
				// Se não encontou o dois pontos
				instruction = par.second.substr(0, par.second.find(" "));
			}
			else
			{
				// Se encontrou os dois pontos, tira o label
				// L1: DIV DOIS -> DIV DOIS
				instruction = par.second.substr(par.second.find(":") + 1, par.second.size());

				// e pega somente a primeira parte da string que sobrou
				// DIV DOIS -> DIV
				stringstream ss(instruction);
				ss >> instruction;
			}

			// Checa se esta no mapa de opcodes, se nao estiver da o valor 0
			auto opcode = opcodes.find(instruction);
			if (opcode != opcodes.end())
				codeObject[par.first] = opcode->second;
			else
				codeObject[par.first] = 0;

			/*********************************
			 * 	Calculando espaco em memoria
			 *********************************/
			auto sizeInMemory = instructions.find(codeObject[par.first]);
			if (sizeInMemory != instructions.end())
			{
				address[par.first] = localAddress;
				localAddress += getMemorySpace(sizeInMemory->second);
			}
			else
			{
				string lineOfCode = program[par.first];
				address[par.first] = localAddress;

				// Pega depois dos dois pontos para labels, ou antes do espaco para diretivas
				if (lineOfCode.find(":") == string::npos)
				{
					localAddress += getMemorySpace(lineOfCode.substr(0, lineOfCode.find(" ")));
				}
				else
				{
					// string instruction = lineOfCode.substr(lineOfCode.find(" ") + 1, lineOfCode.size());
					string instruction = tokens[par.first][1];
					localAddress += getMemorySpace(instruction);

					// Calcula espaco de memoria para SPACE com tamanho diferente
					if (instruction == "SPACE" && tokens[par.first].size() == 3)
					{
						localAddress += stoi(tokens[par.first][2]) - 1;
					}
				}
			}

			// Gerando Tabela de Simbolos
			StatusReturn log = assembler.processLineOfTokens(
				tokens[par.first],
				address[par.first],
				lineNumber,
				assembler.getCurrentSession(),
				assembler.getHasBegin());

			if (log.hasErrors)
			{
				assembler.setError("Line " + to_string(lineNumber) + ": " + log.status);
			}

			/*********************************
			 * Prompt
			 *********************************/
			cout << "\nMontador\nAutor: Lucas Araujo Pena [130056162]\n"
				 << endl;
			cout << "- Program: " << argv[i] << ".asm" << endl;
			cout << "\n#Linha\tConteudo" << endl
				 << endl;

			for (const auto &par : program)
			{
				if (par.first == programIndex)
					cout << "~>";

				cout << par.first << ": \t" << par.second << endl;
			}

			cout << endl
				 << "#Linha\tOpcode\tMem\tTokens" << endl
				 << endl;
			for (const auto &par : program)
			{

				if (par.first == programIndex)
					cout << "~>";

				cout << par.first << ": \t" << codeObject[par.first] << "\t" << address[par.first] << "\t";

				for (unsigned int i = 0; i < tokens[par.first].size(); i++)
				{
					cout << "[" << tokens[par.first][i] << "]";
					if (i + 1 < tokens[par.first].size())
						cout << ",";
				}

				cout << endl;
			}

			cout << "\n\nTabela de Simbolos {Simbolo,(Valor),Definido, Externo e [Lista].} \n----------------------------------------------" << endl;
			for (Symbol symbol : symbolTable)
			{

				string isDefined = symbol.getIsDefined() ? "True" : "False";
				string isExtern = symbol.getIsExtern() ? "True" : "False";

				cout <<

					symbol.getSymbol() << ", (" << symbol.getValue() << "), " << isDefined << ", " << isExtern << ", [";

				unsigned int i = 0;
				for (int dependency : symbol.getPendanciesList())
				{
					cout << dependency;
					if (i + 1 < symbol.getPendanciesList().size())
						cout << ",";

					i++;
				}

				cout << "]" << endl;
			}
			programIndex++;

			cout << "\n\n";

			cout << "Erros e avisos: \n----------------------------------------------" << endl;
			int index = 1;
			if (assembler.getErrors().size() != 0)
			{
				for (string error : assembler.getErrors())
				{
					cout << "   " << index << "  -> " << error << endl;
					index++;
				}
			}
			else
			{
				cout << "Sem erros e avisos." << endl;
			}

			cout << "\n\nAssembler Status\n----------------------------------------------\n";
			cout << "- Sessao atual: " << assembler.getCurrentSession() << endl;
			cout << "- Linha atual: " << par.first << endl;

			cout << "- Argumentos: ";
			for (int i = 1; i < argc; ++i)
			{
				std::string arg = argv[i];
				
				if (arg.size() > 1 && arg[0] == '-')
				{
					// Process the flag
					cout << arg;
					if (i < argc - 1)
						cout << ", ";
				}
			}
			if (argc == 2)
				cout << "None";
			cout << endl;

			cout << "- Log: " << log.status << endl;

			// size_t p;
			// int a = stoi("0xxx40", &p, 16);
			// cout << a << endl;

			if (mode == "-enter")
			{
				cout << endl;
				pressEnter();
			}

			if (mode == "-default")
				sleep(1);

			// Para se encontrou um erro que imposibilita a montagem
			if (fatalError)
				break;
		}

		// Abre o arquivo de saida
		string outName = argv[i];
		if (!assembler.getHasBegin())
			outName += ".exc";
		if (assembler.getHasBegin())
			outName += ".obj";
		ofstream out(outName);

		// Mostra os simbolos nao definidos
		bool symbolNotDefined = false;
		for (Symbol &sym : symbolTable)
		{
			if (!sym.getIsDefined())
			{
				symbolNotDefined = true;
			}
		}
		if (symbolNotDefined)
		{
			cout << "\n\n- Simbolos nao definidos:" << endl;

			for (Symbol &sym : symbolTable)
			{
				if (!sym.getIsDefined())
				{
					cout << "\t" << sym.getSymbol() + " nao foi definido." << endl;
				}
			}

			cout << endl;
		}

		// Preenche tabela de Uso
		for (Symbol &sym : symbolTable)
		{
			if (sym.getIsExtern())
			{
				for (unsigned int i = 0; i < sym.getPendanciesList().size(); i++)
				{
					UseSymbol useSym(sym.getSymbol(), sym.getPendanciesList().at(i));
					useTable.push_back(useSym);
				}
			}
		}
		cout << "\n\nTabela de Uso\n----------------------------------------------" << endl;
		for (UseSymbol &sym : useTable)
		{
			cout << " " << sym.getSymbol() << "\t\t" << sym.getValue() << "+" << endl;
		}

		// Preenche a tabela de Definicoes
		for (Symbol &sym : symbolTable)
		{
			if (!sym.getIsExtern())
			{
				UseSymbol useSym(sym.getSymbol(), sym.getValue());
				definitionTable.push_back(useSym);
			}
		}
		cout << "\n\nTabela de Definicoes\n----------------------------------------------" << endl;
		for (UseSymbol &sym : definitionTable)
		{
			cout << " " << sym.getSymbol() << "\t\t" << sym.getValue() << endl;
		}

		cout << "\n\nCodigo exportado para " << outName
			 << "\n----------------------------------------------\n";

		// Caso o programa nao precise de ligacao
		if (!assembler.getHasBegin())
		{
			for (unsigned int i = 1; i < program.size(); i++)
			{
				string memorySpace = "";

				out << codeObject[i] << " ";
				cout << codeObject[i] << " ";

				int thisOperandAddress = address[i] + 1;

				// unsigned int j = 0;
				for (Symbol &sym : symbolTable)
				{
					for (int dependency : sym.getPendanciesList())
					{
						if (dependency == thisOperandAddress && codeObject[i] != 0)
						{
							out << sym.getValue() << " ";
							cout << sym.getValue() << " ";
						}
						if (dependency == thisOperandAddress && codeObject[i] == 0)
						{
							out << 0 << " ";
							cout << 0 << " ";
						}
					}
				}

				// // STOP pode nao ter operando e segfault em token[i][1]
				// if( tokens[i][0].find("STOP") == string::npos )
				// {
				// 	for( Symbol& sym : symbolTable )
				// 	{
				// 		if( tokens[i][1] == sym.getSymbol() )
				// 		{
				// 			out << sym.getValue() << " ";
				// 			cout << sym.getValue() << " ";
				// 		}
				// 	}
				// }
			}
		}

		else
		{
			cout << "USO" << endl;
			out << "USO" << endl;
			for (UseSymbol &sym : useTable)
			{
				cout << sym.getSymbol() << " " << sym.getValue() << endl;
				out << sym.getSymbol() << " " << sym.getValue() << endl;
			}

			cout << "DEF" << endl;
			out << "DEF" << endl;
			for (UseSymbol &sym : definitionTable)
			{
				cout << sym.getSymbol() << " " << sym.getValue() << endl;
				out << sym.getSymbol() << " " << sym.getValue() << endl;
			}

			cout << "RELATIVOS" << endl;
			out << "RELATIVOS" << endl;
			for (UseSymbol &sym : useTable)
			{
				cout << sym.getValue() << " ";
				out << sym.getValue() << " ";
			}
			cout << endl;
			out << endl;

			cout << "CODE" << endl;
			out << "CODE" << endl;
			for (unsigned int i = 0; i < program.size(); i++)
			{
				string memorySpace = "";

				out << codeObject[i] << " ";
				cout << codeObject[i] << " ";
			}
		}

		out.close();
	

		if (mode == "-default")
		{
			cout << "\n\nDica: Voce pode usar a flag [-enter] para pular uma linha de cada vez ou "
					<< "a flag [-fast] para executar tudo de uma vez." << endl;
		}

		cout << "\n\nFim da execucao.";
		cout << "Pressione -Enter- para continuar.\n";
		pressEnterNoMsg();
	} 

	return 0;
}