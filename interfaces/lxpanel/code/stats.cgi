<?php

	$s_information = array(
		"title" => "Information",
		"query" => "deref(%Information)",
		"struct" => array(
			"type" => "table1",
			"title" => "System Information",
		),
	);

	$s_transactions = array(
		"title" => "Transactions",
		"query" => "deref(%Transactions)",
		"struct" => array(
			"type" => "table1",
			"title" => "Summary",
		),
	);

	$s_store = array(
		"title" => "Store",
		"query" => "deref(%Store)",
		"struct" => array(
			"type" => "table1",
			"title" => "Summary",
		),
	);

	$s_sessions = array(
		"title" => "Sessions",
		"query" => "deref(%Sessions.SESSION)",
		"struct" => array(
			"type" => "combo",
			"items" => array(
			array(
				"type" => "legend",
				"title" => "Legend",
				"info" => array(
					"SID" => "Session ID",
					"SEC" => "Duration in seconds",
					"USR" => "User name",
					"DPR" => "Disk Page Reads",
					"DPW" => "Disk Page Writes",
					"PR" => "Page Reads",
					"PRH" => "Page Reads Hit Ratio",
					"PW" => "Page Writes",
					"PWH" => "Page Writes Hit Ratio",
					"ST" => "Start time",
				),
			),
			array(
				"type" => "table2",
				"title" => "Sessions",
				"fields" => array(
					"SESSION_ID" => "SID",
					"DURATION_IN_SECONDS" => "SEC",
					"USER_LOGIN" => "USR",
					"DISK_PAGE_READS" => "DPR",
					"DISK_PAGE_WRITES" => "DPW",
					"PAGE_READS" => "PR",
					"PAGE_READS_HIT" => "PRH",
					"PAGE_WRITES" => "PW",
					"PAGE_WRITES_HIT" => "PWH",
					"START_TIME" => "ST",
				),
			),
			),
		),
	);

	$s_queries = array(
		"title" => "Queries",
		"query" => "deref(%Queries.(AVG_QUERY_TIME as AVG_QUERY_TIME, MAX_QUERY_TIME as MAX_QUERY_TIME, NUMBER_OF_QUERIES as NUMBER_OF_QUERIES, SESSIONS_QUERIES.QUERY group as 'Session', TOP_IO_QUERIES.QUERY order by WEIGHT group as 'Top_IO', TOP_TIME_QUERIES.QUERY order by WEIGHT group as 'Top_Time'))",
		"struct" => array(
			"type" => "combo",
			"items" => array(
			array(
				"type" => "table1",
				"title" => "Summary",
			),
			array(
				"type" => "legend",
				"title" => "Legend",
				"info" => array(
					"SID" => "Session ID",
					"DIO" => "Disk I/O",
					"ST" => "State",
					"MS" => "Milliseconds",
					"TXT" => "Query Text",
				),
			),
			array(
				"type" => "sequence",
				"title_k" => true,
				"title_f" => null,
				"data_f" => null,
				"items" => array(
					"type" => "table2",
					"title" => "???",
					"fields" => array(
						"SESSION_ID" => "SID",
						"STATE" => "ST",
						"DISK_IO" => "DIO",
						"MILISEC" => "MS",
						"TEXT" => "TXT",
					),
				),
			),
			),
		),
	);

	$s_configs = array(
		"title" => "Configs",
		"query" => "deref(%Configs.MODULE.(MODULE_NAME as name, OPTION group as opts))",
		"struct" => array(
			"type" => "sequence",
			"title_k" => false,
			"title_f" => "name",
			"data_f" => "opts",
			"items" => array(
				"type" => "table1kv",
				"title" => "???",
				"key" => "KEY",
				"value" => "VALUE",
			),
		),
	);

	$stats = array(
		"information" => $s_information,
		"configs" => $s_configs,
		"queries" => $s_queries,
		"sessions" => $s_sessions,
		"store" => $s_store,
		"transactions" => $s_transactions,
	);

?>
