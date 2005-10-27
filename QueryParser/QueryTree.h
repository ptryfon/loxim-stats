class QueryTree
{

private:
    int nodesNum;
    QueryTree *lSon;
    QueryTree *rSon;
    
public:
    QueryTree() {};
    virtual ~QueryTree(){};
    int toString(){
	return 0;
    };

};

