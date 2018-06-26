let (ensure, report) = TestLib.go();

Printexc.record_backtrace(true);
DemoTest.go(ensure("DemoTest"));
BigList.go(ensure("BigList"));

report();