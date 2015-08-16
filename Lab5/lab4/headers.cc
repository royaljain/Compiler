#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stack>
using namespace std;

extern int goffset;
extern int loffset;
extern string dtype;
extern string constantExpressionType;
extern string currentfunctype;
extern vector<string> callfuncparam;
extern string currentfuncid;
extern string currentfuncparam;
extern bool isreturn;
extern int ctr;
extern string callfuncid;

class localattributes
{
	public:
	string id;
	string type;
	int off;
	int width;
	localattributes(){;}
	localattributes(string id1, string type1)
	{
		id = id1;	
		type = type1;
		
		if(type == "int")
		{
			width = 4;
		}

		else if(type == "float")
		{
			width = 4;
		}
		
		else if(type == "void")
		{
			width = 0;
		}		
	}
	
	localattributes(localattributes* a1, string c1)
	{
		id = a1->id;	
		type = "array(" + c1 + "," + a1->type +")";
		width = atoi(c1.c_str())*a1->width;
	}
	void print()
	{
		cout<<"Id: "<< id <<",";
		cout<<"type: "<< type <<",";
		cout<<"offset: "<< off <<",";
		cout<<"width: "<< width <<",";
	}
};

class localsymboltable
{
	public:
	map<string,localattributes> M;
	void add(localattributes* l)
	{
		l->off = loffset;
		loffset += l->width;
		M[l->id] = *l;
	}
	
	bool exists(string id)
	{
		return !(M.find(id)==M.end());			
	}
	
	void print()
	{
		for (std::map<string,localattributes>::iterator it=M.begin(); it!=M.end(); ++it)
		{
			std::cout << it->first <<"\n";
			(it->second).print();
			cout<<"\n";	
		}
	}
	string gettype(string id)
	{
		if(M.find(id)!=M.end())
		{
			return (M[id]).type;	
		}
		else
		{
			return "false";	
		}		
	}
	
};

extern localsymboltable* lsym;



class globalattributes
{
	public:
	string id;
	string type;
	int off;
	localsymboltable* T;
	string parameters; //add everything related to this
	globalattributes(){;}	
	globalattributes(string id1,string type1,int offset,localsymboltable* T1,string par1)
	{
		id = id1;
		type = type1;
		off = offset;
		T = T1;
		parameters = par1;
	}
	void print()
	{
		cout<<"Id: "<< id <<",";
		cout<<"type: "<< type <<",";
		cout<<"offset: "<< off <<",";
		cout<<"parameters : "<<parameters<<"\n";
		cout<<"localsymbolTable : "<<"\n";
		T->print();
	}
	
};


class globalsymboltable
{
	public:
	map<string,globalattributes> M;
	
	bool add(string id1,string type1,localsymboltable* T1)
	{
		int  i = id1.find("-");
		string iden;
		string para;
		
		int	paramlength;
		
		if(i==-1)
		{
			iden = id1;
			para = "false";
			paramlength = 0;
		}
		else
		{
			iden = id1.substr(0,i);
			para = id1.substr(i+1);
			paramlength = std::count(para.begin(), para.end(), ':')+1;
		}	
		
		iden = iden + "#" + to_string(paramlength); 
		
		if(M.find(iden)==M.end())
		{	
			globalattributes* temp = new globalattributes(iden,type1,goffset,T1,para);
			goffset += loffset;
			M[iden] = *temp;
			return true;
		}
		else
		{
			return false;
		}	
	}
	
	void print()
	{
		for (std::map<string,globalattributes>::iterator it=M.begin(); it!=M.end(); ++it)
		{
			std::cout << it->first <<"\n";
			(it->second).print();
			cout<<"\n";	
		}
	}
	string gettype(string id)
	{
		if(M.find(id)!=M.end())
		{
			return (M[id]).type;	
		}
		else
		{
			return "false";	
		}
	}
	string getparameters(string id)
	{
		if(M.find(id)!=M.end())
		{
			return (M[id]).parameters;	
		}
		else
		{
			return "false";	
		}
	}
};

extern globalsymboltable* gsym;



class abstract_node
{
	public:
		virtual void print();
		//virtual void add(abstract_node* a);
		//virtual std::string generate_code(const symbolTable&) = 0;
		//virtual basic_types getType() = 0;
		//virtual bool checkTypeofAST() = 0;
		string type;
		string constant;
		string cast1 = "default";
		string cast2 = "default";
		vector<abstract_node *> statement_vector;
	
	protected:
		abstract_node *exp1;
		abstract_node *exp2;
		abstract_node *exp3;
		abstract_node *statement1;
		abstract_node *statement2;
		
	
		//virtual void setType(basic_types) = 0;
	private:
		//typeExp astnode_type;
};


class for_node :  public abstract_node {
  protected:
  public:
    for_node(abstract_node *e1,abstract_node *e2, abstract_node *e3, abstract_node *s);
    void print();
};

class while_node :  public abstract_node {
  protected:
  public:
    while_node(abstract_node *e1, abstract_node *s);
    void print();
};

class if_node :  public abstract_node {
  protected:
  public:
    if_node(abstract_node *e1,abstract_node *s1, abstract_node *s2);
    void print();
};


class return_node :  public abstract_node {
  protected:
    
  public:
    return_node(abstract_node *e1);
    void print();
};

class block_node :  public abstract_node {
  protected:

    
  public:
    block_node(abstract_node *stmt_vector);
    void print();
};

class seq_node :  public abstract_node {
  protected:
    
    
  public:
	//vector use ki thi
    seq_node(abstract_node* s); 
    seq_node(abstract_node* l,abstract_node* s);    
    void add(abstract_node* s);
    void print();
};

class expList_node :  public abstract_node {
  protected:
    
    
  public:
	//vector use ki thi
    expList_node(abstract_node* s); 
    expList_node(abstract_node* l,abstract_node* s);    
    void print();
};

class ass_node :  public abstract_node {
  protected:
  public:
    ass_node(abstract_node *e1,abstract_node *e2);
    void print();
};

class unary_op_node : public abstract_node{
  
  protected:

  public:

    unary_op_node(abstract_node* op , abstract_node *op1);
    void print();

};

class funcall_node : public abstract_node{
  
  protected:

  public:
    
    funcall_node(string id);
    funcall_node(string id,abstract_node * exp);
    void print();

};


class binary_op_node : public abstract_node{
  
  protected:

  public:

    binary_op_node(abstract_node *op1 ,string op ,abstract_node *op2);
    void print();

};

class float_const_node : public abstract_node{
  
  protected:
  public:

    float_const_node(string str);
    void print();

};

class int_const_node : public abstract_node{
  
  protected:
  public:

    int_const_node(string str);
    void print();

};

class string_const_node : public abstract_node{
  
  protected:
  public:

    string_const_node(string str);
    void print();

};


class index_node : public abstract_node{
  
  protected:
  public:

    index_node(abstract_node * lexp1,abstract_node * e2);
    void print();

};


class identifier_node : public abstract_node{
  
  protected:
  public:

    identifier_node(string id);
    void print();

};

class empty_node : public abstract_node{

  public:

    empty_node();
    void print();

};

string arraystrip(string arr);
bool compatible(string type1,string type2);
bool paramcompatible(string type1,string type2);

