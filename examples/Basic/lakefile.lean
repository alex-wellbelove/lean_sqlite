import Lake
open System Platform Lake DSL

package Basic

require LeanSqlite from ".."/".."

@[default_target]
lean_exe Main {
  moreLinkArgs := #["-L/opt/homebrew/opt/sqlite/lib","-lsqlite3"]
}