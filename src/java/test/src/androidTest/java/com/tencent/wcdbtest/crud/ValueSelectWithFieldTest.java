// Created by chenqiuwen on 2023/5/14.
//

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

package com.tencent.wcdbtest.crud;

import static org.junit.Assert.assertTrue;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdb.winq.ResultColumnConvertible;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.ObjectCRUDTestCase;

import org.junit.Test;

public class ValueSelectWithFieldTest extends ObjectCRUDTestCase {
    @Test
    public void testDatabaseGetValue() {
        doTestSQLs(new String[]{
                "SELECT content FROM testTable",
                "SELECT content FROM testTable WHERE id == 1",
                "SELECT COUNT(content) FROM testTable ORDER BY content ASC",
                "SELECT SUM(id) FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 0",
                "SELECT COUNT(content) FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT SUM(id) FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 0"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(database.getValue(DBTestObject.content).getText().length() > 0);
                assertTrue(database.getValue(DBTestObject.content, DBTestObject.id.eq(1)).getText().length() > 0);
                assertTrue(database.getValue(DBTestObject.content.count(), tableName, DBTestObject.content.order(Order.Asc)).getInteger() == 2);
                assertTrue(database.getValue(DBTestObject.id.sum(), tableName, DBTestObject.content.order(Order.Asc), 0).getInteger() > 0);
                assertTrue(database.getValue(DBTestObject.content.count(), tableName, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc)).getInteger() == 1);
                assertTrue(database.getValue(DBTestObject.id.sum(), tableName, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 0).getInteger() > 0);
            }
        });
    }

    @Test
    public void testDatabaseGetColumn() {
        doTestSQLs(new String[]{
                "SELECT content FROM testTable",
                "SELECT content FROM testTable WHERE id == 1",
                "SELECT id FROM testTable ORDER BY content ASC",
                "SELECT id FROM testTable ORDER BY content ASC LIMIT 1",
                "SELECT id FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 1",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(database.getOneColumn(DBTestObject.content).size() == 2);
                assertTrue(database.getOneColumn(DBTestObject.content, DBTestObject.id.eq(1)).get(0).getText().length() > 0);
                assertTrue(database.getOneColumn(DBTestObject.id, DBTestObject.content.order(Order.Asc)).size() == 2);
                assertTrue(database.getOneColumn(DBTestObject.id, DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(database.getOneColumn(DBTestObject.id, DBTestObject.content.order(Order.Asc), 1, 1).size() == 1);
                assertTrue(database.getOneColumn(DBTestObject.content, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc)).size() == 1);
                assertTrue(database.getOneColumn(DBTestObject.content, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(database.getOneColumn(DBTestObject.content, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1, 1).size() == 0);
            }
        });
    }

    @Test
    public void testDatabaseGetRow() {
        doTestSQLs(new String[]{
                "SELECT id, content FROM testTable",
                "SELECT id, content FROM testTable WHERE id == 1",
                "SELECT id, COUNT(content) FROM testTable ORDER BY content ASC",
                "SELECT SUM(id), content FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 0",
                "SELECT id, COUNT(content) FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT SUM(id), content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 0"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(database.getOneRow(DBTestObject.allFields()).length == 2);
                assertTrue(database.getOneRow(DBTestObject.allFields(), DBTestObject.id.eq(1))[0].getInteger() == 1);
                assertTrue(database.getOneRow(new ResultColumnConvertible[]{DBTestObject.id, DBTestObject.content.count()}, tableName, DBTestObject.content.order(Order.Asc))[1].getInteger() == 2);
                assertTrue(database.getOneRow(new ResultColumnConvertible[]{DBTestObject.id.sum(), DBTestObject.content}, tableName, DBTestObject.content.order(Order.Asc), 0)[0].getInteger() > 0);
                assertTrue(database.getOneRow(new ResultColumnConvertible[]{DBTestObject.id, DBTestObject.content.count()}, tableName, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc))[1].getInteger() == 1);
                assertTrue(database.getOneRow(new ResultColumnConvertible[]{DBTestObject.id.sum(), DBTestObject.content}, tableName, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 0)[0].getInteger() > 0);
            }
        });
    }

    @Test
    public void testDatabaseGetRows() {
        doTestSQLs(new String[]{
                "SELECT id, content FROM testTable",
                "SELECT id, content FROM testTable WHERE id == 1",
                "SELECT id, content FROM testTable ORDER BY content ASC",
                "SELECT id, content FROM testTable ORDER BY content ASC LIMIT 1",
                "SELECT id, content FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 1",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(database.getAllRows(DBTestObject.allFields()).size() == 2);
                assertTrue(database.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(1)).get(0)[1].getText().length() > 0);
                assertTrue(database.getAllRows(DBTestObject.allFields(), DBTestObject.content.order(Order.Asc)).size() == 2);
                assertTrue(database.getAllRows(DBTestObject.allFields(), DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(database.getAllRows(DBTestObject.allFields(), DBTestObject.content.order(Order.Asc), 1, 1).size() == 1);
                assertTrue(database.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc)).size() == 1);
                assertTrue(database.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(database.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1, 1).size() == 0);
            }
        });
    }

    @Test
    public void testTableGetValue() {
        doTestSQLs(new String[]{
                "SELECT content FROM testTable",
                "SELECT content FROM testTable WHERE id == 1",
                "SELECT COUNT(content) FROM testTable ORDER BY content ASC",
                "SELECT SUM(id) FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 0",
                "SELECT COUNT(content) FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT SUM(id) FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 0"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(table.getValue(DBTestObject.content).getText().length() > 0);
                assertTrue(table.getValue(DBTestObject.content, DBTestObject.id.eq(1)).getText().length() > 0);
                assertTrue(table.getValue(DBTestObject.content.count(), DBTestObject.content.order(Order.Asc)).getInteger() == 2);
                assertTrue(table.getValue(DBTestObject.id.sum(), DBTestObject.content.order(Order.Asc), 0).getInteger() > 0);
                assertTrue(table.getValue(DBTestObject.content.count(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc)).getInteger() == 1);
                assertTrue(table.getValue(DBTestObject.id.sum(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 0).getInteger() > 0);
            }
        });
    }

    @Test
    public void testTableGetColumn() {
        doTestSQLs(new String[]{
                "SELECT content FROM testTable",
                "SELECT content FROM testTable WHERE id == 1",
                "SELECT id FROM testTable ORDER BY content ASC",
                "SELECT id FROM testTable ORDER BY content ASC LIMIT 1",
                "SELECT id FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 1",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(table.getOneColumn(DBTestObject.content).size() == 2);
                assertTrue(table.getOneColumn(DBTestObject.content, DBTestObject.id.eq(1)).get(0).getText().length() > 0);
                assertTrue(table.getOneColumn(DBTestObject.id, DBTestObject.content.order(Order.Asc)).size() == 2);
                assertTrue(table.getOneColumn(DBTestObject.id, DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(table.getOneColumn(DBTestObject.id, DBTestObject.content.order(Order.Asc), 1, 1).size() == 1);
                assertTrue(table.getOneColumn(DBTestObject.content, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc)).size() == 1);
                assertTrue(table.getOneColumn(DBTestObject.content, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(table.getOneColumn(DBTestObject.content, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1, 1).size() == 0);
            }
        });
    }

    @Test
    public void testTableGetRow() {
        doTestSQLs(new String[]{
                "SELECT id, content FROM testTable",
                "SELECT id, content FROM testTable WHERE id == 1",
                "SELECT id, COUNT(content) FROM testTable ORDER BY content ASC",
                "SELECT SUM(id), content FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 0",
                "SELECT id, COUNT(content) FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT SUM(id), content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 0"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(table.getOneRow(DBTestObject.allFields()).length == 2);
                assertTrue(table.getOneRow(DBTestObject.allFields(), DBTestObject.id.eq(1))[0].getInteger() == 1);
                assertTrue(table.getOneRow(new ResultColumnConvertible[]{DBTestObject.id, DBTestObject.content.count()}, DBTestObject.content.order(Order.Asc))[1].getInteger() == 2);
                assertTrue(table.getOneRow(new ResultColumnConvertible[]{DBTestObject.id.sum(), DBTestObject.content}, DBTestObject.content.order(Order.Asc), 0)[0].getInteger() > 0);
                assertTrue(table.getOneRow(new ResultColumnConvertible[]{DBTestObject.id, DBTestObject.content.count()}, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc))[1].getInteger() == 1);
                assertTrue(table.getOneRow(new ResultColumnConvertible[]{DBTestObject.id.sum(), DBTestObject.content}, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 0)[0].getInteger() > 0);
            }
        });
    }

    @Test
    public void testTableGetRows() {
        doTestSQLs(new String[]{
                "SELECT id, content FROM testTable",
                "SELECT id, content FROM testTable WHERE id == 1",
                "SELECT id, content FROM testTable ORDER BY content ASC",
                "SELECT id, content FROM testTable ORDER BY content ASC LIMIT 1",
                "SELECT id, content FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 1",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(table.getAllRows(DBTestObject.allFields()).size() == 2);
                assertTrue(table.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(1)).get(0)[1].getText().length() > 0);
                assertTrue(table.getAllRows(DBTestObject.allFields(), DBTestObject.content.order(Order.Asc)).size() == 2);
                assertTrue(table.getAllRows(DBTestObject.allFields(), DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(table.getAllRows(DBTestObject.allFields(), DBTestObject.content.order(Order.Asc), 1, 1).size() == 1);
                assertTrue(table.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc)).size() == 1);
                assertTrue(table.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(table.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1, 1).size() == 0);
            }
        });
    }

    @Test
    public void testHandleGetValue() {
        doTestSQLs(new String[]{
                "SELECT content FROM testTable",
                "SELECT content FROM testTable WHERE id == 1",
                "SELECT COUNT(content) FROM testTable ORDER BY content ASC",
                "SELECT SUM(id) FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 0",
                "SELECT COUNT(content) FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT SUM(id) FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 0"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(handle.getValue(DBTestObject.content).getText().length() > 0);
                assertTrue(handle.getValue(DBTestObject.content, DBTestObject.id.eq(1)).getText().length() > 0);
                assertTrue(handle.getValue(DBTestObject.content.count(), tableName, DBTestObject.content.order(Order.Asc)).getInteger() == 2);
                assertTrue(handle.getValue(DBTestObject.id.sum(), tableName, DBTestObject.content.order(Order.Asc), 0).getInteger() > 0);
                assertTrue(handle.getValue(DBTestObject.content.count(), tableName, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc)).getInteger() == 1);
                assertTrue(handle.getValue(DBTestObject.id.sum(), tableName, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 0).getInteger() > 0);
            }
        });
    }

    @Test
    public void testHandleGetColumn() {
        doTestSQLs(new String[]{
                "SELECT content FROM testTable",
                "SELECT content FROM testTable WHERE id == 1",
                "SELECT id FROM testTable ORDER BY content ASC",
                "SELECT id FROM testTable ORDER BY content ASC LIMIT 1",
                "SELECT id FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 1",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(handle.getOneColumn(DBTestObject.content).size() == 2);
                assertTrue(handle.getOneColumn(DBTestObject.content, DBTestObject.id.eq(1)).get(0).getText().length() > 0);
                assertTrue(handle.getOneColumn(DBTestObject.id, DBTestObject.content.order(Order.Asc)).size() == 2);
                assertTrue(handle.getOneColumn(DBTestObject.id, DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(handle.getOneColumn(DBTestObject.id, DBTestObject.content.order(Order.Asc), 1, 1).size() == 1);
                assertTrue(handle.getOneColumn(DBTestObject.content, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc)).size() == 1);
                assertTrue(handle.getOneColumn(DBTestObject.content, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(handle.getOneColumn(DBTestObject.content, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1, 1).size() == 0);
            }
        });
    }

    @Test
    public void testHandleGetRow() {
        doTestSQLs(new String[]{
                "SELECT id, content FROM testTable",
                "SELECT id, content FROM testTable WHERE id == 1",
                "SELECT id, COUNT(content) FROM testTable ORDER BY content ASC",
                "SELECT SUM(id), content FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 0",
                "SELECT id, COUNT(content) FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT SUM(id), content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 0"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(handle.getOneRow(DBTestObject.allFields()).length == 2);
                assertTrue(handle.getOneRow(DBTestObject.allFields(), DBTestObject.id.eq(1))[0].getInteger() == 1);
                assertTrue(handle.getOneRow(new ResultColumnConvertible[]{DBTestObject.id, DBTestObject.content.count()}, tableName, DBTestObject.content.order(Order.Asc))[1].getInteger() == 2);
                assertTrue(handle.getOneRow(new ResultColumnConvertible[]{DBTestObject.id.sum(), DBTestObject.content}, tableName, DBTestObject.content.order(Order.Asc), 0)[0].getInteger() > 0);
                assertTrue(handle.getOneRow(new ResultColumnConvertible[]{DBTestObject.id, DBTestObject.content.count()}, tableName, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc))[1].getInteger() == 1);
                assertTrue(handle.getOneRow(new ResultColumnConvertible[]{DBTestObject.id.sum(), DBTestObject.content}, tableName, DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 0)[0].getInteger() > 0);
            }
        });
    }

    @Test
    public void testHandleGetRows() {
        doTestSQLs(new String[]{
                "SELECT id, content FROM testTable",
                "SELECT id, content FROM testTable WHERE id == 1",
                "SELECT id, content FROM testTable ORDER BY content ASC",
                "SELECT id, content FROM testTable ORDER BY content ASC LIMIT 1",
                "SELECT id, content FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 1",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(handle.getAllRows(DBTestObject.allFields()).size() == 2);
                assertTrue(handle.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(1)).get(0)[1].getText().length() > 0);
                assertTrue(handle.getAllRows(DBTestObject.allFields(), DBTestObject.content.order(Order.Asc)).size() == 2);
                assertTrue(handle.getAllRows(DBTestObject.allFields(), DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(handle.getAllRows(DBTestObject.allFields(), DBTestObject.content.order(Order.Asc), 1, 1).size() == 1);
                assertTrue(handle.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc)).size() == 1);
                assertTrue(handle.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1).size() == 1);
                assertTrue(handle.getAllRows(DBTestObject.allFields(), DBTestObject.id.eq(2), DBTestObject.content.order(Order.Asc), 1, 1).size() == 0);
            }
        });
    }
}
