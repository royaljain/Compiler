#include <iostream>
#include "Scanner.h"
#include "Parser.h"
using namespace std;
int main (int argc, char* argv[])
{
	Parser parser;
	int inp=argc;
    
	//cout<< "input the number of functions of which you would like to see the AST"<<endl;
	
	
    //Scanner::linenum += inp;
    for(int i=0;i<inp;i++)
	{
		//cout<<"input function name "<<i<<endl;
		string a(argv[i]);
		parser.functions.insert(a);
	}
  
  parser.parse();
}


