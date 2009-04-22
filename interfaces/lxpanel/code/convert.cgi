<?php

function _data2xml($data, $w)
{
	$tag = "";
	$val = $data;
	if (is_null($data)) {
		$tag = "void";
		$val = "";
	} else if (is_int($data))
		$tag = "int";
	else if (is_float($data))
		$tag = "double";
	else if (is_string($data))
		$tag = "string";
	else if (is_bool($data)) {
		$tag = "bool";
		$val = $data ? "1" : "0";
	} else if (is_resource($data)) {
		$tag = "ref";
		$val = lx_refid($data);
	} else if (is_array($data)) {
		$tag = "array";
		if (isset($data['_type']))
			$tag = $data['_type'];
		$w->startElement($tag);
		foreach ($data as $k => $v)
			if (is_int($k))
				_data2xml($v, $w);
			else if (substr($k, 0, 1) != "_") {
				$w->startElement("bind");
				$w->writeElement("k", $k);
				$w->startElement("v");
				_data2xml($v, $w);
				$w->endElement();
				$w->endElement();
			}
		$w->endElement();
		return;
	} else
		$tag = "unknown";
	$w->writeElement($tag, $val);
}

function data2xml($data)
{
	global $config;
	$w = new XMLWriter;
	$w->openMemory();
	$w->setIndent(true);
	$w->setIndentString("  ");
	$w->writeDTD("data", "-//kk219459//DTD loxim data//EN",
		$config['uri'] . "xml/lx_data.dtd");
	$w->text("\n");
	$w->startElement("data");
	$w->writeAttribute("xmlns", $config['uri'] . "xml/lx_data.dtd");
	$w->startElement("part");
	_data2xml($data, $w);
	$w->endElement();
	$w->endElement();
	return $w->outputMemory();
}


function _xml2data_complex(XMLReader $xr)
{
	$ret = array();
	$done = false;

	$type = $xr->name;
	$ret["_type"] = $type;
	if ($xr->isEmptyElement)
		return $ret;

	$inkey = false;
	$key = "";
	while (@ $xr->read()) {
		switch ($xr->nodeType) {
		case XMLReader::ELEMENT:
			switch ($xr->name) {
			case "bind":
				break;
			case "k":
				$inkey = true;
				$key = "";
				break;
			case "v":
				$ret[$key] = _xml2data($xr, false);
				break;
			default:
				$ret[] = _xml2data($xr, true);
			}
			break;
		case XMLReader::END_ELEMENT:
			if ($xr->name == "k")
				$inkey = false;
			if ($xr->name == $type)
				$done = true;
			break;
		case XMLReader::TEXT:
			if ($inkey)
				$key .= $xr->value;
			break;
		default:
		}
		if ($done)
			break;
	}
	$ret["_type"] = $type;
	return $ret;
}

function _xml2data(XMLReader $xr, $skipfst = false)
{
	$done = false;
	$ret = null;
	$val = "";
	while ($skipfst || @$xr->read()) {
		$skipfst = false;
		switch ($xr->nodeType) {
		case XMLReader::ELEMENT:
			if ($xr->name == "int" || $xr->name == "double" ||
					$xr->name == "string" ||
					$xr->name == "bool" ||
					$xr->name == "ref" ||
					$xr->name == "void")
				$val = "";
			if ($xr->isEmptyElement) {
				$ret = null;
				$done = true;
			}
			if ($xr->name == "bag" || $xr->name == "struct" ||
					$xr->name == "seq") {
				$done = true;
				$ret = _xml2data_complex($xr);
			}
			break;
		case XMLReader::END_ELEMENT:
			switch ($xr->name) {
			case "int":
				$ret = intval($val);
				$done = true;
				break;
			case "double":
				$ret = doubleval($val);
				$done = true;
				break;
			case "string":
				$ret = $val;
				$done = true;
				break;
			case "bool":
				$ret = intval($val) != 0;
				$done = true;
				break;
			case "ref":
				$ret = lx_mkref($val);
				$done = true;
				break;
			case "void":
				$ret = null;
				$done = true;
				break;
			case "part":
				$done = true;
				break;
			default:
			}
			break;
		case XMLReader::TEXT:
			$val .= $xr->value;
			break;
		default:
		}
		if ($done)
			break;
	}
	return $ret;
}

function xml2data($xml, &$err)
{
	$parts = array();
	$err = "";

	if ($xml == "") {
		$err = "empty document";
		return;
	}
	$xr = XMLReader::xml($xml);
	$xr->setParserProperty(XMLReader::VALIDATE, true);
	$doctype = "";
	while (@ $xr->read()) {
		switch ($xr->nodeType) {
		case XMLReader::DOC_TYPE:
			$doctype = $xr->name;
			break;
		case XMLReader::ELEMENT:
			if ($xr->name == "part") {
				$v = $xr->getAttribute("version");
				$id = $xr->getAttribute("id");
				if ($v != "1") {
					$err = "unsupported version: $v";
					if ($id != "")
						$err .= " (part id: $id)";
					break;
				}
				$parts[] = _xml2data($xr);
			}
			break;
		case XMLReader::END_ELEMENT:
			break;
		case XMLReader::TEXT:
			break;
		default:
		}
		if ($doctype != "" && $doctype != "data")
			$err = "invalid doctype: $doctype";
		if ($err != "")
			break;
	}

	if (!array_key_exists(0, $parts) && $err == "")
		$err = "document does not contain any data parts";
	if (!$xr->isValid())
		$err = "document not valid";
	$xr->close();
	if ($err != "")
		return null;
	return $parts[0];
}


function _data2sbql($data)
{
	$s = "";
	if (is_array($data)) {
		switch ($data["_type"]) {
		case "bag":
		case "seq":
			$s .= "(";
			$i = 0;
			foreach ($data as $k => $v) {
				if (substr($k, 0, 1) == "_")
					continue;

				$ch = _data2sbql($v);
				if ($ch === null)
					continue;

				if ($i > 0)
					$s .= " union ";
				$i++;
				if (is_int($k))
					$s .= $ch;
				else
					$s .= "($ch as $k)";
			}
			$s .= ")";
			if ($i == 0)
				return null;
			break;
		case "struct":
			$s .= "(";
			$i = 0;
			foreach ($data as $k => $v) {
				if (substr($k, 0, 1) == "_")
					continue;

				$ch = _data2sbql($v);
				if ($ch === null)
					continue;

				if ($i > 0)
					$s .= ", ";
				$i++;
				if (is_int($k))
					$s .= $ch;
				else
					$s .= "$ch as $k";
			}
			$s .= ")";
			if ($i == 0)
				return null;
			break;
		default:
			$s = "\"unknown data type\"";
		}
	} else if (is_null($data))
		return null;
	else if (is_resource($data))
		$s = "refid(" . lx_refid($data) . ")";
	else if (is_string($data))
		$s = "\"" . $data . "\"";
	else if (is_bool($data))
		$s = $data ? "1>0" : "0>1";
	else
		$s = $data;
	return $s;
}

function data2sbql($data, $root = "")
{
	$q = "";
	$q .= "create ";
	$v = _data2sbql($data);
	if ($v === null)
		return array();
	$q .= $v;
	$q .= " as $root";
	return array($q);
}

?>
