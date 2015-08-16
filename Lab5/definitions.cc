#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stack>

using namespace std;

int goffset;
int loffset;
string dtype;
globalsymboltable* gsym;
localsymboltable* lsym;
string constantExpressionType="";
string currentfunctype;
string currentfuncid;
vector<string> callfuncparam;
stack<string> callfuncid;
string currentfuncparam;
bool isreturn;
bool parameters;
int ctr = 0;
vector<string> code;
int label = 0;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}


string arraystrip(string arr)
{
	int i = arr.find(",");
	return arr.substr(i+1,arr.size()-i-2);
}



bool compatible(string type1,string type2)
{
	if(type1 == "void" || type2 == "void" || type1.substr(0,6) == "array(" || type2.substr(0,6) == "array(" || type1 =="error" || type2 =="error")
	{
		return false;
	}
	else
	{
		return true;	
	}	
}

bool paramcompatible(string type1,string type2)
{
		
		vector<string> tokens1 = split(type1,':');
		vector<string> tokens2 = split(type2,':'); 
		
		
		
		if(tokens1.size()!=tokens2.size())return false;
		
		for(int i=0;i<tokens1.size();i++)
		{
			if(!(compatible(tokens1[i],tokens2[i])))
			return false;		
		}
		
		return true;
}


string binaryType(string type1,string type2)
{
	if(type1=="float" || type2=="float")
	{
		return "float";
	}
	else
	{
		return "int";
	}
}
funcdef_node::funcdef_node(string id1,string type1,abstract_node *e1)
{
	constant=id1;
	type=type1;
	exp1=e1;
}

void funcdef_node::generate_code()
{
	int i =constant.find("#");
	code.push_back("void "+constant.substr(0,i)+"()\n");
	(exp1)->onStack=false;
	(exp1)->generate_code();
}

compoundstmt_node :: compoundstmt_node()
{
	;
}

compoundstmt_node :: compoundstmt_node(abstract_node *stmt_vector)
{
	statement1=stmt_vector;
}

compoundstmt_node :: compoundstmt_node(abstract_node *decl_vector,abstract_node *stmt_vector)
{
	statement1=stmt_vector;
	statement2=decl_vector;
}
void compoundstmt_node::generate_code()
{
	code.push_back("{\n ");
	
	if(currentfuncid!="main#0")
	{
		code.push_back("pushi(ebp);\n"); // Setting dynamic link
		code.push_back("move(esp,ebp);\n");
	}
	if(statement2!=0)
	{
		(statement2)->onStack=false;
		(statement2)->generate_code();
	}
	if(statement1!=0)
	{
		(statement1)->onStack=false;
		(statement1)->generate_code();
	}
	if(currentfunctype=="void")
	{
		map<string,localattributes> copy;
		copy=lsym->M;
		int min=-1;
		while(min<0)
		{
			min = 0;
			
			localattributes localmin;
			for (map<string,localattributes>::iterator it=copy.begin(); it!=copy.end(); ++it)
			{
				if((it->second).off<min)
				{
					min = (it->second).off;
					localmin=it->second;
				}
			}
			
			copy.erase(localmin.id);
			
			string temp = localmin.type;
			
			while(temp.find("array(")==0)
			{
				temp = arraystrip(temp);
			}

			int k = localmin.width/4;
			
			if(temp=="int")
			{
				code.push_back("popi("+to_string(k)+");\n");
			}
			else if(temp=="float")
			{
				code.push_back("popf("+to_string(k)+");\n");
			}	
		}
		
		
		if(currentfuncid!="main#0")
		{
			code.push_back("loadi(ind(ebp), ebp);\n"); // restoring dynamic link
			code.push_back("popi(1);\n"); //pop stack
		}
		code.push_back("return;\n");
	}
	code.push_back("}\n ");
}

decllist_node::decllist_node(abstract_node* d)
{
	statement_vector.push_back(d);
}
decllist_node::decllist_node(abstract_node* dl,abstract_node* d)
{
	for(int i=0;i<((dl)->statement_vector).size();i++)
	{
		statement_vector.push_back(((dl)->statement_vector)[i]);
	}
	
	statement_vector.push_back(d);
}
 void decllist_node :: generate_code()
 {
	for(int i=0;i<statement_vector.size();i++)
	{
		(statement_vector[i])->onStack=false;
		(statement_vector[i])->generate_code();
	} 
 }
decl_node::decl_node(abstract_node* declr_vector)
{
	statement1=declr_vector;
}

void decl_node::generate_code()
{
	if(statement1!=0)
	{
		(statement1)->onStack=false;
		(statement1)->generate_code();
	}
}

declrlist_node::declrlist_node(localattributes* d)
{
	declarator_vector.push_back(d);
}
declrlist_node::declrlist_node(abstract_node* dl,localattributes* d)
{
	for(int i=0;i<((dl)->declarator_vector).size();i++)
	{
		declarator_vector.push_back(((dl)->declarator_vector)[i]);
	}
	
	declarator_vector.push_back(d);
}
 void declrlist_node :: generate_code()
 {
	for(int i=0;i<declarator_vector.size();i++)
	{
		code.push_back("addi(-"+to_string((declarator_vector[i])->width)+",esp);\n");
	} 
 }

for_node::for_node(abstract_node *e1,abstract_node *e2, abstract_node *e3, abstract_node *s)
{
	exp1 = e1;
	exp2 = e2;
	exp3 = e3;
	statement1 = s;
}
while_node::while_node(abstract_node *e1, abstract_node *s)
{
	exp1 = e1;
	statement1 = s;
}
if_node::if_node(abstract_node *e1,abstract_node *s1, abstract_node *s2)
{
	exp1 = e1;
	statement1 = s1;
	statement2 = s2;
}
return_node::return_node(abstract_node *e1)
{
	exp1 = e1;

	if(currentfunctype == (exp1)->type)
	{
		;
	}
	else
	{
		if(currentfunctype == "float")
		{
			(exp1)->cast1 = "FLOAT";
		}
		else
		{
			(exp1)->cast1 = "INT";
		}
	}	
}
block_node::block_node(abstract_node *stmt_vector)
{
	statement1 = stmt_vector;
}

void block_node::generate_code()
{
	(statement1)->onStack=false;
	(statement1)->generate_code();
}

seq_node::seq_node(abstract_node* s)
{
	statement_vector.push_back(s);
}
seq_node::seq_node(abstract_node* l,abstract_node* s)
{
	for(int i=0;i<((l)->statement_vector).size();i++)
	{
		statement_vector.push_back(((l)->statement_vector)[i]);
	}
	
	statement_vector.push_back(s);
}

/*void seq_node::generate_code()
{
	for(int i=0;i<statement_vector.size();i++)
	{
		(statement_vector[i])->onStack=false;
		(statement_vector[i])->generate_code();
	}
}*/

expList_node::expList_node(abstract_node* s)
{
	statement_vector.push_back(s);
	
	string temp = callfuncid.top()+"#"+to_string((statement_vector).size());
	//cout<<"TEMP "<<temp<<endl;

	//cout<<gsym->getparameters(temp)<<" ############# "<<temp<<endl;
	
	if(gsym->getparameters(temp)!="false")
	{
		
		
		callfuncparam = split(gsym->getparameters(temp),':');
		
			
		for(int i=0;i< statement_vector.size();i++)
		{	
			
			if(callfuncparam[i] == (statement_vector[i])->type)
			{
				;
			}
			else
			{
				if(callfuncparam[i] == "float")
				{
					(statement_vector[i])->cast1 = "FLOAT";	
				}
				else
				{
					(statement_vector[i])->cast1 = "INT";
					
				}
			}
			//cout<<(statement_vector[i])->cast1<<" cast1"<<endl;
		}
	}
}
expList_node::expList_node(abstract_node* l,abstract_node* s)
{
	for(int i=0;i<((l)->statement_vector).size();i++)
	{
		statement_vector.push_back(((l)->statement_vector)[i]);
	}
	
	statement_vector.push_back(s);

	string temp = callfuncid.top()+"#"+to_string((statement_vector).size());
	//cout<<"TEMP "<<temp<<endl;

	
	if(gsym->getparameters(temp)!="false")
	{
		
		callfuncparam = split(gsym->getparameters(temp),':');
		
			
		for(int i=0;i< statement_vector.size();i++)
		{	
			
			if(callfuncparam[i] == (statement_vector[i])->type)
			{
				;
			}
			else
			{
				if(callfuncparam[i] == "float")
				{
					(statement_vector[i])->cast1 = "FLOAT";	
				}
				else
				{
					(statement_vector[i])->cast1 = "INT";
				}
			}

		}
	}
}


ass_node::ass_node(abstract_node *e1,abstract_node *e2)
{
	exp1 = e1;
	exp2 = e2;
	constant = "ASSIGN";
	
	if((exp1)->type == "float" && (exp2)->type == "int")
	{ 
		(exp2)->cast1 = "FLOAT"; 
	}
	
	else if((exp1)->type == "int" && (exp2)->type == "float")
	{ 
		(exp2)->cast1 = "INT";
	}
}
unary_op_node::unary_op_node(abstract_node* op , abstract_node *op1)
{
	exp1 = op;
	exp2 = op1;
}
funcall_node::funcall_node(string id)
{
    exp1 = 0;
	constant = id;
}
funcall_node::funcall_node(string id,abstract_node *  exp)
{
	exp1 = exp;
	constant = id;
}
binary_op_node::binary_op_node(abstract_node *op1 ,string op ,abstract_node *op2) 
{
	exp1 = op1;
	exp2 = op2;
	constant = op;
	
	/*if(constant == "AND_OP" || constant == "OR_OP")
	{ 
		if((exp1)->type == "float" )
		{ 
			(exp1)->cast1  = "INT";
		}
		
		if((exp2)->type == "float" )
		{ 
			(exp2)->cast1  = "INT";
		}
		
		cast2 = "INT";
		
	}
	else
	{*/
		if((exp1)->type == "float" && (exp2)->type == "float")
		{ 
			cast2 = "FLOAT";
		}
		else if((exp1)->type == "float" && (exp2)->type == "int")
		{ 
			(exp2)->cast1 = "FLOAT";
			cast2 = "FLOAT";
		}
		else if((exp1)->type == "int" && (exp2)->type == "float")
		{ 
			(exp1)->cast1 = "FLOAT";
			cast2 = "FLOAT";
		}
		else if((exp1)->type == "int" && (exp2)->type == "int")
		{ 
			cast2 ="INT";
		}
	//}
}
float_const_node::float_const_node(string str)
{
	constant=str;
}
int_const_node::int_const_node(string str)
{
	constant=str;
}
string_const_node::string_const_node(string str)
{
	constant=str;
}
index_node::index_node(abstract_node * lexp1,abstract_node * e2)
{
	exp1 = lexp1;
	exp2 = e2;
}
identifier_node::identifier_node(string id)
{
	constant=id;
}
empty_node::empty_node()
{;}

void abstract_node::print(){}



void index_node::generate_code()
{	
		(exp2)->onStack = true;
		(exp2)->generate_code();
		(exp1)->onStack = onStack;
		(exp1)->address = address;
		(exp1)->generate_code();
	
}

void for_node::print()
{
	cout<<"(For ";(exp1)->print();(exp2)->print();(exp3)->print();(statement1)->print();cout<<")";	
}  
void while_node::print()
{
	cout<<"(While ";(exp1)->print();(statement1)->print();cout<<")";	
}  
void if_node::print()
{
	cout<<"(If ";(exp1)->print();(statement1)->print();(statement2)->print();cout<<")";	
}  
void return_node::print()
{
	cout<<"(Return ";
	
	if((exp1)->cast1 == "default")
	{
		(exp1)->print();cout<<")";	
	}
	else
	{
		cout<<"(TO_"<<(exp1)->cast1;(exp1)->print();cout<<"))";		
	}
}  
void block_node::print()
{
	cout<<"(Block  [";(statement1)->print();cout<<"])";	
}  
void seq_node::print()
{
	for(int i=0;i<statement_vector.size();i++)
	{
		(statement_vector[i])->print();
		cout<<"\n";
	}
}  
void expList_node::print()
{
		
	for(int i=0;i< statement_vector.size();i++)
	{	
		
		if((statement_vector[i])->cast1 == "default")
		{
			(statement_vector[i])->print();
		}
		else
		{
			//cout<<"WHOOOO "<<(statement_vector[i])->cast1<<endl;
			cout<<"(TO_"<<(statement_vector[i])->cast1;
			(statement_vector[i])->print();
			cout<<")";
		}
	}	
}  
void expList_node::generate_code()
{
	if(cleanup==0)
	{
		string a="";
		for(int i=0;i<statement_vector.size();i++)
		{
			statement_vector[i]->onStack=false;
			statement_vector[i]->generate_code();
			
			//code.push_back("floatToint(ebx);\n");
			//cout<<(statement_vector[i])->cast1<<" this is cast1"<<endl;
			if((statement_vector[i])->cast1!="default")
			{
				if((statement_vector[i])->cast1=="INT" || (statement_vector[i])->cast1=="int")
				{
					code.push_back("floatToint(ebx);\n");
					code.push_back("pushi(ebx);\n");
				}
				else if((statement_vector[i])->cast1=="FLOAT" || (statement_vector[i])->cast1=="float")
				{
					code.push_back("intTofloat(ebx);\n");
					code.push_back("pushf(ebx);\n");
				}
			}
			else
			{
				if((statement_vector[i])->type=="int")
				{
					code.push_back("pushi(ebx);\n");
				}
				else if((statement_vector[i])->type=="float")
				{
					code.push_back("pushf(ebx);\n");
				}		
			}	
		}
	}
	else
	{
		string a="";
		for(int i=0;i<statement_vector.size();i++)
		{
			if(statement_vector[i]->cast1=="default")
			{
				if(statement_vector[i]->type=="int")
				{	
					code.push_back("popi(1);\n");
				}
				else
				{
					code.push_back("popf(1);\n");
				}
			}
			else if(statement_vector[i]->cast1=="int" || statement_vector[i]->cast1=="INT")
			{
				code.push_back("popi(1);\n");
			}
			else if(statement_vector[i]->cast1=="float" || statement_vector[i]->cast1=="FLOAT")
			{
				code.push_back("popf(1);\n");
			}
			
		}
	}
}




void ass_node::print()
{
	cout<<"("<<constant ;(exp1)->print();
	
	if((exp2)->cast1 != "default")
	{ 	
		cout<<"(TO_"<<(exp2)->cast1;(exp2)->print();cout<<")";
	}
	else
	{ 	
		(exp2)->print();
	}
	cout<<")";
	
}  
void unary_op_node::print()
{
	cout<<"("<<(exp1)->constant;(exp2)->print();cout<<")";	
}  
void funcall_node::print()
{
    if(exp1==0)
    {
        cout<<"(Func (FuncName "<<constant<<")";cout<<")";	
    }
    else
    {
        cout<<"(Func (FuncName "<<constant<<")";(exp1)->print();cout<<")";	
    }
}
void funcall_node::generate_code()
{
	if(constant=="printf")
	{
		for(int i=0;i<(exp1)->statement_vector.size();i++)
		{
			abstract_node* t = (exp1)->statement_vector[i];
			
			if( t->type == "int")
			{
				t->onStack = false;
				t->generate_code();
				code.push_back("print_int(ebx);\n");
			}
			
			else if( t->type == "float")
			{
				t->onStack = false;
				t->generate_code();
				code.push_back("print_float(ebx);\n");
			}
			
			else if( t->type == "string")
			{
				t->onStack = false;
				code.push_back("print_string("+t->constant+");\n");
			}
		}
		
	}
	
	else
	{
		string a="";
		
		
		int size = 0;
		
		if(exp1==0);
		else
		{
			size = (exp1)->statement_vector.size();	
		}
		
		string temp  = constant + "#" + to_string(size);
		string callfunctype = gsym->gettype(temp);
		
		//cout<<"callfunctype "<<callfunctype<<endl; 
		if(callfunctype=="float")
		{
			code.push_back("pushf(0.0);\n");
		}
		else if(callfunctype=="int")
		{
			code.push_back("pushi(0);\n");
		}
		
		if(exp1!=0)
		{
			(exp1)->cleanup=0;
			(exp1)->generate_code();
		}
		
		code.push_back(constant + "();"+"\n");
		
		if(exp1!=0)
		{
			(exp1)->cleanup=1;
			(exp1)->generate_code();
		}
		
		if(callfunctype=="float")
		{
			code.push_back("loadf(ind(esp),ebx);\n");
			code.push_back("popf(1);\n");
		}
		else if(callfunctype=="int")
		{
			code.push_back("loadi(ind(esp),ebx);\n");
			code.push_back("popi(1);\n");
		}
		if(onStack)
		{
			if(callfunctype=="float")
			{
				code.push_back("pushf(ebx);\n");
			}
			else if(callfunctype=="int")
			{
				code.push_back("pushi(ebx);\n");
			}
			else
			{
				code.push_back("pushi(ebx);\n");
			}
		}
	}
}

void binary_op_node::print()
{
	if(cast2 != "default")cout<<"("<<constant + "_" + cast2;
	else cout<<"("<<constant;
	
	if((exp1)->cast1 != "default")
	{ 
		cout<<"(TO_"<<(exp1)->cast1<<" ";(exp1)->print();cout<<")";
	}
	else 
	{ 
		(exp1)->print();
	}
	
	if((exp2)->cast1 != "default")
	{ 
		cout<<"(TO_"<<(exp2)->cast1<<" ";(exp2)->print();cout<<")";
	}
	else 
	{ 
		(exp2)->print();
	}
	
	cout<<")";	
}  
void binary_op_node::generate_code()
{	
	string a="";
	(exp1)->generate_code();
	(exp2)->onStack=false;
	(exp2)->generate_code();
	
	//cout<<"Cons "<<constant<<endl;
	if((exp1)->type=="int")
	{
		code.push_back("loadi(ind(esp),eax);\n");
		code.push_back("popi(1);\n");
		if((exp1)->cast1!="default")
		{
			code.push_back("intTofloat(eax);\n");
		}
	}
	else if((exp1)->type=="float")
	{
		code.push_back("loadf(ind(esp),eax);\n");
		code.push_back("popf(1);\n");
		if((exp1)->cast1!="default")
		{
			code.push_back("floatToint(eax);\n");
		}
	}
	if((exp2)->cast1!="default")
	{
		if((exp2)->cast1=="int" || (exp2)->cast1=="INT")
		{
			code.push_back("floatToint(ebx);\n");
		}
		else if((exp2)->cast1=="float" || (exp2)->cast1=="FLOAT")
		{
			code.push_back("intTofloat(ebx);\n");
		}
	}
	if(constant=="Plus")
	{
		//cout<<"cast2 "<<cast2<<endl;
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("addi(eax,ebx);\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("addf(eax,ebx);\n");
		}
	}
	else if(constant=="Multiply")
	{
		//cout<<"cast2 "<<cast2<<endl;
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("muli(eax,ebx);\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("mulf(eax,ebx);\n");
		}
	}
	else if(constant=="Division")
	{
		//cout<<(exp2)->cast1<<" (exp2)->cast1"<<endl;
		//cout<<cast2	<<" cast2"<<endl;
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("divi(eax,ebx);\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("divf(eax,ebx);\n");
		}
	}
	else if(constant=="Minus")
	{
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("muli(-1,ebx);\n");
			code.push_back("addi(eax,ebx);\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("mulf(-1.0,ebx);\n");
			code.push_back("divf(eax,ebx);\n");
		}
	}
	else if(constant=="AND_OP")
	{
		//cout<<"cast2 "<<cast2<<endl;
		if(cast2=="int" || cast2=="INT")
		{
			//cout<<"pahoch gaya"<<endl;
			code.push_back("cmpi(0,eax);\n");
			code.push_back("je(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("cmpi(0,ebx);\n");
			code.push_back("je(l"+ to_string(x1) +");\n");
			code.push_back("move(1,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(0,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("cmpf(0.0,eax);\n");
			code.push_back("je(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("cmpf(0.0,ebx);\n");
			code.push_back("je(l"+ to_string(x1) +");\n");
			code.push_back("move(1.0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(0.0,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
	}
	else if(constant=="OR_OP")
	{
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("cmpi(0,eax);\n");
			code.push_back("jne(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("cmpi(0,ebx);\n");
			code.push_back("jne(l"+ to_string(x1) +");\n");
			code.push_back("move(0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("cmpf(0.0,eax);\n");
			code.push_back("jne(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("cmpf(0.0,ebx);\n");
			code.push_back("jne(l"+ to_string(x1) +");\n");
			code.push_back("move(0.0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1.0,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
	}
	else if(constant=="EQ_OP")
	{
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("cmpi(eax,ebx);\n");
			code.push_back("je(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("cmpf(eax,ebx);\n");
			code.push_back("je(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0.0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1.0,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
	}
	else if(constant=="NE_OP")
	{
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("cmpi(eax,ebx);\n");
			code.push_back("jne(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("cmpf(eax,ebx);\n");
			code.push_back("jne(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0.0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1.0,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
	}
	else if(constant=="LT")
	{
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("cmpi(eax,ebx);\n");
			code.push_back("jl(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("cmpf(eax,ebx);\n");
			code.push_back("jl(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0.0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1.0,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
	}
	else if(constant=="GT")
	{
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("cmpi(eax,ebx);\n");
			code.push_back("jg(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("cmpf(eax,ebx);\n");
			code.push_back("jg(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0.0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1.0,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
	}
	else if(constant=="LE_OP")
	{
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("cmpi(eax,ebx);\n");
			code.push_back("jle(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("cmpf(eax,ebx);\n");
			code.push_back("jle(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0.0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1.0,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
	}
	
	else if(constant=="GE_OP")
	{
		if(cast2=="int" || cast2=="INT")
		{
			code.push_back("cmpi(eax,ebx);\n");
			code.push_back("jge(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
		else if(cast2=="float" || cast2=="FLOAT")
		{
			code.push_back("cmpf(eax,ebx);\n");
			code.push_back("jge(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			code.push_back("move(0.0,ebx);\n");
			int x2=label;
			label++;
			code.push_back("j(l"+to_string(x2)+");\n");
			code.push_back("l"+to_string(x1)+": move(1.0,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
	}
	//code for other binary operators
	
	if(onStack)
	{
		if(cast2=="INT")
		{
			code.push_back("pushi(ebx);\n");
		}
		else
		{
			code.push_back("pushf(ebx);\n");
		}
	}
}

void float_const_node::print()
{
	cout<<"(FloatConst "<<constant<<")";
}  
void int_const_node::print()
{
	cout<<"(IntConst "<<constant<<")";
}  
void string_const_node::print()
{
	cout<<"(StringConst "<<constant<<")";
}  
void index_node::print()
{
	cout<<"(Array ";
	(exp1)->print();
	(exp2)->print();
	cout<<")";
}  
void identifier_node::print()
{
	cout<<"(Id \""<<constant<<"\")";
}  
void empty_node::print()
{
	cout<<"(Empty)";
}  
void seq_node::add(abstract_node* s)
{
	statement_vector.push_back(s);
}

void return_node::generate_code()
{
	
	if(currentfuncid!="main#0")
	{
		(exp1)->onStack=false;
		(exp1)->generate_code();
		if((exp1)->cast1=="FLOAT")
		{
			code.push_back("intTofloat(ebx);\n");
		}
		else if((exp1)->cast1=="INT")
		{
			code.push_back("floatToint(ebx);\n");
		}
	if(currentfunctype=="int")
		code.push_back("storei(ebx,ind(ebp,"+to_string(lsym->get_offset("r-"))+"));\n");
	else if(currentfunctype=="float")
		code.push_back("storef(ebx,ind(ebp,"+to_string(lsym->get_offset("r-"))+"));\n");
	
	}
	map<string,localattributes> copy;
	copy=lsym->M;
	int min=-1;
	while(min<0)
	{
		min = 0;
		
		localattributes localmin;
		for (map<string,localattributes>::iterator it=copy.begin(); it!=copy.end(); ++it)
		{
			if((it->second).off<min)
			{
				min = (it->second).off;
				localmin=it->second;
			}
		}
		
		copy.erase(localmin.id);
		
		string temp = localmin.type;
		
		while(temp.find("array(")==0)
		{
			temp = arraystrip(temp);
		}

		int k = localmin.width/4;
		
		if(temp=="int")
		{
			code.push_back("popi("+to_string(k)+");\n");
		}
		else if(temp=="float")
		{
			code.push_back("popf("+to_string(k)+");\n");
		}	
	}
	
	
	if(currentfuncid!="main#0")
	{
		code.push_back("loadi(ind(ebp), ebp);\n"); // restoring dynamic link
		code.push_back("popi(1);\n"); //pop stack
	}
	code.push_back("return;\n");
}

void float_const_node::generate_code()
{
	if(onStack)
	{
		code.push_back("pushf("+ constant+");\n");
	}
	else
	{
		code.push_back("move("+constant+",ebx);\n");
	}
}

void int_const_node::generate_code()
{
	if(onStack)
	{
		code.push_back("pushi("+ constant+");\n");
	}
	else
	{
		code.push_back("move("+constant+",ebx);\n");
	}
}

void string_const_node::generate_code()
{
;
}

void identifier_node::generate_code()
{
	if(lsym->gettype(constant).find("array(")!=0)
	{
		int i=lsym->get_offset(constant);
		string i1=to_string(i);
		if(!address)
		{
			if(lsym->gettype(constant)=="int")
				{
					code.push_back("loadi(ind(ebp,"+i1+"),ebx);\n");
				}
			else
				{
					code.push_back("loadf(ind(ebp,"+i1+"),ebx);\n");
				}
		}
		else
		{
			code.push_back("move(ebp,ebx);\n");
			code.push_back("addi("+i1+",ebx);\n");
		}
		if(onStack)
		{
			if(lsym->gettype(constant)=="int")
			{
				code.push_back("pushi(ebx);\n");
			}
			else
			{
				code.push_back("pushf(ebx);\n");
			}
		}
		
	}
	else
	{
		
		string temp = lsym->gettype((constant));	
		vector<int> sizes;
		
		int offset = lsym->get_offset((constant));	
		int width = lsym->M[constant].width;
		
		while((temp).find("array") == 0)
		{
			int i = temp.find(",");
			string num = temp.substr(6,i-6);
			sizes.push_back(atoi(num.c_str()));
			temp = temp.substr(i+1);		
		}
		
		reverse(sizes.begin(),sizes.end());
		
		int prod = 1;
		
		for(int i = sizes.size()-1;i>=0;i--)
		{
			int tq = sizes[i];
			sizes[i] = prod;
			prod  =  prod * tq;
		}
				
		code.push_back("move(0,ebx);\n");
		
		for(int i=0;i<sizes.size();i++)
		{
			code.push_back("loadi(ind(esp),eax);\n");
			code.push_back("muli("+to_string(sizes[i])+",eax);\n");
			code.push_back("addi(eax,ebx);\n");
			code.push_back("popi(1);\n");
		}
		
		code.push_back("muli(4,ebx);\n");
		code.push_back("muli(-1,ebx);\n");
//addi(-4,ebx);
		//cout<<"OFF "<<offset<<endl;
		code.push_back("addi("+to_string(offset+width-4)+",ebx);\n");
		code.push_back("addi(ebp,ebx);\n");
		
		if(!address)
		{
			code.push_back("loadi(ind(ebx),ebx);\n");
		}
		
		
		if(onStack)
		{
			code.push_back("pushi(ebx);\n");
		}
		
	}
}

/*void block_node::generate_code()
{
	for(int i=0;i<statement_vector.size();i++)
	{
		statement_vector[i]->onStack=false;
		statement_vector[i]->generate_code();
	}
}*/

void seq_node::generate_code()
{
	for(int i=0;i<statement_vector.size();i++)
	{
		statement_vector[i]->onStack=false;
		statement_vector[i]->generate_code();
	}
}

void ass_node::generate_code()
{
	(exp2)->onStack=true;
	(exp2)->generate_code();
	//code.push_back("move(ebx,edx);\n");
	(exp1)->onStack = false;
	(exp1)->address = true;
	(exp1)->generate_code();
	//code.push_back("move(edx,ebx);\n");
	//int i=lsym->get_offset((exp1)->constant);
	//string i1=to_string(i);
	if((exp2)->type=="int")
	{
		code.push_back("loadi(ind(esp),eax);\n");
		code.push_back("popi(1);\n");
	}
	else if((exp2)->type=="float")
	{
		code.push_back("loadf(ind(esp),eax);\n");
		code.push_back("popf(1);\n");
	}
	
	code.push_back("pushi(ebx);\n");
	code.push_back("move(eax,ebx);\n");
	code.push_back("loadi(ind(esp),eax);\n");
	code.push_back("popi(1);\n");
	if((exp2)->cast1=="INT")
	{
		code.push_back("floatToint(ebx);\n");
	}
	else if((exp2)->cast1=="FLOAT")
	{
		code.push_back("intTofloat(ebx);\n");
	}
	if((exp1)->type=="int")
	{
		code.push_back("storei(ebx,ind(eax));\n");
	}
	else
	{
		code.push_back("storef(ebx,ind(eax));\n");
	}
	if(onStack)
	{
		if((exp1)->type=="int" )
		{
			code.push_back("pushi(ebx);\n");
		}
		else
		{
			code.push_back("pushf(ebx);\n");
		}
	}
}

void unary_op_node::generate_code()
{
	if((exp1)->constant=="PP") 
	{
		//int i=lsym->get_offset((exp2)->constant);
		//string i1=to_string(i);
		(exp2)->onStack = true;
		(exp2)->address = true;
		(exp2)->generate_code();
		
		/*(exp2)->onStack = false;
		(exp2)->address = false;
		(exp2)->generate_code();
		*/
		if((exp2)->type=="int" )
		{
			code.push_back("loadi(ind(esp),eax);\n");
			code.push_back("popi(1);\n");
			code.push_back("loadi(ind(eax),ebx);\n");
			
			//code.push_back("loadi(ind(ebp,"+i1+"),ebx);\n");	 //if int
			code.push_back("move(ebx,ecx);\n");
			code.push_back("addi(1,ecx);\n");
			code.push_back("storei(ecx,(ind(eax)));\n");
			
		}
		else if((exp2)->type=="float" )
		{
			code.push_back("loadf(ind(esp),eax);\n");
			code.push_back("popf(1);\n");
			code.push_back("loadf(ind(eax),ebx);\n");
			//code.push_back("loadf(ind(ebp,"+i1+"),ebx);\n");	 //if float
			code.push_back("move(ebx,ecx);\n");
			code.push_back("addf(1.0,ecx);\n");
			code.push_back("storef(ecx,(ind(eax)));\n");
		}
		
	}
	else if((exp1)->constant=="Negative")
	{
		int i=lsym->get_offset((exp2)->constant);
		string i1=to_string(i);
		if((exp2)->type=="int" )
		{
			code.push_back("loadi(ind(ebp,"+i1+"),ebx);\n");	 //if int
			code.push_back("muli(-1,ebx);\n");
		}
		else if((exp2)->type=="float" )
		{
			code.push_back("loadf(ind(ebp,"+i1+"),ebx);\n");	 //if float
			code.push_back("mulf(-1.0,ebx);\n");
		}
	}
	else if((exp1)->constant=="Not")
	{
		int i=lsym->get_offset((exp2)->constant);
		string i1=to_string(i);

		if((exp2)->type=="int" )
		{
			code.push_back("loadi(ind(ebp,"+i1+"),ebx);\n");	 //if int
			code.push_back("cmpi(0,ebx);\n");
			code.push_back("je(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			int x2=label;
			label++;
			code.push_back("move(0,ebx);\n");
			code.push_back("j(l"+ to_string(x2) +");\n");
			code.push_back("l"+to_string(x1)+":	move(1,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
		else if((exp2)->type=="float" )
		{
			
			code.push_back("loadf(ind(ebp,"+i1+"),ebx);\n");	 //if int
			code.push_back("cmpf(0.0,ebx);\n");
			code.push_back("je(l"+ to_string(label) +");\n");
			int x1=label;
			label++;
			int x2=label;
			label++;
			code.push_back("move(0.0,ebx);\n");
			code.push_back("j(l"+ to_string(x2) +");\n");
			code.push_back("l"+to_string(x1)+":	move(1.0,ebx);\n");
			code.push_back("l"+to_string(x2)+":;\n");
		}
		//complete this when you do EQ_OP
	}
	if(onStack)
	{
		if((exp2)->type=="int" )
		{
			code.push_back("pushi(ebx);\n");
		}
		else
		{
			code.push_back("pushf(ebx);\n");
		}
	}
}

void empty_node::generate_code()
{
	if(onStack)
	{
		code.push_back("pushi(eax);\n");
	}
}



void if_node::generate_code()
{
	(exp1)->onStack=false;
	(statement1)->onStack=false;
	(statement2)->onStack=false;
	(exp1)->generate_code();
	
	if((exp1)->cast2=="INT" || ((exp1)->cast2=="default" && (exp1)->type=="int"))
	{
		code.push_back("cmpi(0,ebx);\n");	
	}
	else
	{
		code.push_back("cmpf(0.0,ebx);\n");	
	}
	int x1=label;
	label++;
	code.push_back("je(l"+to_string(x1)+");\n");
	(statement1)->generate_code();
	int x2=label;
	label++;
	code.push_back("j(l"+to_string(x2)+");\n");
	code.push_back("l"+to_string(x1)+":;\n");
	(statement2)->generate_code();
	code.push_back("l"+to_string(x2)+":;\n");
}

void while_node::generate_code()
{
	(exp1)->onStack=false;
	
	(statement1)->onStack=false;
	int x1=label;
	label++;
	code.push_back("l"+to_string(x1)+":;\n");
	(exp1)->generate_code();
	if((exp1)->cast2=="INT" || ((exp1)->cast2=="default" && (exp1)->type=="int"))
	{
		code.push_back("cmpi(0,ebx);\n");	
	}
	else
	{
		code.push_back("cmpf(0.0,ebx);\n");	
	}
	int x2=label;
	label++;
	code.push_back("je(l"+to_string(x2)+");\n");
	(statement1)->generate_code();
	code.push_back("j(l"+to_string(x1)+");\n");
	code.push_back("l"+to_string(x2)+":;\n");
}

void for_node::generate_code()
{
	(exp1)->onStack=false;
	(exp2)->onStack=false;
	(exp3)->onStack=false;
	(statement1)->onStack=false;
	(exp1)->generate_code();
	int x1=label;
	label++;
	code.push_back("l"+to_string(x1)+":;\n");
	(exp2)->generate_code();
	if((exp2)->cast2=="INT" || ((exp2)->cast2=="default" && (exp2)->type=="int"))
	{	
		code.push_back("cmpi(0,ebx);\n");	
	}
	else
	{
		code.push_back("cmpf(0.0,ebx);\n");	
	}
	int x2=label;
	label++;
	code.push_back("je(l"+to_string(x2)+");\n");
	(statement1)->generate_code();
	(exp3)->generate_code();
	code.push_back("j(l"+to_string(x1)+");\n");
	code.push_back("l"+to_string(x2)+":;\n");
}
