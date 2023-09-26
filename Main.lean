import LeanSqlite.Database
import LeanSqlite.Rows

open Database
open Statement
open Rows


def main : IO Unit := do
  IO.println s!"Hello"
  let v : Database ← (open_db "test.sqlite")
  let stmt : Statement ←  prepare v "select 2 from test_table"
  let r : List (Int) ← getRows stmt
  IO.println r
  close_statement stmt
  IO.println s!"Closed stmt"
  close v
  return ()