<?php
	include("code/stats.cgi");

	$stat = "";
	if (isset($R['stat']))
		$stat = $R['stat'];
	$statobj = null;
	if (isset($stats[$stat]))
		$statobj = $stats[$stat];

	$x = "";
	if ($statobj !== null)
		$x = ": " . $statobj['title'];
	echo "<h1>Statistics{$x}</h1>\n";

	if ($stat == "") {
		echo "<p>Available statistics:</p>\n";
		echo "<ul>\n";
		foreach ($stats as $k => $v)
			echo "<li><a href=\"$action?page=$page&amp;stat=$k\">" .
				$v['title'] . "</a></li>\n";
		echo "</ul>\n";
	} else if ($statobj === null) {
		echo "<p>invalid stat requested</p>\n";
	} else {
		$begin_time = microtime(true);
		lx_query("begin");
		$res = lx_query($statobj['query']);
		lx_query("end");
		$query_time = microtime(true) - $begin_time;
		$rootobj = lx_fetch($res);
		$err = lx_error($res);
		if ($err == "") {
			echo "<p>Gathering time: " .
				sprintf("%.02f", $query_time) . " sec.</p>\n";
			include("pages/viewstat.cgi");
		} else
			echo "<p>unable to read stats: $err</p>\n";
	}

?>
