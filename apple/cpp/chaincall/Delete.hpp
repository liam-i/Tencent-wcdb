//
// Created by qiuwenchen on 2022/8/31.
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

#pragma once

#include <WCDB/CPPORM.h>
#include <WCDB/ChainCall.hpp>
#include <WCDB/ValueArray.hpp>

namespace WCDB {

class Delete final : public ChainCall<StatementDelete> {
    template<class ObjectType>
    friend class TableORMOperation;
    friend class HandleORMOperation;

public:
    ~Delete() override final;

    Delete &fromTable(const UnsafeStringView &tableName);

    Delete &where(const Expression &condition);

    Delete &orders(const OrderingTerms &orders);

    Delete &limit(const Expression &limit);

    Delete &offset(const Expression &offset);

    bool execute();

protected:
    Delete(Recyclable<InnerDatabase *> databaseHolder);
};

} //namespace WCDB
