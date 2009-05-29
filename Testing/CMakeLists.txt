set (tester "$ENV{LOXIM_HOME}/Testing/scripts/tester.sh")
add_test(BasicSBQLTest ${tester} -n basic.sbql)
add_test(SzpitalCreateSBQLTest ${tester} -n szpital-create.sbql)

message($ENV{LOXIM_HOME})
message(${tester})
