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

/// Chain call for inserting
public final class Insert {
    private let database: Database
    private var properties: [PropertyConvertible]?
    private let name: String
    private let isReplace: Bool

    init(with database: Database,
         named name: String,
         on propertyConvertibleList: [PropertyConvertible]?,
         isReplace: Bool = false) {
        self.name = name
        self.properties = propertyConvertibleList
        self.isReplace = isReplace
        self.database = database
    }

    private var conflict: ConflictAction? {
        return isReplace ? ConflictAction.Replace : nil
    }

    private lazy var statement: StatementInsert = StatementInsert()
        .insert(intoTable: name).columns(properties!).onConflict(self.conflict)
        .values(BindParameter.bindParameters(Int32(properties!.count)))

    /// Execute the insert chain call with objects.
    ///
    /// Note that it will run embedded transaction while objects.count>1.  
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameter objects: Object to be inserted
    /// - Throws: Error
    public func execute<Object: TableEncodable>(with objects: Object...) throws {
        try execute(with: objects)
    }
    public func execute<Object: WCTTableCoding>(with objects: Object...) throws {
        try execute(with: objects)
    }

    /// Execute the insert chain call with objects.
    ///
    /// Note that it will run embedded transaction while objects.count>1.  
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameter objects: Object to be inserted
    /// - Throws: Error
    public func execute<Object: TableEncodable>(with objects: [Object]) throws {
        guard objects.count > 0 else {
            ErrorBridge.report(level: .Warning, code: .Misuse, infos: [
                .tag: ErrorValue(Int(database.tag ?? 0)),
                .path: ErrorValue(database.path),
                .message: ErrorValue("Inserting with an empty/nil object")
            ])
            return
        }
        let orm = Object.CodingKeys.objectRelationalMapping
        func doInsertObject() throws {
            properties = properties ?? Object.Properties.all
            let handleStatement: HandleStatement = try database.prepare(statement)
            let encoder = TableEncoder(properties!.asCodingTableKeys(), on: handleStatement)
            if !isReplace {
                encoder.primaryKeyHash = orm.getPrimaryKey()?.stringValue.hashValue
            }

            for var object in objects {
                let isAutoIncrement = object.isAutoIncrement
                encoder.isPrimaryKeyEncoded = !isAutoIncrement
                try object.encode(to: encoder)
                try handleStatement.step()
                if !isReplace && isAutoIncrement {
                    object.lastInsertedRowID = handleStatement.lastInsertedRowID
                }
                handleStatement.reset()
            }
        }
        return objects.count == 1 ? try doInsertObject() : try database.run(transaction: doInsertObject )
    }

    public func execute<Object: WCTTableCoding>(with objects: [Object]) throws {
        guard objects.count > 0 else {
            ErrorBridge.report(level: .Warning, code: .Misuse, infos: [
                .tag: ErrorValue(Int(database.tag ?? 0)),
                .path: ErrorValue(database.path),
                .message: ErrorValue("Inserting with an empty/nil object")
            ])
            return
        }
        let handle = try database.getHandle()
        if !WCTAPIBridge.insertObjects(objects, intoTable: name,
                                       withProperties: properties?.asWCTBridgeProperties(),
                                       orReplace: isReplace,
                                       with: handle.handle) {
            throw handle.getError()
        }
    }
}

extension Insert: DatabaseRepresentable {
    /// The tag of the related database.
    public var tag: Tag? {
        return database.tag
    }

    /// The path of the related database.
    public var path: String {
        return database.path
    }
}
