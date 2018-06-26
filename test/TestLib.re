let go = () => {
  let tests = ref(0);
  let failures = ref(0);
  let ensure = (group, bool, message) => {
    tests := tests^ + 1;
    if (! bool) {
      print_endline("[" ++ group ++ "] Failed: " ++ message);
      failures := failures^ + 1
    }
  };
  let report = () => {
    print_newline();
    if (failures^ == 0) {
      print_endline("Success! All " ++ string_of_int(tests^) ++ " tests passed");
      exit(0)
    } else {
      print_endline("Failures: " ++ string_of_int(failures^) ++ " / " ++ string_of_int(tests^));
      exit(1)
    }
  };
  (ensure, report)
};