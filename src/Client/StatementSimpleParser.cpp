#include <Client/StatementSimpleParser.h>
#include <string.h>
#include <stdlib.h>

using namespace Client;

StatementSimpleParser::StatementSimpleParser()
{
	buffer=strdup("");
}

StatementSimpleParser::~StatementSimpleParser()
{
	free(buffer);
}


void StatementSimpleParser::append(char* str)
{	
	int b_len  =strlen(buffer);
	int str_len=strlen(str);
	buffer=(char*)realloc(buffer,b_len+str_len+1);
	memcpy(buffer+b_len,str,str_len+1);
}

char* StatementSimpleParser::getStatement(char* for_head)
{
		char stack[STACK_SIZE];
		int stack_level=0;
		int mode=0; 
			// 0 - NORMAL
			// 1-  QUOT
			// 2 - QUOT-esc
		
		int i=0;
		while (buffer[i])
		{
			switch (mode)
			{
				case 0 /*NORM*/:
					switch(buffer[i])
					{
						case ';' :
							if (stack_level==0)
							{	
								char* res=(char*)malloc(i+2);
								memcpy(res,buffer,i+1); //Kopiujemy ze ¶rednikiem w³±cznie
								res[i+1]=0;
								
								//Kopiujemy ogon na pocz±tek:
								int j=1;
								while(buffer[i+j]!=0)
								{
									buffer[j-1]=buffer[i+j];
									j++;
								}									
								buffer[j-1]=0;
								return res;
							};
							break;
						case '"' :
							stack[stack_level++]='"';
							mode=1;
							break;
							
						case '(' :
							stack[stack_level++]='(';
							break;
						case ')' :
							if ((stack_level>0)&&(stack[stack_level-1]=='('))
								stack_level--;
							break;
								
						case '[' :
							stack[stack_level++]='[';
							break;
						case ']' :
							if ((stack_level>0)&&(stack[stack_level-1]=='['))
								stack_level--;
							break;						
					}
					break;
					
				case 1 /*QUOT*/:
					switch(buffer[i])
					{
						case '"':
							stack_level--;
							mode=0;
							break;
						case '\\':
							mode=2;
							break;
					}				
				case 2 /*QUAT-esc*/:
					mode=1;
					break;				
			}
			i++;
		};
		
		if (for_head)
		{
			if(stack_level==0)
				*for_head=0;
			else
				*for_head=stack[stack_level-1];
		}
		
		return NULL;
}
