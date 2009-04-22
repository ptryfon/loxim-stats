<?php

	$input = "";
	if (isset($R['input']))
		$input = $R['input'];
	$uri = "";
	if (isset($R['uri']))
		$uri = $R['uri'];
	$type = "input";
	if (isset($R['type']))
		$type = $R['type'];
	$root = "";
	if (isset($R['root']))
		$root = $R['root'];

	$querylist = array();
	$xmlerror = "";
	if (isset($R['import']) || isset($R['preview'])) {
		$xml = "";
		switch ($type) {
		case "input":
			$xml = $input;
			break;
		case "uri":
			$xml = @file_get_contents($uri);
			if ($xml === false)
				$xmlerror = "unable to fetch: $uri";
			break;
		case "upload":
			$xml = "";
			$t = "";
			if (isset($_FILES['upload']))
				$t = $_FILES['upload']['tmp_name'];
			if ($t != "" && is_uploaded_file($t))
				$xml = file_get_contents($t);
			break;
		default:
			echo "import: unsupported source";
			return;
		}
		if ($xmlerror == "")
			$data = xml2data($xml, $xmlerror);
		if ($root == "" && $xmlerror == "")
			$xmlerror = "root location must not be empty";

		if ($xmlerror == "")
			$querylist = data2sbql($data, $root);
	}

	if ($xmlerror != "")
		echo "<div class=\"error\">$xmlerror</div>\n";
	$preview = "";
	if (isset($R['preview']) && $xmlerror == "") {
		foreach ($querylist as $q)
			$preview .= $q . "\n";
	}
	if (isset($R['import']) && $xmlerror == "") {
		echo "<h1>Import</h1>\n";
		echo "<ul>\n";
		lx_query("begin");
		foreach ($querylist as $q) {
			echo "<li>" . $q . "</li>\n";
			$res = lx_query($q);
			$err = lx_error($res);
			if ($err != "")
				echo "<div class=\"error\">$err</div>\n";
		}
		lx_query("end");
		echo "</ul>\n";
		return;
	}

	echo "<h1>Import from XML</h1>\n";

	echo "<div class=\"resultview\">\n";
	echo "<a href=\"#\" onclick=\"import_vis('input'); return false; \">Input</a>\n";
	echo "| <a href=\"#\" onclick=\"import_vis('upload'); return false; \">Upload</a>\n";
	echo "| <a href=\"#\" onclick=\"import_vis('uri'); return false; \">URI</a>\n";
	echo "</div>\n";

	echo "<form action=\"$action\" method=\"post\" enctype=\"multipart/form-data\">\n";
	echo "<input type=\"hidden\" name=\"page\" value=\"$page\" />\n";
	echo "<input type=\"hidden\" id=\"type\" name=\"type\" value=\"\" />\n";
	echo "<input type=\"hidden\" name=\"MAX_FILE_SIZE\" value=\"$config[max_upload]\" />\n";
	echo "<table class=\"form\">\n";
	echo "<tr id=\"input\">\n";
	echo "<td colspan=\"2\">\n";
	echo "<textarea id=\"focus_input\" name=\"input\" rows=\"10\" cols=\"50\">$input</textarea>\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr id=\"upload\">\n";
	echo "<th>File</th>\n";
	echo "<td>\n";
	echo "<input type=\"file\" id=\"focus_upload\" name=\"upload\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr id=\"uri\">\n";
	echo "<th>URI</th>\n";
	echo "<td>\n";
	echo "<input type=\"text\" id=\"focus_uri\" name=\"uri\" value=\"$uri\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr>\n";
	echo "<th>Create as</th>\n";
	echo "<td>\n";
	echo "<input type=\"text\" name=\"root\" value=\"$root\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr class=\"submit\">\n";
	echo "<td colspan=\"2\">\n";
	echo "<input type=\"submit\" name=\"preview\" value=\"Preview\" />\n";
	echo "<input type=\"submit\" name=\"import\" value=\"Import\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "</table>\n";
	echo "</form>\n";
	echo "<script type=\"text/javascript\">\n";
	echo "import_vis('$type');\n";
	echo "</script>\n";

	if ($preview != "")
		echo "<p class=\"preview\">$preview</p>\n";
?>
