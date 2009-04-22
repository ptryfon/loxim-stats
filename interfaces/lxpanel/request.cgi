#!.../php-5.2.9/sapi/cgi/php-cgi
<?php

function die_error($s)
{
	echo $s;
	exit();
}

	include("code/defs.cgi");
	include("code/requests.cgi");
	header("Content-type: text/html");

	$authinfo = null;
	if (isset($_SESSION['authinfo']))
		$authinfo = $_SESSION['authinfo'];
	if ($authinfo === null)
		die_error("not authorized");

	$op = "";
	if (isset($R['op']))
		$op = $R['op'];
	switch ($op) {
	case "favourites":
		req_favourites($R);
		break;
	case "deref":
		req_deref($R);
		break;
	case "obj_del":
		req_obj_del($R);
		break;
	case "obj_insert":
		req_obj_insert($R);
		break;
	case "obj_edit":
		req_obj_edit($R);
		break;
	case "obj_create_root":
		req_obj_create_root($R);
		break;
	case "":
		die_error("invalid usage");
	default:
		die_error("unknown op: " . $R['op']);
	}
?>
