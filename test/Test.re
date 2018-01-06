

let tests = ref(0);
let failures = ref(0);

let ensure = (bool, message) => {
  tests := tests^ + 1;
  if (!bool) {
    print_endline("Failed: " ++ message);
    failures := failures^ + 1;
  };
};

let report = () => {
  print_newline();
  if (failures^ == 0) {
    print_endline("Success! All " ++ string_of_int(tests^) ++ " tests passed");
    exit(0);
  } else {
    print_endline("Failures: " ++ string_of_int(failures^) ++ " / " ++ string_of_int(tests^));
    exit(1);
  }
};

open Json;
ensure(parse("123") == Number(123.), "parse number");
ensure(parse("[]") == Array([]), "empty array");
ensure(parse("[1,2,3,
]") == Array([Number(1.), Number(2.), Number(3.)]), "arr");
ensure(parse({|"awesome\"sauces"|}) == String("awesome\"sauces"), "str");
ensure(parse({|{"a": 2, "b": "3"}|}) == Object([("a", Number(2.)), ("b", String("3"))]), "obj");
ensure(parse({|{"a": 2, "b": ["3", []]}|}) == Object([("a", Number(2.)), ("b", Array([String("3"), Array([])]))]), "obj");
ensure(parse({|{"a": 2,// hello folks
 "b": "3"}|}) == Object([("a", Number(2.)), ("b", String("3"))]), "obj");


let bsconfig = {|{
  "name": "rex-json",
  "bsc-flags": "-w -27 -g",
  "warnings": {
    "number": "-40+6+7-26-27+32..39-28-44+45", // aweomse
    "error": "+8",
  },
  "sources": [ // stuff
    "./src",
    {"dir": "test", "type": "dev"}, ],
  "package-specs": ["commonjs", "es6"], // here
  "entries": [{
    "awesome": true,
    "other": false,
    "final": null,
    "backend": "native",
    "main-module": "Test"
  }],
  "refmt": 3,
}
|};

let optBind = (fn, v) => switch (v) { | None => None | Some(v) => fn(v) };

let data = parse(bsconfig);
print_endline(stringify(data));
ensure(get(["name"], data) == Some(String("rex-json")), "parsed name");
ensure(get(["warnings", "error"], data) == Some(String("+8")), "parsed deeper");
ensure(get(["entries"], data) |> optBind(nth(0)) |> optBind(get(["backend"])) == Some(String("native")), "parsed quite deep");

 report();