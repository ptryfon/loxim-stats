<?php

function hr()
{
	echo "----\n";
}

	if (!extension_loaded('lxsbql'))
		dl('lxsbql.' . PHP_SHLIB_SUFFIX);

	var_dump(function_exists('lx_query'));
	hr();

	$c = lx_connect_params("localhost", 2000, "u", "p");
	var_dump($c);
	var_dump(lx_error_conn());
	hr();
	var_dump(lx_status());
	hr();
	hr();

	$r = lx_query("begin");
	var_dump($r);
	hr();
	$res = lx_query("(1 as x, 2 as y)");
	var_dump($res);
	hr();
	$r = lx_query("end");
	var_dump($r);
	hr();
	var_dump(lx_fetch($res));
	hr();

	lx_query("qwe", $c);
	var_dump($c);
	hr();

$d = lx_connect("host=localhost port=2000 user=root");
var_dump($d);
var_dump(lx_error_conn());
lx_close();
lx_close();
var_dump($d);
hr();

	$r = lx_query("ppp", $c);
	var_dump($r);
	$f = lx_fetch($r);
	var_dump($f);
	hr();

//	lx_close($c);
//	unset($c);
	var_dump($c);
	hr();

?>
