<?php
	if (isset($R['do_login'])) {
		if (isset($R['connstr'])) {
			$connstr = $R['connstr'];
			$arr = split("[ \r\n]+", $connstr);
			$connarr = array();
			foreach ($arr as $v)
				if ($v != "")
					$connarr[] = $v;
			$connstr = implode(" ", $connarr);
			$dbconn = @lx_connect($connstr);
			if (!$dbconn)
				$error = lx_error_conn();
			else
				$_SESSION['authinfo'] = $connstr;
		} else {
			$ai = array();
			$ai['host'] = $R['host'];
			$ai['port'] = $R['port'];
			$ai['user'] = $R['user'];
			$ai['password'] = $R['password'];
			$dbconn = @lx_connect_params($ai['host'],
				$ai['port'], $ai['user'], $ai['password']);
			if (!$dbconn)
				$error = lx_error_conn();
			else
				$_SESSION['authinfo'] = $ai;
		}
	}

	if (isset($R['do_logout'])) {
		unset($_SESSION['authinfo']);
		unset($_SESSION['history']);
		unset($_SESSION['favourites']);
		$info = "Logged out";
	}
?>
