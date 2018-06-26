
let go = (ensure) => {
  let tryparse = string => try (Some(Json.parse(string))) { | _ => None };

  let canParse = (string) => {
    switch (Json.parse(string)) {
      | exception e => {
        ensure(false, "Was supposed to parse: " ++ string ++ " but failed with " ++ Printexc.to_string(e));
      }
      | _ => ensure(true, "")
    }
  };

  let roundTrip = (string) => {
    switch (Json.parse(string)) {
      | exception e => {
        ensure(false, "Was supposed to parse: " ++ string ++ " but failed with " ++ Printexc.to_string(e));
      }
      | result => {
        let str = Json.stringify(result);
        ensure(str == string, "Roundtrip " ++ str ++ " from " ++ string)
      }
    }
  };

  roundTrip("[\"new\\nline\"]");
  roundTrip("[\"new\\tline\"]");
  canParse("{\"π\":\"0\",}");
  canParse("{\"a\":\"b\"}/**/");
  canParse("{\"a\":\"b\"}//");
  canParse("[\"\",]");
  canParse("[1,]");
  canParse("{\"id\":0,}");

  canParse("{\"a\":/*comment*/\"b\"}");
  canParse("[[] ]");
  canParse("[\"\"]");
  canParse("[]");
  canParse("[\"a\"]");
  canParse("[false]");
  canParse("[null, 1, \"1\", {}]");
  canParse("[null]");
  canParse("[1 ]");
  canParse(" [1]");
  canParse("[1,null,null,null,2]");
  canParse("[2] ");
  canParse("[123e65]");
  canParse("[0e+1]");
  canParse("[0e1]");
  canParse("[ 4]");
  canParse("[-0.000000000000000000000000000000000000000000000000000000000000000000000000000001]");
  canParse("[20e1]");
  canParse("[-0]");
  canParse("[-123]");
  canParse("[-1]");
  canParse("[1E22]");
  canParse("[1E-2]");
  canParse("[1E+2]");
  canParse("[123e45]");
  canParse("[123.456e78]");
  canParse("[1e-2]");
  canParse("[1e+2]");
  canParse("[123]");
  canParse("[123.456789]");
  canParse("{\"asd\":\"sdf\", \"dfg\":\"fgh\"}");
  canParse("{\"asd\":\"sdf\"}");
  canParse("{\"a\":\"b\",\"a\":\"c\"}");
  canParse("{\"a\":\"b\",\"a\":\"b\"}");
  canParse("{}");
  canParse("{\"\":0}");
  canParse("{ \"min\": -1.0e+28, \"max\": 1.0e+28 }");
  canParse("{\"x\":[{\"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}], \"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}");
  canParse("{\"a\":[]}");
  canParse("{ \"a\": \"b\" }");
  roundTrip("[\"\\\"\\\\/\\b\\f\\n\\r\\t\"]");
  canParse("[\"\\\"\"]");
  canParse("[\"a/*b*/c/*d//e\"]");
  canParse("[\"\\\\a\"]");
  canParse("[\"\\\\n\"]");
  canParse("[\"asd\"]");
  canParse("[ \"asd\"]");
  canParse("[\"�\"]");
  canParse("[\"𛿿\"]");
  canParse("[\"�\"]");
  canParse("[\"π\"]");
  canParse("[\"asd \"]");
  canParse("\" \"");
  canParse("[\" \"]");
  canParse("[\" \"]");
  canParse("[\"�\"]");
  canParse("[\"⍂㈴⍂\"]");
  canParse("[\"€𝄞\"]");
  canParse("[\"a�a\"]");
  canParse("42");
  canParse("-0.1");
  canParse("\"asd\"");
  canParse("\"\"");
  canParse("[\"a\"]");
  canParse("[true]");
  canParse(" [] ");

  /* UTF-8 Strings */
  /* Only first supported in OCaml 4.06.0, ReasonML runs currently on 4.02.3  */
  /*
  roundtrip("\"\\u1234\"");
  roundtrip("\"\\uabcd\"");
  roundtrip("\"\\uCDEF\"");

  roundtrip("{\"foo\\u0000bar\": 42}");
  roundtrip("{\"title\":\"\\u041f\\u043e\\u043b\\u0442\\u043e\\u0440\\u0430 \\u0417\\u0435\\u043c\\u043b\\u0435\\u043a\\u043e\\u043f\\u0430\" }");
  roundtrip("[\"\\u0060\\u012a\\u12AB\"]");
  roundtrip("[\"\\uD801\\udc37\"]");
  roundtrip("[\"\\ud83d\\ude39\\ud83d\\udc8d\"]");
  roundtrip("[\"\\\\u0000\"]");
  roundtrip("[\"\\u0012\"]");
  roundtrip("[\"\\uFFFF\"]");
  roundtrip("[\"\\uDBFF\\uDFFF\"]");
  roundtrip("[\"new\\u00A0line\"]");
  roundtrip("[\"\\u0000\"]");
  roundtrip("[\"\\u002c\"]");
  roundtrip("[\"\\uD834\\uDd1e\"]");
  roundtrip("[\"\\u0821\"]");
  roundtrip("[\"\\u0123\"]");
  roundtrip("[\"\\u0061\\u30af\\u30EA\\u30b9\"]");
  roundtrip("[\"new\\u000Aline\"]");
  roundtrip("[\"\\uA66D\"]");
  roundtrip("[\"\\u0022\"]");
  roundtrip("[\"\\uDBFF\\uDFFE\"]");
  roundtrip("[\"\\uD83F\\uDFFE\"]");
  roundtrip("[\"\\u200B\"]");
  roundtrip("[\"\\u2064\"]");
  roundtrip("[\"\\uFDD0\"]");
  roundtrip("[\"\\uFFFE\"]");
  roundtrip("[\"\\u005C\"]");
  */

  /* number */
  roundTrip("[1]");
  roundTrip("[1.1]");
  canParse("[-2.0]");

  roundTrip("123");
  roundTrip("-123");
  roundTrip("-1.23");
  roundTrip("-100.23");

  /* null */
  roundTrip("null");

  /* true */
  roundTrip("true");

  /* false */
  roundTrip("false");

};