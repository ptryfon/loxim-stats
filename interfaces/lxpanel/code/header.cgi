<?php
	echo "<div class=\"page\">\n";

	echo "<div class=\"pack\">\n";

	$status = lx_status($dbconn);
	echo "<div class=\"status\">\n";
	echo "Connected to: {$status['hostname']}" .
		"({$status['hostaddr']}):{$status['port']}" .
		" as {$status['user']}" .
		"<br />\n";
	$time = $status['time'];
	$h = $time / 3600;
	$time %= 3600;
	$m = $time / 60;
	$time %= 60;
	$s = $time;
	echo "Connection time: " . sprintf("%dh %02dm %02ds", $h, $m, $s) .
		"<br />\n";

	lx_query("begin");
	$res = lx_query("deref(%Information)");
	lx_query("end");
	$infos = array();
	if (lx_error($res) == "")
		$infos = lx_fetch($res);
	$server = "(unknown server)";
	if (isset($infos['Name']))
		$server = $infos['Name'];
	$version = "(unknown version)";
	if (isset($infos['Version']))
		$version = $infos['Version'];
	echo "Server: $server $version\n";
	echo "</div>\n";

	echo "<div class=\"content\">\n";
?>
