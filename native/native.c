#include <lean/lean.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// ==============================================================================
// # Statics and Types
// ==============================================================================

/**
 * External class for SQLite3 Database and Statement.
 * 
 * This class registers `sqlite3 *` as a Lean external class.
 */

static lean_external_class *g_sqlite_db_external_class = NULL;
static lean_external_class *g_sqlite_stmt_external_class = NULL;

// ==============================================================================
// # Utilities
// ==============================================================================

// ## Conversion

extern lean_obj_res lean_mk_io_user_error(lean_obj_arg);

static lean_obj_res get_sqlite_error(sqlite3 *db)
{
    const char *err_msg = sqlite3_errmsg(db);
    lean_object *details = lean_mk_string(err_msg);
    return lean_mk_io_user_error(details);
}

static lean_obj_res get_stmt_error(sqlite3_stmt *stmt)
{
    const char *err_msg = sqlite3_errmsg(stmt);
    lean_object *details = lean_mk_string(err_msg);
    return lean_mk_io_user_error(details);
}

// ## SQLite3 Database Pointer Conversion

/**
 * `sqlite3 *` -> `lean_object *` conversion
 * use macro instead of function to avoid forward declaration
 */
lean_object *sqlite_db_box(sqlite3 *db)
{
    return lean_alloc_external(g_sqlite_db_external_class, db);
}

/**
 * `lean_object *` -> `sqlite3 *` conversion
 * use macro instead of function to avoid forward declaration
 */
sqlite3 *sqlite_db_unbox(lean_object *db)
{
    return (sqlite3 *)(lean_get_external_data(db));
}



/**
 * `sqlite3 *` -> `lean_object *` conversion
 * use macro instead of function to avoid forward declaration
 */
lean_object *sqlite_stmt_box(sqlite3_stmt *stmt)
{
    return lean_alloc_external(g_sqlite_stmt_external_class, stmt);
}

/**
 * `lean_object *` -> `sqlite3 *` conversion
 * use macro instead of function to avoid forward declaration
 */
sqlite3_stmt *sqlite_stmt_unbox(lean_object *db)
{
    return (sqlite3_stmt *)(lean_get_external_data(db));
}

// ==============================================================================
// # Initialization
// ==============================================================================

// ## Finalizers

/**
 * `sqlite3 *` destructor, which ensures that the database is closed when garbage collected.
 */
inline static void sqlite_db_finalizer(void *db_ptr)
{
    sqlite3 *db = (sqlite3 *)db_ptr;
    sqlite3_close(db);
}
inline static void sqlite_stmt_finalizer(void *stmt_ptr)
{
    sqlite3_stmt *stmt = (sqlite3_stmt *)stmt_ptr;
    sqlite3_finalize(stmt); 
}
/**
 * A do nothing iterator.
 */
inline static void noop_foreach(void *mod, b_lean_obj_arg fn) {}

/**
 * Initialize SQLite environment.
 * 
 * This function does the following things:
 * 1. register `sqlite3 *` class
 */
lean_obj_res lean_sqlite_initialize()
{
    g_sqlite_db_external_class = lean_register_external_class(sqlite_db_finalizer, noop_foreach);
    g_sqlite_stmt_external_class = lean_register_external_class(sqlite_db_finalizer, noop_foreach);
    printf("initialized");
    return lean_io_result_mk_ok(lean_box(0));
}

// ==============================================================================
// # External Implementation
// ==============================================================================

/**
 * opaque Database.open (path : String) : IO Database
 */
lean_obj_res lean_sqlite3_open(b_lean_obj_arg path_obj, lean_obj_arg w)
{   
    const char *path = lean_string_cstr(path_obj);
    sqlite3 *db;

    int rc = sqlite3_open(path, &db);

    if (rc != SQLITE_OK)
    {

        return lean_io_result_mk_error(get_sqlite_error(db));
    }

    printf("Opened db in native.c");
    return lean_io_result_mk_ok(sqlite_db_box(db));
}

/**
 * opaque Database.close (db : Database) : IO Unit
 */
lean_obj_res lean_sqlite3_close(b_lean_obj_arg db_obj, lean_obj_arg w)
{
    printf("Closing db\n");
    sqlite3 *db = sqlite_db_unbox(db_obj);
    int rc = sqlite3_close(db);

    if (rc != SQLITE_OK)
    {
        return lean_io_result_mk_error(get_sqlite_error(db));
    }

    return lean_io_result_mk_ok(lean_box(0));
}

/**
 * opaque Database.exec (db : Database) (sql : String) : IO Int
 */
lean_obj_res lean_sqlite3_exec(b_lean_obj_arg db_obj, b_lean_obj_arg sql_obj, lean_obj_arg w) 
{
    printf("Executing\n");
    sqlite3 *db = sqlite_db_unbox(db_obj);
    const char *sql = lean_string_cstr(sql_obj);
    char *err_msg = 0;

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        lean_object *details = lean_mk_string(err_msg);
        sqlite3_free(err_msg);
        return lean_io_result_mk_error(lean_mk_io_user_error(details));
    }

    return lean_io_result_mk_ok(lean_box(0));
}

/**
 *  NULL. The value is a NULL value.
INTEGER. The value is a signed integer, stored in 0, 1, 2, 3, 4, 6, or 8 bytes depending on the magnitude of the value.
REAL. The value is a floating point value, stored as an 8-byte IEEE floating point number.
TEXT. The value is a text string, stored using the database encoding (UTF-8, UTF-16BE or UTF-16LE).
BLOB. The value is a blob of data, stored exactly as it was input.
*/

lean_obj_res lean_stmt_prepare(b_lean_obj_arg db_obj, b_lean_obj_arg sql_obj, lean_obj_arg w) {
    sqlite3 *db = sqlite_db_unbox(db_obj);

    const char *sql = lean_string_cstr(sql_obj);
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return lean_io_result_mk_error(get_sqlite_error(db));
    }

    return lean_io_result_mk_ok(sqlite_stmt_box(stmt));
}

lean_obj_res lean_stmt_get_int_column (b_lean_obj_arg stmt_obj, b_lean_obj_arg index,  lean_obj_arg w) {
    sqlite3_stmt *stmt = sqlite_stmt_unbox(stmt_obj);
    int index_int = lean_unbox(index);
    int col = sqlite3_column_int(stmt, index_int);
    return lean_io_result_mk_ok(lean_box(col));
}

lean_obj_res lean_stmt_close (b_lean_obj_arg stmt_obj, lean_obj_arg w) {
    sqlite3_stmt *stmt = sqlite_stmt_unbox(stmt_obj);
    int rc = sqlite3_finalize(stmt); 
    if (rc != SQLITE_OK) {
        return lean_io_result_mk_error(get_stmt_error(stmt));
    }
    return lean_io_result_mk_ok(lean_box(0));
}

lean_obj_res lean_stmt_step (b_lean_obj_arg stmt_obj, lean_obj_arg w) {
    sqlite3_stmt *stmt = sqlite_stmt_unbox(stmt_obj);
    int rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        return lean_io_result_mk_ok(lean_box(0));
    }

    if (rc == SQLITE_DONE) {
        return lean_io_result_mk_ok(lean_box(1));
    }

    return lean_io_result_mk_error(get_stmt_error(stmt));
}