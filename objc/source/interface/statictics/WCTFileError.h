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

#import <WCDB/WCTError.h>

typedef NS_ENUM(NSUInteger, WCTFileOperation) {
    WCTFileOperationNotSet = 0,
    WCTFileOperationLstat = 1,
    WCTFileOperationAccess = 2,
    WCTFileOperationLink = 3,
    WCTFileOperationUnlink = 4,
    WCTFileOperationRemove = 5,
    WCTFileOperationMkdir = 6,
    WCTFileOperationGetAttribute = 7,
    WCTFileOperationSetAttribute = 8,
};

@interface WCTFileError : WCTError

@property(nonatomic, readonly) WCTFileOperation operation;

@property(nonatomic, readonly) NSString *path;

@end
