<?php
	session_start();
	include("config.cgi");
	include("code/convert.cgi");
	include("code/print.cgi");
	$R = $_SERVER['REQUEST_METHOD'] == "POST" ? $_POST : $_GET;
	$action = "index.cgi";
	$request = "request.cgi";

function query_short($q)
{
	$max = 40;
	if (strlen($q) <= $max)
		return $q;
	return substr($q, 0, $max) . "...";
}

function query_long($q)
{
	$max = 100;
	if (strlen($q) <= $max)
		return $q;
	return substr($q, 0, $max) . "...";
}

function escape_query($q)
{
	return urlencode($q);
}

function connectdb()
{
	global $dbconn, $authinfo;
	if ($authinfo === null)
		return false;
	if ($dbconn !== null)
		return true;
	if (is_array($authinfo))
		$dbconn = @lx_connect_params($authinfo['host'],
			$authinfo['port'], $authinfo['user'],
			$authinfo['password']);
	else
		$dbconn = @lx_connect($authinfo);
	return !$dbconn ? false : true;
}

function multi_none($s)
{
	return array($s);
}
function multi_slash($s)
{
	return explode("/", $s);
}
function multi_semicolon($s)
{
	return explode(";", $s);
}
$multi_queries = array(
	"n" => array("name" => "None", "func" => "multi_none"),
	"sl" => array("name" => "Slash", "func" => "multi_slash"),
	"sc" => array("name" => "Semicolon", "func" => "multi_semicolon"),
);
$onerror_opts = array("a" => "Abort", "c" => "Continue");

?>
