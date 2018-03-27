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

struct TableValueStruct
{

  std::vector<string> values;

};

struct TableStruct
{

  std::string name;
  std::vector<TableValueStruct*> storage;
  int numberOfVariables;
  std::vector<string> variableNames;
  std::vector<string> variableTypes;

};

struct DataBaseStruct
{

  string name;
  std::vector<TableStruct*> tables;

};

struct SelectedStruct
{

  string name;
  std::vector<int> positions;
  bool all;

};

string USED;
SelectedStruct SELECTED;

////// FUNCTION DECLARATION ///////////////

std::vector<string*>* stringBreakDown(std::string& mstring);

void stringHandler(std::vector<string*>* mvector,
                   std::vector<DataBaseStruct*>* vectorDataBases,
                   ifstream& inputFile);

bool searchDataBaseNames(std::string mstring,
                         std::vector<DataBaseStruct*>* mvector);

int getTableIndex(std::string mstring, std::vector<TableStruct*>* mvector);

int getDataBaseIndex(std::string mstring,
                     std::vector<DataBaseStruct*>* mvector);

//////////////////////////////////////////////

int main(int argc, char *argv[])
{

  std::cout << std::endl << std::endl;

  std::string fileName;
  if(argc <= 1)
  {
    //std::cout << "Using default file name PA1_test.sql" << std::endl;
    fileName = "PA2_test.sql";
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
  std::vector<DataBaseStruct*> indexDataBases;
  std::string newLine;
  std::string holdString;
  std::vector<string*>* returnedStrings;
  int lineCount = 0; // Keeping track for error correction
  while(inputFile.good())
  {
    lineCount++;

    // Take a line
    getline(inputFile, newLine);

    if(inputFile.good())
    {
      // if its not a eat line then send it to lineHandler
      if(newLine[0] != '-' && newLine[0] != '\r')
      {

        std::cout << lineCount << " ";
        std::cout << "the line is " << newLine << std::endl;
        newLine.resize(newLine.size()-1);

        if(newLine.at(newLine.size()-1) == ';')
        {

          returnedStrings = stringBreakDown(newLine);
          // handle the returnedString
          stringHandler(returnedStrings, &indexDataBases, inputFile);

        }else
        {

          holdString = newLine;
          getline(inputFile, newLine);

          // else the string still has more lines to read in
          while(newLine.at(newLine.size()-1) != ';')
          {

            lineCount++;
            getline(inputFile, newLine);
            newLine.resize(newLine.size()-1); // kill end line
            holdString.append(" "); // throw a space where the end line was
            holdString.append(newLine);

          }

          returnedStrings = stringBreakDown(holdString);

        }
      }

    }

  }

  std::cout << std::endl << std::endl;

  return(0);


}

// Passing the file for multiple line commands. Determine what we are going for then modify the algorithm based on the first line.
void stringHandler(std::vector<string*>* mvector,
                   std::vector<DataBaseStruct*>* vectorDataBases,
                   ifstream& inputFile)
{

  DataBaseStruct* newDataBaseStruct;
  string* stringHold = mvector->at(0);
  int intHold;
  bool exist = false;
  bool loopEnd = true;

  if(*stringHold == "CREATE")
  {
    stringHold = mvector->at(1);
    if(*stringHold == "DATABASE")
    {
      stringHold = mvector->at(2);
      // Does this database already exist?
      // NO? then we can create the database
      if(!searchDataBaseNames(*stringHold, vectorDataBases))
      {
        newDataBaseStruct = new DataBaseStruct();
        newDataBaseStruct->name = *stringHold;
        vectorDataBases->push_back(newDataBaseStruct);
        if(debugging)
        {
          std::cout << "Database " << *stringHold << " created." << std::endl;
        }
      }else
      {
          std::cout << "Database " << *stringHold << " could not be created due to duplicate." << std::endl;
      }


    }else
    if(*stringHold == "TABLE")
    {
      stringHold = mvector->at(2); // get the table name

      // Locate the database to add a table to it
      int dataBaseIndex = getDataBaseIndex(USED, vectorDataBases);
      // Using the dataBaseIndex find the table
      int tableIndex = getTableIndex(*stringHold, &vectorDataBases->at(dataBaseIndex)->tables);

      // If the tableIndex is -1 then create the table and place it into the database
      if(tableIndex == -1)
      {

        TableStruct* newTable = new TableStruct();
        newTable->numberOfVariables = 0; // None ATM
        // set name
        newTable->name = *stringHold;
        // build a way to handle x amount of variables
        // Handle the first value
        stringHold = mvector->at(3); // grab for next part
        *stringHold = stringHold->substr(1, stringHold->size()); // eat (
        newTable->variableNames.push_back(*stringHold);
        newTable->numberOfVariables++;
        // set the type for first value
        intHold = 4;
        stringHold = mvector->at(intHold);
        *stringHold = stringHold->substr(0, stringHold->size()-1); // eat , or )
        newTable->variableTypes.push_back(*stringHold);
        intHold++;

        // Now for the loop
        while(loopEnd)
        {
          newTable->numberOfVariables++;
          // Handle Name of variable
          stringHold = mvector->at(intHold);
          intHold++;
          newTable->variableNames.push_back(*stringHold);
          // Handle Type of variable
          stringHold = mvector->at(intHold);
          intHold++;
          // Check if this is last variable
          if(stringHold->at(stringHold->size()-1) == ')')
          {
            // Since its ')' this is end
            loopEnd = !loopEnd;
          }

          // Now modify and place like usual
          *stringHold = stringHold->substr(0, stringHold->size()-1); // eat , or )
          newTable->variableTypes.push_back(*stringHold);
        }

        vectorDataBases->at(dataBaseIndex)->tables.push_back(newTable);
        if(debugging)
        {
          std::cout << "Table " << newTable->name << " created." << std::endl;
        }

      }else
      {
        // Table already exists
        std::cout << "ERROR: Cannot CREATE a table that already exists" << std::endl;
      }

    }else
    {

      // Can't create whatever was sent to this function
      std::cout << "ERROR(stringHandler): Cannot CREATE a " << *stringHold << std::endl;

    }
  }else
  if(*stringHold == "USE")
  {
    stringHold = mvector->at(1);
    // If the DB exist then you can use it
    if(searchDataBaseNames(*stringHold, vectorDataBases))
    {
      USED = *stringHold;
      std::cout << "Using database " << *stringHold << std::endl;
    }else
    {
      std::cout << "Error: Cannot use a database that does not exist." << std::endl;
    }
  }else
  if(*stringHold == "insert")
  {
    stringHold = mvector->at(1); // Take in into
    stringHold = mvector->at(2); // Which table?
    intHold = 3;

    // Locate the database to add a table to it
    int dataBaseIndex = getDataBaseIndex(USED, vectorDataBases);
    // Using the dataBaseIndex find the table
    int tableIndex = getTableIndex(*stringHold, &vectorDataBases->at(dataBaseIndex)->tables);

    TableValueStruct* newTableValues;
    if(tableIndex != -1) // it exists
    {
      newTableValues = new TableValueStruct();
      // Time to into all the values into the tables
      stringHold = mvector->at(intHold);
      *stringHold = stringHold->substr(7, stringHold->size());
      stringHold->resize(stringHold->size()-1); // just clean first value
      newTableValues->values.push_back(*stringHold);
      intHold++;

      // After handling the first value, handle the others with a loop
      while(loopEnd)
      {
        newTableValues = new TableValueStruct();
        // Grab next value to run with
        stringHold = mvector->at(intHold);
        intHold++;

        // check if finished
        if(stringHold->at(stringHold->size()-1) == ')')
        {
          // Since its ')' this is end
          loopEnd = !loopEnd;
        }

        // Resize the string by 1
        stringHold->resize(stringHold->size()-1);

        vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.push_back(newTableValues);
      }

      std::cout << "1 new record inserted." << std::endl;

    }else
    {
      std::cout << "ERROR: Cannot insert into a table that doesn't exist." << std::endl;
    }


  }else
  if(*stringHold == "select")
  {
    // What to select?
    stringHold = mvector->at(1);

    if(*stringHold == "*")// grab all values
    {
      SELECTED.all = true;
      stringHold = mvector->at(2); // from
      stringHold = mvector->at(3); // name of TableStruct

      // Locate the database to add a table to it
      int dataBaseIndex = getDataBaseIndex(USED, vectorDataBases);
      // Using the dataBaseIndex find the table
      int tableIndex = getTableIndex(*stringHold, &vectorDataBases->at(dataBaseIndex)->tables);

      // We now have the location of, for our example, Product
      if(tableIndex == -1) // couldn't find it
      {

        std::cout << "ERROR: Cannot select a table that does not exist." << std::endl;

      }else
      {

        SELECTED.name = *stringHold;

        // else, selected all so lets output for that
        int index;
        for(index = 0;
            index < (vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables-1);
            index++
        )
        {
          // Print each variableName and variableType
          std::cout <<
          vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableNames.at(index) << " " <<
          vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableTypes.at(index) << "|";

        }

        intHold = vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables - 1;

        std::cout <<
        vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableNames.at(intHold) << " " <<
        vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableTypes.at(intHold) << std::endl;


      }



    }else // Wasn't select *
    {

      SELECTED.all = false;
      intHold = 2;

      std::vector<string> holdStrings;
      // So whats the string then. We've got either 1 or many selects

      while(stringHold->at(stringHold->size()-1) == ',')
      {
        // Remove the comma
        stringHold->resize(stringHold->size()-1);
        // Place the item into the holdStrings
        holdStrings.push_back(*stringHold);

        *stringHold = stringHold->at(intHold);
        intHold++;

      }

      holdStrings.push_back(*stringHold);
      intHold++;

      // Take all these selected FROM who?
      /**stringHold = stringHold->at(intHold);
      intHold++;
      if(*stringHold == "from")
      {

        std::cout << "yes" << std::endl;

      }else
      {

        std::cout << "ERROR: from selected but from who?" << std::endl;

      }
*/

      // Got all the selected values. Need positions of variableNames
      // Locate the database to add a table to it
      /*
      int dataBaseIndex = getDataBaseIndex(USED, vectorDataBases);
      // Using the dataBaseIndex find the table
      int tableIndex = getTableIndex(*stringHold, &vectorDataBases->at(dataBaseIndex)->tables);
      */

      std::cout << "The int = " << intHold << std::endl;


    }

  }

}

int getDataBaseIndex(std::string mstring, std::vector<DataBaseStruct*>* mvector)
{

  int index;
  for(index = 0; index < mvector->size(); index++)
  {
    if(mvector->at(index)->name == mstring)
    {
      return index;
    }
  }

  return -1;
}

int getTableIndex(std::string mstring, std::vector<TableStruct*>* mvector)
{

  int index;
  for(index = 0; index < mvector->size(); index++)
  {
    if(mvector->at(index)->name == mstring)
    {
      return index;
    }
  }

  return -1;
}

std::vector<string*>* stringBreakDown(std::string& mstring)
{

  std::vector<string*>* stringVector = new vector<string*>();
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
    stringVector->push_back(stringHold);
    //cout << *stringHold << endl;

    // change mstring to minus the word we just took
    mstring = mstring.substr(intHold+1, mstring.size());

    //cin >> intHold;
  }

  // Handle the last word for each string
  stringHold = new string();
  *stringHold = mstring.substr(0, mstring.find_first_of(';', 0));
  stringVector->push_back(stringHold);

  return stringVector;

}

bool searchDataBaseNames(std::string mstring,
                        std::vector<DataBaseStruct*>* mvector)
{

  int index;
  for(index = 0; index < mvector->size(); index++)
  {
    if(mstring == mvector->at(index)->name)
    {
      return true; // found a database with the same name
    }
  }
  return false;

}
/*

























*/
