#ifndef PRIORITYOUTPUTSTREAM_H_
#define PRIORITYOUTPUTSTREAM_H_

#include "AbstractOutputStream.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h> 

namespace protocol{


/**
 * 	  Klasa rozbudowuje dany strumieñ o dodatkow± funkcjê "writePriority". </br>
 * 
 * 	  Funkcja ta s³u¿y temu, ¿e, w sytuacji gdy do klasy s± skierowane z ró¿nych w±tków
 * 	  polecenia wykonania funkcji write i writePriority - to polecenia writePriority
 * 	  zostan± wykonane z priorytetowo (nawet z mo¿liwo¶ci± zag³odzenia zapisów
 * 	  dokonywanych funkcj± write) 
 */
class PriorityOutputStream: public AbstractOutputStream
{
	private:
		/**
		 * 	Para dwóch semaforów (binarnych):
		 * 	<ul>
		 * 		<li>0 - typowy mutex na wykonywanie której¶ z funkcji: write lub writePriority</li>
		 * 		<li>1 - semafor wspomagaj±cy - który ³apie funkcja writePriority, by
		 * 				zagwarantowaæ sobie pierwszeñstwo nad "write".
		 * 	<ul>
		 * 
		 *  Schemat synchronizacji funkcji write jest nastêpuj±cy:<br/>
		 *  <code>
		 *      op(0: -1 ; 1: -1 ; 1: +1)   - opu¶ciæ 0 i 1 równocze¶nie - a nastêpnie podnie¶ 1. 
		 * 			wy¶lij(). 
		 * 		op(0: +1)				  - podnie¶ 0
		 *  </code> 
		 * 
		 *  Schemat synchronizacji funkcji writePriority jest nastêpuj±cy:<br/>
		 *  <code>
		 * 		op(1: -1);				   - opu¶æ 1
		 *      op(0: -1; 1:+1);		   - opu¶æ 0 (mutexa), podnie¶æ 1
		 * 		 	wy¶lij(). 
		 * 		P(0: +1)				  - podnie¶ 0
		 *  </code>
		 * 
		 * Czyli writePriority chwyta "1" i w ten sposób blokuje zwyk³e "write" do czasu, a¿ siê wykona
		 * */
		int semaphores;
		
	protected:
		AbstractOutputStream *baseOutputStream;
	
	 public:
  		 PriorityOutputStream(AbstractOutputStream *a_baseOutputStream);
  		 virtual ~PriorityOutputStream();
  
	/**
	 * Zwraca status po zapisie
	 */
	virtual int write(char* buffor,unsigned long int off,unsigned long int length);
	
	virtual int writePriority(char* buffor,unsigned long int off,unsigned long int length);
	
	virtual int flush(){return baseOutputStream->flush();};	
	
	int getStatus(){return baseOutputStream->getStatus();};
		
	virtual void close(void);	
};
}
#endif /*PRIORITYOUTPUTSTREAM_H_*/
