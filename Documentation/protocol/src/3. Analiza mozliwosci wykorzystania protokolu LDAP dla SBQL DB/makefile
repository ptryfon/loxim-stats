all: sbql_via_ldap.pdf

sbql_via_ldap.pdf: sbql_via_ldap.tex \
 img/ldap_mimuw.jpg \
 img/ServerLDAPzDostepemSBQL.jpg \
 img/ServerSBQLjakoBackendLDAP.jpg \
 img/ServerSBQLzDostepemLDAP.jpg

	pdflatex sbql_via_ldap.tex
	pdflatex sbql_via_ldap.tex

img/%.jpg: dia/%.dia
	dia $< -e $@

clean:
	rm -f sbql_via_ldap.pdf
	rm -f img/*.jpg
	rm -f tmp/*
