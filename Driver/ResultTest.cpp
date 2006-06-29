
#include "Result.h"

using namespace Driver;

int main(int argc, char *argv[])
{
  ResultBag*      bag     = new ResultBag();
  ResultSequence* seq     = new ResultSequence();
  ResultStruct*   str     = new ResultStruct();
  ResultBinder*   rbinder = new ResultBinder("ala",bag);
  ResultString*   rstring = new ResultString("Ala ma kota");
  ResultInt*      rint    = new ResultInt(5);
  ResultDouble*   rdouble = new ResultDouble(2.3);
  ResultBool*     rbool   = new ResultBool(true);
  ResultReference* rref   = new ResultReference("321");
  ResultVoid*     rvoid   = new ResultVoid();
  ResultError*    rerror  = new ResultError();
  
  cout << seq << endl;
  cout << str << endl;
  cout << rbinder << endl;
  cout << rstring << endl;
  cout << rint << endl;  
  cout << rdouble << endl;
  cout << rbool << endl;
  cout << rref << endl;
  cout << rvoid << endl;
  cout << rerror << endl;    
  
  return 0;
}
