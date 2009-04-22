<?php

function hr()
{
	echo "----\n";
}

function query($q)
{
	$r = lx_query($q);
	if (lx_error($r) != "") {
		echo "Error: " . lx_error($r) . "\n";
		hr();
		return;
	}
	$f = lx_fetch($r);
	var_dump($f);
//if (is_array($f) && is_resource($f[0])) $f = $f[0];
	if (is_resource($f)) {
//echo "RES! Q: $q //\n";
		var_dump(lx_refid($f));
		var_dump(lx_deref($f));
	}
	lx_dispose($r);
	hr();
}

	if (!extension_loaded('lxsbql'))
		dl('lxsbql.' . PHP_SHLIB_SUFFIX);

	var_dump(function_exists('lx_query'));
	hr();

	$c = lx_connect_params("localhost", 2000, "root", "");
	var_dump($c);
	if (!$c) {
		echo lx_error_conn() . "\n";
		exit;
	}
	hr();

	query("begin");
	query("x");
//query("(1 as x, 2 as y).(x, y, y)");
	query("92;");
query("pick;");
	query("7.4;");
	query("1>2;");
	query('"napisik";');
	query("7 union 6;"); //bag
	query("(6 union 4) order by x;"); //seq
	query("(7 as x) as n;"); //bind bind
	query("6 as c;"); //bag bind
	query("(9, 8);"); //bag struct
	query("(4 as x, 5 as y);"); //bag struct bind
var_dump(lx_query_params("pick", array("foo" => "bar", "x" => 5, "y" => 3.2, "z" => true)));
var_dump(lx_query_params("pickk", array()));
$x = lx_query_params("pickkk");
var_dump($x);
var_dump(lx_error($x));
	query("end");

	lx_close($c);
	hr();
?>
