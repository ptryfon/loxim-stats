<?php

function history_add($query)
{
	$history = &$_SESSION['history'];
	if (!isset($history))
	$history = array();
	if (!empty($history)) {
		$top = end($history);
		if ($top == $query)
			return;
	}
	$history[] = $query;
	if (count($history) > $GLOBALS['config']['max_history'])
		array_shift($history);
}

	$query = "";
	if (isset($R['query']))
		$query = $R['query'];
	$multi = "n";
	if (isset($R['multi']))
		$multi = $R['multi'];
	$onerror = "a";
	if (isset($R['onerror']))
		$onerror = $R['onerror'];
	if (isset($R['execute']) || isset($R['export'])) {
		$multidesc = null;
		$multifun = null;
		if (isset($multi_queries[$multi]))
			$multidesc = $multi_queries[$multi];
		if ($multidesc !== null)
			$multifun = $multidesc['func'];
		if ($multifun !== null)
			$queryarr = $multifun($query);
		else
			$queryarr = array($query);

		$begin_time = microtime(true);
		lx_query("begin", $dbconn);
		$success = 0;
		foreach ($queryarr as $q) {
			$result = lx_query($q, $dbconn);
			if (lx_error($result) == "")
				$success++;
			else
				if ($onerror == "a")
					break;
		}
		lx_query("end", $dbconn);
		$query_time = microtime(true) - $begin_time;
		if (isset($R['history']))
			history_add($query);
	}
	if (isset($R['execute'])) {
		include("pages/result.cgi");
		return;
	}
	if (isset($R['export'])) {
		header("Content-type: text/xml");
		$data = lx_fetch($result, LXSBQL_FETCH_ALL);
		echo data2xml($data);
		return;
	}

	echo "<h1>Enter query</h1>\n";
	echo "<form action=\"$action\" method=\"get\">\n";
	echo "<input type=\"hidden\" name=\"page\" value=\"$page\" />\n";
	echo "<input type=\"hidden\" name=\"history\" value=\"1\" />\n";
	echo "<table class=\"form\">\n";
	echo "<tr>\n";
	echo "<td colspan=\"2\">\n";
	echo "<textarea name=\"query\" id=\"query\" rows=\"10\" cols=\"50\">$query</textarea>\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr>\n";
	echo "<th>Multiple queries</th>\n";
	echo "<td>\n";
	echo "<select name=\"multi\">\n";
	foreach ($multi_queries as $k => $vv)
		echo "<option value=\"$k\"" . ($multi == $k ? " selected=\"selected\"" : "") . ">{$vv['name']}</option>\n";
	echo "</select>\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr>\n";
	echo "<th>On error</th>\n";
	echo "<td>\n";
	echo "<select name=\"onerror\">\n";
	foreach ($onerror_opts as $k => $v)
		echo "<option value=\"$k\"" . ($onerror == $k ? " selected=\"selected\"" : "") . ">$v</option>\n";
	echo "</select>\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr class=\"submit\">\n";
	echo "<td colspan=\"2\">\n";
	echo "<input type=\"submit\" name=\"execute\" value=\"Execute\" />\n";
	echo "<input type=\"reset\" name=\"reset\" value=\"Clear\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "</table>\n";
	echo "</form>\n";
	echo "<script type=\"text/javascript\">\n";
	echo "document.getElementById('query').focus();\n";
	echo "</script>\n";

?>
