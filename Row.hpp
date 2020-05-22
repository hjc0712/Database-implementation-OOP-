//
//  Row.hpp
//  Database2
//
//  Created by rick gessner on 3/22/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Row_h
#define Row_h

#include <stdio.h>
#include <vector>
#include "Value.hpp"
#include "Attribute.hpp"
//#include "Entity.hpp"

namespace ECE141 {

	struct Block;
    class  Entity;

	class Row {
	public:
		Row(int32_t aBlockNumber=0);
		Row(const Row &aRow);
		Row(const Block &aBlock);
        Row(KeyValues &aKeyValueList); //build a row
		~Row();

		int32_t			getBlockNumber() {return blockNumber;}
		KeyValues&		getColumns() {return columns;}
		Row&			addColumn(const std::string &aString, const Value &aValue);

		bool			operator==(const Row &aCopy);

		friend class	RowCollection;
		friend class	Database;
		friend class	RecordsView;
		friend class	Filters;
        

	protected:
		int32_t       blockNumber;
		KeyValues     columns;
	};

	using RowList = std::vector<Row*>;

	class RowCollection {
	public:
		RowCollection();
		RowCollection(const RowCollection &aCopy);
		~RowCollection();

		RowList&          getRows() {return rows;}
		RowCollection&    add(Row* aRow);
        RowCollection& reorder(const PropertyList &anOrder, Entity &anEntity);

	protected:
		RowList rows;
	};

}
#endif /* Row_h */
