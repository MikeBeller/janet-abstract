(declare-project
  :name "abstract"
  :description "Permit abstract types to be created in Janet without coding in C"
  :author "Mike Beller"
  :license "MIT"
  :url "https://github.com/mikebeller/janet-abstract"
  :repo "git+https://github.com/mikebeller/janet-abstract"
  :dependencies [])


(def cflags
  (case (os/which)
    :windows []
    ["-std=c99" "-Wall" "-Wextra" "-O3"]))


(def lflags
  [])


(declare-native
  :name "abstract"
  :cflags cflags
  :lflags lflags
  :headers ["src/abstract.h"]
  :source ["src/abstract.c"])
