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
extern stack<string> callfuncid;
extern vector<string> code;
extern int label;
extern bool parameters;

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
		if(parameters)
		{
			l->off = loffset;
			loffset += l->width;
			M[l->id] = *l;
		}
		else
		{
			loffset -= l->width;
			l->off = loffset;	
			M[l->id] = *l;
		}
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
	int get_offset(string id)
	{
		if(M.find(id)!=M.end())
		{
			return (M[id]).off;	
		}
		else
		{
			return -1;	
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
		virtual void generate_code()=0;
		//virtual basic_types getType() = 0;
		//virtual bool checkTypeofAST() = 0;
		string type;
		string constant;
		string cast1 = "default";
		string cast2 = "default";
		vector<abstract_node *> statement_vector;
		vector<localattributes *> declarator_vector ;

		bool onStack=true;
		int cleanup=0;
		int offset;
		bool address = false;
		
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



class funcdef_node : public abstract_node {
	protected:
	public:
		funcdef_node(string id1,string type1,abstract_node *e1);
		void generate_code();
};

class compoundstmt_node : public abstract_node {
	protected:
	public:
		compoundstmt_node();
		compoundstmt_node(abstract_node *stmt_vector);
		compoundstmt_node(abstract_node *decl_vector,abstract_node *stmt_vector);
		void generate_code();
};

class decllist_node : public abstract_node {
	protected:
	public:
		decllist_node(abstract_node *d);
		decllist_node(abstract_node *dl,abstract_node *d);
		void generate_code();
};

class decl_node : public abstract_node {
	protected:
	public:
		decl_node(abstract_node *declr_vector);
		void generate_code();
};

class declrlist_node : public abstract_node {
	protected:
	public:
		declrlist_node(localattributes *d);
		declrlist_node(abstract_node *dl,localattributes *d);
		void generate_code();
};
class for_node : public abstract_node {
  protected:
  public:
    for_node(abstract_node *e1,abstract_node *e2, abstract_node *e3, abstract_node *s);
    void print();
    void generate_code();
};

class while_node :  public abstract_node {
  protected:
  public:
    while_node(abstract_node *e1, abstract_node *s);
    void print();
    void generate_code();
};

class if_node :  public abstract_node {
  protected:
  public:
    if_node(abstract_node *e1,abstract_node *s1, abstract_node *s2);
    void print();
    void generate_code();
};


class return_node :  public abstract_node {
  protected:
    
  public:
    return_node(abstract_node *e1);
    void print();
    void generate_code();
};

class block_node :  public abstract_node {
  protected:

    
  public:
    block_node(abstract_node *stmt_vector);
    void print();
    void generate_code();
};

class seq_node :  public abstract_node {
  protected:
    
    
  public:
	//vector use ki thi
    seq_node(abstract_node* s); 
    seq_node(abstract_node* l,abstract_node* s);    
    void add(abstract_node* s);
    void print();
    void generate_code();
};

class expList_node :  public abstract_node {
  protected:
    
    
  public:
	//vector use ki thi
    expList_node(abstract_node* s); 
    expList_node(abstract_node* l,abstract_node* s);    
    void print();
    void generate_code();
};

class ass_node : public abstract_node {
  protected:
  public:
    ass_node(abstract_node *e1,abstract_node *e2);
    void print();
    void generate_code();
};

class unary_op_node : public abstract_node{
  
  protected:

  public:

    unary_op_node(abstract_node* op , abstract_node *op1);
    void print();
    void generate_code();

};

class funcall_node : public abstract_node{
  
  protected:

  public:
    
    funcall_node(string id);
    funcall_node(string id,abstract_node * exp);
    void print();
    void generate_code();

};


class binary_op_node : public abstract_node{
  
  protected:

  public:

    binary_op_node(abstract_node *op1 ,string op ,abstract_node *op2);
    void print();
    void generate_code();
};

class float_const_node : public abstract_node{
  
  protected:
  public:

    float_const_node(string str);
    void print();
    void generate_code();
};

class int_const_node : public abstract_node{
  
  protected:
  public:

    int_const_node(string str);
    void print();
    void generate_code();
};

class string_const_node : public abstract_node{
  
  protected:
  public:

    string_const_node(string str);
    void print();
    void generate_code();
};


class index_node : public abstract_node{
  
  protected:
  public:

    index_node(abstract_node * lexp1,abstract_node * e2);
    void print();
    void generate_code();
};


class identifier_node : public abstract_node{
  
  protected:
  public:

    identifier_node(string id);
    void print();
    void generate_code();
};

class empty_node : public abstract_node{

  public:

    empty_node();
    void print();
    void generate_code();
};

string arraystrip(string arr);
bool compatible(string type1,string type2);
bool paramcompatible(string type1,string type2);

