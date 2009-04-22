<?php
	echo "<div class=\"querywin\">\n";

	echo "<h1>Enter query</h1>\n";
	echo "<form action=\"$action\" method=\"get\" onsubmit=\"window.opener.location = '$action?page=query&amp;query=' + escape(document.getElementById('query').value) + '&amp;execute' + '&amp;history=1'; return false;\">\n";
	echo "<input type=\"hidden\" name=\"page\" value=\"query\" />\n";
	echo "<table class=\"form\">\n";
	echo "<tr>\n";
	echo "<td colspan=\"2\">\n";
	echo "<textarea name=\"query\" id=\"query\" rows=\"10\" cols=\"50\">$query</textarea>\n";
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

	echo "</div>\n";
?>
