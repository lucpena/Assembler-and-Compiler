#include "../headers/system.hpp"

int main(int argc, char *argv[])
{
	/**************************************************
		Adquirindo os dados dos programas
	***************************************************/
	if (argc < 2 || argc > 5)
	{
		cout << "\n\tUsando o ligador:" << endl
			 << "\t./linker arquivo1 arquivo2" << endl
			 << "\t./linker arquivo1 arquivo2 arquivo3" << endl
			 << "\t./linker arquivo1 arquivo2 arquivo3 arquivo4 " << endl;
		return -1;
	}

	cout << "\nLigador\nAutor: Lucas Araujo Pena [130056162]\n"
		 << endl;

	for (unsigned int i = 0; (int)i < argc - 1; i++)
	{
		string fileName = argv[i + 1];
		string fileLine = "";

		ifstream input(fileName);

		vector<pair<unsigned int, string>> thisProgramDefinitionTable = {};
		vector<pair<unsigned int, string>> thisProgramUseTable = {};

		vec_RelativeTable.push_back(vector<unsigned int>());
		vec_CodeObject.push_back(vector<unsigned int>());

		string currentLineContent = "";
		unsigned int currentLineNumber = 1;

		string variable = "", stringValue = "";
		int value = 0;

		// USO = 1, DEF = 2, RELATIVOS = 3, CODE = 4
		int currentSection = 0;

		while (getline(input, fileLine))
		{
			pair<unsigned int, string> definitionTablePair = {};
			pair<unsigned int, string> useTablePair = {};

			bool sectionLine = false;

			if (fileLine.find("USO") != string::npos)
			{
				currentSection = 1;
				sectionLine = true;
			}
			if (fileLine.find("DEF") != string::npos)
			{
				currentSection = 2;
				sectionLine = true;
			}
			if (fileLine.find("RELATIVOS") != string::npos)
			{
				currentSection = 3;
				sectionLine = true;
			}
			if (fileLine.find("CODE") != string::npos)
			{
				currentSection = 4;
				sectionLine = true;
			}

			stringstream ss(fileLine);

			if (!sectionLine)
			{
				switch (currentSection)
				{
				case 1: // USO
					variable = fileLine.substr(0, fileLine.find(" "));
					value = stoi(fileLine.substr(fileLine.find(" ") + 1, fileLine.size()));
					useTablePair = {value, variable};
					thisProgramUseTable.push_back(useTablePair);
					break;

				case 2: // DEF
					variable = fileLine.substr(0, fileLine.find(" "));
					value = stoi(fileLine.substr(fileLine.find(" ") + 1, fileLine.size()));
					definitionTablePair = {value, variable};
					thisProgramDefinitionTable.push_back(definitionTablePair);
					break;

				case 3: // RELATIVOS
					while (ss >> stringValue)
					{
						value = stoi(stringValue);
						vec_RelativeTable[i].push_back(value);
					}

					break;

				case 4: // CODE
					while (ss >> stringValue)
					{
						value = stoi(stringValue);
						vec_CodeObject[i].push_back(value);
					}

					break;

				default:
					break;
				}
			}

			sectionLine = false;
			currentLineNumber++;
		}

		vec_AllLinkerUseTables.push_back(thisProgramUseTable);
		vec_AllLinkerDefinitionTables.push_back(thisProgramDefinitionTable);
		input.close();

	}

	/**************************************************
	*	Calculando o espaco de memoria
	*	e fator de correcao
	***************************************************/
	vector<unsigned int> vec_fatoresDeCorrecao = {0};
	unsigned int programCounter = 0;

	for (unsigned int i = 0; (int)i < argc - 1; i++)
	{
		for (const auto& co : vec_CodeObject[i])
		{
			programCounter += getMemorySpace(co);
			//cout << co << " ";
		}

		vec_fatoresDeCorrecao.push_back(programCounter);
		//log(vec_fatoresDeCorrecao[i]);
	}

	/**************************************************
	 * Adicionando o fator de correcao
	 **************************************************/
	string outFileName = argv[1];
	outFileName = outFileName.substr(0, outFileName.find(".obj"));
	outFileName += ".exc";
	ofstream outFile(outFileName);

	vector<unsigned int> finalCode = {};
	for (unsigned int i = 0; (int)i < argc - 1; i++)
	{
		//for (const auto &co : vec_CodeObject[i])
		for( unsigned int j = 0; j < vec_CodeObject[i].size(); j++)
		{
			programCounter = 0;
			//log(vec_CodeObject[i][j]);
			if (j % 2 == 0 && vec_CodeObject[i][j] != 0)
			{
				programCounter = vec_CodeObject[i][j] + vec_fatoresDeCorrecao[i];
			}
			else
			{
				programCounter = vec_CodeObject[i][j];
			}
			//log(programCounter);
			finalCode.push_back(programCounter);
		}
	}
	cout << "Programa ligado\n----------------------------------------------"	<< endl;
	for( unsigned int i = 0; i < finalCode.size(); i++ )
	{
		cout << finalCode[i] << " ";
		outFile << finalCode[i] << " ";
	}
	cout << "\n\n\nProgramas lidos:\n" << endl;

	outFile.close();

	for (unsigned int i = 0; (int)i < argc - 1; i++)
	{
		cout << "Programa " << argv[i + 1] << 
		      "\n----------------------------------------------" << endl;

		cout << "USO" << endl;
		for (const auto &ut : vec_AllLinkerUseTables[i])
		{
			cout << ut.second << " " << ut.first << endl;
		}

		cout << "DEF" << endl;
		for (const auto &dt : vec_AllLinkerDefinitionTables[i])
		{
			cout << dt.second << " " << dt.first << endl;
		}

		cout << "RELATIVOS" << endl;
		for (const auto &rt : vec_RelativeTable[i])
		{
			cout << rt << " ";
		}
		cout << endl;

		cout << "CODE" << endl;
		for (const auto &co : vec_CodeObject[i])
		{
			cout << co << " ";
		}
		cout << "\n\n" << endl;
	}

	return 0;
}