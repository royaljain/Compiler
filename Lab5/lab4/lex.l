%%
[\n] {Scanner::linenum++;}
[ \t]+
void return Parser::VOID;
int return Parser::INT;
return return Parser::RETURN;
float return Parser::FLOAT;
if return Parser::IF;
else return Parser::ELSE;
while return Parser::WHILE;
for return Parser::FOR;
\|\| return Parser::OR_OP;
&& return Parser::AND_OP;
== return Parser::EQ_OP;
!= return Parser::NE_OP;
\<= return Parser::LE_OP;
\>= return Parser::GE_OP;
\+\+ {Parser::d_val__ = std::string(matched()); return Parser::INC_OP;}
([1-9][0-9]*)|(0[bB][01]+)|(0[0-7]*)|(0[xX][0-9a-fA-F]+) {Parser::d_val__ = std::string(matched()); return Parser::INT_CONSTANT;}
(\+|-)?(([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))([eE][+|-]?[0-9]+)? {Parser::d_val__ = std::string(matched());return Parser::FLOAT_CONSTANT;}
\"([^"\\]*(\\(\\|\"|n|t))*)*\"	{Parser::d_val__ = std::string(matched());return Parser::STRING_LITERAL;}
[a-zA-Z_][a-zA-Z0-9_]* {Parser::d_val__ = std::string(matched());return Parser::IDENTIFIER;}
.											return matched()[0];
