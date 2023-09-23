import Lake
open Lake DSL

package LeanSqlite {
  precompileModules := true
}

lean_lib LeanSqlite


@[default_target]
lean_exe «lean_sqlite» {
  root := `Main
  moreLinkArgs := #["-L/opt/homebrew/opt/sqlite/lib","-lsqlite3"]
}


def cDir   := "native"
def ffiSrc := "native.c"
def ffiO   := "ffi.o"
def ffiLib := "ffi"

target ffi.o (pkg : NPackage "LeanSqlite") : FilePath := do
  let oFile := pkg.buildDir / ffiO
  let srcJob ← inputFile <| pkg.dir / cDir / ffiSrc
  buildFileAfterDep oFile srcJob fun srcFile => do
    let flags := #["-I", (← getLeanIncludeDir).toString, "-fPIC"]
    compileO ffiSrc oFile srcFile flags

extern_lib ffi (pkg : NPackage "LeanSqlite") := do
  let name := nameToStaticLib ffiLib
  let ffiO ← fetch <| pkg.target ``ffi.o
  buildStaticLib (pkg.buildDir / "lib" / name) #[ffiO]

script examples do
  let examplesDir ← ("examples2" : FilePath).readDir
  for ex in examplesDir do
    IO.println ex.path
    let o ← IO.Process.output {
      cmd := "lake"
      args := #["build"]
      cwd := ex.path
    }
    IO.println o.stderr
  return 0

script clean do
  let examplesDir ← ("examples" : FilePath).readDir
  let _ ← IO.Process.output {
      cmd := "lake"
      args := #["clean"]
  }
  for ex in examplesDir do
    let _ ← IO.Process.output {
      cmd := "lake"
      args := #["clean"]
      cwd := ex.path
    }
  return 0
