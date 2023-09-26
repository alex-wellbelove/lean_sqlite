import LeanSqlite.Database
namespace Rows
open Statement

class FromRow (a : Type) where
  fromRow : Statement → (idx : Int) → IO a

instance : FromRow Int where
  fromRow stmt i := Statement.get_int_column stmt i

instance : FromRow String where
  fromRow stmt i := Statement.get_string_column stmt i

instance : FromRow Float where
  fromRow stmt i := Statement.get_double_column stmt i

partial def getRows {a : Type} [FromRow a] (stmt: Statement) : IO (List a) := do
  let ret ← step stmt
  match ret with 
    | StepResult.StepRow => do 
      let row ← FromRow.fromRow stmt 0
      let rows ← getRows stmt
      return row :: rows
    | StepResult.StepDone => do
      close_statement stmt
      return []
