
type t =
  | String(string)
  | Number(float)
  | Array(list(t))
  | Object(list((string, t)))
  | True
  | False
  | Null;

let fail = (text, pos, message) => {
  let pre = String.sub(text, 0, pos);
  let lines = Str.split(Str.regexp("\n"), pre);
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

/* and parseArrayValue = (text, pos) => {
  switch (text.[pos]) {
  | '\n' | '\t' | ' ' => parseArrayValue(text, skipWhite(text, pos))
  | ']' => ([], pos + 1)
  | '/' => parseComment(text, pos + 1, parseArrayValue)
  | _ => {
    let (item, pos) = parse(text, pos + 1);
    let (rest, pos) = parseArrayInner(text, pos);
    ([item, ...rest], pos)
  }
  | _ => fail(text, pos, "Unexpected character in array")
  }

}

and parseArrayInner = (text, pos) => {
  switch (text.[pos]) {
  | '\n' | '\t' | ' ' => parseArrayInner(text, skipWhite(text, pos))
  | ']' => ([], pos + 1)
  | '/' => parseComment(text, pos + 1, parseArrayInner)
  | ',' => {
    parseArrayValue(text, pos + 1)
    /* let (item, pos) = parse(text, pos + 1);
    let (rest, pos) = parseArrayInner(text, pos);
    ([item, ...rest], pos) */
  }
  | _ => fail(text, pos, "Unexpected character in array")
  }
}

and parseArray = (text, pos) => {
  switch (text.[pos]) {
  | '\n' | '\t' | ' ' => parseArray(text, skipWhite(text, pos))
  | ']' => (Array([]), pos + 1)
  | '/' => parseComment(text, pos + 1, parseArray)
  | _ => {
    let (item, pos) = parse(text, pos);
    let (rest, pos) = parseArrayInner(text, pos);
    (Array([item, ...rest]), pos)
  }
  }
} */

and parseArrayValue = (text, pos) => {
  let pos = skip(text, pos);
  let (value, pos) = parse(text, pos);
  let pos = skip(text, pos);
  switch (text.[pos]) {
  | ',' => {
    let pos = skip(text, pos + 1);
    if (text.[pos] == ']') {
      ([value], pos)
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
        ([(key, value)], pos)
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

/* and parseObjectValue = (text, pos) => {
  switch (text.[pos]) {
  | '\n' | '\t' | ' ' => parseObjectValue(text, skipWhite(text, pos))
  | '/' => parseComment(text, pos + 1, parseObjectValue)
  | '}' => ([], pos + 1)
  | '"' => {
    let (k, pos) = parseString(text, pos + 1);
    let pos = expect(':', text, skipWhite(text, pos), "colon");
    let (v, pos) = parse(text, pos);
    let (rest, pos) = parseObjectInner(text, pos);
    ([(k, v), ...rest], pos)
  }
  | _ => fail(text, pos, "Unexpected character in object value")
  }
}

and parseObjectInner = (text, pos) => {
  switch (text.[pos]) {
  | '\n' | '\t' | ' ' => parseObjectInner(text, skipWhite(text, pos))
  | '/' => parseComment(text, pos + 1, parseObjectInner)
  | '}' => ([], pos + 1)
  | ',' => {
    parseObjectValue(text, pos + 1)
  }
  | _ => fail(text, pos, "Unexpected character in object")
  }
}

and parseObject = (text, pos) => {
  switch (text.[pos]) {
  | '\n' | '\t' | ' ' => parseObject(text, skipWhite(text, pos))
  | '}' => (Object([]), pos + 1)
  | '/' => parseComment(text, pos + 1, parseObject)
  | _ => {
    let (items, pos) = parseObjectValue(text, pos);
    (Object(items), pos)
  }
  }
} */

;

let parse = text => {
  let (item, _) = parse(text, 0);
  item
};