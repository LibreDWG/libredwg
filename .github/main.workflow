workflow "Github CI" {
  on = "push"
  resolves = ["testall"]
}

action "testall" {
  needs = ["test", "disttest"]
  uses = "actions/action-builder/shell@master"
  runs = "make"
  args = "test"
}

action "disttest" {
  needs = ["make"]
  uses = "actions/action-builder/shell@master"
  runs = "make"
  args = "disttest"
}

action "test" {
  needs = ["make"]
  uses = "actions/action-builder/shell@master"
  runs = "make"
  args = "test"
}

action "make" {
  needs = ["cfg"]
  uses = "actions/action-builder/shell@master"
  runs = "make"
  args = "-j4 -s"
}

action "cfg" {
  needs = ["autogen"]
  uses = "actions/action-builder/shell@master"
  runs = "./configure"
  args = ""
}

action "autogen" {
  needs = ["deps"]
  uses = "actions/action-builder/shell@master"
  runs = "sh ./autogen.sh"
}

action "deps" {
  needs = "checkout"
  uses = "actions/action-builder/shell@master"
  runs = "apt-get install -y texinfo texlive libxml2 python-libxml2 swig help2man dejagnu libpcre2-dev libpcre2-32-0 libconvert-binary-c-perl"
}

action "checkout" {
  #runs-on = "ubuntu-latest"
  uses = "actions/checkout@master"
}
