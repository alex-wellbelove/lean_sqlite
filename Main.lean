import LeanSqlite.FFI.Database

open Database
open Statement

def getRows (stmt: Statement) : IO Unit := do
  -- let ret ← step stmt
  repeat do
    let mut ret ← step stmt
    IO.println ret
    if ret == StepResult.StepDone then break
    let mut col ← (get_int_column stmt 0)
    -- let (col : Int) ← get_int_column stmt 0
    IO.println (col : Int)
    


def main : IO Unit := do
  IO.println s!"Hello"
  let v : Database ← (Sqlite.Database.open "test.sqlite")
  let stmt : Statement ←  prepare v "select value from test_table"
  getRows stmt
  close_statement stmt
  IO.println s!"Closed stmt"
  close v
  return ()