#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

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
string callfuncid;
string currentfuncparam;
bool isreturn;
int ctr = 0;

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
	return arr.substr(i+1,arr.size()-1);
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

expList_node::expList_node(abstract_node* s)
{
	statement_vector.push_back(s);
}
expList_node::expList_node(abstract_node* l,abstract_node* s)
{
	for(int i=0;i<((l)->statement_vector).size();i++)
	{
		statement_vector.push_back(((l)->statement_vector)[i]);
	}
	
	statement_vector.push_back(s);

	string temp = callfuncid+"#"+to_string((statement_vector).size());
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
	constant = "ASSIGN"
	;
	
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
	
	if(constant == "AND_OP" || constant == "OR_OP")
	{ 
		;
	}
	else
	{
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
	}
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
			cout<<"(TO_"<<(statement_vector[i])->cast1;
			(statement_vector[i])->print();
			cout<<")";
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
