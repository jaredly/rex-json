let (ensure, report) = TestLib.go();

DemoTest.go(ensure("DemoTest"));
BigList.go(ensure("BigList"));

report();