package pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams;

import java.io.Closeable;
import java.io.IOException;
import java.net.Socket;
import java.net.SocketException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import pl.edu.mimuw.loxim.protocol.packages.PackagesFactory;
import pl.edu.mimuw.loxim.protogen.lang.java.template.base_packages.ASCPingPackage;
import pl.edu.mimuw.loxim.protogen.lang.java.template.base_packages.ASCPongPackage;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.BadPackageException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

public class PackageIO implements Closeable {

	private static final Log log = LogFactory.getLog(PackageIO.class);
	
	private final Socket socket;
	private final PackageInputStream pis;
	private final PackageOutputStream pos;
	private final Reader reader;
	private final Object writeMutex = new Object();
	private volatile boolean closed = false;
	
	private static class PoisonPillPackage extends Package {

		public static final long ID = Long.MIN_VALUE;
		
		public PoisonPillPackage(Exception e) {
			this.e = e;
		}
		
		private Exception e;
		
		@Override
		public long getPackageType() {
			return ID;
		}
		
		public Exception getException() {
			return e;
		}
	}
	
	private class Reader implements Runnable {
		private final Log log = LogFactory.getLog(Reader.class);
		private BlockingQueue<Package> buffer = new LinkedBlockingQueue<Package>();
		
		public Reader() {

		}
		
		@Override
		public void run() {
			log.debug("Starting reader thread");
			try {
				while (!closed) {
					log.debug("Reading package...");
					filterPackage(pis.readPackage());
				}
			} catch (Exception e) { // Yes, all exceptions as they have to be forwarded to the PackageIO object
				log.debug("Exception " + e + " was thrown and will be forwarded as a poison pill", e);
				try {
					PackageIO.this.close();
				} catch (IOException ioe) {
					log.debug(ioe, ioe);
				}
				buffer.add(new PoisonPillPackage(e));
			}
		}
		
		private void filterPackage(Package pac) {
			log.debug("Read package " + pac.getClass().getSimpleName());
			switch ((int) pac.getPackageType()) {
			case ASCPingPackage.ID:
				try {
					log.debug("PING...PONG");
					PackageIO.this.write(new ASCPongPackage());
				} catch (ProtocolException e) {
					// Could not send PONG? That's not critical, however the client can be disconnected.
					log.warn("Could not send PONG response to PING", e);
				}
				break;
			default:
				buffer.add(pac);
			}
		}
		
		public Package read() throws ProtocolException {
			try {
				return buffer.take();
			} catch (InterruptedException e) {
				throw new IllegalStateException(e); // this should never happen
			}
		}
	}
	
	public PackageIO(Socket socket) throws IOException {
		this.socket = socket;
		pos = new PackageOutputStream(socket.getOutputStream());
		pis = new PackageInputStream(socket.getInputStream(), PackagesFactory.getInstance());
		reader = new Reader();
		Thread readerThread = new Thread(reader);
		readerThread.start();
	}

	public void write(Package pac) throws ProtocolException {
		if (closed) {
			throw new ProtocolException("Connection closed");
		}
		log.debug("Writing package " + pac.getClass().getSimpleName());
		synchronized (writeMutex) {
			pos.writePackage(pac);			
		}
	}
	
	public Package read() throws ProtocolException {
		if (closed) {
			throw new ProtocolException("Connection closed");
		}
		Package pac = reader.read();
		if (pac.getPackageType() == PoisonPillPackage.ID) {
			PoisonPillPackage pill = (PoisonPillPackage) pac;
			Exception e = pill.getException();
			log.debug("Received poison pill package from the reader. Exception " + e + " was thrown and will be forwarded", e);
			if (e instanceof RuntimeException) {
				log.debug("Throwing RTE");
				throw new RuntimeException(e); // For a new stack trace
			} else if (e instanceof ProtocolException) {
				log.debug("Throwing PE");
				throw new ProtocolException(e); // For a new stack trace
			} else {
				log.debug("Throwing IE");
				throw new IllegalStateException("Unexpected exception", e);
			}
		}
		
		log.debug("Read package " + pac.getClass().getSimpleName());
		return pac;
	}
	
	public <T extends Package> T read(Class<T> packageType) throws ProtocolException {
		Package pac = read();
		 try {
			 return packageType.cast(pac);
		 } catch (ClassCastException e) {
			throw new BadPackageException(pac.getClass());
		}
	}
	
	@Override
	public void close() throws IOException {
		closed = true;
		socket.close();
	}
}
