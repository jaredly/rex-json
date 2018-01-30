# A simple cross-target JSON library for Reason/OCaml

Works with both native and js targets (compiled with bsb-native).

Why would you want this library?
- you want minimal dependencies
- you want forgiving json parsing (comments, trailing commas allowed)
- you're fine with "good enough" performance

## Installation
Add `rex-json` to your `package.json` and your `bsconfig.json`.

## Usage:
```rust
let data = {|
{
  "some": "json", // with a comment!
  "more": [1,3,],
  "this": {
    "object": {
      "is": {
        "really": "nested"
      }
    }
  },
  "nested": [{
    "and": [1,2,{"stuff": 5}]
  }], // trailing commas!
}
|};
let json = Json.parse(data);
let simple = Json.get("some", json); /* == Some(String("json")) */

/** Yay get us a bind of optionals */
let (|>>) = Json.bind;

let stuff = json
  |> Json.get("nested")
  |>> Json.nth(0)
  |>> Json.get("and")
  |>> Json.nth(2)
  |>> Json.get("stuff")
  |>> Json.number; /* == Some(5.) */

/** Using a json path for nested objects */
let nestedObj = json |> Json.getPath("this.object.is.really"); /* Some(String("nested")) */

let str = Json.stringify(json); /* back to a string */
```

```rust
type t =
  | String(string)
  | Number(float)
  | Array(list(t))
  | Object(list((string, t)))
  | True
  | False
  | Null;
```

```rust
/* doing let (|>>) = Json.bind can be quite nice */
let bind: (option('a), 'a => 'b) => option('b);
let get: (string, t) => option(t) // object access
let nth: (int, t) => option(t) // array access

/* helpers for unwrapping `t` */
let array: t => option(list(t))
let obj: t => option(list((string, t)))

let string: t => option(string)
let number: t => option(float)
let bool t => option(bool)
let null: t => option(())
```
