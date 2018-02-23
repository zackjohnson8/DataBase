/* Author: Zachary L. Johnson
 * Date: February 22, 2018
 * Verison: 1.0
 * Description: To create a simple database which can be provided source files.
 * The files are then parsed and databases/tables are created and saved to
 * storage.
 *
*/

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>

struct DataBaseStruct
{

  std::string name;

};

struct TableStruct
{

  std::string name;
  std::vector<std::string*> variables;

};

// Functions

int main(int argc, char *argv[])
{

  std::cout << std::endl << std::endl;

  std::string fileName;
  if(argc <= 1)
  {
    //std::cout << "Using default file name PA1_test.sql" << std::endl;
    fileName = "PA1_test.sql";
  }else
  {
    //std::cout << "Using provided file name " << argv[1] << std::endl;
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
  std::vector<TableStruct*> holdTables;
  DataBaseStruct* usedDataBase = NULL;
  DataBaseStruct* newDataBase;
  TableStruct* newTable;
  std::string* newString;
  std::string inputLine;
  std::string parsedWord;
  int holdInt;
  int holdInt2;
  char waitChar;
  int lineCount = 0;
  bool foundDup = false;
  int holdIndex;
  while(inputFile.good())
  {
    lineCount++;
    foundDup = false;

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

          // So far only have create TABLE
          holdInt2 = inputLine.find_first_of(' ', holdInt+1);
          parsedWord = inputLine.substr(holdInt+1, holdInt2);

          holdInt = parsedWord.find_first_of(' ',0);

          if(parsedWord.substr(0, holdInt) == "DATABASE")
          {

            //std::cout << parsedWord << std::endl;
            // Get the name of the DATABASE
            holdInt2 = parsedWord.find_first_of(';',0);
            parsedWord = parsedWord.substr(holdInt+1, holdInt2);
            parsedWord.resize(parsedWord.size()-2);
            // Create a DATABASE
            newDataBase = new DataBaseStruct();
            newDataBase->name = parsedWord;

            // Check existing databases to see if already there
            for(int index = 0; index < holdDataBases.size(); index++)
            {
              if(newDataBase->name == holdDataBases.at(index)->name)
              {
                foundDup = true;
              }
            }

            if(foundDup)
            {
              std::cout << "!Failed to create database " << newDataBase->name << " because it already exists." << std::endl;
              delete newDataBase;
              newDataBase = NULL;
            }else
            {
              holdDataBases.push_back(newDataBase);
              std::cout << "DataBase " << newDataBase->name << " created." << std::endl;
            }
            foundDup = false;

          }else
          if(parsedWord.substr(0, holdInt) == "TABLE")
          {

            parsedWord = inputLine.substr(holdInt+2, inputLine.size());

            // Take this statement and create what you need
            // TABLE tbl_1 (a1 int, a2 varchar(20));
            parsedWord.erase(parsedWord.begin(), parsedWord.begin()+parsedWord.find_first_of(' ', 0) + 1);

            // Create a table for this
            newTable = new TableStruct();
            newTable->name = parsedWord.substr(0, parsedWord.find_first_of(' ', 0));

            // Check for dup of table
            for(int index = 0; index < holdTables.size(); index++)
            {
              if(newTable->name == holdTables.at(index)->name)
              {
                foundDup = true;
              }
            }

            if(foundDup)
            {
              std::cout << "!Failed to create table " << newTable->name << " because it already exists." << std::endl;
              delete newTable;
              newTable = NULL;
            }else
            {

              holdInt = parsedWord.find_first_of('(', 0);
              parsedWord = parsedWord.substr(holdInt+1, parsedWord.size()-1);
              parsedWord.resize(parsedWord.size()-2);
              newString = new std::string();
              *newString = parsedWord.substr(0, parsedWord.find_first_of(',',0));
              newTable->variables.push_back(newString);

              holdInt = parsedWord.find_first_of(',', 0);
              parsedWord = parsedWord.substr(holdInt+2, parsedWord.size()-1);
              parsedWord.resize(parsedWord.size()-1);
              newString = new std::string();
              *newString = parsedWord;
              newTable->variables.push_back(newString);

              holdTables.push_back(newTable);
              std::cout << "Table " << newTable->name << " created." << std::endl;
            }
            foundDup = false;

          }else
          {

            std::cout << "Trouble reading in CREATE what? at line " << lineCount << std::endl;

          }

        }else
        if(parsedWord == "DROP")
        {

          holdInt2 = inputLine.find_first_of(' ', holdInt+1);
          parsedWord = inputLine.substr(holdInt+1, holdInt2);

          holdInt = parsedWord.find_first_of(' ',0);

          if(parsedWord.substr(0, holdInt) == "DATABASE")
          {

            // Get the name of the DATABASE
            holdInt2 = parsedWord.find_first_of(';',0);
            parsedWord = parsedWord.substr(holdInt+1, holdInt2);
            parsedWord.resize(parsedWord.size());

            // Check existing databases to see if already there
            for(int index = 0; index < holdDataBases.size(); index++)
            {
              if(parsedWord == holdDataBases.at(index)->name)
              {
                foundDup = true;
                holdIndex = index;
              }
            }

            if(!foundDup)
            {
              std::cout << "!Failed to delete " << parsedWord << " because it does not exists." << std::endl;
            }else
            {
              std::cout << "Database " << holdDataBases.at(holdIndex)->name << " deleted." << std::endl;
              holdDataBases.erase(holdDataBases.begin()+holdIndex);
            }
            foundDup = false;

          }else
          if(parsedWord.substr(0, holdInt) == "TABLE")
          {

            // Get the name of the TABLE
            parsedWord = inputLine.substr(holdInt, parsedWord.find_first_of(';', 0));

            parsedWord = parsedWord.substr(parsedWord.find_first_of(' ',0)+1, parsedWord.size());
            parsedWord.resize(parsedWord.size()-2);

            // Check existing databases to see if already there
            for(int index = 0; index < holdTables.size(); index++)
            {
              if(parsedWord == holdTables.at(index)->name)
              {
                foundDup = true;
                holdIndex = index;
              }
            }

            if(!foundDup)
            {
              std::cout << "!Failed to delete " << parsedWord << " because it does not exists." << std::endl;
            }else
            {
              std::cout << "Table " << holdTables.at(holdIndex)->name << " deleted." << std::endl;
              holdTables.erase(holdTables.begin()+holdIndex);
            }
            foundDup = false;

          }else
          {

            std::cout << "Trouble reading in CREATE what? at line " << lineCount << std::endl;

          }

        }else
        if(parsedWord == "USE")
        {

          holdInt = inputLine.find_first_of(' ', 0);
          parsedWord = inputLine.substr(holdInt+1, inputLine.size()-2);

          parsedWord.resize(parsedWord.size()-2);

          for(int index = 0; index < holdDataBases.size(); index++)
          {
            if(parsedWord == holdDataBases.at(index)->name)
            {
              foundDup = true;
              holdIndex = index;
            }
          }
          // found the searched for database
          if(foundDup)
          {

            usedDataBase = holdDataBases.at(holdIndex);
            holdTables.clear();
            std::cout << "Using database " << usedDataBase->name << "." << std::endl;

          }else
          {

            std::cout << "!Failed cannot USE a database that does not exist." << std::endl;

          }
          foundDup = false;

        }else
        if(parsedWord == "SELECT")
        {

          holdInt = inputLine.find_first_of(' ', 0);
          parsedWord = inputLine.substr(holdInt+1, inputLine.size()-1);
          holdInt = parsedWord.find_first_of(' ', 0);
          parsedWord = parsedWord.substr(holdInt+1, parsedWord.size()-1);
          holdInt = parsedWord.find_first_of(' ', 0);
          parsedWord = parsedWord.substr(holdInt+1, parsedWord.size()-1);
          parsedWord.resize(parsedWord.size()-2);

          for(int index = 0; index < holdTables.size(); index++)
          {
            if(parsedWord == holdTables.at(index)->name)
            {
              foundDup = true;
              holdIndex = index;
            }
          }
          std::string tableName = parsedWord;
          // found the searched for database
          if(foundDup)
          {

            for(
                int index = 0;
                index < holdTables.at(holdIndex)->variables.size();
                index++
               )
            {

              std::cout << *(holdTables.at(holdIndex)->variables.at(index));
              if(index < holdTables.at(holdIndex)->variables.size()-1)
              {
                std::cout << " | ";
              }
            }
            std::cout << std::endl;
            foundDup = false;
          }else
          {

            std::cout << "!Failed to query table " << parsedWord << " because it does not exist." << std::endl;

          }
          foundDup = false;


        }else
        if(parsedWord == "ALTER")
        {

          parsedWord = inputLine;

          holdInt = parsedWord.find_first_of(' ', 0);
          parsedWord = parsedWord.substr(holdInt+1, parsedWord.size()-1);
          holdInt = parsedWord.find_first_of(' ', 0);
          parsedWord = parsedWord.substr(holdInt+1, parsedWord.size()-1);
          parsedWord.resize(parsedWord.size()-2);

          std::string holdString = parsedWord.substr(0, parsedWord.find_first_of(' ',0));


          for(int index = 0; index < holdTables.size(); index++)
          {
            if(holdString == holdTables.at(index)->name)
            {
              foundDup = true;
              holdIndex = index;
            }
          }

          if(foundDup)
          {

            holdInt = parsedWord.find_first_of(' ', 0);
            parsedWord = parsedWord.substr(holdInt+1, parsedWord.size()-1);
            holdInt = parsedWord.find_first_of(' ', 0);
            parsedWord = parsedWord.substr(holdInt+1, parsedWord.size()-1);
            newString = new std::string();
            *newString = parsedWord;
            holdTables.at(holdIndex)->variables.push_back(newString);

            std::cout << "Table " << holdString << " modified." << std::endl;

          }else
          {
            std::cout << "!Failed to alter " << holdString << " because it does not exist." << std::endl;
          }


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

          std::cout << "All done." << std::endl;

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

  std::cout << std::endl << std::endl;

  return(0);
}
