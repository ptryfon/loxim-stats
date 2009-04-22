<?php

function req_favourites(array $R)
{
	$add = null;
	if (isset($R['add']))
		$add = $R['add'];
	$del = null;
	if (isset($R['del']))
		$del = $R['del'];

	$fav = &$_SESSION['favourites'];
	if (!isset($fav))
		$fav = array();
	if ($add !== null && array_search($add, $fav) === false)
		$fav[] = $add;
	if ($del !== null)
		unset($fav[$del]);
	if (empty($fav))
		return;
	global $action, $request;
	echo "<ul id=\"favourites\">\n";
	foreach ($fav as $k => $v)
		echo "<li><a href=\"$action?page=query&amp;query=" . escape_query($v) . "&amp;execute\" title=\"" . htmlentities(query_long($v)) . "\">" . htmlentities(query_short($v)) . "</a> <a href=\"#\" onclick=\"request_id('favourites', '$request?op=favourites&amp;del=$k'); return false; \" title=\"delete\" class=\"delete\">&#215;</a></li>\n";
	echo "</ul>\n";
}

function req_deref(array $R)
{
	$id = null;
	if (isset($R['id']))
		$id = $R['id'];

	if ($id === null)
		die_error("invalid usage");
	if (!connectdb())
		die_error("unable to connect");

	lx_query("begin");
	$res = lx_query("deref(refid({$id}))");
	lx_query("end");
	if (lx_error($res) != "")
		die_error("error: " . lx_error($res));
	$data = lx_fetch($res, LXSBQL_FETCH_ALL);
	echo printdata($data, $id);
}

function req_obj_del(array $R)
{
	$id = null;
	if (isset($R['id']))
		$id = $R['id'];
	$path = "";
	if (isset($R['path']))
		$path = $R['path'];

	if ($id === null)
		die_error("invalid usage");
	if (!connectdb())
		die_error("unable to connect");

	$ps = "";
	if ($path != "")
		$ps = "." . $path;
	lx_query("begin");
	$res = lx_query("delete(refid({$id})){$ps}");
	lx_query("end");
	if (lx_error($res) != "")
		die_error("error: " . lx_error($res));
	echo "OK";
}

function req_obj_insert(array $R)
{
	$id = null;
	if (isset($R['id']))
		$id = $R['id'];
	$path = "";
	if (isset($R['path']))
		$path = $R['path'];
	$field = null;
	if (isset($R['field']))
		$field = $R['field'];

	if ($id === null || $field === null)
		die_error("invalid usage");
	if (!connectdb())
		die_error("unable to connect");

	$ps = "";
	if ($path != "")
		$ps = "." . $path;
	$val = 0;
	lx_query("begin");
	$res = lx_query("refid({$id}){$ps} :< $val as $field");
	lx_query("end");
	if (lx_error($res) != "")
		die_error("error: " . lx_error($res));
	$fs = explode(".", $path);
	if ($fs[0] == "")
		$fs = array();
	_printdata($val, $id, $field, $fs);
}

function req_obj_edit(array $R)
{
	$id = null;
	if (isset($R['id']))
		$id = $R['id'];
	$path = "";
	if (isset($R['path']))
		$path = $R['path'];
	$type = null;
	if (isset($R['type']))
		$type = $R['type'];
	$value = null;
	if (isset($R['value']))
		$value = $R['value'];

	if ($id === null || $type === null || $value === null)
		die_error("invalid usage");
	if (!connectdb())
		die_error("unable to connect");

	$val = null;
	$pval = null;
	switch ($type) {
	case "int":
		$val = $value;
		$pval = intval($value);
		break;
	case "double":
		$val = $value;
		if (!strstr($value, "."))
			$val .= ".0";
		$pval = floatval($value);
		break;
	case "string":
		$val = "\"" . $value . "\"";
		$pval = $value;
		break;
	case "struct":
		$val = "(0 as empty)";
		$pval = array("_type" => "struct", "empty" => 0);
		break;
	default:
		die_error("unknown type: $type");
	}
	$ps = "";
	if ($path != "")
		$ps = "." . $path;
	lx_query("begin");
	$res = lx_query("refid({$id}){$ps} := $val");
	lx_query("end");
	if (lx_error($res) != "")
		die_error("error: " . lx_error($res));
	$fs = explode(".", $path);
	if ($fs[0] == "")
		$fs = array();
	$label = 0;
	if (!empty($fs))
		$label = array_pop($fs);
	_printdata($pval, $id, $label, $fs);
}

function req_obj_create_root(array $R)
{
	$name = "";
	if (isset($R['name']))
		$name = $R['name'];
	$count = 1;
	if (isset($R['count']))
		$count = $R['count'];

	if ($name == "" || !ctype_digit($count) || $count == 0)
		die_error("invalid usage");
	if (!connectdb())
		die_error("unable to connect");

	$err = "";
	lx_query("begin");
	for ($i = 0; $i < $count; $i++) {
		$res = lx_query("create $i as $name");
		$e = lx_error($res);
		if ($e != "")
			$err .= $e . "\n";
	}
	lx_query("end");

	if ($err != "")
		die_error($err);
	printroots();
}

?>
