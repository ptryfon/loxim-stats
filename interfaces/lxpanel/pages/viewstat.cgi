<?php

function render_stat($schema, $obj)
{
	switch ($schema['type']) {
	case "table1":
	case "table1kv":
		$title = "";
		if ($schema['title'] !== null)
			$title = $schema['title'];
		
		echo "<table class=\"stat1\">\n";
		if (title != "") {
			echo "<tr class=\"title\">\n";
			echo "<td colspan=\"2\">\n";
			echo $title;
			echo "</td>\n";
			echo "</tr>\n";
		}
		if (false) {
		} else if ($schema['type'] == "table1") {
			foreach ($obj as $k => $v) {
				if (is_array($v))
					continue;
				echo "<tr>\n";
				echo "<th>$k</th>\n";
				echo "<td>$v</td>\n";
				echo "</tr>\n";
			}
		} else if ($schema['type'] == "table1kv") {
			if (isset($obj[$schema['key']]))
				$obj = array($obj);
			foreach ($obj as $o) {
				echo "<tr>\n";
				echo "<th>{$o[$schema['key']]}</th>\n";
				echo "<td>{$o[$schema['value']]}</td>\n";
				echo "</tr>\n";
			}
		} else {}
		echo "</table>\n";
		break;
	case "table2":
		$title = "";
		if (isset($schema['title']))
			$title = $schema['title'];
		if ($title != "")
			echo "<h1>$title</h1>\n";
		echo "<table class=\"stat2\">\n";
		echo "<tr class=\"title\">\n";
		foreach ($schema['fields'] as $k => $v)
			echo "<td>$v</td>\n";
		echo "</tr>\n";
		if (($x = array_keys($schema['fields'])) && isset($obj[$x[0]]))
			$obj = array($obj);
		foreach ($obj as $row) {
			echo "<tr>\n";
			foreach ($schema['fields'] as $k => $v) {
				$val = "?";
				if (isset($row[$k]))
					$val = $row[$k];
				echo "<td>$val</td>\n";
			}
			echo "</tr>\n";
		}
		echo "</table>\n";
		break;
	case "legend":
		$title = "";
		if (isset($schema['title']))
			$title = $schema['title'];
		echo "<table class=\"statl\">\n";
		if ($title != "") {
			echo "<tr class=\"title\">\n";
			echo "<td colspan=\"2\">\n";
			echo $title;
			echo "</td>\n";
			echo "</tr>\n";
		}
		foreach ($schema['info'] as $k => $v) {
			echo "<tr>\n";
			echo "<th>$k</th>\n";
			echo "<td>$v</td>\n";
			echo "</tr>\n";
		}
		echo "</table>\n";
		break;
	case "sequence":
		foreach ($obj as $key => $o) {
			if (!is_array($o))
				continue;
			$sub = $schema['items'];
			if ($schema['title_f'] !== null)
				$sub['title'] = $o[$schema['title_f']];
			if (isset($schema['title_k']) && $schema['title_k'])
				$sub['title'] = $key;
			$ch = $o;
			if ($schema['data_f'] !== null)
				$ch = $o[$schema['data_f']];
			render_stat($sub, $ch);
		}
		break;
	case "combo":
		foreach ($schema['items'] as $s)
			render_stat($s, $obj);
		break;
	default:
		echo "<p>invalid stat type: {$schema['type']}</p>\n";
	}
}

	if ($statobj === null || !is_array($statobj)) {
		echo "<p>invalid usage</p>\n";
		return;
	}
	if ($rootobj === null || !is_array($rootobj)) {
		echo "<p>invalid usage</p>\n";
		return;
	}

	render_stat($statobj['struct'], $rootobj);
?>
