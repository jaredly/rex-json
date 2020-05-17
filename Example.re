// https://github.com/jaredly/rex-json#usage
let data = {|
{
  "some": "json", // with a comment!
  "more": [1,3,],
  /* also
  multi-line comment */
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

open Json.Infix;

let stuff = json
  |> Json.get("nested")
  |?> Json.nth(0)
  |?> Json.get("and")
  |?> Json.nth(2)
  |?> Json.get("stuff")
  |?> Json.number; /* == Some(5.) */

/** Using a json path for nested objects */
let nestedString = json
|> Json.getPath("this.object.is.really")
|?> Json.string; /* Some("nested") */

let str = Json.stringify(json); /* back to a string */
Js.log2(stuff, nestedString)