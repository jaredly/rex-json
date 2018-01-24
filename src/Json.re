
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

let rec stringify = t => switch t {
| String(value) => "\"" ++ String.escaped(value) ++ "\""
| Number(num) => string_of_number(num)
| Array(items) => "[" ++ String.concat(", ", List.map(stringify, items)) ++ "]"
| Object(items) => "{" ++ String.concat(", ", List.map(((k, v)) => "\"" ++ String.escaped(k) ++ "\":" ++ stringify(v), items)) ++ "}"
| True => "true"
| False => "false"
| Null => "null"
};

let get = (key, t) => switch t {
| Object(items) => try (Some(List.assoc(key, items))) { | Not_found => None}
| _ => None
};

let nth = (n, t) => switch t {
| Array(items) => if (n < List.length(items)) {
  Some(List.nth(items, n))
} else {
  None
}
| _ => None
};

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

let unwrap = (message, t) =>
  switch t {
  | Some(v) => v
  | None => failwith(message)
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

let getPath = (path, t) => {
  let keys = split_by(c => c == '.', path);
  switch t {
  | Object(items) => Some(parsePath(keys, t))
  | _ => None
  };
};

let fail = (text, pos, message) => {
  let pre = String.sub(text, 0, pos);
  let lines = split_by((c) => c == '\n', pre);
  let last = List.nth(lines, List.length(lines) - 1);
  let col = String.length(last) + 1;
  let line = List.length(lines);
  (Printf.printf("Error \"%s\" at %d:%d -> %s\n", message, line, col, last));
  failwith("Parse error");
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

let rec skipWhite = (text, pos) => {
  if (text.[pos] == ' ' || text.[pos] == '\t' || text.[pos] == '\n') {
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

let parseNumber = (text, pos) => {
  let i = ref(pos);
  let len = String.length(text);
  while (i^ < len && Char.code('0') <= Char.code(text.[i^]) && Char.code(text.[i^]) <= Char.code('9')) {
    i := i^ + 1;
    /* print_endline(">" ++ string_of_int(pos) ++ " : " ++ string_of_int(i^)); */
  };
  let s = String.sub(text, pos, i^ - pos);
  /* print_endline(s); */
  (Number(float_of_string(s)), i^)
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
    failwith("Invalid syntax")
  } else {
    next(text, skipToNewline(text, pos + 1))
  }
};

let maybeSkipComment = (text, pos) => {
  if (text.[pos] == '/') {
    if (text.[pos + 1] == '/') {
      skipToNewline(text, pos + 1)
    } else {
      fail(text, pos, "Invalid synatx")
    }
  } else {
    pos
  }
};

let rec skip = (text, pos) => {
  let n = skipWhite(text, pos) |> maybeSkipComment(text);
  if (n > pos) skip(text, n)
  else n
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
    | '\n' | '\t' | ' ' => parse(text, skipWhite(text, pos))
    | '"' => {
      let (s, pos) = parseString(text, pos + 1);
      (String(s), pos)
    }
    | '0'..'9' => parseNumber(text, pos)
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
  | _ => {
    let (rest, pos) = parseArrayValue(text, pos);
    ([value, ...rest], pos)
  }
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
}

;

let parse = text => {
  let (item, _) = parse(text, 0);
  item
};
