/* Changes:  */

/* 1. Character constants removed */
/* 2. Changed INTCONSTANT to INT_CONSTANT */
/* 3. Changed the production for constant_expression to include FLOAT_CONSTANT */
/* 4. Added examples of FLOAT_CONSTANTS */
/* 5. Added the description of STRING_LITERAL */
/* 6. Changed primary_expression and FOR */
/* 7. The grammar permits a empty statement. This should be  */
/*    explicitly represented in the AST. */
/* 8. To avoid local decl inside blocks, a rule for statement  */
/*    has been changed. */

/* ----------------------------------------------------------------------- */

/* start symbol is translation_unit */

/* ---------------------------------------------------- */
%scanner Scanner.h
%scanner-token-function d_scanner.lex()


%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP EQ_OP NE_OP LE_OP GE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN IDENTIFIER INC_OP

%polymorphic typeAttr: abstract_node*; TYPE_STRING: string; typeAttr2 : localattributes*;
%type <typeAttr> translation_unit function_definition  main    compound_statement statement_list statement assignment_statement expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression postfix_expression primary_expression l_expression expression_list unary_operator selection_statement iteration_statement  
%type <TYPE_STRING> type_specifier fun_declarator IDENTIFIER INT_CONSTANT STRING_LITERAL FLOAT_CONSTANT constant_expression parameter_declaration parameter_list
%type <typeAttr2>   declarator declaration_list declaration declarator_list
%%

main 
	: 
	translation_unit { if(gsym->gettype("main#0") == "false"){ cout<<Scanner::linenum<<":- error: no definition for main "<<endl; }
    
    cout<<"Symbol tables \n";gsym->print(); cout<<"\n";
    
     }
	;
	
translation_unit
	: { goffset = 0; gsym = new globalsymboltable(); 
		isreturn = false;
		loffset = 4; lsym = new localsymboltable();}
		
		function_definition  		
		
	| translation_unit 
	
	 { 
		isreturn = false;
		loffset = 4; lsym = new localsymboltable();} 
	
	 function_definition 
     ;

function_definition
	: type_specifier { currentfunctype = $1;} fun_declarator { loffset += 4 ;}compound_statement 
		{
		
		 if (  currentfunctype != "void" && !isreturn)
		 {
			cout<<Scanner::linenum<<":- error: non-void function doesn't have return statement "<<currentfuncid<<endl;
		 }
		 
		
		 if (!(gsym->add($3,$1,lsym))) //creates side effects
		 {
			cout<<Scanner::linenum<<":- error: duplicate definition of function  "<<currentfuncid<<endl;
		 }
		
		}	 
		
	;

type_specifier
	: VOID 	{dtype = "void"; ($$) = "void";}
    | INT 	{dtype = "int" ; ($$) = "int";}
    | FLOAT {dtype = "float";($$) = "float";}
    ;

fun_declarator
	: IDENTIFIER '(' parameter_list ')' 
	{
		string para = ($3);
			
		int len = std::count(para.begin(), para.end(), ':') + 1;
			
		string id = $1 + "#" + to_string(len);
		currentfuncid = id;
		currentfuncparam = $3;
		($$) = ($1) + "-" + ($3); 
	}
    
    | 
    
		IDENTIFIER '(' ')' 
		{
			($$) = ($1); 
			currentfuncid = $1+"#0"; 
			currentfuncparam = "";
		}
    ;

parameter_list
	: parameter_declaration
	{
		$$ = ($1);
	} 
	| parameter_list ',' parameter_declaration 
	{
		$$ = ($1) + ":" + ($3);	
	}
	;

parameter_declaration
	: type_specifier declarator 
	{ 	
		if(lsym->exists(($2)->id))
		{
			cout<<Scanner::linenum<<":- error: parameter declared before "<<endl;
		}	
		else
		{
			lsym->add($2);
			$$ = ($2)->type;			
		}
		
	} 
    ;

declarator
	: IDENTIFIER 
	{ 
		if(dtype == "void")
		{
			cout<<Scanner::linenum<<":- error: void declarations not allowed "<<endl;
		}
		
		($$) = new localattributes(($1),dtype);
		
	}
	| declarator '[' constant_expression ']' 
		{ 
			if(constantExpressionType != "int")
			{
				cout<<Scanner::linenum<<":- error: array index not an integer "<<endl;
				($$) = new localattributes(($1),($3));
			}
			else
			{
				($$) = new localattributes(($1),($3));
			}
		}
    ;

constant_expression 
        : INT_CONSTANT 
        {
			($$) = ($1);
			constantExpressionType="int";
        }
        | FLOAT_CONSTANT 
        {
			($$) = ($1);
			constantExpressionType="float";
		}	
        ;

compound_statement
	: '{' '}' 
	| '{' statement_list '}'
	{

		abstract_node * t3  = new block_node($2);
		abstract_node * t1 = ($2);
		
		t3->type = t1->type;
	
		$2 = t1;
		$$ = t3;
		
		
		
		int i = currentfuncid.find("#");
		string temp = currentfuncid.substr(0,i);
		
		if(Parser::functions.find(temp)!=functions.end())
		{
            cout<<"AST of "<<temp<<endl;
			($$)->print();
		cout<<"\n";}
		else
		{
			cout<<"Not printing AST of "<<temp<<endl;
		}
		
	} 
    | '{' declaration_list statement_list '}' 
	{
		
		abstract_node * t3  = new block_node($3);
		abstract_node * t2 = ($3);
		
		
		if(t2->type == "error")
		{
			t3->type = "error";
		}
		else
		{
			t3->type = "OK";
		}
		
		
		$3 = t2;
		$$ = t3;
		
		int i = currentfuncid.find("#");
		string temp = currentfuncid.substr(0,i);
		
		if(Parser::functions.find(temp)!=functions.end()){
			cout<<"AST of "<<temp<<endl;
            ($$)->print();
			cout<<"\n";}
		else
		{
            cout<<"Not printing AST of "<<temp<<endl;
		}	
	}
	;

statement_list
	: statement	
	{
		
		abstract_node * t3  = new seq_node($1);
		abstract_node * t1 = ($1);
		
		t3->type = t1->type;
	
		$1 = t1;
		$$ = t3;
		
	}	
    | statement_list statement
    {
		abstract_node * t3  = new seq_node($1,$2);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($2);
		
		
		if(t1->type == "error" || t2->type == "error")
		{
			t3->type = "error";
		}
		else
		{
			t3->type = "OK";
		}
		
		$1 = t1;
		$2 = t2;
		$$ = t3;
    }	
	;

statement
		: '{' statement_list '}'  //a solution to the local decl problem
        	{
				
				abstract_node * t3  = new block_node($2);
				abstract_node * t1 = ($2);
				
				t3->type = t1->type;
			
				$2 = t1;
				$$ = t3;
				
        	}
        | selection_statement 	
        	{
				$$ = $1;
        	}
        | iteration_statement 	
			{
				$$ = $1;
        	}
		| assignment_statement	
        	{
				$$ = $1;
        	}
        | RETURN expression ';'	
        	{
				$$ = new return_node($2);
				
				if(!compatible(currentfunctype,($2)->type))
				{
					isreturn = true;
					cout<<Scanner::linenum<<":- error: Invalid return type for function "<<currentfuncid<<endl;
					($$)->type = "error";
				}
        		
        		else
        		{
					isreturn = true;
					($$)->type = "OK";
        		}       		
        	}
        | IDENTIFIER '(' ')' ';'
         	{


			abstract_node * t3  = new funcall_node($1);
			string t1 = ($1);
			string id = t1 + "#0";
			
			if (currentfuncid == id || t1 == "printf")
			{
				(t3)->type =  "OK";
			}
			else if (gsym->gettype(id) == "false")
			{
				cout<<Scanner::linenum<<":- error: no such function "<<t1<<endl;
				(t3)->type="error";				
			} 
			else if(gsym->getparameters(id) != "false")
			{
				cout<<Scanner::linenum<<":- error: function  call parameters  not matching function definition"<<endl;
				(t3)->type="error";			
			}
			else
			{
				(t3)->type = "OK";
			}
			
			$1 = t1;
			$$ = t3;


		} 
        | IDENTIFIER '(' {callfuncid = ($1);} expression_list ')' ';'
         	{
			
			
			abstract_node * t3  = new funcall_node($1,$4);
			string t1 = ($1);
			abstract_node * t2 = ($4);
			string para = (t2)->type;
			
			
			int len = std::count(para.begin(), para.end(), ':') + 1;
			
			string id = t1 + "#" + to_string(len);
			
			t2->constant = id;
			
							
			if(t2->type == "error")
			{
				(t3)->type="error";
			}
			else if ( (currentfuncid == id && paramcompatible(currentfuncparam,t2->type)) || t1 == "printf")
			{
				(t3)->type =  "OK";				
			}
			else if (gsym->gettype(id) == "false")
			{
				cout<<Scanner::linenum<<":- error: no such function "<<t1<<endl;
				(t3)->type="error";				
			} 
			else if(!paramcompatible(t2->type,gsym->getparameters(id)))
			{
				cout<<Scanner::linenum<<":- error: function  call parameters  not matching function definition"<<endl;
				(t3)->type="error";			
			}
			else
			{
				(t3)->type = "OK";
			}
			
			$1 = t1;
			$4 = t2;
			$$ = t3;

		}
        ;

assignment_statement
	: 
		';'
		{
			$$ = new empty_node();
		} 								
	|  
	
		l_expression '=' expression ';'
		{
		
		
		abstract_node * t3  = new ass_node($1,$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: LHS not compatible wIth RHS "<<(t1)->type<<" "<<(t2)->type<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = "OK";
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	}
	;

expression
		
		: logical_and_expression
        	{
				$$ = $1;
        	} 
        | expression OR_OP logical_and_expression
			{
		
		abstract_node * t3  = new binary_op_node($1,"OR_OP",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = "int";
		}

		$1 = t1;
		$3 = t2;
		$$ = t3;
	}
	;

logical_and_expression
        : 
        
        equality_expression
        	{
				$$ = $1;
        	} 
        | logical_and_expression AND_OP equality_expression
        	{
		
		abstract_node * t3  = new binary_op_node($1,"AND_OP",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = "int";
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	} 
	;

equality_expression
	: 
		
		relational_expression 
		{
			$$ = $1;
       	} 
        | equality_expression EQ_OP relational_expression 	
        {
		
		abstract_node * t3  = new binary_op_node($1,"EQ_OP",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = "int";
		}
		
		
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	}
	| equality_expression NE_OP relational_expression
		{
		
		abstract_node * t3  = new binary_op_node($1,"NE_OP",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = "int";
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	}
	;
relational_expression
	: 
		
		additive_expression
		{
			$$ = $1;
       	} 	
        | relational_expression '<' additive_expression 
		{
		
		abstract_node * t3  = new binary_op_node($1,"LT",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = "int";
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	} 

	| relational_expression '>' additive_expression 
	{
		
		abstract_node * t3  = new binary_op_node($1,"GT",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = "int";
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	}	
	| relational_expression LE_OP additive_expression 
	{
		
		abstract_node * t3  = new binary_op_node($1,"LE_OP",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = "int";
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	}
	| relational_expression GE_OP additive_expression  
	{
	
	
	abstract_node * t3  = new binary_op_node($1,"GE_OP",$3);
	abstract_node * t1 = ($1);
	abstract_node * t2 = ($3);
	
	if(t1->type=="error" || t2->type=="error")
	{
		(t3)->type="error";
	}
	else if(!compatible(t1->type,t2->type))
	{
		cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
		(t3)->type="error";
	}
	else
	{
		(t3)->type = "int";
	}
	
	$1 = t1;
	$3 = t2;
	$$ = t3;
	}

	;

additive_expression 
	: multiplicative_expression
		{
			$$ = $1;
       	} 

	| additive_expression '+' multiplicative_expression 
	{
	
		abstract_node * t3  = new binary_op_node($1,"Plus",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = binaryType((t1)->type,(t2)->type);
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	}	
	| additive_expression '-' multiplicative_expression 
	{
	
		abstract_node * t3  = new binary_op_node($1,"Minus",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = binaryType((t1)->type,(t2)->type);
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	}
	;

multiplicative_expression
	: unary_expression
		{
			$$ = $1;
       	} 

	| multiplicative_expression '*' unary_expression  
	{
	
		abstract_node * t3  = new binary_op_node($1,"Multiply",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = binaryType((t1)->type,(t2)->type);
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	}
	| multiplicative_expression '/' unary_expression 
	{
	
		abstract_node * t3  = new binary_op_node($1,"Division",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: operands not compatible "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = binaryType((t1)->type,(t2)->type);
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	} 

	;

unary_expression
	: postfix_expression  				
		{
			$$ = $1;
       	} 

	| unary_operator postfix_expression 
		{
	
			
			abstract_node * t3  = new unary_op_node($1,$2);
			abstract_node * t1 = ($1);
			abstract_node * t2 = ($2);
			
			if(t2->type == "error")
			{
				(t3)->type="error";
			}
			else
			{
				(t3)->type = (t2)->type;
			}
			
			$1 = t1;
			$2 = t2;
			$$ = t3;
		} 

	;

postfix_expression
	: primary_expression
		{
			$$ = $1;
       	} 

    | IDENTIFIER '(' ')'
		{

			abstract_node * t3  = new funcall_node($1);
			string t1 = ($1);
			string id = t1 + "#0";
			
			if (currentfuncid == id )
			{
				(t3)->type =  currentfunctype;				
			}
			
			else if (t1 == "printf")
			{
				(t3)->type =  "void";				
			}
			
			else if (gsym->gettype(id) == "false")
			{
				cout<<Scanner::linenum<<":- error: no such function "<<t1<<endl;
				(t3)->type="error";				
			} 
			else if(gsym->getparameters(id) != "false")
			{
				cout<<Scanner::linenum<<":- error: function  call parameters  not matching function definition"<<endl;
				(t3)->type="error";			
			}
			else
			{
				(t3)->type = gsym->gettype(id);
			}
			
			$1 = t1;
			$$ = t3;


		} 

	| IDENTIFIER '(' {callfuncid = ($1); } expression_list ')'
 		{
	
			
			
			abstract_node * t3  = new funcall_node($1,$4);
			string t1 = ($1);
			abstract_node * t2 = ($4);
			
			string para = (t2)->type;
			int len = std::count(para.begin(), para.end(), ':') + 1;
			string id = t1 + "#" + to_string(len);
			
			t2->constant = id;			
						
			if(t2->type == "error")
			{
				(t3)->type="error";
			}
			else if( (currentfuncid == id && paramcompatible(currentfuncparam,t2->type)) )
			{
				(t3)->type =  currentfunctype;				
			}
			else if (t1 == "printf")
			{
				(t3)->type =  "void";				
			}
			else if(gsym->gettype(id) == "false")
			{
				cout<<Scanner::linenum<<":- error: no such function "<<t1<<endl;
				(t3)->type="error";				
			} 
			else if(!paramcompatible(t2->type,gsym->getparameters(id)))
			{
				cout<<Scanner::linenum<<":- error: function  call parameters  not matching function definition"<<endl;
				(t3)->type="error";			
			}
			else
			{
				(t3)->type = gsym->gettype(id);
			}
			
			$1 = t1;
			$4 = t2;
			$$ = t3;

		} 
	| l_expression INC_OP
		{
	
			abstract_node* temp = new identifier_node("PP");
			abstract_node * t3  = new unary_op_node(temp,$1);
			abstract_node * t1 = ($1);
			
			if(t1->type=="error")
			{
				(t3)->type="error";
			}
			else
			{
				(t3)->type = t1->type;
			}
			
			$1 = t1;
			$$ = t3;
		} 

	;

primary_expression
	: l_expression
	{
		$$ = $1;
	} 

	| l_expression '=' expression // added this production
	{
	
		abstract_node * t3  = new binary_op_node($1,"Assign_exp",$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(!compatible(t1->type,t2->type))
		{
			cout<<Scanner::linenum<<":- error: LHS not compatible wIth RHS "<<(t1)->type<<" "<<(t2)->type<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = t1->type;
		}
		
		$1 = t1;
		$3 = t2;
		$$ = t3;
	} 

	| INT_CONSTANT
	{
		$$= new int_const_node($1);
		($$)->type = "int";
	}
	| FLOAT_CONSTANT
	{
		$$= new float_const_node($1);
		($$)->type = "float";
	}	
        | STRING_LITERAL
 	{
 		$$= new string_const_node($1);
		($$)->type = "string";
	}       
	| '(' expression ')' 	
		{
        	$$ = $2;
       	} 

	;

l_expression
        : IDENTIFIER
	{
		abstract_node *t= new identifier_node($1);
		string type = lsym->gettype($1); 
		
		if(type == "false")
		{
			cout<<Scanner::linenum<<":- error: Undeclared identifier "<<($1)<<endl;
			(t)->type="error";
		}
		else
		{
			(t)->type = type;
		}
		$$=t;
	}        
        | l_expression '[' expression ']' 	
	{
	
		abstract_node * t3  = new index_node($1,$3);
		abstract_node * t1 = ($1);
		abstract_node * t2 = ($3);
		
		if(t1->type=="error" || t2->type=="error")
		{
			(t3)->type="error";
		}
		else if(t2->type != "int" )
		{
			cout<<Scanner::linenum<<":- error: index not integer "<<endl;
			(t3)->type="error";
		}
		else if((t1->type).substr(0,6)!= "array(")
		{
			cout<<Scanner::linenum<<":- error: identifier not an array "<<endl;
			(t3)->type="error";
		}
		else
		{
			(t3)->type = arraystrip(($1)->type);
		}
		$1 = t1;
		$3 = t2;
		$$ = t3;
	}        
       ;

expression_list
        : expression
        {
	
			
			abstract_node * t3  = new expList_node($1);
			abstract_node * t1 = ($1);
			
			if(t1->type=="error")
			{
				(t3)->type="error";
			}
			else
			{
				(t3)->type = (t1)->type;
			}
			
			$1 = t1;
			$$ = t3;
        }
        | expression_list ',' expression
        {
    
        
			abstract_node * t3  = new expList_node($1,$3);
			abstract_node * t1 = ($1);
			abstract_node * t2 = ($3);
			
			if(t1->type=="error" || t2->type=="error")
			{
				(t3)->type="error";
			}
			else
			{
				(t3)->type = (t1)->type+":"+(t2)->type;
			}
			
			$1 = t1;
			$3 = t2;
			$$ = t3;
		}
        ;
unary_operator
    : '-'
    	{
        	$$ = new identifier_node("Negative");
       	}

	| '!'
		{
        	$$ = new identifier_node("Not");
       	} 	
	;

selection_statement
        : IF '(' expression ')' statement ELSE statement 
        {
  
        
			abstract_node * t5  = new if_node($3,$5,$7);
			abstract_node * t2 = ($3);
			abstract_node * t3 = ($5);
			abstract_node * t4 = ($7);
			
			if(t2->type=="error" || t3->type=="error" || t4->type=="error")
			{
				(t5)->type="error";
			}
			else if(t2->type == "void")
			{
				cout<<Scanner::linenum<<":- error: void expression "<<endl;
				(t5)->type = "error";
			}
			else
			{
				(t3)->type = "OK";
			}
			
			$3 = t2;
			$5 = t3;
			$7 = t4;
			$$ = t5;
		}
	;

iteration_statement
	: WHILE '(' expression ')' statement
        	{
  
        
			abstract_node * t5  = new while_node($3,$5);
			abstract_node * t2 = ($3);
			abstract_node * t3 = ($5);
			
			if(t2->type=="error" || t3->type=="error")
			{
				(t5)->type="error";
			}
			else if(t2->type == "void")
			{
				cout<<Scanner::linenum<<":- error: void expression "<<endl;
				(t5)->type = "error";
			}
			else
			{
				(t3)->type = "OK";
			}
			
			$3 = t2;
			$5 = t3;
			$$ = t5;
		}	
        | FOR '(' expression ';' expression ';' expression ')' statement  //modified this production
        
        	{
  
        
			abstract_node * t6  =  new for_node($3,$5,$7,$9);
			abstract_node * t2 = ($3);
			abstract_node * t3 = ($5);
			abstract_node * t4 = ($7);
			abstract_node * t5 = ($9);
			
			if(t2->type=="error" || t3->type=="error" || t4->type=="error" || t5->type=="error" )
			{
				(t6)->type="error";
			}
			else if(t3->type == "void")
			{
				cout<<Scanner::linenum<<":- error: void expression "<<endl;
				(t6)->type = "error";
			}
			else
			{
				(t6)->type = "OK";
			}
			
			$3 = t2;
			$5 = t3;
			$7 = t4;
			$9 = t5;
			$$ = t6;
		}
        ;

declaration_list
        : declaration  					
        | declaration_list declaration
	;

declaration
	: type_specifier declarator_list ';' 
	;

declarator_list
	: 
	 declarator 
	{
	
	if(lsym->exists(($1)->id))
	{
		cout<<Scanner::linenum<<":- error: parameter declared before "<<endl;
	}	
	else
	{
		lsym->add($1);			
	}
	
	}
	
	| 
	
	declarator_list ',' declarator 
	{
	
	
	if(lsym->exists(($3)->id))
	{
		cout<<Scanner::linenum<<":- error: parameter declared before "<<endl;
	}	
	else
	{
		lsym->add($3);			
	}
	
	}
	;


/* A description of integer and float constants. Not part of the grammar.   */

/* Numeric constants are defined as:  */

/* C-constant: */
/*   C-integer-constant */
/*   floating-point-constant */
 
/* C-integer-constant: */
/*   [1-9][0-9]* */
/*   0[bB][01]* */
/*   0[0-7]* */
/*   0[xX][0-9a-fA-F]* */
 
/* floating-point-constant: */
/*   integer-part.[fractional-part ][exponent-part ] */

/* integer-part: */
/*   [0-9]* */
 
/* fractional-part: */
/*   [0-9]* */
 
/* exponent-part: */
/*   [eE][+-][0-9]* */
/*   [eE][0-9]* */

/* The rule given above is not entirely accurate. Correct it on the basis of the following examples: */

/* 1. */
/* 23.1 */
/* 01.456 */
/* 12.e45 */
/* 12.45e12 */
/* 12.45e-12 */
/* 12.45e+12 */

/* The following are not examples of FLOAT_CONSTANTs: */

/* 234 */
/* . */

/* We have not yet defined STRING_LITERALs. For our purpose, these are */
/* sequence of characters enclosed within a pair of ". If the enclosed */
/* sequence contains \ and ", they must be preceded with a \. Apart from */
/* \and ", the only other character that can follow a \ within a string */
/* are t and n.  */



