
#include "Result.h"

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
}
