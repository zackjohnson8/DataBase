#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>

struct DataBaseStruct
{

  std::string name;

};

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
  std::vector<DataBaseStruct*> holdDataBases;
  DataBaseStruct* newDataBase;
  std::string inputLine;
  std::string parsedWord;
  int holdInt;
  int holdInt2;
  char waitChar;
  int lineCount = 0;
  while(inputFile.good())
  {
    lineCount++;

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

      if(holdInt != -1)
      {
        parsedWord = inputLine.substr(0, holdInt);
        //std::cout << parsedWord << std::endl;
        if(parsedWord == "CREATE")
        {

          //std::cout << inputLine << std::endl;
          // TODO
          // So far only have create TABLE
          holdInt2 = inputLine.find_first_of(' ', holdInt+1);
          parsedWord = inputLine.substr(holdInt+1, holdInt2);

          holdInt = parsedWord.find_first_of(' ',0);

          if(parsedWord.substr(0, holdInt) == "DATABASE")
          {

            //std::cout << parsedWord << std::endl;
            // Get the name of the DATABASE
            parsedWord = parsedWord.substr(holdInt+1, parsedWord.size()-4);

            // Create a DATABASE
            newDataBase = new DataBaseStruct();
            newDataBase->name = parsedWord;
            std::cout << newDataBase->name << std::endl;

          }else
          if(parsedWord.substr(0, holdInt) == "TABLE")
          {

            // TODO

          }else
          {

            std::cout << "Trouble reading in CREATE what? at line " << lineCount << std::endl;

          }

          //std::cout << std::endl << std::endl;
          //std::cout << parsedWord << std::endl << std::endl;

        }else
        if(parsedWord == "DROP")
        {

          // TODO

        }else
        if(parsedWord == "USE")
        {

          // TODO

        }else
        if(parsedWord == "SELECT")
        {

          // TODO

        }else
        if(parsedWord == "ALTER")
        {

          // TODO

        }else
        {
          if(inputFile.good())
          {
            std::cout << "ERROR: Problem detected reading in line " << lineCount << std::endl;
          }
        }
      }else // Handle single word commands
      {

        if(inputLine == ".EXIT\r")
        {

          // TODO

        }else
        {
          if(inputFile.good())
          {
            std::cout << "ERROR: Problem detected reading in line " << lineCount << std::endl;
          }
        }

      }

    }

  }


  return(0);
}
