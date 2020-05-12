//
//  Storage.hpp
//  Database3
//
//  Created by rick gessner on 3/24/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <stdint.h>
#include <string>
#include <fstream>
#include "Errors.hpp"
#include <functional>
#include "Row.hpp"
#include "MemoryStream.hpp"

namespace ECE141 {
  
  
  const char kDataBlockType   = 'D';
  const char kEntityBlockType = 'E';
  const char kFreeBlockType   = 'F';
  const char kIndexBlockType  = 'I';
  
  
  struct BlockHeader {
    
    BlockHeader() : type('D') {reserved=0; extra=0; id=0;}
    
    BlockHeader(char aType) : type(aType)  {reserved=0; extra=0; id=0;}
    
    BlockHeader(const BlockHeader &aCopy) {
      type=aCopy.type;
      reserved=aCopy.reserved;
      extra=aCopy.extra;
      id=aCopy.id;
    }
    
    char      type;     //block type {[D]ata, [F]ree... }
    char      reserved; //maybe later?
    uint32_t  id;       //use this anyway you like
    uint32_t  extra;    //use this anyway you like
    
  };
  
  const size_t kPayloadSize = 1012; 
  
  //======= These are for persisting entities (tables) to storage...
  
  const size_t kStoredIdentifierLen = 15;
  
  struct PersistEntity { //a single entity
    PersistEntity() {name[0]=0; blocknum=0;};
    PersistEntity(const PersistEntity &aCopy) {
      
      size_t theLen=strlen(aCopy.name);
#if defined(__APPLE__) || defined(__linux)
      std::strncpy(name, aCopy.name, theLen);
#else
      strncpy_s(name, kStoredIdentifierLen, aCopy.name, theLen);
#endif
      name[theLen]=0;
      blocknum=aCopy.blocknum;
    }
    
    char      name[kStoredIdentifierLen+1];
    uint32_t  blocknum;
  };
  
  struct PersistEntitiesHeader {
    uint8_t           count; //how many attributes...
    uint8_t           reserved; //not used yet
  };
  
  const size_t kEntitiesPerBlock =
    (kPayloadSize-sizeof(PersistEntitiesHeader)) / sizeof(PersistEntity); //49-ish
  
  struct PersistEntities {
    PersistEntitiesHeader header;
    PersistEntity         items[kEntitiesPerBlock];
  };
  
  //======= This is for tracking free-blocks (NOT CURRENTLY USED) -- if you plan to do so...
  
  const size_t kFreeItemsPerBlock = kPayloadSize / sizeof(bool);
  struct FreeMap {
    bool items[kFreeItemsPerBlock];
  };
  
  //======== Define single block in the storage file... ==============
  
  struct Block {
    
    Block(char aType='D') : header(aType) {
      memset(data,0,sizeof(data));
    }
    
    Block(const Block &aCopy) : header(aCopy.header) {}
    
    Block(BlockHeader &aHeader) : header(aHeader), data() {}
    
    // USE: encode a key/value list into this block payload area...
    Block(const KeyValues  &aKVList) {
      //STUDENT: You need to implement this...
      ECE141::BufferWriter theWriter(data, kPayloadSize);
      theWriter << (uint8_t)aKVList.size(); //record number of fields being written...
      for(auto &thePair : aKVList) {
          theWriter << thePair.first << thePair.second;
          
//          switch(thePair.second.getType()){
//              case DataType::bool_type:{
//                  bool aBool = thePair.second;
//                  theWriter << aBool;
//                  break;
//              }
//              case DataType::float_type:{
//                  float aFloat = thePair.second;
//                  theWriter << aFloat;
//                  break;
//              }
//              case DataType::int_type:{
//                  uint32_t aInt = thePair.second;
//                  theWriter << aInt;
//                  break;
//              }
//              case DataType::varchar_type:{
//                  std::string aString = thePair.second;
//                  theWriter << aString;
//                  break;
//              }
//              case DataType::timestamp_type:{
//                  unsigned int vuint = thePair.second;
//                  theWriter << vuint;
//                  break;
//              }
//              default:
//                  break;
//              }
          }
      }
    
    
    //we use attributes[0] as table name...
    BlockHeader   header;
    union {
      char              data[kPayloadSize];
      PersistEntities   entities;
      //FreeMap         available;  //totally optional...
    };
  };
  
  //============================================================
  
  struct CreateNewStorage {};
  struct OpenExistingStorage {};
  
  class Storage;
  using StorageCallback = std::function<StatusResult(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum)>;
  
  // USE: Our main class for managing storage...
  class Storage {
  public:
    
    Storage(const std::string aName, CreateNewStorage);
    Storage(const std::string aName, OpenExistingStorage);
    //Storage(const Storage &aStorage);
    
    ~Storage();
    
    static const char* getDefaultStoragePath();
    static std::string getDatabasePath(const std::string &aDBName); //build a path to db with given name...
    
    StatusResult    loadTOC();  //get it from where you placed it in the storage file...
    StatusResult    makeEmpty(); //This works by default..
    StatusResult    setupEmptyStorage();
    uint32_t        getTotalBlockCount();

    StatusResult    readBlock(int aBlockNumber, Block &aBlock, std::size_t aBlockSize=sizeof(Block));
    StatusResult    writeBlock(int aBlockNumber, Block &aBlock);
    
    StatusResult    addBlock(Block &aBlock);
    StatusResult    releaseBlock(int aBlockNumber); //mark the block as free...
    
    StatusResult    addEntity(const std::string &aName, Block &aBlock);
    StatusResult    dropEntity(const std::string &aName);
    
    PersistEntity*  findEntityInTOC(const std::string &aName); //return NULL if not found...
    

    Storage&    eachBlock(StorageCallback aCallback);

    
  protected:
    Block           toc;
    std::fstream    stream;
    std::string     name;
    
    bool            isReady() const;
    StatusResult    findFreeBlockNum();
    
    friend class Database;
  };
  
}

#endif /* Storage_hpp */
