#ifndef PRIORITYOUTPUTSTREAM_H_
#define PRIORITYOUTPUTSTREAM_H_

#include "AbstractOutputStream.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h> 

namespace protocol{


/**
 * 	  Klasa rozbudowuje dany strumie� o dodatkow� funkcj� "writePriority". </br>
 * 
 * 	  Funkcja ta s�u�y temu, �e, w sytuacji gdy do klasy s� skierowane z r�nych w�tk�w
 * 	  polecenia wykonania funkcji write i writePriority - to polecenia writePriority
 * 	  zostan� wykonane z priorytetowo (nawet z mo�liwo�ci� zag�odzenia zapis�w
 * 	  dokonywanych funkcj� write) 
 */
class PriorityOutputStream: public AbstractOutputStream
{
	private:
		/**
		 * 	Para dw�ch semafor�w (binarnych):
		 * 	<ul>
		 * 		<li>0 - typowy mutex na wykonywanie kt�rej� z funkcji: write lub writePriority</li>
		 * 		<li>1 - semafor wspomagaj�cy - kt�ry �apie funkcja writePriority, by
		 * 				zagwarantowa� sobie pierwsze�stwo nad "write".
		 * 	<ul>
		 * 
		 *  Schemat synchronizacji funkcji write jest nast�puj�cy:<br/>
		 *  <code>
		 *      op(0: -1 ; 1: -1 ; 1: +1)   - opu�ci� 0 i 1 r�wnocze�nie - a nast�pnie podnie� 1. 
		 * 			wy�lij(). 
		 * 		op(0: +1)				  - podnie� 0
		 *  </code> 
		 * 
		 *  Schemat synchronizacji funkcji writePriority jest nast�puj�cy:<br/>
		 *  <code>
		 * 		op(1: -1);				   - opu�� 1
		 *      op(0: -1; 1:+1);		   - opu�� 0 (mutexa), podnie�� 1
		 * 		 	wy�lij(). 
		 * 		P(0: +1)				  - podnie� 0
		 *  </code>
		 * 
		 * Czyli writePriority chwyta "1" i w ten spos�b blokuje zwyk�e "write" do czasu, a� si� wykona
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
