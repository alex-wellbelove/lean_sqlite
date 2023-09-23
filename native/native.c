#include <lean/lean.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

// ==============================================================================
// # Statics and Types
// ==============================================================================

/**
 * External class for SQLite3 Database.
 * 
 * This class registers `sqlite3 *` as a Lean external class.
 */
static lean_external_class *g_sqlite_db_external_class = NULL;

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

    return lean_io_result_mk_ok(sqlite_db_box(db));
}

/**
 * opaque Database.close (db : Database) : IO Unit
 */
lean_obj_res lean_sqlite3_close(b_lean_obj_arg db_obj, lean_obj_arg w)
{
    sqlite3 *db = sqlite_db_unbox(db_obj);
    int rc = sqlite3_close(db);

    if (rc != SQLITE_OK)
    {
        return lean_io_result_mk_error(get_sqlite_error(db));
    }

    return lean_io_result_mk_ok(lean_box(0));
}

/**
 * opaque Database.exec (db : Database) (sql : String) : IO Unit
 */
lean_obj_res lean_sqlite3_exec(b_lean_obj_arg db_obj, b_lean_obj_arg sql_obj, lean_obj_arg w)
{
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
