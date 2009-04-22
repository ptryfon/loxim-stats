<?php
	echo "<h1>Results</h1>\n";
	echo "<p>Evaluation time: " . sprintf("%.02f", $query_time) .
		" sec.</p>\n";
	if (count($queryarr) > 1)
		echo "<p>Successfully executed queries: " . $success .
			" of " . count($queryarr) . "</p>\n";

	$error = true;
	if (lx_error($result) != "") {
		echo "<div class=\"error\">" . lx_error($result) . "</div>\n";
	} else {
		$arr = lx_fetch($result, LXSBQL_FETCH_ALL);
		echo "<div class=\"resultblock\" id=\"resultblock\">\n";
		printdata($arr, null);
		echo "</div>\n";
		$error = false;
	}

	echo "<div class=\"resultview\">\n";
	echo "<a href=\"#\" onclick=\"exp_coll_all('resultblock', true); return false; \">Expand all</a>\n";
	echo "| <a href=\"#\" onclick=\"exp_coll_all('resultblock', false); return false; \">Collapse all</a>\n";
	echo "| <a href=\"#\" onclick=\"deref_all('resultblock', false); return false; \">Deref all</a>\n";
	echo "| <a href=\"#\" onclick=\"deref_all('resultblock', true); return false; \">Deref visible</a>\n";
	echo "</div>\n";

	echo "<script type=\"text/javascript\">\n";
	echo "exp_coll_all('resultblock', true);\n";
	echo "</script>\n";

	echo "<div class=\"resultops\">\n";
	echo "<form action=\"$action\" method=\"get\">\n";
	echo "<input type=\"hidden\" name=\"page\" value=\"$page\" />\n";
	echo "<input type=\"hidden\" name=\"history\" value=\"1\" />\n";
	echo "<table class=\"form\">\n";
	echo "<tr>\n";
	echo "<td colspan=\"2\">\n";
	echo "<div class=\"queryblock\">\n";
	echo "<div class=\"query\" id=\"queryview\">\n";
	echo "<textarea name=\"query\" id=\"querytext\" rows=\"10\" cols=\"50\">$query</textarea>\n";
	echo "<div>\n";
	echo "Multiple queries:\n";
	echo "<select name=\"multi\">\n";
	foreach ($multi_queries as $k => $vv)
		echo "<option value=\"$k\"" . ($multi == $k ? " selected=\"selected\"" : "") . ">{$vv['name']}</option>\n";
	echo "</select>\n";
	echo "On error:\n";
	echo "<select name=\"onerror\">\n";
	foreach ($onerror_opts as $k => $v)
		echo "<option value=\"$k\"" . ($onerror == $k ? " selected=\"selected\"" : "") . ">$v</option>\n";
	echo "</select>\n";
	echo "</div>\n";
	echo "</div>\n";
	echo "<a href=\"#\" id=\"showhide\" onclick=\"showhidequery_click(this, 'queryview'); return false; \">show/hide query</a>\n";
	echo "</div>\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr class=\"submit\">\n";
	echo "<td colspan=\"2\">\n";
	echo "<input type=\"submit\" name=\"execute\" value=\"Re-evaluate\" />\n";
	echo "<input type=\"submit\" name=\"favourite\" value=\"Add to favourites\" onclick=\"request_id('favourites', '$request?op=favourites&amp;add=' + escape_uri(document.getElementById('querytext').value)); return false; \" />\n";
	echo "<input type=\"submit\" name=\"export\" value=\"Export to XML...\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "</table>\n";
	echo "</form>\n";

	echo "<script type=\"text/javascript\">\n";
	echo "showhidequery_click(document.getElementById('showhide'), 'queryview');\n";
	if ($error)
		echo "showhidequery_click(document.getElementById('showhide'), 'queryview');\n";
	echo "</script>\n";
	echo "</div>\n";
?>
