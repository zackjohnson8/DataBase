#include <iostream>
#include <cstdlib>
#include <fstream>

// Functions

int main(int argc, char *argv[])
{

  std::string fileName;
  if(argc <= 1)
  {
    std::cout << "Using default file name PA1_test.sql" << std::endl;
    fileName = "PA1_test.sql";
  }else
  {
    std::cout << "Using provided file name " << argv[1] << std::endl;
    fileName = argv[1];
  }

  // Open the specified file, if the file does not exist loop until a corrected
  // name is provided.
  std::ifstream inputFile;
  inputFile.open(fileName.c_str());
  while(!inputFile.good())
  {
    std::cout << "The provided input file does not exist. Please provide the corrected name or create the file as needed: ";
    std::cin >> fileName;
    inputFile.open(fileName.c_str());

    std::cout << std::endl;
  }

  // Begin parsing the data from .sql file
  std::string inputLine;
  std::string parsedWord;
  int holdInt;
  char waitChar;
  while(inputFile.good())
  {
    // Take a line
    getline(inputFile, inputLine);

    // Eat comments and blank lines
    if(inputLine[0] != '-' && inputLine[0] != '\r')
    {
      // else the lines have content that must be parsed and handled
      //std::cout << inputLine << std::endl;
      holdInt = inputLine.find_first_of(' ', 0);

      // determine how the line will be handled
      // Parse the first word from the string
      parsedWord = inputLine.substr(0, holdInt);
      std::cout << parsedWord << std::endl;

    }

  }


  return(0);
}
