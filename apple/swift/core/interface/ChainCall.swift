/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import Foundation

/// ChainCall interface for inserting
public protocol InsertChainCallInterface: AnyObject {

    /// Prepare chain call for inserting of `TableEncodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table object
    ///   - table: Table name
    /// - Returns: `Insert`
    func prepareInsert<Root: TableEncodable>(of cls: Root.Type, intoTable table: String) -> Insert
    func prepareInsert<Root: WCTTableCoding>(of cls: Root.Type, intoTable table: String) -> Insert

    /// Prepare chain call for inserting or replacing of `TableEncodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table object
    ///   - table: Table name
    /// - Returns: `Insert`
    func prepareInsertOrReplace<Root: TableEncodable>(of cls: Root.Type, intoTable table: String) -> Insert
    func prepareInsertOrReplace<Root: WCTTableCoding>(of cls: Root.Type, intoTable table: String) -> Insert

    /// Prepare chain call for inserting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list 
    ///   - table: Table name
    /// - Returns: `Insert`
    func prepareInsert(on propertyConvertibleList: PropertyConvertible..., intoTable table: String) -> Insert

    /// Prepare chain call for inserting or replacing on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Returns: `Insert`
    func prepareInsertOrReplace(on propertyConvertibleList: PropertyConvertible...,
                                intoTable table: String) -> Insert

    /// Prepare chain call for inserting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Returns: `Insert`
    func prepareInsert(on propertyConvertibleList: [PropertyConvertible],
                       intoTable table: String) -> Insert

    /// Prepare chain call for inserting or replacing on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Returns: `Insert`
    func prepareInsertOrReplace(on propertyConvertibleList: [PropertyConvertible],
                                intoTable table: String) -> Insert
}

extension InsertChainCallInterface where Self: Database {
    public func prepareInsert<Root: TableEncodable>(of cls: Root.Type, intoTable table: String) -> Insert {
        return Insert(with: self, named: table, on: cls.Properties.all, isReplace: false)
    }
    public func prepareInsert<Root: WCTTableCoding>(of cls: Root.Type, intoTable table: String) -> Insert {
        return Insert(with: self, named: table, on: cls.allProperties(), isReplace: false)
    }

    public func prepareInsertOrReplace<Root: TableEncodable>(
        of cls: Root.Type,
        intoTable table: String) -> Insert {
        return Insert(with: self, named: table, on: cls.Properties.all, isReplace: true)
    }
    public func prepareInsertOrReplace<Root: WCTTableCoding>(
        of cls: Root.Type,
        intoTable table: String) -> Insert {
        return Insert(with: self, named: table, on: cls.allProperties(), isReplace: true)
    }

    public func prepareInsert(on propertyConvertibleList: PropertyConvertible...,
                              intoTable table: String) -> Insert {
        return prepareInsert(on: propertyConvertibleList, intoTable: table)
    }

    public func prepareInsertOrReplace(on propertyConvertibleList: PropertyConvertible...,
                                       intoTable table: String) -> Insert {
        return prepareInsertOrReplace(on: propertyConvertibleList, intoTable: table)
    }

    public func prepareInsert(on propertyConvertibleList: [PropertyConvertible],
                              intoTable table: String) -> Insert {
        return Insert(with: self, named: table, on: propertyConvertibleList, isReplace: false)
    }

    public func prepareInsertOrReplace(on propertyConvertibleList: [PropertyConvertible],
                                       intoTable table: String) -> Insert {
        return Insert(with: self, named: table, on: propertyConvertibleList, isReplace: true)
    }
}

/// ChainCall interface for deleting
public protocol DeleteChainCallInterface: AnyObject {

    /// Prepare chain call for deleting on specific properties
    ///
    /// - Parameter table: Table name
    /// - Returns: `Delete`
    func prepareDelete(fromTable table: String) -> Delete
}

extension DeleteChainCallInterface where Self: Database {
    public func prepareDelete(fromTable table: String) -> Delete {
        return Delete(with: self, andTableName: table)
    }
}

/// ChainCall interface for updating
public protocol UpdateChainCallInterface: AnyObject {

    /// Prepare chain call for updating on specific properties
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Update`
    func prepareUpdate(table: String, on propertyConvertibleList: PropertyConvertible...) -> Update

    /// Prepare chain call for updating on specific properties
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Update`
    func prepareUpdate(table: String, on propertyConvertibleList: [PropertyConvertible]) -> Update
}

extension UpdateChainCallInterface where Self: Database {
    public func prepareUpdate(table: String, on propertyConvertibleList: PropertyConvertible...) -> Update {
        return prepareUpdate(table: table, on: propertyConvertibleList)
    }

    public func prepareUpdate(table: String, on propertyConvertibleList: [PropertyConvertible]) -> Update {
        return Update(with: self, on: propertyConvertibleList, andTable: table)
    }
}

/// ChainCall interface for row-selecting
public protocol RowSelectChainCallInterface: AnyObject {

    /// Prepare chain call for row-selecting on specific column results
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: `ColumnResult` list
    ///   - tables: Table name list
    ///   - isDistinct: Is distinct or not
    /// - Returns: `RowSelect`
    func prepareRowSelect(on columnResultConvertibleList: ResultColumnConvertible...,
                          fromTables tables: [String],
                          isDistinct: Bool) -> RowSelect

    /// Prepare chain call for row-selecting on specific column results
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: `ColumnResult` list
    ///   - tables: Table name list
    ///   - isDistinct: Is distinct or not
    /// - Returns: `RowSelect`
    func prepareRowSelect(on columnResultConvertibleList: [ResultColumnConvertible],
                          fromTables tables: [String],
                          isDistinct: Bool) -> RowSelect

    /// Prepare chain call for row-selecting on specific column results
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: `ColumnResult` list
    ///   - tables: Table name
    ///   - isDistinct: Is distinct or not
    /// - Returns: `RowSelect`
    func prepareRowSelect(on columnResultConvertibleList: ResultColumnConvertible...,
                          fromTable table: String,
                          isDistinct: Bool) -> RowSelect

    /// Prepare chain call for row-selecting on specific column results
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: `ColumnResult` list
    ///   - tables: Table name
    ///   - isDistinct: Is distinct or not
    /// - Returns: `RowSelect`
    func prepareRowSelect(on columnResultConvertibleList: [ResultColumnConvertible],
                          fromTable table: String,
                          isDistinct: Bool) -> RowSelect
}

extension RowSelectChainCallInterface where Self: Database {
    public func prepareRowSelect(on columnResultConvertibleList: ResultColumnConvertible...,
                                 fromTables tables: [String],
                                 isDistinct: Bool = false) -> RowSelect {
        return prepareRowSelect(on: columnResultConvertibleList.isEmpty ?
            [Column.all] : columnResultConvertibleList,
                                    fromTables: tables,
                                    isDistinct: isDistinct)
    }

    public func prepareRowSelect(on columnResultConvertibleList: [ResultColumnConvertible],
                                 fromTables tables: [String],
                                 isDistinct: Bool = false) -> RowSelect {
        return RowSelect(with: self, results: columnResultConvertibleList, tables: tables, isDistinct: isDistinct)
    }

    public func prepareRowSelect(on columnResultConvertibleList: ResultColumnConvertible...,
                                 fromTable table: String,
                                 isDistinct: Bool = false) -> RowSelect {
        return prepareRowSelect(on: columnResultConvertibleList.isEmpty ?
            [Column.all] : columnResultConvertibleList,
                                    fromTable: table,
                                    isDistinct: isDistinct)
    }

    public func prepareRowSelect(on columnResultConvertibleList: [ResultColumnConvertible],
                                 fromTable table: String,
                                 isDistinct: Bool = false) -> RowSelect {
        return RowSelect(with: self,
                             results: columnResultConvertibleList,
                             tables: [table],
                             isDistinct: isDistinct)
    }
}

/// ChainCall interface for selecting
public protocol SelectChainCallInterface: AnyObject {

    /// Prepare chain call for selecting of `TableDecodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table decodable object
    ///   - table: Table name
    ///   - isDistinct: Is distinct or not
    /// - Returns: `Select`
    func prepareSelect<Root: TableDecodable>(of cls: Root.Type,
                                             fromTable table: String,
                                             isDistinct: Bool) -> Select
    func prepareSelect<Root: WCTTableCoding>(of cls: Root.Type,
                                             fromTable table: String,
                                             isDistinct: Bool) -> Select

    /// Prepare chain call for selecting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    ///   - isDistinct: Is distinct or not
    /// - Returns: `Select`
    func prepareSelect(on propertyConvertibleList: PropertyConvertible...,
                       fromTable table: String,
                       isDistinct: Bool) -> Select

    /// Prepare chain call for selecting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    ///   - isDistinct: Is distinct or not
    /// - Returns: `Select`
    func prepareSelect(on propertyConvertibleList: [PropertyConvertible],
                       fromTable table: String,
                       isDistinct: Bool) -> Select
}

extension SelectChainCallInterface where Self: Database {
    public func prepareSelect<Root: TableDecodable>(of cls: Root.Type,
                                                    fromTable table: String,
                                                    isDistinct: Bool = false) -> Select {
        return Select(with: self, on: cls.Properties.all, table: table, isDistinct: isDistinct)
    }
    public func prepareSelect<Root: WCTTableCoding>(of cls: Root.Type,
                                                    fromTable table: String,
                                                    isDistinct: Bool = false) -> Select {
        return Select(with: self, on: cls.allProperties(), table: table, isDistinct: isDistinct)
    }

    public func prepareSelect(on propertyConvertibleList: PropertyConvertible...,
                              fromTable table: String,
                              isDistinct: Bool = false) -> Select {
        return prepareSelect(on: propertyConvertibleList,
                                 fromTable: table,
                                 isDistinct: isDistinct)
    }

    public func prepareSelect(on propertyConvertibleList: [PropertyConvertible],
                              fromTable table: String,
                              isDistinct: Bool = false) -> Select {
        return Select(with: self, on: propertyConvertibleList, table: table, isDistinct: isDistinct)
    }
}

/// ChainCall interface for multi-selecting
public protocol MultiSelectChainCallInterface: AnyObject {

    /// Prepare chain call for multi-selecting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - tables: Table name list
    /// - Returns: `MultiSelect`
    func prepareMultiSelect(on propertyConvertibleList: PropertyConvertible...,
                            fromTables tables: [String]) -> MultiSelect

    /// Prepare chain call for multi-selecting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - tables: Table name list
    /// - Returns: `MultiSelect`
    func prepareMultiSelect(on propertyConvertibleList: [PropertyConvertible],
                            fromTables tables: [String]) -> MultiSelect
}

extension MultiSelectChainCallInterface where Self: Database {
    public func prepareMultiSelect(on propertyConvertibleList: PropertyConvertible...,
                                   fromTables tables: [String]) -> MultiSelect {
        return prepareMultiSelect(on: propertyConvertibleList, fromTables: tables)
    }

    public func prepareMultiSelect(on propertyConvertibleList: [PropertyConvertible],
                                   fromTables tables: [String]) -> MultiSelect {
        return MultiSelect(with: self, on: propertyConvertibleList, tables: tables)
    }
}
