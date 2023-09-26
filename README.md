
Lean 4 bindings to libsqlite. 
Very much untested, buyer beware.

## API

### Database
Open a database with `let db <- Database. open_db "your_db.sqlite"`, the file will be created if it is not already there.
Once you are done with it, run `Database.close db`.

### Basic queries

To run a basic query, you can do  `Database.exec "INSERT foo INTO bar"`.

### Getting data from queries
If you need to retrieve data from your queries, you need to first:

- Prepare your statement. 
```
let stmt <- Statement.prepare "SELECT user_id from users"
```
- Return the rows.
```
let (user_ids : List Int) <- Rows.getRows db stmt
```

`getRows` uses instances of the `FromRow` typeclass in Rows.lean.

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

N.B: Only (non-nullable) String, Int, and Double types are supported. 

## Compilation
The build is compiled against Mac's homebrew libsqlite, in "opt/homebrew/opt/sqlite/lib".
To compile against a different libsqlite, modify lakefile.lean.