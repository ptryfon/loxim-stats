TCPProto.pdf: TCPProto.tex TCPProto.ind
	pdflatex TCPProto.tex
	pdflatex TCPProto.tex


TCPProto.ind: TCPProto.tex
	pdflatex TCPProto.tex
	pdflatex TCPProto.tex
	makeindex TCPProto.idx

clean: 
	rm -f -r tmp
	rm -f TCPProto.pdf TCPProto.idx TCPProto.aux TCPProto.ilg  TCPProto.aux TCPProto.ind TCPProto.log TCPProto.toc