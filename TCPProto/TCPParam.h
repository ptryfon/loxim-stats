#ifndef _TCPPARAM_H_
#define _TCPPARAM_H_


namespace TCPProto {

	class TCPParam {
		
		private:
			string server;
			int port;
			
		public:
			string getServer() {return server;}
			void setServer(string s) {server = s;}
			int getPort() {return port;}
			void setPort(int p) {port = p;}
	};

} //namespace

#endif //_TCPPARAM_H_
