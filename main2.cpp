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

using namespace std;

struct TableStruct
{

  std::string name;
  std::vector<std::string*> variables;

};

struct DataBaseStruct
{

  DataBaseStruct* name;
  std::vector<TableStruct*>* tables;

};

std::vector<string*> stringBreakDown(std::string& mstring);

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
  std::vector<TableStruct*> tableNew;
  std::vector<DataBaseStruct*> indexDataBases;
  std::string newLine;
  std::vector<string*> returnedStrings;
  int lineCount = 0; // Keeping track for error correction
  while(inputFile.good())
  {
    lineCount++;

    // Take a line
    getline(inputFile, newLine);

    // if its not a eat line then send it to lineHandler
    if(newLine[0] != '-' && newLine[0] != '\r')
    {
      returnedStrings = stringBreakDown(newLine);
    }

    // handle the returnedString
    stringHandler(returnedStrings);

  }

  std::cout << std::endl << std::endl;

  return(0);


}

std::vector<string*> stringBreakDown(std::string& mstring)
{

  std::vector<string*> stringVector;
  std::string* stringHold;
  int intHold;

  // Take the string and break it up into parts
  while(mstring.find_first_of(' ', 0) != -1)
  {

    // Determine from 0 to intHold is the first value
    intHold = mstring.find_first_of(' ', 0);

    // fill stringHold with new data and push to stringVector
    stringHold = new string();
    *stringHold = mstring.substr( 0, intHold );
    stringVector.push_back(stringHold);
    //cout << *stringHold << endl;

    // change mstring to minus the word we just took
    mstring = mstring.substr(intHold+1, mstring.size());

    //cin >> intHold;

  }

  // Handle the last word for each string
  stringHold = new string();
  *stringHold = mstring.substr(0, mstring.find_first_of(';', 0));
  stringVector.push_back(stringHold);

  return stringVector;

}


/*

























*/
