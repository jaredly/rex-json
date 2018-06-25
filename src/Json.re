
/** # Json parser
 *
 * Works with bucklescript and bsb-native
 *
 * ## Basics
 *
 * ```
 * open Json.Infix; /* for the nice infix operators */
 * let raw = {|{"hello": "folks"}|};
 * let who = Json.parse(raw) |> Json.get("hello") |?> Json.string;
 * Js.log(who);
 * ```
 *
 * ## Parse & stringify
 *
 * @doc parse, stringify
 *
 * ## Accessing descendents
 *
 * @doc get, nth, getPath
 *
 * ## Coercing to types
 *
 * @doc string, number, array, obj, bool, null
 *
 * ## The JSON type
 *
 * @doc t
 *
 * ## Infix operators for easier working
 *
 * @doc Infix
 */;

type t =
  | String(string)
  | Number(float)
  | Array(list(t))
  | Object(list((string, t)))
  | True
  | False
  | Null;

let string_of_number = f => {
  let s = string_of_float(f);
  if (s.[String.length(s) - 1] == '.') {
    String.sub(s, 0, String.length(s) - 1)
  } else {
    s
  }
};

/**
 * This module is provided for easier working with optional values.
 */
let module Infix = {
  /** The "force unwrap" operator
   *
   * If you're sure there's a value, you can force it.
   * ```
   * open Json.Infix;
   * let x: int = Some(10) |! "Expected this to be present";
   * Js.log(x);
   * ```
   *
   * But you gotta be sure, otherwise it will throw.
   * ```reason;raises
   * open Json.Infix;
   * let x: int = None |! "This will throw";
   * ```
   */
  let (|!) = (o, d) => switch o { | None => failwith(d) | Some(v) => v };
  /** The "upwrap with default" operator
   * ```
   * open Json.Infix;
   * let x: int = Some(10) |? 4;
   * let y: int = None |? 5;
   * Js.log2(x, y);
   * ```
   */
  let (|?) = (o, d) => switch o { | None => d | Some(v) => v };
  /** The "transform contents into new optional" operator
   * ```
   * open Json.Infix;
   * let maybeInc = x => x > 5 ? Some(x + 1) : None;
   * let x: option(int) = Some(14) |?> maybeInc;
   * let y: option(int) = None |?> maybeInc;
   * ```
   */
  let (|?>) = (o, fn) => switch o { | None => None | Some(v) => fn(v) };
  /** The "transform contents into new value & then re-wrap" operator
   * ```
   * open Json.Infix;
   * let inc = x => x + 1;
   * let x: option(int) = Some(7) |?>> inc;
   * let y: option(int) = None |?>> inc;
   * Js.log2(x, y);
   * ```
   */
  let (|?>>) = (o, fn) => switch o { | None => None | Some(v) => Some(fn(v)) };
  /** "handle the value if present, otherwise here's the default"
   *
   * It's called fold because that's what people call it :?. It's the same as "transform contents to new value" + "unwrap with default".
   *
   * ```
   * open Json.Infix;
   * let inc = x => x + 1;
   * let x: int = fold(Some(4), 10, inc);
   * let y: int = fold(None, 2, inc);
   * Js.log2(x, y);
   * ```
   */
  let fold = (o, d, f) => switch o { | None => d | Some(v) => f(v) };
};

let escape = text => {
  let ln = String.length(text);
  let buf = Buffer.create(ln);
  let rec loop = i => {
    if (i < ln) {
      switch (text.[i]) {
      | '\\' => Buffer.add_string(buf, "\\\\")
      | '\"' => Buffer.add_string(buf, "\\\"")
      | '\n' => Buffer.add_string(buf, "\\n")
      | '\r' => Buffer.add_string(buf, "\\r")
      | '\t' => Buffer.add_string(buf, "\\t")
      | c => Buffer.add_char(buf, c)
      };
      loop(i + 1)
    }
  };
  loop(0);
  Buffer.contents(buf)
};

/** ```
 * let text = {|{"hello": "folks", "aa": [2, 3, "four"]}|};
 * let result = Json.stringify(Json.parse(text));
 * Js.log(result);
 * assert(text == result);
 * ```
 */
let rec stringify = t => switch t {
| String(value) => "\"" ++ escape(value) ++ "\""
| Number(num) => string_of_number(num)
| Array(items) => "[" ++ String.concat(", ", List.map(stringify, items)) ++ "]"
| Object(items) => "{" ++ String.concat(", ", List.map(((k, v)) => "\"" ++ String.escaped(k) ++ "\": " ++ stringify(v), items)) ++ "}"
| True => "true"
| False => "false"
| Null => "null"
};

let unwrap = (message, t) =>
  switch t {
  | Some(v) => v
  | None => failwith(message)
  };

[@nodoc]
let module Parser = {

let split_by = (~keep_empty=false, is_delim, str) => {
  let len = String.length(str);
  let rec loop = (acc, last_pos, pos) =>
    if (pos == (-1)) {
      if (last_pos == 0 && ! keep_empty) {
        acc
      } else {
        [String.sub(str, 0, last_pos), ...acc]
      }
    } else if (is_delim(str.[pos])) {
      let new_len = last_pos - pos - 1;
      if (new_len != 0 || keep_empty) {
        let v = String.sub(str, pos + 1, new_len);
        loop([v, ...acc], pos, pos - 1)
      } else {
        loop(acc, pos, pos - 1)
      }
    } else {
      loop(acc, last_pos, pos - 1)
    };
  loop([], len, len - 1)
};

let fail = (text, pos, message) => {
  let pre = String.sub(text, 0, pos);
  let lines = split_by((c) => c == '\n', pre);
  let count = List.length(lines);
  let last = count > 0 ? List.nth(lines, count - 1) : "";
  let col = String.length(last) + 1;
  let line = List.length(lines);
  let string = (Printf.sprintf("Error \"%s\" at %d:%d -> %s\n", message, line, col, last));
  failwith(string);
};

let rec skipToNewline = (text, pos) => {
  if (pos >= String.length(text)) {
    pos
  } else if (text.[pos] == '\n') {
    pos + 1
  } else {
    skipToNewline(text, pos + 1)
  }
};

let stringTail = (text) => {
  let len = String.length(text);
  if (len > 1) {
    String.sub(text, 1, len - 1);
  } else {
    ""
  }
};

let rec skipToCloseMultilineComment = (text, pos) => {
  if (pos + 1 >= String.length(text)) {
    failwith("Unterminated comment")
  } else if (text.[pos] == '*' && text.[pos + 1] == '/') {
    pos + 2
  } else {
    skipToCloseMultilineComment(text, pos + 1)
  }
};

let rec skipWhite = (text, pos) => {
  if (pos < String.length(text) && (
    text.[pos] == ' ' ||
    text.[pos] == '\t' ||
    text.[pos] == '\n' ||
    text.[pos] == '\r'
  )) {
    skipWhite(text, pos + 1)
  } else {
    pos
  }
};

let parseString = (text, pos) => {
  let i = ref(pos);
  while (text.[i^] != '"') {
    i := i^ + (text.[i^] == '\\' ? 2 : 1)
  };
  /* print_endline(text ++ string_of_int(pos)); */
  (Scanf.unescaped(String.sub(text, pos, i^ - pos)), i^ + 1)
};

let rec toCharList = (text, maxDepth) => {
  if (String.length(text) < 2 || maxDepth < 2) {
    [text.[0]]
  } else {
    [text.[0], ...toCharList(stringTail(text), maxDepth - 1)]
  }
};

let continousDigits = (text, pos, len) => {
  let i = ref(pos);
  let dec = ref(false);
  let isNumber = n => switch (text.[n]) {
  | '0'..'9' => true
  | _ => false
  };
  let isDec = n => dec^ == false && '.' == text.[n];

  while (i^ < len && (isNumber(i^) || isDec(i^))) {
    if (isDec(i^)) {
      dec := true;
    };
    i := i^ + 1;
  };

  let s = String.sub(text, pos, i^ - pos);
  (s, i^)
};

let parseNumber = (text, pos) => {
  let len = String.length(text);
  let next = String.sub(text, pos, len - pos);

  switch (toCharList(next, 3)) {
  | ['0', ..._]  =>  (Number(0.), pos + 1)
  | ['-', '0', ..._]  => (Number(-0.), pos + 2)
  | ['-', '1'..'9', ..._]  =>  {
    let (value, pos) = continousDigits(text, pos + 1, len);
    (Number(float_of_string(value) *. -1.), pos)
  }
  | ['1'..'9', ..._]=> {
    let (value, pos) = continousDigits(text, pos, len);
    (Number(float_of_string(value)), pos)
  }
  | _ => fail(text, pos, "Could not parse number")
  }
};

let expect = (char, text, pos, message) => {
  if (text.[pos] != char) {
    fail(text, pos, "Expected: " ++ message)
  } else {
    pos + 1
  }
};

let parseComment: 'a . (string, int, (string, int) => 'a) => 'a = (text, pos, next) => {
  if (text.[pos] != '/') {
    if (text.[pos] == '*') {
      next(text, skipToCloseMultilineComment(text, pos + 1))
    } else {
      failwith("Invalid syntax")
    }
  } else {
    next(text, skipToNewline(text, pos + 1))
  }
};

let maybeSkipComment = (text, pos) => {
  if (pos < String.length(text) && text.[pos] == '/') {
    if (pos + 1 < String.length(text) && text.[pos + 1] == '/') {
      skipToNewline(text, pos + 1)
    } else if (pos + 1 < String.length(text) && text.[pos + 1] == '*') {
      skipToCloseMultilineComment(text, pos + 1)
    } else {
      fail(text, pos, "Invalid synatx")
    }
  } else {
    pos
  }
};

let rec skip = (text, pos) => {
  if (pos == String.length(text)) {
    pos
  } else {
    let n = skipWhite(text, pos) |> maybeSkipComment(text);
    if (n > pos) skip(text, n)
    else n
  }
};

let rec parse = (text, pos) => {
  if (pos >= String.length(text)) {
    fail(text, pos, "Reached end of file without being done parsing")
  } else {
    switch (text.[pos]) {
    | '/' => parseComment(text, pos + 1, parse)
    | '[' => parseArray(text, pos + 1)
    | '{' => parseObject(text, pos + 1)
    | 'n' => {
      if (String.sub(text, pos, 4) == "null") {
        (Null, pos + 4)
      } else {
        fail(text, pos, "unexpected character")
      }
    }
    | 't' => {
      if (String.sub(text, pos, 4) == "true") {
        (True, pos + 4)
      } else {
        fail(text, pos, "unexpected character")
      }
    }
    | 'f' => {
      if (String.sub(text, pos, 5) == "false") {
        (False, pos + 5)
      } else {
        fail(text, pos, "unexpected character")
      }
    }
    | '\n' | '\t' | ' ' | '\r' => parse(text, skipWhite(text, pos))
    | '"' => {
      let (s, pos) = parseString(text, pos + 1);
      (String(s), pos)
    }
    | '-' | '0'..'9' => parseNumber(text, pos)
    | _ => fail(text, pos, "unexpected character")
    }
  }
}

and parseArrayValue = (text, pos) => {
  let pos = skip(text, pos);
  let (value, pos) = parse(text, pos);
  let pos = skip(text, pos);
  switch (text.[pos]) {
  | ',' => {
    let pos = skip(text, pos + 1);
    if (text.[pos] == ']') {
      ([value], pos + 1)
    } else {
      let (rest, pos) = parseArrayValue(text, pos);
      ([value, ...rest], pos)
    }
  }
  | ']' => ([value], pos + 1)
  | _ => fail(text, pos, "unexpected character")
  }
}

and parseArray = (text, pos) => {
  let pos = skip(text, pos);
  switch (text.[pos]) {
  | ']' => (Array([]), pos + 1)
  | _ => {
    let (items, pos) = parseArrayValue(text, pos);
    (Array(items), pos)
  }
  }
}

and parseObjectValue = (text, pos) => {
  let pos = skip(text, pos);
  if (text.[pos] != '"') {
    fail(text, pos, "Expected string")
  } else {
    let (key, pos) = parseString(text, pos + 1);
    let pos = skip(text, pos);
    let pos = expect(':', text, pos, "Colon");
    let (value, pos) = parse(text, pos);
    let pos = skip(text, pos);
    switch (text.[pos]) {
    | ',' => {
      let pos = skip(text, pos + 1);
      if (text.[pos] == '}') {
        ([(key, value)], pos + 1)
      } else {
        let (rest, pos) = parseObjectValue(text, pos);
        ([(key, value), ...rest], pos)
      }
    }
    | '}' => ([(key, value)], pos + 1)
    | _ => {
      let (rest, pos) = parseObjectValue(text, pos);
      ([(key, value), ...rest], pos)
    }
    }
  }
}

and parseObject = (text, pos) => {
  let pos = skip(text, pos);
  if (text.[pos] == '}') {
    (Object([]), pos + 1)
  } else {
    let (pairs, pos) = parseObjectValue(text, pos);
    (Object(pairs), pos)
  }
};

};

/** Turns some text into a json object. throws on failure */
let parse = text => {
  let (item, pos) = Parser.parse(text, 0);
  let pos = Parser.skip(text, pos);
  if (pos < String.length(text)) {
    failwith("Extra data after parse finished: " ++ String.sub(text, pos, String.length(text) - pos))
  } else {
    item
  }
};

/* Accessor helpers */

let bind = (v, fn) => switch (v) { | None => None | Some(v) => fn(v) };

/** If `t` is an object, get the value associated with the given string key */
let get = (key, t) => switch t {
| Object(items) => try (Some(List.assoc(key, items))) { | Not_found => None}
| _ => None
};

/** If `t` is an array, get the value associated with the given index */
let nth = (n, t) => switch t {
| Array(items) => if (n < List.length(items)) {
  Some(List.nth(items, n))
} else {
  None
}
| _ => None
};

let string = t => switch t {
| String(s) => Some(s)
| _ => None
};

let number = t => switch t {
| Number(s) => Some(s)
| _ => None
};

let array = t => switch t {
| Array(s) => Some(s)
| _ => None
};

let obj = t => switch t {
| Object(s) => Some(s)
| _ => None
};

let bool = t => switch t {
| True => Some(true)
| False => Some(false)
| _ => None
};

let null = t => switch t {
| Null => Some(())
| _ => None
};

let rec parsePath = (keyList, t) =>
  switch keyList {
  | [] => t
  | [head, ...rest] =>
    parsePath(
      rest,
      get(head, t) |> unwrap("Could not find object '" ++ head ++ "'")
    )
  };

/** Get a deeply nested value from an object `t`.
 * ```
 * open Json.Infix;
 * let json = Json.parse({|{"a": {"b": {"c": 2}}}|});
 * let num = Json.getPath("a.b.c", json) |?> Json.number;
 * assert(num == Some(2.))
 * ```
 */
let getPath = (path, t) => {
  let keys = Parser.split_by(c => c == '.', path);
  switch t {
  | Object(items) => Some(parsePath(keys, t))
  | _ => None
  };
};
