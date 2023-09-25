//
// Created by qiuwenchen on 2022/8/3.
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
#include "AuxiliaryFunctionModule.hpp"
#include "FTSTokenizerUtil.hpp"
#include "Handle.hpp"
#include "HandleORMOperation.hpp"
#include "Statement.hpp"
#include "TokenizerModule.hpp"
#include "WCDBError.hpp"
#include <thread>

namespace WCDB {

class WCDB_API Database final : public HandleORMOperation {
public:
    /**
     @brief Init a database from path.
     @note  All database objects with same path share the same core. So you can create multiple database objects. WCDB will manage them automatically.
            WCDB will not generate a sqlite db handle until the first operation, which is also called as lazy initialization.
     @param path Path to your database
     */
    Database(const UnsafeStringView &path);
    Database() = delete;
    Database(const Database &);
    Database &operator=(const Database &);
    ~Database() override final;

private:
    Database(InnerDatabase *database);
    RecyclableHandle getHandleHolder(bool writeHint) override final;
    Recyclable<InnerDatabase *> getDatabaseHolder() override final;
    Recyclable<InnerDatabase *> m_databaseHolder;
    InnerDatabase *m_innerDatabase;

#pragma mark - Basic
public:
    /**
     @brief Set the tag of the database
     @Note The `WCDB::Error` generated by the database will carry its tag. You can set the same tag for related databases for classification.
     */
    void setTag(const long &tag);

    /**
     @brief Get the tag of the database. Tag is 0 by default.
     */
    long getTag() const;

    /**
     @brief Get the file path of the database
     */
    const StringView &getPath() const;

    /**
     @brief Get the most recent error for current database in the current thread.
            Since it is too cumbersome to get the error after every database operation, it‘s better to use monitoring interfaces to obtain database errors and print them to the log.
     @see   `static Database::globalTraceError()`
     @see   `Database::traceError()`
     @return WCDB::Error
     */
    const Error &getError() const;

    /**
     @brief Create a `WCDB::Handle` for current database.
     `WCDB::Handle` is a wrapper for sqlite db handle of type `sqlite3*`, and the sqlite db handle is lazy initialized and will not be actually generated until the first operation on current handle takes place.
     @note  All `WCDB::Handle` created by the current database in the current thread will share the same sqlite db handle internally, so it can avoid the deadlock between different sqlite db handles in some extreme cases.
     @return A `WCDB::Handle` object.
     */
    Handle getHandle();

    /**
     @brief Since WCDB is using lazy initialization, you can create an instance of `WCDB::Database` even the database can't open. So you can call this to check whether the database can be opened.
     @return False if an error occurs during sqlite db handle initialization.
     */
    bool canOpen() const;

    /**
     @brief Check database is already opened.
     @return isOpened
     */
    bool isOpened() const;

    /**
     Trigger on database closed.
     */
    typedef std::function<void(void)> ClosedCallback;

    /**
     @brief close the database.
            Since Multi-threaded operation is supported in WCDB, other operations in different thread can open the closed database. So this function can make sure database is closed in the "onClosed" block. All other operations will be blocked until this function returns.
            A close operation consists of 4 steps:
                1. blockade, which blocks all other operations.
                2. close, which waits until all sqlite db handles return and closes them.
                3. onClosed, which trigger the callback.
                4. unblokade, which unblocks all other opreations.
            You can simply call close: to do all steps above or call these separately.
            Since this function will wait until all sqlite db handles return, it may lead to deadlock in some bad practice. The key to avoid deadlock is to make sure all WCDB objects in current thread is dealloced. In detail:
                1. You should not keep WCDB objects, including `WCDB::Handle`, `WCDB::PreparedStatement`, `WCDB::Insert`, `WCDB::Delete`, `WCDB::Update`, `WCDB::Select`, `WCDB::MultiSelect`. These objects should not be kept. You should get them, use them, then release them(set to nil) right away.
                2. WCDB objects may not be out of its' scope.
                3. Further more, those WCDB objects may be kept by NSAutoReleasePool, which is done by ARC automatically. So you should make sure that all WCDB objects in NSAutoReleasePool is drained.
                The best practice is to call close: in sub-thread and display a loading animation in main thread.

     @param onClosed Trigger on database closed.
     */
    void close(const ClosedCallback &onClosed = nullptr);

    /**
     @brief Blockade the database.
     @see `Database::close()`
     */
    void blockade();

    /**
     @brief Check whether database is blockaded.
     @see `Database::close()`
     @return isBlockaded
     */
    bool isBlockaded() const;

    /**
     @brief Unblockade the database.
     @see `Database::close()`
     */
    void unblockade();

    /**
     @brief Checkpoint the current database with truncate mode.
     */
    bool truncateCheckpoint();

    /**
     @brief Checkpoint the current database with passive mode.
     */
    bool passiveCheckpoint();

#if defined(_WIN32)
    /**
     @brief Config the id of UI thread.
     When multiple threads compete for the lock of the database, the UI thread can acquire the lock with high priority.
     */
    static void setUIThreadId(std::thread::id uiThreadId);
#endif

#pragma mark - Monitor
public:
    /**
     Trigger when error occurs
     */
    typedef std::function<void(const Error &)> ErrorNotification;

    /**
     @brief You can register a reporter to monitor all errors.
     
         WCDB::Database::globalTraceError([](const WCDB::Error &error){
             printf("%s", error.getDescription().data());
         });
     
     @param trace closure
     @see `ErrorNotification`
     */
    static void globalTraceError(ErrorNotification trace);

    /**
     @brief You can register a reporter to monitor all errors of current database.
     @param trace closure
     @see `ErrorNotification`
     */
    void traceError(ErrorNotification trace);

    /**
     Triggered when a transaction or a normal sql ends.
     */
    typedef std::function<void(long tag, const UnsafeStringView &path, const UnsafeStringView &sql, double cost, const void *handleIdentifier)> PerformanceNotification;

    /**
     @brief You can register a tracer to monitor the performance of all SQLs.
     It returns
         1. Every SQL executed by the database.
         2. Time consuming in seconds.
         3. Tag of database.
         4. Path of database.
         5. The id of the handle executing this SQL.
     @note  You should register trace before all db operations. Global tracer and db tracer do not interfere with each other.
     
         WCDB::Database::globalTracePerformance([](long tag,
                                                   const WCDB::UnsafeStringView &path,
                                                   const WCDB::UnsafeStringView &sql,
                                                   double cost,
                                                   const void *handleIdentifier) {
             printf("Tag: %ld", tag);
             printf("Path: %s", path.data());
             printf("The handle with id %p took %f seconds to execute %s",
                    handleIdentifier,
                    cost,
                    sql.data());
         });
     
     @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
     */
    static void globalTracePerformance(PerformanceNotification trace);

    /**
     @brief You can register a tracer to monitor the performance of all SQLs executed in the current database.
     @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
     @param trace closure
     @see `PerformanceNotification`
     */
    void tracePerformance(PerformanceNotification trace);

    /**
     Triggered when a SQL is executed.
     */
    typedef std::function<void(
    long tag, const UnsafeStringView &path, const void *handleIdentifier, const UnsafeStringView &sql, const UnsafeStringView &info)>
    SQLNotification;

    /**
     @brief You can register a tracer to monitor the execution of all SQLs.
     It returns:
         1. Every SQL executed by the database.
         2. Tag of database.
         3. Path of database.
         4. The id of the handle executing this SQL.
         5. Detailed execution information of SQL. It is valid only when full sql trace is enable.
     
     @note  You should register trace before all db operations. Global tracer and db tracer do not interfere with each other.
     
         WCDB::Database::globalTraceSQL([](long tag,
                                           const WCDB::UnsafeStringView &path,
                                           const WCDB::UnsafeStringView &sql,
                                           const void *handleIdentifier
                                           const WCDB::UnsafeStringView &info) {
             printf("Tag: %ld", tag);
             printf("Path: %s", path.data());
             printf("The handle with id %p executed %s",
                    handleIdentifier,
                    sql.data());
             printf("Excution info %s", info.data());
         });
     
     @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
     @see `SQLNotification`
     @param trace closure
     */
    static void globalTraceSQL(SQLNotification trace);

    /**
     @brief You can register a tracer to monitor the execution of all SQLs executed in the current database.
     @note  You should register trace before all db operations.
     @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
     @param trace trace
     @see `SQLNotification`
     */
    void traceSQL(SQLNotification trace);

    /**
     @brief Enable to collect more SQL execution information in SQL tracer.
     @note  The detailed execution information of sql will include all bind parameters, step counts of `SELECT` statement,
        last inserted rowid of `INSERT` statement, changes of `UPDATE` and `DELETE` statements.
        These informations will be returned in the last parameter of `SQLNotification`.
     @warning Collecting these informations will significantly reduce the performance of wcdb,
        please enable it only when necessary, and disable it when unnecessary.
     @see `SQLNotification`
     @param enable enable or not.
     */
    void setFullSQLTraceEnable(bool enable);

    enum Operation : short {
        Create = 0,
        SetTag,
        OpenHandle,
    };
    /**
     Triggered when a specific event of database occurs.
     */
    typedef std::function<void(Database &,            /* database */
                               Operation,             /* type of operation*/
                               StringViewMap<Value> & /* infos about current operation */
                               )>
    DBOperationTrace;

    /**
     The following are the keys in the infos from the callback of database operation monitoring.
     */
    // The number of alive handles to the current database.
    static const StringView &MonitorInfoKeyHandleCount;
    // The time in microseconds spent to open and config the handle.
    static const StringView &MonitorInfoKeyHandleOpenTime;
    // The cpu time in microseconds spent to open and config the handle.
    static const StringView &MonitorInfoKeyHandleOpenCPUTime;
    // The memory in bytes used to store the schema in sqlite handle.
    static const StringView &MonitorInfoKeySchemaUsage;
    // Number of tables in current database.
    static const StringView &MonitorInfoKeyTableCount;
    // Number of indexes in current database.
    static const StringView &MonitorInfoKeyIndexCount;
    // Number of triggers in current database.
    static const StringView &MonitorInfoKeyTriggerCount;

    /**
     @brief You can register a tracer to these database events:
         1. creating a database object for the first time;
         2. setting a tag on the database;
         3. opening a new database handle.
     @param trace closure.
     @see `DBOperationTrace`
     */
    static void globalTraceDatabaseOperation(DBOperationTrace trace);

#pragma mark - File
public:
    /**
     @brief Remove all database-related files.
     @return True if all files are removed.
     */
    bool removeFiles();

    /**
     @brief Move all database-related files to another directory safely.
     @warning Since file operation is not atomic, There may be some accidents during this period. For example, app may crash while db file is moved to destination and wal file is not. Then none of destination and source contains the whole data. This interface can make sure all of your data is in source or destination.
     @param directory destination
     @return True if all files are moved.
     */
    bool moveFiles(const UnsafeStringView &directory);

    /**
     @brief Paths to all database-related files.
     @return paths
     */
    std::list<StringView> getPaths() const;

    /**
     @brief Get the space used by the database files.
     @return The sum of files size in bytes.
     */
    Optional<size_t> getFilesSize() const;

#pragma mark - FTS
public:
    /**
     @brief Enable to automatically merge b-tree segments of fts5 index after modifying the content in fts5 table.
     The merge operation will be performed in a separate sub-thread, and will be suspended when it detects that other threads need to modify the fts table. So it will improve the performance of modifying the fts5 table.
     Using auto-merge, WCDB can maintain only one segment per level in the fts5 index structure. So it will significantly improve the search performance of the fts5 index.
     @param flag to enable auto-merge.
     */
    void enableAutoMergeFTS5Index(bool flag);

    /**
     @brief Setup tokenizer with name for current database.
     It's recomemded to use the builtin tokenizers defined in `FTSConst.h`.
     @Note  You can set up the built-in tokenizers of sqlite and the tokenizers implemented by WCDB directly. If you want to use your custom tokenizer, you should firstly register it through `static Database::registerTokenizer()`.
     @see   `static Database::registerTokenizer()`
     */
    void addTokenizer(const UnsafeStringView &tokenize);

    /**
     @brief Register custom tokenizer.
     @Note  The custom tokenizer needs to inherit `WCDB::AbstractFTSTokenizer`.
     @param module Tokenizer module for fts. You can use WCDB::FTS3TokenizerModuleTemplate to construct your custom tokenizer as tokenizer module for fts3/4, or use WCDB::FTS5TokenizerModuleTemplate to construct for fts5.
     @param name Name of tokenizer.
     */
    static void
    registerTokenizer(const UnsafeStringView &name, const TokenizerModule &module);

    /**
     @brief Setup auxiliary function with name for current database.
     You can use the builtin tokenizers defined in `FTSConst.h`.
     @Note  You can set up the built-in auxiliary functions of sqlite and the auxiliary functions implemented by WCDB directly. If you want to use your custom auxiliary function, you should firstly register it through `static Database::registerAuxiliaryFunction()`.
     @see   `static Database::registerAuxiliaryFunction()`
     */
    void addAuxiliaryFunction(const UnsafeStringView &functionName);

    /**
     @brief Register custom auxiliary function for fts5.
     @Note  The custom auxiliary function needs to inherit `WCDB::AbstractFTS5AuxiliaryFunctionObject`.
     @param module Auxiliary function module for fts5. You can use WCDB::FTS5AuxiliaryFunctionTemplate to construct your custom auxiliary function as auxiliary function module.
     @param name Name of auxiliary function.
     */
    static void registerAuxiliaryFunction(const UnsafeStringView &name,
                                          const FTS5AuxiliaryFunctionModule &module);

    /**
     Triggered when WCDB implemented tokenizers parsing input content.
     Return true if the input unicode character is symbol.
     */
    typedef unsigned short UnicodeChar;
    typedef std::function<bool(UnicodeChar)> SymbolDetector;

    /**
     @brief Configure a symbol detector.
     @warning You should configure a symbol detector before you use WCDB implemented tokenizers.
     @param detector Symbol detector.
     @see   `SymbolDetector`.
     */
    static void configSymbolDetector(SymbolDetector detector);

    /**
     Triggered when WCDB implemented tokenizers parsing input content.
     Return the normalized unicode charecter of the input unicode character.
     */
    typedef std::function<StringView(const UnsafeStringView &)> UnicodeNormalizer;

    /**
     @brief Configure a unicode normalizer.
     @warning You should configure a unicode normalizer before you use WCDB implemented tokenizers.
     @param normalizer Unicode normalizer.
     @see   `UnicodeNormalizer`.
     */
    static void configUnicodeNormalizer(UnicodeNormalizer normalizer);

    /**
     Triggered when `WCDB::BuiltinTokenizer::Pinyin` parsing input content.
     Return the pinyin vecter of the input Chiniese character.
     */
    typedef std::function<std::vector<StringView>(const UnsafeStringView &)> PinYinConverter;

    /**
     @brief Configure a converter from Chinese characters to their pinyins.
     @warning You should config this converter before using `WCDB::BuiltinTokenizer::Pinyin`.
     @param converter pinyin converter.
     @see   `BuiltinTokenizer::PinYin`.
     */
    static void configPinyinConverter(PinYinConverter converter);

    /**
     Triggered when the WCDB implemented tokenizers with `WCDB::BuiltinTokenizer::Parameter::SimplifyChinese` parsing input content.
     Return the simplify Chiniese character of the input Chiniese character.
     */
    typedef std::function<const StringView(const UnsafeStringView &)> TraditionalChineseConverter;

    /**
     @brief Configure a converter from traditional Chinese characters to their simplified Chinese characters.
     @warning You should config this converter before using the WCDB implemented tokenizers with `BuiltinTokenizer::Parameter::SimplifyChinese`.
     @param converter traditional Chinese character converter.
     @see   `TraditionalChineseConverter`.
     */
    static void configTraditionalChineseConverter(TraditionalChineseConverter converter);

#pragma mark - Memory
    /**
     @brief Purge all free memory of this database.
     WCDB will cache and reuse some sqlite db handles to improve performance.
     The max count of free sqlite db handles is same as the number of concurrent threads supported by the hardware implementation.
     You can call it to save some memory.
     */
    void purge();

    /**
     @brief Purge all free memory of all databases.
     Note that WCDB will call this interface automatically while it receives memory warning on iOS.
     @see   `purge`
     */
    static void purgeAll();

#pragma mark - Repair
    /**
     Triggered when a database is confirmed to be corrupted.
     */
    typedef std::function<void(Database &)> CorruptionNotification;

    /**
     @brief Register database corruption notification callback.
     If the current database reports an error of `SQLITE_CORRUPT` or `SQLITE_NOTADB` during operation, WCDB will asynchronously use `PRAGMA integrity_check` to check whether this database is truely corrupted. Once confirmed, WCDB will notify you through the callback registered by this function. In the callback, you can delete the corrupted database or try to repair the database.
     @see   `Database::removeFiles()`
     @see   `Database::retrieve()`
     @param onCorrupted The callback for database corruption.
     */
    void setNotificationWhenCorrupted(CorruptionNotification onCorrupted);

    /**
     @brief Check if the current database is corrupted.
     @warning Since this function will check all the contents of the database until it finds something corrupted, it may take a lot of time to come to a conclusion.
     @return True if the current database is corrupted.
     */
    bool checkIfCorrupted();

    /**
     @brief Check if the current database is already observed corrupted by WCDB.
     @note  It only checks a internal flag of current database.
     @return True if the current database is already corrupted.
     */
    bool isAlreadyCorrupted();

    /**
     @brief Enable database to automatically backup itself after there are updates.
     The backup content mainly includes the SQL statements related to table creation and all leaf page numbers of each table in database. And the backup file name is the database file name plus "-first.material" and "-last.material" suffixes.
     @note  Auto-backup do not degrade IO performance of the database.
     @param flag to enable auto-backup.
     */
    void enableAutoBackup(bool flag);

    /**
     @brief Back up the database manually.
     @see   `Database::enableAutoBackup()`
     @return True if the database is backuped successfully.
     */
    bool backup();

    /**
     Triggered before backup of each table. Return false to skip backup of this table.
     */
    typedef std::function<bool(const UnsafeStringView &table)> BackupFilter;

    /**
     @brief Set a filter to tell which table should be backed up.
     The filter will be called for every table in database. If there are some tables that do not need to be backed up, return false when these table names are passed into the filter.
     @param tableShouldBeBackedUp block.
     @see   `BackupFilter`
     */
    void filterBackup(BackupFilter tableShouldBeBackedUp);

    /**
     @brief Move the current database to a temporary directory and create a new database at current path.
     This method is designed for conditions where the database is corrupted and cannot be repaired temporarily. It can deposit the corrupted database to a new location and create a temporary database for the user. The contents of the deposited database will be restored when you recover the database.
     @note  This function can be called multiple times without loss of data.
     @return True if new database is successfully created.
     */
    bool deposit();

    /**
     @brief Remove deposited data.
     @see   `Database::deposit()`
     @return True if all deposited data is removed.
     */
    bool removeDeposited();

    /**
     @brief Check whether there is deposited data.
     @see   `Database::deposit()`
     @return True if deposited data exists.
     */
    bool containsDeposited() const;

    /**
     Triggered when repairment progresses.
     */
    typedef std::function<void(double progress /* percentage */, double increment /* increment */)> ProgressUpdateCallback;

    /**
     @brief Recover data from a corruped db.
     If there is a valid backup of this database, most of the uncorrupted data can be recovered, otherwise WCDB will try to read all the data that can still be accessed, starting from the first page of the database.
     In the extreme case where this database is not backed up and the first page is corrupted, no data can be recovered.
     @warning Since repairing a database requires reading all uncorrupted data and writing them to a new database, it may take a lot of time to finish the job. During this period, you'd better display a loading view on the screen and present the processing percentage.
     @param onProgressUpdated block.
     @see   `ProgressUpdateCallback`
     @return Percentage of repaired data. 0 or less then 0 means data recovery failed. 1 means data is fully recovered.
     */
    double retrieve(ProgressUpdateCallback onProgressUpdated);

#pragma mark - Config
    enum CipherVersion : int {
        DefaultVersion = 0,
        Version1,
        Version2,
        Version3,
        Version4,
    };
    /**
     @brief Set cipher key for a database.
     For an encrypted database, you must call it before all other operation.
     The cipher page size defaults to 4096 in WCDB, but it defaults to 1024 in other databases. So for an existing database created by other database framework, you should set it to 1024. Otherwise, you'd better to use cipher page size with 4096 or simply call setCipherKey: interface to get better performance.
     
     @note  If your database is created with the default configuration of WCDB 1.0.x, please set cipherVersion to `CipherVersion::Version3`.
     
     @param cipherKey Cipher key.
     @param cipherPageSize Cipher Page Size
     @param cipherVersion Use the default configuration of a specific version of sqlcipher
     */
    void setCipherKey(const UnsafeData &cipherKey,
                      int cipherPageSize = 4096,
                      CipherVersion cipherVersion = CipherVersion::DefaultVersion);

    /**
     @brief Force SQLCipher to operate with the default settings consistent with that major version number as the default.
     @note  It works the same as `PRAGMA cipher_default_compatibility`.
     
     @param version The specified sqlcipher major version.
     */
    static void setDefaultCipherConfiguration(CipherVersion version);

    /**
     Configuration
     */
    using Invocation = std::function<bool(Handle &)>;
    enum Priority : int {
        Highest = std::numeric_limits<int>::min(), // Only for cipher config
        High = -100,
        Default = 0,
        Low = 100,
    };
    /**
     @brief Set config for this database.
     @note Since WCDB is a multi-handle database, an executing handle will not apply this config immediately. Instead, all handles will run this config before its next operation.
     @warning If you want to add cipher config, please use `Priority::Highest`.
     
         database.setConfig("demo", [](WCDB::Handle &handle){
             return handle.execute(WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(true));
         });
     */
    void setConfig(const UnsafeStringView &name,
                   Invocation invocation,
                   Invocation unInvocation = nullptr,
                   Priority priority = Priority::Default);

    /**
     @brief Remove a config.
     */
    void removeConfig(const UnsafeStringView &name);

    /**
    @brief Set the default directory for temporary database files. If not set, an existing directory will be selected as the temporary database files directory in the following order:
        1. TMPDIR environment value;
        2. /Var/tmp;
        3. /usr/tmp;
        4. /tmp;
        5. The current working directory (".")
    Please see: https://www.sqlite.org/tempfiles.html
    @param directory a global temporary directory.
    @return true if directory exists or create directory success.
    */
    static bool setDefaultTemporaryDirectory(const UnsafeStringView &directory);

#pragma mark - Migration
    typedef struct MigrationInfo {
        StringView table;           // Target table of migration
        StringView sourceTable;     // Source table of migration
        Expression filterCondition; // Filter condition of source table
    } MigrationInfo;

    /**
     Triggered at any time when WCDB needs to know whether a table in the current database needs to migrate data, mainly including creating a new table, reading and writing a table, and starting to migrate a new table. If the current table does not need to migrate data, you need to set the sourceTable in `WCDB::Database::MigrationInfo` to empty string.
     */
    typedef std::function<void(MigrationInfo &)> TableFilter;

    /**
     @brief Configure which tables in the current database need to migrate data, and the source table they need to migrate data from.
     Once configured, you can treat the target table as if it already has all the data of the source table, and can read and write these data through the target table. WCDB will internally convert your CRUD operations on the target table into the CRUD operations on both the target table and the source table appropriately. You neither need to be aware of the existence of the source table, nor care about the progress of data migration.
     @warning  The column definition of the target table must be exactly the same as the column definition of the source table. The database does not record the state of the migration to disk, so if you have data to migrate, you need to use this function to configure the migration before excuting any statements on current database.
     @note  If the source table is in the current database, you can set sourcePath to empty string.
     @note  If the source table is not in the current database, the database containing the source table will be attached to the current database before the migration is complete. After migration, source tables will be dropped.
     @see   `MigrationFilter`
     */
    void addMigration(const UnsafeStringView &sourcePath,
                      const UnsafeData &sourceCipher,
                      const TableFilter &filter);

    /**
     @brief Manually spend about 0.01 sec. to migrate data. You can call this function periodically until all data is migrated.
     @return True if no error occurred.
     */
    bool stepMigration();

    /**
     @brief Configure the database to automatically step migration every two seconds.
     @param flag to enable auto-migration.
     */
    void enableAutoMigration(bool flag);

    /**
     Triggered when a table or a database is migrated completely. When a table is migrated successfully, tableInfo will carry the information of the table. When a database is migrated, tableInfo has no value.
     */
    typedef std::function<void(Database &database, Optional<MigrationInfo> tableInfo)> MigratedCallback;

    /**
     @brief Register a callback for migration notification. The callback will be called when each table completes the migration.
     @param onMigrated closure
     @see   `MigratedCallback`
     */
    void setNotificationWhenMigrated(MigratedCallback onMigrated);

    /**
     @brief Check if all tables in the database has finished migration.
     @note  It only check an internal flag of database.
     @return True if all tables in the database has finished migration.
     */
    bool isMigrated() const;
#pragma mark - version
    /**
     Version of WCDB.
     */
    static const StringView getVersion();

    /**
     Source id of WCDB. It varies from each build of WCDB.
     */
    static const StringView getSourceId();
};

} //namespace WCDB
