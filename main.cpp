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

struct duoString
{

  std::string tableName;
  std::string alsoName;

};

struct TableValueStruct
{

  std::vector<string> values;

};

struct TableStruct
{

  std::string name; // Product
  std::vector<TableValueStruct*> storage;
  int numberOfVariables; // 3
  std::vector<string> variableNames; // pid, name, price
  std::vector<string> variableTypes; // int, varchar(20), float

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
const bool debugging = true;

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

int getIndexStringVector(std::string mstring, std::vector<string>* mvector);

void saveData(std::vector<DataBaseStruct*> vectorDataBases);

void printDatabase(std::vector<DataBaseStruct*> vectorDataBases, std::vector<int> vectorTableIndex);

void printDataBaseSelected(std::vector<DataBaseStruct*> vectorDataBases, std::vector<int> positions);

bool intInVector(int integer, std::vector<int> vectorInt);

//////////////////////////////////////////////

int main(int argc, char *argv[])
{

  std::cout << std::endl << std::endl;

  std::string fileName;
  if(argc <= 1)
  {
    //std::cout << "Using default file name PA1_test.sql" << std::endl;
    fileName = "PA4_test.sql";
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
      if(newLine[0] != '-' && newLine[0] != '\r' && newLine != ".exit\r")
      {

        newLine.resize(newLine.size()-1);

        if(newLine.at(newLine.size()-1) == ';')
        {

          returnedStrings = stringBreakDown(newLine);
          // handle the returnedString
          stringHandler(returnedStrings, &indexDataBases, inputFile);

        }else
        {

          holdString = newLine;

          // else the string still has more lines to read in
          while(holdString.at(holdString.size()-1) != ';')
          {

            lineCount++;
            getline(inputFile, newLine);
            holdString.append(" "); // throw a space where the end line was
            newLine.resize(newLine.size()-1); // kill end line
            holdString.append(newLine);

          }

          returnedStrings = stringBreakDown(holdString);
          stringHandler(returnedStrings, &indexDataBases, inputFile);
          newLine = holdString;
        }


      }

    }

  }

  saveData(indexDataBases);

  std::cout << "All done." << std::endl;
  std::cout << std::endl << std::endl;

  return(0);


}

void saveData(std::vector<DataBaseStruct*> vectorDataBases)
{

  string fileName = "OutPutFile.txt";
  std::ofstream outputFile;
  outputFile.open(fileName.c_str());
  // Databases
  for(int dataBaseIndex = 0;
      dataBaseIndex < vectorDataBases.size();
      dataBaseIndex++)
  {
    outputFile << vectorDataBases.at(dataBaseIndex)->name << std::endl;
    // Tables
    for(int tableIndex = 0; tableIndex < vectorDataBases.at(dataBaseIndex)->tables.size(); tableIndex++)
    {
      outputFile << vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->name << std::endl;
      // Let the system know what the table consists of

      for(int x = 0; x < vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables; x++)
      {
        outputFile << vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->variableNames.at(x) << " " << vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->variableTypes.at(x) << "|";
      }

      outputFile << std::endl;

      // then all the values
      for(int valueIndex = 0; valueIndex < vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->storage.size(); valueIndex++)
      {

        // for each piece of storage we need to print all the values
        for(int y = 0; y < vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables; y++)
        {

          outputFile << vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->storage.at(valueIndex)->values.at(y);

        }
        outputFile << std::endl;

      }

    }
  }

  outputFile.close();

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



  if(*stringHold == "CREATE" || *stringHold == "create")
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
    if(*stringHold == "TABLE" || *stringHold == "table")
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

        //std::cout << *stringHold << std::endl;
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
      if(debugging)
      {
        std::cout << "Using database " << *stringHold << std::endl;
      }

    }else
    {
      std::cout << "Error: Cannot use a database that does not exist." << std::endl;
    }
  }else
  if(*stringHold == "begin")
  {
    stringHold = mvector->at(1); // begin what? transaction

    if(*stringHold == "transaction")
    {
      std::cout << "Transaction start" << std::endl;
    }

  }
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
        // Push the string into storage container
        newTableValues->values.push_back(*stringHold);

      }

      vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.push_back(newTableValues);

      if(debugging)
      {
        std::cout << "1 new record inserted." << std::endl;
      }


    }else
    {
      std::cout << "ERROR: Cannot insert into a table that doesn't exist." << std::endl;
    }


  }else
  if(*stringHold == "select")
  {
    // What to select?
    stringHold = mvector->at(1);
    bool thirdTry = false;

    if(*stringHold == "*")// grab all values
    {
      SELECTED.all = true;
      int wordLocation = 2;
      stringHold = mvector->at(wordLocation); // from
      wordLocation++;

      // Get list of tables we'll be using
      vector<duoString*> vectorDuoString;
      duoString* holdDuoString;

      stringHold = mvector->at(wordLocation); // name of TableStruct
      wordLocation++;

      // while the string hold does not == on
      while(*stringHold != "on" && *stringHold != "where")
      {

        // grab Emplyee
        holdDuoString = new duoString();
        holdDuoString->tableName = *stringHold;

        // grab the letter they are assigning it
        stringHold = mvector->at(wordLocation);
        wordLocation++;
        holdDuoString->alsoName = *stringHold;

        // grab the next word to see where we are
        stringHold = mvector->at(wordLocation);
        wordLocation++; // either inner left or new table

        // so where are we
        if(*stringHold == "inner")
        {

          stringHold = mvector->at(wordLocation);
          wordLocation++; // eat join
          stringHold = mvector->at(wordLocation);
          wordLocation++; // grab the next table

        }else
        if(*stringHold == "left")
        {

          stringHold = mvector->at(wordLocation);
          wordLocation++; // is this a table name or a command

          if(*stringHold == "outer")
          {

            thirdTry = true;

            stringHold = mvector->at(wordLocation);
            wordLocation++; // eat join

            stringHold = mvector->at(wordLocation);
            wordLocation++; // grab the next table

          }



        }

        vectorDuoString.push_back(holdDuoString);

      }
      // Now have vectorDuoString filled with the tables that will needed for the output

      // The problem is get table index is looking for where table which isn't a thing

      // Locate the database to add a table to it
      int dataBaseIndex = getDataBaseIndex(USED, vectorDataBases);

      int tableIndex = 0;
      vector<int> vectorTableIndex;
      // Have mutliple tables now so lets get them all with a vector
      for(int index = 0; index < vectorDuoString.size(); index++)
      {

        // Using the dataBaseIndex find the table
        vectorTableIndex.push_back(getTableIndex(vectorDuoString.at(index)->tableName, &vectorDataBases->at(dataBaseIndex)->tables));

        if(vectorTableIndex.at(index) == -1)
        {

          tableIndex = -1;

        }

      }



      // We now have the location of, for our example, Product
      if(tableIndex == -1) // couldn't find it
      {

        std::cout << "ERROR: Cannot select a table that does not exist." << std::endl;

      }else
      {

        SELECTED.name = *stringHold;
        // else, selected all so lets output for that

        // Now we'll need to select from many different tables
        // Collect all tables and Description letter

        stringHold = mvector->at(wordLocation);
        wordLocation++; // E.id
        string* holdOperator = mvector->at(wordLocation);
        wordLocation++; // =
        string* stringHold2 = mvector->at(wordLocation);
        wordLocation++; // S.employeeID


        int index;

        // Traverse through tables
        for(int indexTables = 0; indexTables < vectorDataBases->at(dataBaseIndex)->tables.size(); indexTables++)
        {

          // Print out each tables name
          if(intInVector(indexTables, vectorTableIndex))
          {

            // Traverse through all the tables variables
            for(index = 0;
                index < (vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->numberOfVariables-1);
                index++
            )
            {
              // Print each variableName and variableType
              if(debugging)
              {
                std::cout <<
                vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->variableNames.at(index) << " " <<
                vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->variableTypes.at(index) << "|";
                // Handling for the last variableName and variableType since the loop above needs to add "|"
                if(index+1 == (vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->numberOfVariables-1))
                {
                  std::cout <<
                  vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->variableNames.at(index+1) << " " <<
                  vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->variableTypes.at(index+1);

                }
              }
            }

            std::cout << "|";

          }

        }

        // TODO
        // std::cout << std::endl;
        printDatabase(*vectorDataBases, vectorTableIndex);
        //
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(0)->storage.at(0)->values.at(0) << "|";
        //
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(0)->storage.at(0)->values.at(1) << "|";
        //
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(1)->storage.at(0)->values.at(0) << "|";
        //
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(1)->storage.at(0)->values.at(1) << "|";
        //
        // std::cout << std::endl;
        // // seperation
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(0)->storage.at(0)->values.at(0) << "|";
        //
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(0)->storage.at(0)->values.at(1) << "|";
        //
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(1)->storage.at(1)->values.at(0) << "|";
        //
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(1)->storage.at(1)->values.at(1) << "|";
        //
        // std::cout << std::endl;
        // // seperation
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(0)->storage.at(1)->values.at(0) << "|";
        //
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(0)->storage.at(1)->values.at(1) << "|";
        //
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(1)->storage.at(2)->values.at(0) << "|";
        //
        // std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(1)->storage.at(2)->values.at(1) << "|";
        //
        // std::cout << std::endl;

        // if(thirdTry)
        // {
        //
        //   std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(0)->storage.at(2)->values.at(0) << "|";
        //
        //   std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(0)->storage.at(2)->values.at(1) << "|";
        //   std::cout << std::endl;
        // }


        // TODO: Currently the printDatabase just isn't doing what is needed of it.
        // For each of the tables
        // if the current table is one of the selected
        // within the if statement treverse through the storage and output the selected ones
        for(int indexTables = 0; indexTables < vectorDataBases->at(dataBaseIndex)->tables.size(); indexTables++)
        {

          // Print out each tables name
          if(intInVector(indexTables, vectorTableIndex))
          {

            // Traverse through all the tables variables
            for(index = 0;
                index < (vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->numberOfVariables-1);
                index++
            )
            {
              // Print each variableName and variableType
              if(debugging)
              {
                std::cout <<
                vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->variableNames.at(index) << " " <<
                vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->variableTypes.at(index) << "|";
                // Handling for the last variableName and variableType since the loop above needs to add "|"
                if(index+1 == (vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->numberOfVariables-1))
                {
                  std::cout <<
                  vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->variableNames.at(index+1) << " " <<
                  vectorDataBases->at(dataBaseIndex)->tables.at(indexTables)->variableTypes.at(index+1);

                }
              }
            }

            std::cout << "|";

          }

        }

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

        stringHold = mvector->at(intHold);
        intHold++;

      }

      holdStrings.push_back(*stringHold);

      // Take all these selected FROM who?
      stringHold = mvector->at(intHold);
      intHold++;

      if(*stringHold == "from")
      {

        // Get the name of database
        stringHold = mvector->at(intHold);
        intHold++;
        // Take that value and find the index. We'll need to convert the first letter to a cap
        stringHold->at(0) = toupper(stringHold->at(0));

        // Locate both the table and database
        int dataBaseIndex = getDataBaseIndex(USED, vectorDataBases);
        // Using the dataBaseIndex find the table
        int tableIndex = getTableIndex(*stringHold, &vectorDataBases->at(dataBaseIndex)->tables);

        // where what?
        stringHold = mvector->at(intHold);
        intHold++;

        vector<int> holdIntVector;

        // select name, price instead of * all
        // Locate the position of these
        for(int index = 0; index < holdStrings.size(); index++)
        {

          // get the position within variableNames
          holdIntVector.push_back(getIndexStringVector(holdStrings.at(index), &(vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableNames)));

        }

        int index;
        for(index = 0;
            index < (vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables-1);
            index++
        )
        {
          // Print each variableName and variableType
          if(intInVector(index, holdIntVector))
          {
            if(debugging)
            {
              std::cout <<
              vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableNames.at(index) << " " <<
              vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableTypes.at(index) << "|";
              // Handling for the last variableName and variableType since the loop above needs to add "|"
              if(index+1 == (vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables-1))
              {
                std::cout <<
                vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableNames.at(index+1) << " " <<
                vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableTypes.at(index+1) << std::endl;

              }
            }
          }
        }


        if(*stringHold == "where")
        {

          string* varName = mvector->at(intHold);
          intHold++;
          string* varOperator = mvector->at(intHold);
          intHold++;
          string* varCheck = mvector->at(intHold);
          intHold++;

          // We now have the location of, for our example, Product
          if(tableIndex == -1) // couldn't find it
          {

            std::cout << "ERROR: Cannot select a table that does not exist." << std::endl;

          }else
          {

            if(*varOperator == "!=")
            { // TODO: This needs to be done after insert is handled

              // else, selected all so lets output for that
              int index;
              for(index = 0;
                  index < (vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.size());
                  index++
              ) // Go from 0 to the size of storage
              {
                // Print each variableName and variableType


                for(
                int index2 = 0;
                index2 < vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables;
                index2++
                    )
                {
                  if(intInVector(index2, holdIntVector))
                  {
                    if(vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.at(index)->values.at(0) != *varCheck)
                    {
                      std::cout << vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.at(index)->values.at(index2) << "|";
                    }
                  }
                }
                if(vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.at(index)->values.at(0) != *varCheck)
                {
                  std::cout << std::endl;
                }
              }

            }
          }

        }else
        {

          std::cout << "no where specifed in select" << std::endl;

        }



      }else
      {

        std::cout << "ERROR: from selected but from who?" << std::endl;

      }


      // Got all the selected values. Need positions of variableNames
      // Locate the database to add a table to it
      /*
      int dataBaseIndex = getDataBaseIndex(USED, vectorDataBases);
      // Using the dataBaseIndex find the table
      int tableIndex = getTableIndex(*stringHold, &vectorDataBases->at(dataBaseIndex)->tables);
      */

      //std::cout << "The int = " << intHold << std::endl;

    }
  }else
  if(*stringHold == "delete")
  {
    stringHold = mvector->at(1);

    if(*stringHold == "from")
    {
      stringHold = mvector->at(2); // table name
      stringHold->at(0) = toupper(stringHold->at(0));

      // Locate the database to add a table to it
      int dataBaseIndex = getDataBaseIndex(USED, vectorDataBases);
      // Using the dataBaseIndex find the table
      int tableIndex = getTableIndex(*stringHold, &vectorDataBases->at(dataBaseIndex)->tables);

      stringHold = mvector->at(3); // eat where

      string* varName = mvector->at(4);
      string* varOperator = mvector->at(5);
      string* varDelete = mvector->at(6);

      // find name in variableNames
      int indexName = getIndexStringVector(*varName, &vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableNames);
      int count = 0;
      // anything = to that must be deleted
      if(*varOperator == "=")
      {

        // delete all the storages correctly
        for(int x = 0; x < vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.size(); x++)
        {
          if(vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.at(x)->values.at(indexName) == *varDelete)
          {
            count++;
            vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.erase(vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.begin()+x);
          }
        }

        // Can I have another in this?
        int indexName = getIndexStringVector(*varName, &vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableNames);
        std::cout << count << " records deleted." << std::endl;

      }else
      if(*varOperator == ">")
      {

        // delete all the storages correctly
        for(int x = 0; x < vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.size(); x++)
        {

          if(
          strtof(vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.at(x)->values.at(indexName).c_str(),0)
          >
          strtof(varDelete->c_str(),0)
          )
          {
            vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.erase(vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.begin()+x);
            count++;
          }
        }

        // Can I have another in this?
        int indexName = getIndexStringVector(*varName, &vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableNames);
        std::cout << count << " records deleted." << std::endl;

      }

    }else
    {
      std::cout << "delete, from what?" << std::endl;
    }


  }else
  if(*stringHold == "update")
  {

    int modifiedCount = 0;
    stringHold = mvector->at(1); // update what?

    // Locate the database to add a table to it
    int dataBaseIndex = getDataBaseIndex(USED, vectorDataBases);
    // Using the dataBaseIndex find the table
    int tableIndex = getTableIndex(*stringHold, &vectorDataBases->at(dataBaseIndex)->tables);

    if(tableIndex != -1) // can continue
    {

      stringHold = mvector->at(2); // set
      stringHold = mvector->at(3);
      string* setName = mvector->at(5); // set what? name of thing
      // locate name
      int indexSetName = getIndexStringVector(*stringHold, &vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableNames);

      stringHold = mvector->at(6); // where
      stringHold = mvector->at(7);
      string* whereName = mvector->at(9); // where what? name of thing
      // locate name
      int indexWhereName = getIndexStringVector(*stringHold, &vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->variableNames);

      // where

      // Now go through the storage and do as it says
      for(int x = 0; x < vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.size();x++)
      {
        if(vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.at(x)->values.at(indexWhereName) == *whereName)
        {
          vectorDataBases->at(dataBaseIndex)->tables.at(tableIndex)->storage.at(x)->values.at(indexSetName) = *setName;
          modifiedCount++;
        }
      }

      if(modifiedCount > 0)
      {

        std::cout << modifiedCount << " records modifed." << std::endl;

      }

    }else
    {

      std::cout << "ERROR: Cannot update a table that does not exist" << std::endl;

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

int getIndexStringVector(std::string mstring, std::vector<string>* mvector)
{
  int index;
  for(index = 0; index < mvector->size(); index++)
  {
    if(mvector->at(index) == mstring)
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

void printDatabase(std::vector<DataBaseStruct*> vectorDataBases, std::vector<int> vectorTableIndex)
{

  // For each database
  for(int dataBaseIndex = 0;
      dataBaseIndex < vectorDataBases.size();
      dataBaseIndex++)
  {

    // For each table
    for(int tableIndex = 0; tableIndex < vectorDataBases.at(dataBaseIndex)->tables.size(); tableIndex++)
    {
      if(intInVector(tableIndex, vectorTableIndex))
      {
      // then all the values
        for(int valueIndex = 0; valueIndex < vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->storage.size(); valueIndex++)
        {

          // for each piece of storage we need to print all the values
          for(int y = 0; y < vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables-1; y++)
          {

            std::cout << vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->storage.at(valueIndex)->values.at(y) << "|";

            // Handling for last one to be output
            if(y+1 == vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables-1)
            {
              std::cout << vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->storage.at(valueIndex)->values.at(y+1);
            }

          }

        }
        std::cout << std::endl;
      }


    }

    std::cout << std::endl;
  }
}

void printDataBaseSelected(std::vector<DataBaseStruct*> vectorDataBases, std::vector<int> positions)
{

  for(int dataBaseIndex = 0;
      dataBaseIndex < vectorDataBases.size();
      dataBaseIndex++)
  {

    // Tables
    for(int tableIndex = 0; tableIndex < vectorDataBases.at(dataBaseIndex)->tables.size(); tableIndex++)
    {

      // then all the values
      for(int valueIndex = 0; valueIndex < vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->storage.size(); valueIndex++)
      {

        // for each piece of storage we need to print all the values
        for(int y = 0; y < vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables-1; y++)
        {

          if(intInVector(y, positions))
          {
            std::cout << vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->storage.at(valueIndex)->values.at(y) << "|";

            // Handling for last one to be output
            if(y+1 == vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->numberOfVariables-1)
            {
              std::cout << vectorDataBases.at(dataBaseIndex)->tables.at(tableIndex)->storage.at(valueIndex)->values.at(y+1) << std::endl;
            }
          }

        }

      }

    }
  }

}

bool intInVector(int integer, std::vector<int> vectorInt)
{

  for(int x = 0; x < vectorInt.size(); x++)
  {

    if(vectorInt.at(x) == integer)
    {
      return true;
    }

  }

  return false;

}

/*

























*/
