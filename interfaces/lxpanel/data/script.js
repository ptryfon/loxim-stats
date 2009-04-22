

function setvisbyid(name, visible)
{
	var e = document.getElementById(name);
	if (e != null)
		e.style.display = visible ? '' : 'none';
}

function togglevisbyid(name)
{
	var e = document.getElementById(name);
	if (e == null)
		return null;
	if (e.style.display == 'none')
		e.style.display = '';
	else
		e.style.display = 'none';
	return e.style.display != 'none';
}


var TreeType = { Expanded:0, Collapsed:1, Atomic:2 };
var imgsrcs = [ 'data/tree1.png', 'data/tree2.png', 'data/tree3.png'];

function nav_tree2row(element)
{
	return element.parentNode;
}

function nav_ref2row(element)
{
	return element.parentNode;
}

function nav_form2row(element)
{
	return element.parentNode;
}

function nav_row2tree(element)
{
	var ch = element.childNodes;
	for (var i = 0; i < ch.length; i++)
		if (ch[i].nodeType == Node.ELEMENT_NODE
					&& ch[i].nodeName == 'A')
			return ch[i];
	return null;
}

function nav_row2ref(element)
{
	var ch = element.childNodes;
	for (var i = 0; i < ch.length; i++)
		if (ch[i].nodeType == Node.ELEMENT_NODE
					&& ch[i].nodeName == 'A'
					&& ch[i].className == 'ref')
			return ch[i];
	return null;
}

function nav_row2img(row)
{
	var e = nav_row2tree(row);
	if (e == null)
		return null;
	var ch = e.childNodes;
	for (var i = 0; i < ch.length; i++)
		if (ch[i].nodeType == Node.ELEMENT_NODE
					&& ch[i].nodeName == 'IMG')
			return ch[i];
	return null;
}

function nav_getrowstate(row)
{
	var img = nav_row2img(row);
	if (img == null)
		return null;
	var src = img.getAttribute('src');
	for (tt in TreeType)
		if (src.search(imgsrcs[TreeType[tt]]) != -1)
			return TreeType[tt];
	return null;
}

function nav_setrowstate(row, tt)
{
	var img = nav_row2img(row);
	if (img == null)
		return null;
	img.setAttribute('src', imgsrcs[tt]);
}

function nav_setrowvis(row, tt)
{
	var vis;
	switch (tt) {
	case TreeType.Expanded:
		vis = '';
		break;
	case TreeType.Collapsed:
		vis = 'none';
		break;
	default:
		vis = null;
	}
	nav_setrowstate(row, tt);
	if (vis == null)
		return;
	for (var s = row.nextSibling; s != null; s = s.nextSibling) {
		if (s.nodeType != Node.ELEMENT_NODE || s.nodeName != 'LI')
			continue;
		s.style.display = vis;
		break;
	}
}

function nav_ishidden(row)
{
	var s = row;
	while (s.nodeName == 'LI' || s.nodeName == 'UL') {
		if (s.style.display == 'none')
			return true;
		s = s.parentNode;
	}
	return false
}

function nav_row2children(row)
{
	for (var s = row.nextSibling; s != null; s = s.nextSibling)
		if (s.nodeType == Node.ELEMENT_NODE && s.nodeName == 'LI')
			break;
	if (s == null)
		return null;
	for (var ch = s.firstChild; ch != null; ch = ch.nextSibling)
		if (ch.nodeType == Node.ELEMENT_NODE && ch.nodeName == 'UL')
			return ch;
	return null;
}

function nav_nextelement(element, name)
{
	for (var s = element; s != null; s = s.nextSibling)
		if (s.nodeType == Node.ELEMENT_NODE && s.nodeName == name)
			break;
	return s;
}

function nav_prevelement(element, name)
{
	for (var s = element; s != null; s = s.previousSibling)
		if (s.nodeType == Node.ELEMENT_NODE && s.nodeName == name)
			break;
	return s;
}

function nav_busy(row)
{
	return nav_nextelement(row.firstChild, 'IMG') != null;
}

function beep()
{
	alert('beep!');
}

function request_error(msg)
{
	alert('Request error:\n' + msg);
}


function loader_create(row)
{
	var img = document.createElement('img');
	img.setAttribute('class', 'loader');
	img.setAttribute('alt', 'loader');
	img.setAttribute('src', 'data/loader.gif');
	row.appendChild(img);
	return img;
}

function loader_destroy(img)
{
	img.parentNode.removeChild(img);
}

function vis_elt(element)
{
	var row = nav_tree2row(element);
	var st = nav_getrowstate(row);
	var nst;
	switch (st) {
	case TreeType.Expanded:
		nst = TreeType.Collapsed;
		break;
	case TreeType.Collapsed:
		nst = TreeType.Expanded;
		break;
	default:
		nst = null;
	}
	if (nst == null)
		return;
	nav_setrowvis(row, nst);
}

function exp_coll_all(id, show)
{
	var element = document.getElementById(id);
	if (element == null)
		return;
	var nst;
	if (show)
		nst = TreeType.Expanded;
	else
		nst = TreeType.Collapsed;
	var ch = element.getElementsByTagName('li');
	for (var i = 0; i < ch.length; i++) {
		var st = nav_getrowstate(ch[i]);
		if (st != TreeType.Collapsed && st != TreeType.Expanded)
			continue;
		nav_setrowvis(ch[i], nst);
	}
}

function refclick(elem, uri)
{
	var row = nav_ref2row(elem);
	if (nav_busy(row)) {
		beep();
		return;
	}
	var st = nav_getrowstate(row);
	if (st != TreeType.Atomic)
		return;

	var img = loader_create(row);
	nli = document.createElement('li');
	row.parentNode.insertBefore(nli, row.nextSibling);

	nav_setrowstate(row, TreeType.Expanded);
	request(nli, uri, function() {
		loader_destroy(img);
	});
}

function deref_all(id, onlyvisible)
{
	var root = document.getElementById(id);
	var ch = root.getElementsByTagName('li');
	var queue = [];
	for (var i = 0; i < ch.length; i++) {
		var st = nav_getrowstate(ch[i]);
		if (st != TreeType.Atomic)
			continue;
		if (onlyvisible && nav_ishidden(ch[i]))
			continue;
		var rl = nav_row2ref(ch[i]);
		if (rl != null)
			queue.push(rl);
	}
	for (var i = 0; i < queue.length; i++)
		queue[i].onclick();
}

function commondelclick(row, uri)
{
	if (nav_busy(row)) {
		beep();
		return;
	}
	var ch = nav_row2children(row);
	var img = loader_create(row);
	request(null, uri, function(t) {
		loader_destroy(img);
		if (t.search('OK') == -1) {
			request_error(t);
			return;
		}
		row.parentNode.removeChild(row);
		if (ch != null)
			ch.parentNode.parentNode.removeChild(ch.parentNode);
	});
}

function refdelclick(element, uri)
{
	return commondelclick(nav_ref2row(element), uri);
}

function insertclick(element)
{
	var f = nav_nextelement(element, 'FORM');
	if (f == null)
		return;
	f.style.display = 'inline';
	element.style.display = 'none';
	f.elements['field'].focus();
}

function editclick(element)
{
	var f = nav_nextelement(element, 'FORM');
	if (f == null)
		return;
	f.style.display = 'inline';
	element.style.display = 'none';
	f.elements['value'].focus();
}

function insertcancelclick(element)
{
	var f = element.parentNode;
	var a = nav_prevelement(f, 'A');
	if (a == null)
		return;
	a.style.display = '';
	f.style.display = 'none';
	f.elements['field'].value = f.elements['field'].defaultValue;
}

function editcancelclick(element)
{
	var f = element.parentNode;
	var a = nav_prevelement(f, 'A');
	if (a == null)
		return;
	a.style.display = '';
	f.style.display = 'none';
	f.elements['value'].value = f.elements['value'].defaultValue;
	var opts = f.elements['type'].options;
	for (var i = 0; i < opts.length; i++)
		if (opts[i].defaultSelected)
			f.elements['type'].selectedIndex = i;
}

function insertokclick(element)
{
	var f = element.parentNode;
	var uri = f.elements['uri'].value;
	uri += '&id=' + escape_uri(f.elements['id'].value);
	uri += '&path=' + escape_uri(f.elements['path'].value);
	uri += '&field=' + escape_uri(f.elements['field'].value);
	var row = nav_form2row(f);
	if (nav_busy(row)) {
		beep();
		return;
	}
	var ch = nav_row2children(row);
	var img = loader_create(row);
	var n = document.createElement('li');
	request(n, uri, function(t) {
		insertcancelclick(f.firstChild);
		loader_destroy(img);
		var li = nav_nextelement(n.firstChild, 'LI');
		if (li == null) {
			request_error(t);
			return;
		}
		var chn = n.childNodes;
		for (var i = 0; i < chn.length; i++)
			ch.appendChild(chn[i]);
	});
}

function editokclick(element)
{
	var f = element.parentNode;
	var uri = f.elements['uri'].value;
	uri += '?op=' + escape_uri('obj_edit');
	uri += '&id=' + escape_uri(f.elements['id'].value);
	uri += '&path=' + escape_uri(f.elements['path'].value);
	uri += '&value=' + escape_uri(f.elements['value'].value);
	uri += '&type=' + escape_uri(f.elements['type'].value);
	var row = nav_form2row(f);
	if (nav_busy(row)) {
		beep();
		return;
	}
	var ch = nav_row2children(row);
	var img = loader_create(row);
	var n = document.createElement('li');
	request(n, uri, function(t) {
		loader_destroy(img);
		var li = nav_nextelement(n.firstChild, 'LI');
		if (li == null) {
			request_error(t);
			return;
		}
		var par = row.parentNode;
		var next = row.nextSibling;
		row.parentNode.removeChild(row);
		if (ch != null)
			ch.parentNode.parentNode.removeChild(ch.parentNode);
		var chn = n.childNodes;
		for (var i = 0; i < chn.length; i++)
			par.insertBefore(chn[i], next);
	});
}

function editdeleteclick(element)
{
	var f = element.parentNode;
	var uri = f.elements['uri'].value;
	uri += '?op=' + escape_uri('obj_del');
	uri += '&id=' + escape_uri(f.elements['id'].value);
	uri += '&path=' + escape_uri(f.elements['path'].value);
	return commondelclick(nav_form2row(f), uri);
}

function showhidequery_click(element, id)
{
	element.firstChild.nodeValue = togglevisbyid(id) ?
		'Hide query' : 'Show query';
}

function import_vis(type)
{
	document.getElementById('type').value = type;
	var types = new Array('input', 'upload', 'uri');
	for (i in types) {
		var vis = 'none';
		if (types[i] == type)
			vis = '';
		document.getElementById(types[i]).style.display = vis;
	}
	document.getElementById('focus_' + type).focus();
}

function createroot(element)
{
	var f = element.form;
	var uri = f.elements['uri'].value;
	uri += "&name=" + f.elements['name'].value;
	uri += "&count=" + f.elements['count'].value;
	var ldr = document.getElementById('createrootbusy');
	if (ldr == null)
		return;
	if (nav_busy(ldr)) {
		beep();
		return;
	}
	var roots = document.getElementById('roots');
	if (roots == null)
		return;
	var n = document.createElement('p');
	var img = loader_create(ldr);
	request(n, uri, function(t) {
		loader_destroy(img);
		var div = nav_nextelement(n.firstChild, 'DIV');
		if (div == null || div.getAttribute('id') != 'roots') {
			request_error(t);
			return;
		}
		var par = roots.parentNode;
		var tmp = roots.nextSibling;
		par.removeChild(roots);
		par.insertBefore(div, tmp);
	});
}

function openwindow(uri)
{
	var params = 'toolbar=no,width=500,height=300,resizable=yes,scrollbars=no';
	var w = window.open(uri, 'lxpanel window', params);
	w.focus();
	return w;
}

function getajax()
{
	if (window.XMLHttpRequest)
		return new XMLHttpRequest();
	if (window.ActiveXObject)
		return new ActiveXObject("Microsoft.XMLHTTP");
	return null;
}

function request_id(id, uri, destructor)
{
	var tgt = document.getElementById(id);
	if (tgt == null)
		return;
	return request(tgt, uri, destructor);
}

function request(tgt, uri, destructor)
{
	var ajax = getajax();
	if (ajax == null)
		return;
	ajax.open("GET", uri);
	ajax.onreadystatechange = function() {
		if (ajax.readyState != 4)
			return;
		if (ajax.status == 200) {
			if (tgt != null)
				tgt.innerHTML = ajax.responseText;
		} else
			alert("GET failed status: " + ajax.status);
		if (destructor != null)
			destructor(ajax.responseText);
		delete ajax;
		ajax = null;
	}
	ajax.send(null);
}

function escape_uri(uri)
{
	var s = escape(uri);
	s = s.replace(/\+/g, "%2B");
	s = s.replace(/\//g, "%2F");
	return s;
}

