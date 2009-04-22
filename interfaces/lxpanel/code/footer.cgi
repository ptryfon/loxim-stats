<?php
	echo "</div>\n";

	echo "</div>\n";

	echo "<div class=\"menu\">\n";
	echo "<h2>LoXiM menu</h2>\n";
	echo "<ul id=\"menu\">\n";
	echo "<li><a href=\"$action?do_logout\">Logout</a></li>\n";
	echo "<li><a href=\"$action?page=home\">Home page</a></li>\n";
	echo "<li><a href=\"$action?page=browse\">Browse database</a></li>\n";
	echo "<li><a href=\"$action?page=query\">Query</a></li>\n";
	echo "<li><a href=\"$action?page=querywin\" onclick=\"openwindow('$action?page=querywin'); return false; \">Query window</a></li>\n";
	echo "<li><a href=\"$action?page=import\">Import from XML</a></li>\n";
	echo "<li><a href=\"$action?page=stats\">Statistics</a></li>\n";
	echo "</ul>\n";

	echo "<div class=\"favourites\">\n";
	echo "<h3>Favourites</h3>\n";
	echo "</div>\n";
	echo "<div id=\"favourites\">\n";
	echo "</div>\n";
	echo "<script type=\"text/javascript\">\n";
	echo "request_id('favourites', '$request?op=favourites');\n";
	echo "</script>\n";

	echo "<div class=\"history\">\n";
	echo "<h3>History</h3>\n";
	echo "</div>\n";
	$history = array();
	if (isset($_SESSION['history']))
		$history = array_reverse($_SESSION['history']);
	if (count($history) > 0) {
		echo "<ol id=\"history\">\n";
		foreach ($history as $q)
			echo "<li><a href=\"$action?page=query&amp;query=" . escape_query($q) . "&amp;execute\" title=\"" . htmlentities(query_long($q)) . "\">" . htmlentities(query_short($q)) . "</a></li>\n";
		echo "</ol>\n";
	}

	echo "</div>\n";

	echo "</div>\n";
?>
