
let go = (ensure) => {
  let tryparse = string => try (Some(Json.parse(string))) { | _ => None };

  let invalid = (string) => ensure(tryparse(string) == None, "Was supposed to fail: " ++ string);
  let roundtrip = (string) => ensure(tryparse(string) != None, "Was supposed to parse: " ++ string);

  /* Beware, there's a lot of invisible characters in there */
  invalid("--1");
  invalid("-42-1");
  invalid("-42+1");
  invalid("[1 true]");
  invalid("[aÂ]");
  invalid("[\"\": 1]");
  invalid("[\"\"],");
  invalid("[,1]");
  invalid("[1,,2]");
  invalid("[\"x\",,]");
  invalid("[\"x\"]]");
  invalid("[\"x\"");
  invalid("[x");
  invalid("[3[4]]");
  invalid("[ˇ]");
  invalid("[1:2]");
  invalid("[,]");
  invalid("[-]");
  invalid("[ , \"\"]");
  invalid("[\"a\", 4 ,1,");
  invalid("[1,,]");
  invalid("[\"�a\"\\f]");
  invalid("[*]");
  invalid("[\"\"");
  invalid("[1,");
  invalid("[1, 1 ,1");
  invalid("[{}");
  invalid("[fals]");
  invalid("[nul]");
  invalid("[tru]");
  invalid("[++1234]");
  invalid("[+1]");
  invalid("[+Inf]");
  invalid("[-01]");
  invalid("[-1.0.]");
  invalid("[-2.]");
  invalid("[-NaN]");
  invalid("[.-1]");
  invalid("[.2e-3]");
  invalid("[0.1.2]");
  invalid("[0.3e+]");
  invalid("[0.3e]");
  invalid("[0.e1]");
  invalid("[0E+]");
  invalid("[0E]");
  invalid("[0e+]");
  invalid("[0e]");
  invalid("[1.0e+]");
  invalid("[1.0e-]");
  invalid("[1.0e]");
  invalid("[1 000.0]");
  invalid("[1eE2]");
  invalid("[2.e+3]");
  invalid("[2.e-3]");
  invalid("[2.e3]");
  invalid("[9.e+]");
  invalid("[1+2]");
  invalid("[0x1]");
  invalid("[0x42]");
  invalid("[Inf]");
  invalid("[Infinity]");
  invalid("[0e+-1]");
  invalid("[-123.123foo]");
  invalid("[123Â]");
  invalid("[1e1Â]");
  invalid("[0Â]");
  invalid("[-Infinity]");
  invalid("[-foo]");
  invalid("[- 1]");
  invalid("[NaN]");
  invalid("[-012]");
  invalid("[-.123]");
  invalid("[-1x]");
  invalid("[1ea]");
  invalid("[1eÂ]");
  invalid("[1.]");
  invalid("[.123]");
  invalid("[Ôºë]");
  invalid("[1.2a-3]");
  invalid("[1.8011670033376514H-308]");
  invalid("[012]");
  invalid("[\"x\", truth]");
  invalid("{[: \"x\"}");
  invalid("{\"x\", null}");
  invalid("{\"x\"::\"b\"}");
  invalid("{üá®üá≠}");
  invalid("{\"a\":\"a\" 123}");
  invalid("{key: 'value'}");
  invalid("{\"a\" b}");
  invalid("{:\"b\"}");
  invalid("{\"a\" \"b\"}");
  invalid("{\"a\":");
  invalid("{\"a\"");
  invalid("{1:1}");
  invalid("{9999E9999:1}");
  invalid("{null:null,null:null}");
  invalid("{\"id\":0,,,,,}");
  invalid("{'a':0}");
  invalid("{\"a\":\"b\"}/**//");
  invalid("{\"a\":\"b\"}/");
  invalid("{\"a\":\"b\",,\"c\":\"d\"}");
  invalid("{a: \"b\"}");
  invalid("{\"a\":\"a");
  invalid("{ \"foo\" : \"bar\", \"a\" }");
  invalid("{\"a\":\"b\"}#");
  invalid(" ");
  invalid("[\"\\uD800\\\"]");
  invalid("[\"\\uD800\\u\"]");
  invalid("[\"\\uD800\\u1\"]");
  invalid("[\"\\uD800\\u1x\"]");
  invalid("[√©]");
  invalid("[\"\\\"]");
  invalid("[\"\\x00\"]");
  invalid("[\"\\\\\\\"]");
  invalid("[\"\\ \"]");
  invalid("[\"\\üåÄ\"]");
  invalid("[\"\\\"]");
  invalid("[\"\\u00A\"]");
  invalid("[\"\\uD834\\uDd\"]");
  invalid("[\"\\uD800\\uD800\\x\"]");
  invalid("[\"\\uÂ\"]");
  invalid("[\"\\a\"]");
  invalid("[\"\\uqqqq\"]");
  invalid("[\"\\Â\"]");
  invalid("[\\u0020\"asd\"]");
  invalid("[\\n]");
  invalid("\"");
  invalid("['single quote']");
  invalid("abc");
  invalid("[\"\\");
  invalid("[\"a\\fa\"]");
  invalid("[\"\t\"]");
  invalid("\"\\UA66D\"");
  invalid("\"\"x");
  /* Too long */
  invalid("<.>");
  invalid("[<null>]");
  invalid("[1]x");
  invalid("[1]]");
  invalid("[\"asd]");
  invalid("a√•");
  invalid("[True]");
  invalid("1]");
  invalid("{\"x\": true,");
  invalid("[][]");
  invalid("]");
  invalid("Ôª{}");
  invalid("Â");
  invalid("[");
  invalid("");
  invalid("[]\\0");
  invalid("2@");
  invalid("{}}");
  invalid("{\"\":");
  invalid("{\"a\":/*comment*/\"b\"}");
  invalid("{\"a\": true} \"x\"");
  invalid("['");
  invalid("[,");
  /* Too long */
  invalid("[{");
  invalid("[\"a");
  invalid("[\"a\"");
  invalid("{");
  invalid("{]");
  invalid("{,");
  invalid("{[");
  invalid("{\"a");
  invalid("{'a'");
  invalid("[\"\\{[\"\\{[\"\\{[\"\\{");
  invalid("È");
  invalid("*");
  invalid("{\"a\":\"b\"}#{}");
  invalid("[‚Å†]");
  invalid("[\\u000A\"\"]");
  invalid("[1");
  invalid("[ false, nul");
  invalid("[ true, fals");
  invalid("[ false, tru");
  invalid("{\"asd\":\"asd\"");
  invalid("√•");
  invalid("Ôªø");
  invalid("[]");
  invalid("[‚Å†]");

  roundtrip("[\"new\nline\"]");
  roundtrip("{\"π\":\"0\",}");
  roundtrip("{\"a\":\"b\"}/**/");
  roundtrip("{\"a\":\"b\"}//");
  roundtrip("[\"\",]");
  roundtrip("[1,]");
  roundtrip("{\"id\":0,}");

  roundtrip("[[] ]");
  roundtrip("[\"\"]");
  roundtrip("[]");
  roundtrip("[\"a\"]");
  roundtrip("[false]");
  roundtrip("[null, 1, \"1\", {}]");
  roundtrip("[null]");
  roundtrip("[1 ]");
  roundtrip(" [1]");
  roundtrip("[1,null,null,null,2]");
  roundtrip("[2] ");
  roundtrip("[123e65]");
  roundtrip("[0e+1]");
  roundtrip("[0e1]");
  roundtrip("[ 4]");
  roundtrip("[-0.000000000000000000000000000000000000000000000000000000000000000000000000000001]");
  roundtrip("[20e1]");
  roundtrip("[-0]");
  roundtrip("[-123]");
  roundtrip("[-1]");
  roundtrip("[1E22]");
  roundtrip("[1E-2]");
  roundtrip("[1E+2]");
  roundtrip("[123e45]");
  roundtrip("[123.456e78]");
  roundtrip("[1e-2]");
  roundtrip("[1e+2]");
  roundtrip("[123]");
  roundtrip("[123.456789]");
  roundtrip("{\"asd\":\"sdf\", \"dfg\":\"fgh\"}");
  roundtrip("{\"asd\":\"sdf\"}");
  roundtrip("{\"a\":\"b\",\"a\":\"c\"}");
  roundtrip("{\"a\":\"b\",\"a\":\"b\"}");
  roundtrip("{}");
  roundtrip("{\"\":0}");
  roundtrip("{\"foo\\u0000bar\": 42}");
  roundtrip("{ \"min\": -1.0e+28, \"max\": 1.0e+28 }");
  roundtrip("{\"x\":[{\"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}], \"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}");
  roundtrip("{\"a\":[]}");
  roundtrip("{\"title\":\"\\u041f\\u043e\\u043b\\u0442\\u043e\\u0440\\u0430 \\u0417\\u0435\\u043c\\u043b\\u0435\\u043a\\u043e\\u043f\\u0430\" }");
  roundtrip("{ \"a\": \"b\" }");
  roundtrip("[\"\\u0060\\u012a\\u12AB\"]");
  roundtrip("[\"\\uD801\\udc37\"]");
  roundtrip("[\"\\ud83d\\ude39\\ud83d\\udc8d\"]");
  roundtrip("[\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"]");
  roundtrip("[\"\\\\u0000\"]");
  roundtrip("[\"\\\"\"]");
  roundtrip("[\"a/*b*/c/*d//e\"]");
  roundtrip("[\"\\\\a\"]");
  roundtrip("[\"\\\\n\"]");
  roundtrip("[\"\\u0012\"]");
  roundtrip("[\"\\uFFFF\"]");
  roundtrip("[\"asd\"]");
  roundtrip("[ \"asd\"]");
  roundtrip("[\"\\uDBFF\\uDFFF\"]");
  roundtrip("[\"new\\u00A0line\"]");
  roundtrip("[\"�\"]");
  roundtrip("[\"𛿿\"]");
  roundtrip("[\"�\"]");
  roundtrip("[\"\\u0000\"]");
  roundtrip("[\"\\u002c\"]");
  roundtrip("[\"π\"]");
  roundtrip("[\"asd \"]");
  roundtrip("\" \"");
  roundtrip("[\"\\uD834\\uDd1e\"]");
  roundtrip("[\"\\u0821\"]");
  roundtrip("[\"\\u0123\"]");
  roundtrip("[\" \"]");
  roundtrip("[\" \"]");
  roundtrip("[\"\\u0061\\u30af\\u30EA\\u30b9\"]");
  roundtrip("[\"new\\u000Aline\"]");
  roundtrip("[\"�\"]");
  roundtrip("[\"\\uA66D\"]");
  roundtrip("[\"⍂㈴⍂\"]");
  roundtrip("[\"\\u0022\"]");
  roundtrip("[\"\\uDBFF\\uDFFE\"]");
  roundtrip("[\"\\uD83F\\uDFFE\"]");
  roundtrip("[\"\\u200B\"]");
  roundtrip("[\"\\u2064\"]");
  roundtrip("[\"\\uFDD0\"]");
  roundtrip("[\"\\uFFFE\"]");
  roundtrip("[\"\\u005C\"]");
  roundtrip("[\"€𝄞\"]");
  roundtrip("[\"a�a\"]");
  roundtrip("42");
  roundtrip("-0.1");
  roundtrip("\"asd\"");
  roundtrip("\"\"");
  roundtrip("[\"a\"]");
  roundtrip("[true]");
  roundtrip(" [] ");

  /* number */
  roundtrip("123");
  invalid("0.");
  invalid("0.x");
  invalid("0ex");
  invalid("0e-x1");
  invalid("0ex1");

  /* null */
  roundtrip("null");
  invalid("nill");
  invalid("nu1l");
  invalid("nul1");

  /* true */
  roundtrip("true");
  invalid("txue");
  invalid("trxe");
  invalid("trux");

  /* false */
  roundtrip("false");
  invalid("fxlse");
  invalid("faxse");
  invalid("falxe");
  invalid("falsx");

  /* string escapes */
  roundtrip("\"\\u1234\"");
  roundtrip("\"\\uabcd\"");
  roundtrip("\"\\uCDEF\"");
  invalid("\"\\x\"");
  invalid("\"\\u123\"");
  invalid("\"\\uabcx\"");
};