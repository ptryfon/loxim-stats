set (tester "$ENV{LOXIM_HOME}/Testing/scripts/tester.sh")
add_test(BasicSBQLTest ${tester} basic.sbql)

message($ENV{LOXIM_HOME})
message(${tester})
