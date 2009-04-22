<?php
	echo "<h1>Browse database</h1>\n";

	printroots();

	echo "<h3>Create root objects\n";
	echo "<span id=\"createrootbusy\"></span>\n";
	echo "</h3>\n";
	echo "<form action=\"$action\" method=\"get\">\n";
	echo "<input type=\"hidden\" name=\"uri\" value=\"$request?op=obj_create_root\" />\n";
	echo "<table class=\"form\">\n";
	echo "<tr>\n";
	echo "<th>Name</th>\n";
	echo "<td>\n";
	echo "<input type=\"text\" name=\"name\" value=\"\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr>\n";
	echo "<th>Count</th>\n";
	echo "<td>\n";
	echo "<input type=\"text\" name=\"count\" value=\"1\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr class=\"submit\">\n";
	echo "<td colspan=\"2\">\n";
	echo "<input type=\"submit\" name=\"create\" value=\"Create\" onclick=\"createroot(this); return false;\" />\n";
	echo "<input type=\"reset\" name=\"reset\" value=\"Reset\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "</table>\n";
	echo "</form>\n";
?>
