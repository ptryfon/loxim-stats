#ifndef STORELOCK_H_
#define STORELOCK_H_



class StoreLock {
	public:
		string configFileName;
		StoreLock(string _configFileName) { configFileName = _configFileName; }
		int lock();
		int unlock();
};

#endif /*STORELOCK_H_*/
