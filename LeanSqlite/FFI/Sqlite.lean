namespace Sqlite
/--
  Create a new `Socket` using the specified domain and type.
-/
@[extern "lean_sqlite3_open"] opaque myAdd (a : UInt32) (b : UInt32) : UInt32

