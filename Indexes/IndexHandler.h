#ifndef __INDEX_HANDLER_H__
#define __INDEX_HANDLER_H__

namespace Indexes
{

	class IndexHandler {
		
		private:
			string name, root, field;
		
		public:
			IndexHandler(string name, string root, string field) : name(name), root(root), field(field) {}
			string getName() {return name;}
			string getRoot() {return root;}
			string getField() {return field;}
	};

}

#endif /*INDEXHANDLER_H_*/
