#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <mpi.h>
#include <cmath>
#include <cctype>

using namespace std;

void printHashMap(unordered_map<string, int> HashMap, int rank, int size) {
	unordered_map<string, int>::iterator itr;
	ofstream OutputFile;
	string FilePath = "output/file";
	string FileExtension = ".out";
	string NoOfFile = to_string(rank);

	FilePath.append(NoOfFile).append(FileExtension);

	OutputFile.open(FilePath, ios::out);
		if (!OutputFile) {
		cout << "File not created!";
	}

	printf("I am process %d out of %d\n", rank, size);
	OutputFile << "\nThe HashMap is : \n";
	OutputFile << "\tKEY\t\tELEMENT\n";
	for (itr = HashMap.begin(); itr != HashMap.end(); ++itr) {
		OutputFile << '\t' << itr->first << '\t' << '\t' << itr->second << '\n';
	}
	OutputFile << endl;
}

int main(int argc, char *argv[]) {
	int rank, size;
	std::unordered_map<string, int> HashMap;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf("Hello, MPI! ");
	printf("I am process %d out of %d\n", rank, size);

	//Each process opens a file
	//Computes the word count and stores it in ProcessHashMap

	std::unordered_map<string, int> ProcessLocalHashMap;
	string ReadWord;
	string FilePath = "input/file";
	string FileExtension = ".in";
	string NoOfFile = to_string(rank);

	FilePath.append(NoOfFile).append(FileExtension);

	cout << "The FilePath is " << FilePath + "\n";

	ifstream file(FilePath);

	if (!file) {
		cerr << "Incorrect FilePath. \n";
		return -1;
	}

	while (file >> ReadWord)
	{
		transform(ReadWord.begin(), ReadWord.end(), ReadWord.begin(), ::tolower);
		ReadWord.erase(std::remove_if(ReadWord.begin(), ReadWord.end(),  ::ispunct), ReadWord.end());
		//Check if word is already in the map
		//If the word is firstly added
		if (ProcessLocalHashMap.count(ReadWord) < 1) {
			ProcessLocalHashMap.insert(pair<string, int>(ReadWord, 1));
		}
		//If the word is already in the map
		else {
			unordered_map<string, int>::iterator itr = ProcessLocalHashMap.find(ReadWord);
			if(itr != ProcessLocalHashMap.end()) 
				++itr->second;
		}
		
		//cout<< ReadWord << '\n';
	}

	printHashMap(ProcessLocalHashMap, rank, size);

	// or we can split equally by bits every file

	//A BARRIER HERE before adding to the finalHashMap

	//Shuffling on parent process
	
	//All Maps are then sent to a parent process => count the total no of words and splits the words to the processes
	//Each process computes the no of occurences of the words

	//ANOTHER BARRIER HERE?

	//The parent node writes all the occurences to a file

	MPI_Finalize();
	return 0;

}