//
//  main.cpp
//  Database3
//
//  Created by rick gessner on 3/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "CommandProcessor.hpp"
#include "DBManager.hpp"
#include "Tokenizer.hpp"  //or use RGTokenizer.hpp if you prefer...
#include "RGTokenizer.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include "Errors.hpp"


// USE: ---------------------------------------------
void showError(ECE141::StatusResult &aResult) {
  switch(aResult.code) {
    case ECE141::unknownCommand: std::cout << "Unknown command "; break;
    case ECE141::unknownDatabase: std::cout << "Unknown database "; break;
    case ECE141::unknownTable: std::cout << "Unknown table "; break;
    case ECE141::userTerminated: break; //ignore this...
    default: std::cout << "Unknown error"; break;
  }
  std::cout << std::endl;
}


int main(int argc, const char * argv[]) {

  ECE141::DBManager         theManager;
  ECE141::CommandProcessor  theProcessor(&theManager);
  ECE141::StatusResult      theResult{};
  
  if(argc>1) {
    const char *thePath = "/tmp/assignment3.sql";
    std::ifstream theStream;
    theStream.open(thePath); // The name of the file you set up.
    ECE141::Tokenizer theTokenizer(theStream);
    theTokenizer.tokenize();
    theResult=theProcessor.processInput(theTokenizer);
  }
  else {
    
    std::string theUserInput;
    bool running=true;
    
    std::cout << std::endl << "> ";
    while (running && std::getline(std::cin, theUserInput)) {
      if(theUserInput.length()) {
        std::stringstream theStream(theUserInput);
        ECE141::RGTokenizer theTokenizer(theStream);
        theTokenizer.tokenize();
        theResult=theProcessor.processInput(theTokenizer);
        if(!theResult) showError(theResult);

      }
      if(ECE141::userTerminated==theResult.code)
        running=false;
      
      std::cout << std::endl << "> ";
    }
  }
  
  return 0;
}

