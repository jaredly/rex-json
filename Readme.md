# A simple cross-target JSON library for Reason/OCaml

Why would you want this library?
- you want minimal dependencies
- you want forgiving json parsing (comments, trailing commas allowed)
- you're fine with "good enough" performance

Usage:
```
let data = {|
{
  "some": "json", // with a comment!
  "more": [1,3,],
  "nested": [{
    "and": [1,2,{"stuff": 5}]
  }], // trailing commas!
}
|};
let json = Json.parse(data);
let simple = Json.get("some", json); /* == Some(String("json")) */

/** Yay get us a bind of optionals */
let (|>>) = (value, fn) => switch value { | None => None | Some(v) => fn(v) };

let stuff = json
  |> Json.get("nested")
  |>> Json.nth(0)
  |>> Json.get("and")
  |>> Json.nth(2)
  |>> Json.get("stuff"); /* == Some(Number(5.)) */

let str = Json.stringify(json); /* back to a string */
```

```
type t =
  | String(string)
  | Number(float)
  | Array(list(t))
  | Object(list((string, t)))
  | True
  | False
  | Null;
```