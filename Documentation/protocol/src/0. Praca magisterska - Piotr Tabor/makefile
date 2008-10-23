VARS=TEXINPUTS=":.:../protogen-doc:../OldProtoDocs:../newTCPProto:../SBQLviaLDAP"

Praca_mgr_Loxim_protokol: Praca_mgr_Loxim_protokol.tex
	$(VARS) pdflatex Praca_mgr_Loxim_protokol.tex
	$(VARS) pdflatex Praca_mgr_Loxim_protokol.tex > /tmp/res
	$(VARS) pdflatex Praca_mgr_Loxim_protokol.tex > /dev/null
#	grep undefined /tmp/res 

single:
	$(VARS) pdflatex Praca_mgr_Loxim_protokol.tex

clean:
	rm Praca_mgr_Loxim_protokol.pdf
