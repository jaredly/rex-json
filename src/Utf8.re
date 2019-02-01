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

let hex = c =>
  switch (c) {
  | '0'..'9' => int_of_char(c) - int_of_char('0')
  | 'a'..'f' => int_of_char(c) - int_of_char('a') + 10
  | 'A'..'F' => int_of_char(c) - int_of_char('A') + 10
  | _ => assert(false)
  };

let utf8_of_code = (buf, x) => {
  let add = Buffer.add_char;
  /* Straight <= doesn't work with signed 31-bit ints */
  let maxbits = (n, x) => x lsr n == 0;
  if (maxbits(7, x)) {
    /* 7 */
    add(buf, Char.chr(x));
  } else if (maxbits(11, x)) {
    /* 5 + 6 */
    add(buf, Char.chr(192 lor (x lsr 6 land 31)));
    add(buf, Char.chr(128 lor (x land 63)));
  } else if (maxbits(16, x)) {
    /* 4 + 6 + 6 */
    add(buf, Char.chr(224 lor (x lsr 12 land 15)));
    add(buf, Char.chr(128 lor (x lsr 6 land 63)));
    add(buf, Char.chr(128 lor (x land 63)));
  } else if (maxbits(21, x)) {
    /* 3 + 6 + 6 + 6 */
    add(buf, Char.chr(240 lor (x lsr 18 land 7)));
    add(buf, Char.chr(128 lor (x lsr 12 land 63)));
    add(buf, Char.chr(128 lor (x lsr 6 land 63)));
    add(buf, Char.chr(128 lor (x land 63)));
  } else if (maxbits(26, x)) {
    /* 2 + 6 + 6 + 6 + 6 */
    add(buf, Char.chr(248 lor (x lsr 24 land 3)));
    add(buf, Char.chr(128 lor (x lsr 18 land 63)));
    add(buf, Char.chr(128 lor (x lsr 12 land 63)));
    add(buf, Char.chr(128 lor (x lsr 6 land 63)));
    add(buf, Char.chr(128 lor (x land 63)));
  } else {
    assert(maxbits(31, x));
    /* 1 + 6 + 6 + 6 + 6 + 6 */
    add(buf, Char.chr(252 lor (x lsr 30 land 1)));
    add(buf, Char.chr(128 lor (x lsr 24 land 63)));
    add(buf, Char.chr(128 lor (x lsr 18 land 63)));
    add(buf, Char.chr(128 lor (x lsr 12 land 63)));
    add(buf, Char.chr(128 lor (x lsr 6 land 63)));
    add(buf, Char.chr(128 lor (x land 63)));
  };
};

let code_of_surrogate_pair = (i, j) => {
  let high10 = i - 55296;
  let low10 = j - 56320;
  65536 + high10 lsl 10 lor low10;
};

let utf8_of_surrogate_pair = (buf, i, j) =>
  utf8_of_code(buf, code_of_surrogate_pair(i, j));

let fail = (text, pos, message) => {
  let pre = String.sub(text, 0, pos);
  let lines = split_by(c => c == '\n', pre);
  let count = List.length(lines);
  let last = count > 0 ? List.nth(lines, count - 1) : "";
  let col = String.length(last) + 1;
  let line = List.length(lines);
  let string =
    Printf.sprintf("Error \"%s\" at %d:%d -> %s\n", message, line, col, last);
  failwith(string);
};

let finish_surrogate_pair = (buffer, x, i, ln, text, loop) =>
  i + 5 >= ln || text.[i] != '\\' || text.[i + 1] != 'u'
    ? fail(text, i, "Unterminated string - utf16 surrogate pair")
    : {
      let a = text.[i + 2];
      let b = text.[i + 3];
      let c = text.[i + 4];
      let d = text.[i + 5];
      let y = hex(a) lsl 12 lor hex(b) lsl 8 lor hex(c) lsl 4 lor hex(d);
      if (y >= 0xDC00 && y <= 0xDFFF) {
        utf8_of_surrogate_pair(buffer, x, y);
      } else {
        fail(text, i, "Invalid low surrogate for code point beyond U+FFFF");
      };
      loop(i + 6)
    };