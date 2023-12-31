-- Init
@[extern "lean_sqlite_initialize"]
opaque sqliteInit : IO Unit
builtin_initialize sqliteInit


-- Database
namespace Database

opaque Database.Nonempty : NonemptyType
def Database : Type := Database.Nonempty.type
instance : Nonempty Database := Database.Nonempty.property


@[extern "lean_sqlite3_open"]
opaque open_db : String → IO Database
@[extern "lean_sqlite3_close"] opaque close (db : @&Database) : IO Unit
@[extern "lean_sqlite3_exec"] opaque exec (db : @&Database) (sql : String) : IO Unit

end Database

-- Statements
namespace Statement
inductive StepResult where 
  | StepRow
  | StepDone 
deriving BEq
instance : ToString StepResult where
  toString s := match s with 
    | StepResult.StepRow => "StepRow"
    | StepResult.StepDone => "StepDone"

opaque Statement.Nonempty : NonemptyType
def Statement : Type := Statement.Nonempty.type
instance : Nonempty Statement := Statement.Nonempty.property

@[extern "lean_stmt_prepare"] opaque prepare (db : @&Database) (sql : @&String) : IO Statement
@[extern "lean_stmt_close"] opaque close_statement (stmt : @&Statement)  : IO Unit
@[extern "lean_stmt_step"] opaque step (stmt : @&Statement)  : IO StepResult
@[extern "lean_stmt_get_int_column"] opaque get_int_column (stmt : @&Statement) (idx : @&I32)  : IO I32
@[extern "lean_stmt_get_string_column"] opaque get_string_column (stmt : @&Statement) (idx : @&I32)  : IO String
@[extern "lean_stmt_get_double_column"] opaque get_double_column (stmt : @&Statement) (idx : @&I32)  : IO Float

end Statement

