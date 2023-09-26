import LeanSqlite.Database
import LeanSqlite.Rows

open Database
open Statement
open Rows

def main : IO Unit := do
  let db : Database ← (open_db "test.sqlite")
  exec db "CREATE TABLE IF NOT EXISTS test (id INTEGER)"
  exec db "INSERT into test values (1)"
  exec db "INSERT into test values (2)"
  let stmt : Statement ←  prepare db "select id from test"
  let rows : List (Int) ← getRows stmt
  IO.println rows