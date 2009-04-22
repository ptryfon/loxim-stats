<div class="loginpage">
<h1>LoXiM login</h1>

<div class="error" id="javascript_error">
Error: javascript not supported
</div>
<div class="error" id="ajax_error">
Error: ajax not supported
</div>
<script type="text/javascript">
setvisbyid('javascript_error', false);
if (getajax() != null)
	setvisbyid('ajax_error', false);
</script>

<?php
	if (!function_exists('lx_query'))
		echo "<div class=\"error\">Error: lxsbql module not available</div>\n";
	if (!class_exists('XMLReader'))
		echo "<div class=\"error\">Error: xmlreader module not available</div>\n";
	if (!class_exists('XMLWriter'))
		echo "<div class=\"error\">Error: xmlwriter module not available</div>\n";
	if ($error != "")
		echo "<div class=\"error\">Error: $error</div>\n";
	if ($info != "")
		echo "<div class=\"info\">Info: $info</div>\n";
?>

<div class="authsel">
<a href="#" onclick="setLoginType(true); return false;">By fields</a>
| <a href="#" onclick="setLoginType(false); return false;">By connection string</a>
</div>

<?php
	$auth = $config['auth'];
	if (!isset($R['reset'])) {
		if (isset($R['host']))
			$auth['host'] = $R['host'];
		if (isset($R['port']))
			$auth['port'] = $R['port'];
		if (isset($R['user']))
			$auth['user'] = $R['user'];
		if (isset($R['password']))
			$auth['password'] = $R['password'];
	}
	echo "<form action=\"$action\" method=\"post\" id=\"form_params\">\n";
	echo "<table class=\"form\">\n";
	echo "<tr>\n";
	echo "<th>Host</th>\n";
	echo "<td>\n";
	echo "<input type=\"text\" name=\"host\" value=\"$auth[host]\" id=\"host\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr>\n";
	echo "<th>Port</th>\n";
	echo "<td>\n";
	echo "<input type=\"text\" name=\"port\" value=\"$auth[port]\" id=\"port\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr>\n";
	echo "<th>User</th>\n";
	echo "<td>\n";
	echo "<input type=\"text\" name=\"user\" value=\"$auth[user]\" id=\"user\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr>\n";
	echo "<th>Password</th>\n";
	echo "<td>\n";
	echo "<input type=\"password\" name=\"password\" value=\"$auth[password]\" id=\"password\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr class=\"submit\">\n";
	echo "<td colspan=\"2\">\n";
	echo "<input type=\"submit\" value=\"Connect\" name=\"do_login\" />\n";
	echo "<input type=\"submit\" value=\"Reset\" name=\"reset\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "</table>\n";
	echo "</form>\n";

	$str = "";
	if ($auth['host'] != "")
		$str .= "host=$auth[host]\n";
	if ($auth['port'] != "")
		$str .= "port=$auth[port]\n";
	if ($auth['user'] != "")
		$str .= "user=$auth[user]\n";
	if ($auth['password'] != "")
		$str .= "password=$auth[password]\n";
	if (isset($R['connstr']) && !isset($R['reset']))
		$str = $R['connstr'];
	echo "<form action=\"$action\" method=\"post\" id=\"form_string\">\n";
	echo "<table class=\"form\">\n";
	echo "<tr>\n";
	echo "<td colspan=\"2\">\n";
	echo "<textarea name=\"connstr\" id=\"connstr\" rows=\"6\" cols=\"30\">\n";
	echo $str;
	echo "</textarea>\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "<tr class=\"submit\">\n";
	echo "<td colspan=\"2\">\n";
	echo "<input type=\"submit\" value=\"Connect\" name=\"do_login\" />\n";
	echo "<input type=\"submit\" value=\"Reset\" name=\"reset\" />\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "</table>\n";
	echo "</form>\n";
?>

<script type="text/javascript">
function setLoginType(params)
{
	var p = 'none'; if (params) p = 'block';
	var s = 'block'; if (params) s = 'none';
	document.getElementById('form_params').style.display = p;
	document.getElementById('form_string').style.display = s;
	if (params)
		document.getElementById('user').focus();
	else
		document.getElementById('connstr').focus();
}
<?php
	$b = isset($R['connstr']) ? "false" : "true";
	echo "setLoginType($b);\n"
?>
</script>

<div class="version">
lxsbql module version: <?php echo LXSBQL_VERSION; ?>
</div>

</div>
