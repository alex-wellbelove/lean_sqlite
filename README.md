
Lean 4 bindings to libsqlite. 
Very much untested, buyer beware.

## Installation
Add the following to your lakefile.lean, and then run `lake update && lake build`.
```lean
require LeanSqlite from "https://github.com/alex-wellbelove/lean_sqlite"
```
## API

### Database
Open a database with `let db <- Database.open_db "your_db.sqlite"`, the file will be created if it is not already there.
Once you are done with it, run `Database.close db`. 

Lean's GC should deallocate the Database automatically, but it's good practice to do so yourself anyway. 

### Basic queries

To run a basic query, you can do  `Database.exec "INSERT foo INTO bar"`.

### Getting data from queries
If you need to retrieve data from your queries, you need to first:

- Prepare your statement. 
```lean
let stmt <- Statement.prepare "SELECT user_id from users"
```
- Return the rows.
```lean
let (user_ids : List Int) <- Rows.getRows db stmt
```

`getRows` uses instances of the `FromRow` typeclass in `Rows.lean`.

You may need to define your own FromRow instance for your own data structure.

```lean
structure Todo := 
  (id : Int) 
  (sessionId : String) 
  (title : String) 

instance : FromRow Todo where
  fromRow stmt i := do
    let id ← FromRow.fromRow stmt i
    let sessionId ← FromRow.fromRow stmt (i + 1)
    let title ← FromRow.fromRow stmt (i + 2)
    pure {id := id, sessionId := sessionId, title := title}
```

N.B: Only (non-nullable) String, Int, and Double types are currently supported. 

## Compilation
The build is compiled against Mac's homebrew libsqlite, in "opt/homebrew/opt/sqlite/lib".
To compile against a different libsqlite, modify lakefile.lean.
