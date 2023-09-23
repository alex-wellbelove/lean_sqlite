all:
	leanc -o ./build/bin/lean_sqlite ./build/ir/Main.o ./build/ir/LeanSqlite/FFI/Sqlite.o ./build/lib/libffi.a -L/opt/homebrew/opt/sqlite/lib -lsqlite3
