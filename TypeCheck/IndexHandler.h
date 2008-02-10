#ifndef __INDEX_HANDLER_H__
#define __INDEX_HANDLER_H__

namespace Indexes
{

	class IndexHandler {
		
		private:
			string name, root, field;
			LogicalID* lid;
		
		public:
			IndexHandler(string name, string root, string field, LogicalID* lid) : name(name), root(root), field(field), lid(lid) {}
			string getName() {return name;}
			string getRoot() {return root;}
			string getField() {return field;}
			LogicalID* getLogicalID() {return lid;}
	};

}

#endif /*INDEXHANDLER_H_*/
