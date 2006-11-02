package objectBrowser.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Map;
import java.util.Map.Entry;

import objectBrowser.driver.loxim.result.Result;
import objectBrowser.driver.loxim.result.ResultCollection;
import objectBrowser.driver.loxim.result.ResultReference;
import objectBrowser.driver.loxim.result.ResultVoid;

public class ParamStatementPackage implements Package {
	long statementNumber;
	Map<String, Result> params;
	
	public int getResultLength(Result result) {
		 
		if (result instanceof ResultCollection) {
			
		} else if (result instanceof ResultReference) {
			
		} else if (result instanceof ResultVoid) {
			
		}
		return 0; /* TODO */
	}

	public void deserialize(InputStream stream, int size) throws IOException,
			SBQLException {
		// TODO Auto-generated method stub

	}

	public short getType() {
		return Package.TYPE_PARAMSTATEMENT;
	}

	public int serialize(OutputStream stream) throws IOException {
		int length = ConversionUtils.getEnumSize() + 3 * 4;
		/*
		 * Rodzaj pakietu - 1 bajt
		 * Całkowita długość - 4 bajty
		 * Numer zapytania - 4 bajty
		 * Liczba parametrów - 4 bajty
		 */
		
		for (Entry<String, Result> param : params.entrySet()) {
			/* Nazwa parametru - z bajtem #0 na końcu */
			length = length + param.getKey().length() + 1;
		}

		return length;
	}

	public long getStatementNumber() {
		return statementNumber;
	}

	public void setStatementNumber(long statementNumber) {
		this.statementNumber = statementNumber;
	}

	public Map<String, Result> getParams() {
		return params;
	}

	public void setParams(Map<String, Result> params) {
		this.params = params;
	}

}
