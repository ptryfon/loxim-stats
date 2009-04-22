#!.../php-5.2.9/sapi/cgi/php-cgi
<?php
	include("code/defs.cgi");

	$skip_headers = false;
	$skip_intro = false;
	if ($R['page'] == "query" && isset($R['export']))
		$skip_headers = $skip_intro = true;
	if ($R['page'] == "querywin")
		$skip_intro = true;

	$error = "";
	$dbconn = null;
	include("code/actions.cgi");

	$authinfo = null;
	if (isset($_SESSION['authinfo']))
		$authinfo = $_SESSION['authinfo'];
	if (!connectdb() && $authinfo !== null) {
		unset($_SESSION["authinfo"]);
		$authinfo = null;
		$error = "Connection lost:<br />" . lx_error_conn();
	}
	if ($authinfo === null) {
		include("code/begin.cgi");
		include("code/login.cgi");
		include("code/end.cgi");
		exit;
	}

	if (!$skip_headers)
		include("code/begin.cgi");
	if (!$skip_intro)
		include("code/header.cgi");
	$page = isset($R['page']) ? $R['page'] : "";
	if ($page == "")
		$page = "home";
	$pages = array("home", "query", "querywin", "import", "browse",
		"stats");
	if (in_array($page, $pages))
		include("pages/$page.cgi");
	else
		echo "<p>invalid page requested</p>\n";

	if (!$skip_intro)
		include("code/footer.cgi");

	if (!$skip_headers)
		include("code/end.cgi");
?>
