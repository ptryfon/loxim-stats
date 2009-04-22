<?php

function printdata($val, $refid, $key = 0, array &$fields = null)
{
	echo "<ul>\n";
	_printdata($val, $refid, $key, $fields);
	echo "</ul>\n";
}

function _printdata($val, $refid, $key = 0, array &$fields = null)
{
	if ($fields === null)
		$fields = array();
	if (!is_int($key))
		$fields[] = $key;

	$t = "<img src=\"data/tree3.png\" alt=\"tree\" />";
	if (is_array($val))
		$t = "<img src=\"data/tree1.png\" alt=\"tree\" />";
	$t = "<a href=\"#\" onclick=\"vis_elt(this); return false; \">$t</a>";
	$v = $val;
	if (is_null($val))
		$v = "(NULL)";
	if (is_bool($val))
		$v = $val ? "true" : "false";
	if (is_string($val))
		$v = "\"" . htmlspecialchars($val) . "\"";
	$type = null;
	if (is_array($val)) {
		$type = "array";
		if (isset($val['_type']))
			$type = $val['_type'];
		$v = $type;
	}
	global $request;
	if (is_resource($val)) {
		$id = lx_refid($val);
		$v = "<a href=\"#\" class=\"ref\" onclick=\"refclick(this, '$request?op=deref&amp;id={$id}'); return false;\">ref({$id})</a>";
		$v .= " <a href=\"#\" onclick=\"refdelclick(this, '$request?op=obj_del&amp;id={$id}'); return false;\" title=\"delete\"><small>del</small></a>";
	}

	if (!is_int($key))
		$v = "$key = $v";
	if ($refid !== null && !is_array($val) && !is_resource($val)) {
		$path = implode(".", $fields);
		$ev = $val;
		$et = "";
		$types = array("string" => "String", "int" => "Integer",
				"double" => "Double", "struct" => "Struct");
		if (is_array($val))
			$et = "struct";
		if (is_string($val))
			$et = "string";
		if (is_double($val))
			$et = "double";
		if (is_integer($val))
			$et = "int";
		$v .= " <a href=\"#\" onclick=\"editclick(this); return false;\"><small>edit</small></a>\n";
		$v .= "<form action=\"$request\" style=\"display: none;\">";
		$v .= "<input type=\"hidden\" name=\"uri\" value=\"$request\" />";
		$v .= "<input type=\"hidden\" name=\"id\" value=\"$refid\" />";
		$v .= "<input type=\"hidden\" name=\"path\" value=\"$path\" />";
		$v .= "Value:<input type=\"text\" name=\"value\" value=\"" . htmlentities($ev) . "\" />";
		$v .= "<select name=\"type\">";
		foreach ($types as $tk => $tv)
			$v .= "<option value=\"$tk\"" . ($et == $tk ? " selected=\"selected\"" : "") . ">$tv</option>";
		$v .= "</select>";
		$v .= "<input type=\"submit\" value=\"Edit\" onclick=\"editokclick(this); return false;\" />";
		$v .= "<input type=\"reset\" value=\"Delete\" onclick=\"editdeleteclick(this); return false;\" />";
		$v .= "<input type=\"reset\" value=\"Cancel\" onclick=\"editcancelclick(this); return false;\" />";
		$v .= "</form>";
	}
	if ($refid !== null && is_array($val) && $type == "struct") {
		$path = implode(".", $fields);
		$v .= " <a href=\"#\" onclick=\"insertclick(this); return false;\"><small>insert</small></a>\n";
		$v .= "<form action=\"$request\" style=\"display: none;\">";
		$v .= "<input type=\"hidden\" name=\"uri\" value=\"$request?op=obj_insert\" />";
		$v .= "<input type=\"hidden\" name=\"id\" value=\"$refid\" />";
		$v .= "<input type=\"hidden\" name=\"path\" value=\"$path\" />";
		$v .= "Field:<input type=\"text\" name=\"field\" value=\"\" />";
		$v .= "<input type=\"submit\" value=\"Insert\" onclick=\"insertokclick(this); return false;\" />";
		$v .= "<input type=\"reset\" value=\"Cancel\" onclick=\"insertcancelclick(this); return false;\" />";
		$v .= "</form>";
		$v .= " <a href=\"#\" onclick=\"refdelclick(this, '$request?op=obj_del&amp;id={$refid}&amp;path={$path}'); return false;\" title=\"delete\"><small>del</small></a>";
	}
	echo "<li>$t$v</li>\n";
	if (is_array($val)) {
		echo "<li><ul>\n";
		foreach ($val as $k => $v)
			if (substr($k, 0, 1) != "_")
				_printdata($v, $refid, $k, $fields);
		echo "</ul></li>\n";
	}
	array_pop($fields);
}

function printroots()
{
	lx_query("begin");
	$res = lx_query("deref(%Roots)");
	lx_query("end");
	$roots = lx_fetch($res);

	echo "<div id=\"roots\">\n";
	if (empty($roots)) {
		echo "<p>No root objects!</p>\n";
	} else {
		echo "<p>Root objects:</p>\n";
		echo "<ul>\n";
		foreach ($roots as $k => $v) {
			echo "<li>";
			echo "<a href=\"$action?page=query&amp;query=" .
				escape_query($k) .
				"&amp;execute\">$k</a>\n";
			echo "</li>\n";
		}
		echo "</ul>\n";
	}
	echo "</div>\n";
}

?>
