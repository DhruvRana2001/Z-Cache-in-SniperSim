
/* DO NOT EDIT - THIS FILE IS AUTOMATICALLY GENERATED FROM THE .syn FILE */

".syntax translation_unit;\n"
".emtcode REVISION 4\n"
".emtcode EXTERNAL_NULL 0\n"
".emtcode EXTERNAL_FUNCTION_DEFINITION 1\n"
".emtcode EXTERNAL_DECLARATION 2\n"
".emtcode DEFAULT_PRECISION 3\n"
".emtcode INVARIANT_STMT 4\n"
".emtcode PRECISION_DEFAULT 0\n"
".emtcode PRECISION_LOW 1\n"
".emtcode PRECISION_MEDIUM 2\n"
".emtcode PRECISION_HIGH 3\n"
".emtcode DECLARATION_FUNCTION_PROTOTYPE 1\n"
".emtcode DECLARATION_INIT_DECLARATOR_LIST 2\n"
".emtcode FUNCTION_ORDINARY 0\n"
".emtcode FUNCTION_CONSTRUCTOR 1\n"
".emtcode FUNCTION_OPERATOR 2\n"
".emtcode OPERATOR_ADDASSIGN 1\n"
".emtcode OPERATOR_SUBASSIGN 2\n"
".emtcode OPERATOR_MULASSIGN 3\n"
".emtcode OPERATOR_DIVASSIGN 4\n"
".emtcode OPERATOR_LOGICALXOR 11\n"
".emtcode OPERATOR_LESS 15\n"
".emtcode OPERATOR_GREATER 16\n"
".emtcode OPERATOR_LESSEQUAL 17\n"
".emtcode OPERATOR_GREATEREQUAL 18\n"
".emtcode OPERATOR_MULTIPLY 21\n"
".emtcode OPERATOR_DIVIDE 22\n"
".emtcode OPERATOR_INCREMENT 24\n"
".emtcode OPERATOR_DECREMENT 25\n"
".emtcode OPERATOR_PLUS 26\n"
".emtcode OPERATOR_MINUS 27\n"
".emtcode OPERATOR_NOT 29\n"
".emtcode DECLARATOR_NONE 0\n"
".emtcode DECLARATOR_NEXT 1\n"
".emtcode VARIABLE_NONE 0\n"
".emtcode VARIABLE_IDENTIFIER 1\n"
".emtcode VARIABLE_INITIALIZER 2\n"
".emtcode VARIABLE_ARRAY_EXPLICIT 3\n"
".emtcode VARIABLE_ARRAY_UNKNOWN 4\n"
".emtcode TYPE_QUALIFIER_NONE 0\n"
".emtcode TYPE_QUALIFIER_CONST 1\n"
".emtcode TYPE_QUALIFIER_ATTRIBUTE 2\n"
".emtcode TYPE_QUALIFIER_VARYING 3\n"
".emtcode TYPE_QUALIFIER_UNIFORM 4\n"
".emtcode TYPE_QUALIFIER_FIXEDOUTPUT 5\n"
".emtcode TYPE_QUALIFIER_FIXEDINPUT 6\n"
".emtcode TYPE_SPECIFIER_VOID 0\n"
".emtcode TYPE_SPECIFIER_BOOL 1\n"
".emtcode TYPE_SPECIFIER_BVEC2 2\n"
".emtcode TYPE_SPECIFIER_BVEC3 3\n"
".emtcode TYPE_SPECIFIER_BVEC4 4\n"
".emtcode TYPE_SPECIFIER_INT 5\n"
".emtcode TYPE_SPECIFIER_IVEC2 6\n"
".emtcode TYPE_SPECIFIER_IVEC3 7\n"
".emtcode TYPE_SPECIFIER_IVEC4 8\n"
".emtcode TYPE_SPECIFIER_FLOAT 9\n"
".emtcode TYPE_SPECIFIER_VEC2 10\n"
".emtcode TYPE_SPECIFIER_VEC3 11\n"
".emtcode TYPE_SPECIFIER_VEC4 12\n"
".emtcode TYPE_SPECIFIER_MAT2 13\n"
".emtcode TYPE_SPECIFIER_MAT3 14\n"
".emtcode TYPE_SPECIFIER_MAT4 15\n"
".emtcode TYPE_SPECIFIER_SAMPLER1D 16\n"
".emtcode TYPE_SPECIFIER_SAMPLER2D 17\n"
".emtcode TYPE_SPECIFIER_SAMPLER3D 18\n"
".emtcode TYPE_SPECIFIER_SAMPLERCUBE 19\n"
".emtcode TYPE_SPECIFIER_SAMPLER1DSHADOW 20\n"
".emtcode TYPE_SPECIFIER_SAMPLER2DSHADOW 21\n"
".emtcode TYPE_SPECIFIER_SAMPLER2DRECT 22\n"
".emtcode TYPE_SPECIFIER_SAMPLER2DRECTSHADOW 23\n"
".emtcode TYPE_SPECIFIER_STRUCT 24\n"
".emtcode TYPE_SPECIFIER_TYPENAME 25\n"
".emtcode TYPE_SPECIFIER_MAT23 26\n"
".emtcode TYPE_SPECIFIER_MAT32 27\n"
".emtcode TYPE_SPECIFIER_MAT24 28\n"
".emtcode TYPE_SPECIFIER_MAT42 29\n"
".emtcode TYPE_SPECIFIER_MAT34 30\n"
".emtcode TYPE_SPECIFIER_MAT43 31\n"
".emtcode FIELD_NONE 0\n"
".emtcode FIELD_NEXT 1\n"
".emtcode FIELD_ARRAY 2\n"
".emtcode OP_END 0\n"
".emtcode OP_BLOCK_BEGIN_NO_NEW_SCOPE 1\n"
".emtcode OP_BLOCK_BEGIN_NEW_SCOPE 2\n"
".emtcode OP_DECLARE 3\n"
".emtcode OP_ASM 4\n"
".emtcode OP_BREAK 5\n"
".emtcode OP_CONTINUE 6\n"
".emtcode OP_DISCARD 7\n"
".emtcode OP_RETURN 8\n"
".emtcode OP_EXPRESSION 9\n"
".emtcode OP_IF 10\n"
".emtcode OP_WHILE 11\n"
".emtcode OP_DO 12\n"
".emtcode OP_FOR 13\n"
".emtcode OP_PUSH_VOID 14\n"
".emtcode OP_PUSH_BOOL 15\n"
".emtcode OP_PUSH_INT 16\n"
".emtcode OP_PUSH_FLOAT 17\n"
".emtcode OP_PUSH_IDENTIFIER 18\n"
".emtcode OP_SEQUENCE 19\n"
".emtcode OP_ASSIGN 20\n"
".emtcode OP_ADDASSIGN 21\n"
".emtcode OP_SUBASSIGN 22\n"
".emtcode OP_MULASSIGN 23\n"
".emtcode OP_DIVASSIGN 24\n"
".emtcode OP_SELECT 31\n"
".emtcode OP_LOGICALOR 32\n"
".emtcode OP_LOGICALXOR 33\n"
".emtcode OP_LOGICALAND 34\n"
".emtcode OP_EQUAL 38\n"
".emtcode OP_NOTEQUAL 39\n"
".emtcode OP_LESS 40\n"
".emtcode OP_GREATER 41\n"
".emtcode OP_LESSEQUAL 42\n"
".emtcode OP_GREATEREQUAL 43\n"
".emtcode OP_ADD 46\n"
".emtcode OP_SUBTRACT 47\n"
".emtcode OP_MULTIPLY 48\n"
".emtcode OP_DIVIDE 49\n"
".emtcode OP_PREINCREMENT 51\n"
".emtcode OP_PREDECREMENT 52\n"
".emtcode OP_PLUS 53\n"
".emtcode OP_MINUS 54\n"
".emtcode OP_NOT 56\n"
".emtcode OP_SUBSCRIPT 57\n"
".emtcode OP_CALL 58\n"
".emtcode OP_FIELD 59\n"
".emtcode OP_POSTINCREMENT 60\n"
".emtcode OP_POSTDECREMENT 61\n"
".emtcode PARAM_QUALIFIER_IN 0\n"
".emtcode PARAM_QUALIFIER_OUT 1\n"
".emtcode PARAM_QUALIFIER_INOUT 2\n"
".emtcode PARAMETER_NONE 0\n"
".emtcode PARAMETER_NEXT 1\n"
".emtcode PARAMETER_ARRAY_NOT_PRESENT 0\n"
".emtcode PARAMETER_ARRAY_PRESENT 1\n"
".errtext INVALID_EXTERNAL_DECLARATION \"2001: Syntax error.\"\n"
".errtext INVALID_OPERATOR_OVERRIDE \"2002: Invalid operator override.\"\n"
".errtext LBRACE_EXPECTED \"2003: '{' expected but '$err_token$' found.\"\n"
".errtext LPAREN_EXPECTED \"2004: '(' expected but '$err_token$' found.\"\n"
".errtext RPAREN_EXPECTED \"2005: ')' expected but '$err_token$' found.\"\n"
".errtext INVALID_PRECISION \"2006: Invalid precision specifier '$err_token$'.\"\n"
".errtext INVALID_PRECISION_TYPE \"2007: Invalid precision type '$err_token$'.\"\n"
".regbyte parsing_builtin 0\n"
".regbyte shader_type 0\n"
"variable_identifier\n"
" identifier .emit OP_PUSH_IDENTIFIER;\n"
"primary_expression\n"
" floatconstant .or boolconstant .or intconstant .or variable_identifier .or primary_expression_1;\n"
"primary_expression_1\n"
" lparen .and expression .and rparen;\n"
"postfix_expression\n"
" postfix_expression_1 .and .loop postfix_expression_2;\n"
"postfix_expression_1\n"
" function_call .or primary_expression;\n"
"postfix_expression_2\n"
" postfix_expression_3 .or postfix_expression_4 .or\n"
" plusplus .emit OP_POSTINCREMENT .or\n"
" minusminus .emit OP_POSTDECREMENT;\n"
"postfix_expression_3\n"
" lbracket .and integer_expression .and rbracket .emit OP_SUBSCRIPT;\n"
"postfix_expression_4\n"
" dot .and field_selection .emit OP_FIELD;\n"
"integer_expression\n"
" expression;\n"
"function_call\n"
" function_call_generic .emit OP_CALL .and .true .emit OP_END;\n"
"function_call_generic\n"
" function_call_generic_1 .or function_call_generic_2;\n"
"function_call_generic_1\n"
" function_call_header_with_parameters .and rparen .error RPAREN_EXPECTED;\n"
"function_call_generic_2\n"
" function_call_header_no_parameters .and rparen .error RPAREN_EXPECTED;\n"
"function_call_header_no_parameters\n"
" function_call_header .and function_call_header_no_parameters_1;\n"
"function_call_header_no_parameters_1\n"
" \"void\" .or .true;\n"
"function_call_header_with_parameters\n"
" function_call_header .and assignment_expression .and .true .emit OP_END .and\n"
" .loop function_call_header_with_parameters_1;\n"
"function_call_header_with_parameters_1\n"
" comma .and assignment_expression .and .true .emit OP_END;\n"
"function_call_header\n"
" function_identifier .and lparen;\n"
"function_identifier\n"
" identifier;\n"
"unary_expression\n"
" postfix_expression .or unary_expression_1 .or unary_expression_2 .or unary_expression_3 .or\n"
" unary_expression_4 .or unary_expression_5;\n"
"unary_expression_1\n"
" plusplus .and unary_expression .and .true .emit OP_PREINCREMENT;\n"
"unary_expression_2\n"
" minusminus .and unary_expression .and .true .emit OP_PREDECREMENT;\n"
"unary_expression_3\n"
" plus .and unary_expression .and .true .emit OP_PLUS;\n"
"unary_expression_4\n"
" minus .and unary_expression .and .true .emit OP_MINUS;\n"
"unary_expression_5\n"
" bang .and unary_expression .and .true .emit OP_NOT;\n"
"multiplicative_expression\n"
" unary_expression .and .loop multiplicative_expression_1;\n"
"multiplicative_expression_1\n"
" multiplicative_expression_2 .or multiplicative_expression_3;\n"
"multiplicative_expression_2\n"
" star .and unary_expression .and .true .emit OP_MULTIPLY;\n"
"multiplicative_expression_3\n"
" slash .and unary_expression .and .true .emit OP_DIVIDE;\n"
"additive_expression\n"
" multiplicative_expression .and .loop additive_expression_1;\n"
"additive_expression_1\n"
" additive_expression_2 .or additive_expression_3;\n"
"additive_expression_2\n"
" plus .and multiplicative_expression .and .true .emit OP_ADD;\n"
"additive_expression_3\n"
" minus .and multiplicative_expression .and .true .emit OP_SUBTRACT;\n"
"shift_expression\n"
" additive_expression;\n"
"relational_expression\n"
" shift_expression .and .loop relational_expression_1;\n"
"relational_expression_1\n"
" relational_expression_2 .or relational_expression_3 .or relational_expression_4 .or\n"
" relational_expression_5;\n"
"relational_expression_2\n"
" lessequals .and shift_expression .and .true .emit OP_LESSEQUAL;\n"
"relational_expression_3\n"
" greaterequals .and shift_expression .and .true .emit OP_GREATEREQUAL;\n"
"relational_expression_4\n"
" less .and shift_expression .and .true .emit OP_LESS;\n"
"relational_expression_5\n"
" greater .and shift_expression .and .true .emit OP_GREATER;\n"
"equality_expression\n"
" relational_expression .and .loop equality_expression_1;\n"
"equality_expression_1\n"
" equality_expression_2 .or equality_expression_3;\n"
"equality_expression_2\n"
" equalsequals .and relational_expression .and .true .emit OP_EQUAL;\n"
"equality_expression_3\n"
" bangequals .and relational_expression .and .true .emit OP_NOTEQUAL;\n"
"and_expression\n"
" equality_expression;\n"
"exclusive_or_expression\n"
" and_expression;\n"
"inclusive_or_expression\n"
" exclusive_or_expression;\n"
"logical_and_expression\n"
" inclusive_or_expression .and .loop logical_and_expression_1;\n"
"logical_and_expression_1\n"
" ampersandampersand .and inclusive_or_expression .and .true .emit OP_LOGICALAND;\n"
"logical_xor_expression\n"
" logical_and_expression .and .loop logical_xor_expression_1;\n"
"logical_xor_expression_1\n"
" caretcaret .and logical_and_expression .and .true .emit OP_LOGICALXOR;\n"
"logical_or_expression\n"
" logical_xor_expression .and .loop logical_or_expression_1;\n"
"logical_or_expression_1\n"
" barbar .and logical_xor_expression .and .true .emit OP_LOGICALOR;\n"
"conditional_expression\n"
" logical_or_expression .and .loop conditional_expression_1;\n"
"conditional_expression_1\n"
" question .and expression .and colon .and conditional_expression .and .true .emit OP_SELECT;\n"
"assignment_expression\n"
" assignment_expression_1 .or assignment_expression_2 .or assignment_expression_3 .or\n"
" assignment_expression_4 .or assignment_expression_5 .or conditional_expression;\n"
"assignment_expression_1\n"
" unary_expression .and equals .and assignment_expression .and .true .emit OP_ASSIGN;\n"
"assignment_expression_2\n"
" unary_expression .and starequals .and assignment_expression .and .true .emit OP_MULASSIGN;\n"
"assignment_expression_3\n"
" unary_expression .and slashequals .and assignment_expression .and .true .emit OP_DIVASSIGN;\n"
"assignment_expression_4\n"
" unary_expression .and plusequals .and assignment_expression .and .true .emit OP_ADDASSIGN;\n"
"assignment_expression_5\n"
" unary_expression .and minusequals .and assignment_expression .and .true .emit OP_SUBASSIGN;\n"
"expression\n"
" assignment_expression .and .loop expression_1;\n"
"expression_1\n"
" comma .and assignment_expression .and .true .emit OP_SEQUENCE;\n"
"constant_expression\n"
" conditional_expression .and .true .emit OP_END;\n"
"declaration\n"
" declaration_1 .or declaration_2;\n"
"declaration_1\n"
" function_prototype .emit DECLARATION_FUNCTION_PROTOTYPE .and semicolon;\n"
"declaration_2\n"
" init_declarator_list .emit DECLARATION_INIT_DECLARATOR_LIST .and semicolon;\n"
"function_prototype\n"
" function_prototype_1 .or function_prototype_2;\n"
"function_prototype_1\n"
" function_header .and \"void\" .and rparen .error RPAREN_EXPECTED .emit PARAMETER_NONE;\n"
"function_prototype_2\n"
" function_declarator .and rparen .error RPAREN_EXPECTED .emit PARAMETER_NONE;\n"
"function_declarator\n"
" function_header_with_parameters .or function_header;\n"
"function_header_with_parameters\n"
" function_header .and parameter_declaration .and .loop function_header_with_parameters_1;\n"
"function_header_with_parameters_1\n"
" comma .and parameter_declaration;\n"
"function_header\n"
" function_header_nospace .or function_header_space;\n"
"function_header_space\n"
" fully_specified_type_space .and space .and function_decl_identifier .and lparen;\n"
"function_header_nospace\n"
" fully_specified_type_nospace .and function_decl_identifier .and lparen;\n"
"function_decl_identifier\n"
" .if (parsing_builtin != 0) __operator .emit FUNCTION_OPERATOR .or\n"
" .if (parsing_builtin != 0) \"__constructor\" .emit FUNCTION_CONSTRUCTOR .or\n"
" identifier .emit FUNCTION_ORDINARY;\n"
"__operator\n"
" \"__operator\" .and overriden_operator .error INVALID_OPERATOR_OVERRIDE;\n"
"overriden_operator\n"
" plusplus .emit OPERATOR_INCREMENT .or\n"
" plusequals .emit OPERATOR_ADDASSIGN .or\n"
" plus .emit OPERATOR_PLUS .or\n"
" minusminus .emit OPERATOR_DECREMENT .or\n"
" minusequals .emit OPERATOR_SUBASSIGN .or\n"
" minus .emit OPERATOR_MINUS .or\n"
" bang .emit OPERATOR_NOT .or\n"
" starequals .emit OPERATOR_MULASSIGN .or\n"
" star .emit OPERATOR_MULTIPLY .or\n"
" slashequals .emit OPERATOR_DIVASSIGN .or\n"
" slash .emit OPERATOR_DIVIDE .or\n"
" lessequals .emit OPERATOR_LESSEQUAL .or\n"
" \n"
" \n"
" less .emit OPERATOR_LESS .or\n"
" greaterequals .emit OPERATOR_GREATEREQUAL .or\n"
" \n"
" \n"
" greater .emit OPERATOR_GREATER .or\n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" caretcaret .emit OPERATOR_LOGICALXOR ;\n"
"parameter_declarator\n"
" parameter_declarator_nospace .or parameter_declarator_space;\n"
"parameter_declarator_nospace\n"
" type_specifier_nospace .and identifier .and parameter_declarator_1;\n"
"parameter_declarator_space\n"
" type_specifier_space .and space .and identifier .and parameter_declarator_1;\n"
"parameter_declarator_1\n"
" parameter_declarator_2 .emit PARAMETER_ARRAY_PRESENT .or\n"
" .true .emit PARAMETER_ARRAY_NOT_PRESENT;\n"
"parameter_declarator_2\n"
" lbracket .and constant_expression .and rbracket;\n"
"parameter_declaration\n"
" parameter_declaration_1 .emit PARAMETER_NEXT;\n"
"parameter_declaration_1\n"
" parameter_declaration_2 .or parameter_declaration_3;\n"
"parameter_declaration_2\n"
" type_qualifier .and space .and parameter_qualifier .and parameter_declaration_4;\n"
"parameter_declaration_3\n"
" parameter_qualifier .emit TYPE_QUALIFIER_NONE .and parameter_declaration_4;\n"
"parameter_declaration_4\n"
" parameter_declarator .or parameter_type_specifier;\n"
"parameter_qualifier\n"
" parameter_qualifier_1 .or .true .emit PARAM_QUALIFIER_IN;\n"
"parameter_qualifier_1\n"
" parameter_qualifier_2 .and space;\n"
"parameter_qualifier_2\n"
" \"in\" .emit PARAM_QUALIFIER_IN .or\n"
" \"out\" .emit PARAM_QUALIFIER_OUT .or\n"
" \"inout\" .emit PARAM_QUALIFIER_INOUT;\n"
"parameter_type_specifier\n"
" parameter_type_specifier_1 .and .true .emit '\\0' .and parameter_type_specifier_2;\n"
"parameter_type_specifier_1\n"
" type_specifier_nospace .or type_specifier_space;\n"
"parameter_type_specifier_2\n"
" parameter_type_specifier_3 .emit PARAMETER_ARRAY_PRESENT .or\n"
" .true .emit PARAMETER_ARRAY_NOT_PRESENT;\n"
"parameter_type_specifier_3\n"
" lbracket .and constant_expression .and rbracket;\n"
"init_declarator_list\n"
" single_declaration .and .loop init_declarator_list_1 .emit DECLARATOR_NEXT .and\n"
" .true .emit DECLARATOR_NONE;\n"
"init_declarator_list_1\n"
" comma .and identifier .emit VARIABLE_IDENTIFIER .and init_declarator_list_2;\n"
"init_declarator_list_2\n"
" init_declarator_list_3 .or init_declarator_list_4 .or .true .emit VARIABLE_NONE;\n"
"init_declarator_list_3\n"
" equals .and initializer .emit VARIABLE_INITIALIZER;\n"
"init_declarator_list_4\n"
" lbracket .and init_declarator_list_5 .and rbracket;\n"
"init_declarator_list_5\n"
" constant_expression .emit VARIABLE_ARRAY_EXPLICIT .or .true .emit VARIABLE_ARRAY_UNKNOWN;\n"
"single_declaration\n"
" single_declaration_nospace .or single_declaration_space;\n"
"single_declaration_space\n"
" fully_specified_type_space .and single_declaration_space_1;\n"
"single_declaration_nospace\n"
" fully_specified_type_nospace .and single_declaration_nospace_1;\n"
"single_declaration_space_1\n"
" single_declaration_space_2 .emit VARIABLE_IDENTIFIER .or .true .emit VARIABLE_NONE;\n"
"single_declaration_nospace_1\n"
" single_declaration_nospace_2 .emit VARIABLE_IDENTIFIER .or .true .emit VARIABLE_NONE;\n"
"single_declaration_space_2\n"
" space .and identifier .and single_declaration_3;\n"
"single_declaration_nospace_2\n"
" identifier .and single_declaration_3;\n"
"single_declaration_3\n"
" single_declaration_4 .or single_declaration_5 .or .true .emit VARIABLE_NONE;\n"
"single_declaration_4\n"
" equals .and initializer .emit VARIABLE_INITIALIZER;\n"
"single_declaration_5\n"
" lbracket .and single_declaration_6 .and rbracket;\n"
"single_declaration_6\n"
" constant_expression .emit VARIABLE_ARRAY_EXPLICIT .or .true .emit VARIABLE_ARRAY_UNKNOWN;\n"
"fully_specified_type_space\n"
" fully_specified_type_optqual .and fully_specified_type_optprec .and type_specifier_space;\n"
"fully_specified_type_nospace\n"
" fully_specified_type_optqual .and fully_specified_type_optprec .and type_specifier_nospace;\n"
"fully_specified_type_optqual\n"
" fully_specified_type_qual .or .true .emit TYPE_QUALIFIER_NONE;\n"
"fully_specified_type_qual\n"
" type_qualifier .and space;\n"
"fully_specified_type_optprec\n"
" fully_specified_type_prec .or .true .emit PRECISION_DEFAULT;\n"
"fully_specified_type_prec\n"
" precision .and space;\n"
"type_qualifier\n"
" \"const\" .emit TYPE_QUALIFIER_CONST .or\n"
" .if (shader_type == 2) \"attribute\" .emit TYPE_QUALIFIER_ATTRIBUTE .or\n"
" \"varying\" .emit TYPE_QUALIFIER_VARYING .or\n"
" \"uniform\" .emit TYPE_QUALIFIER_UNIFORM .or\n"
"	.if (parsing_builtin != 0) \"__fixed_output\" .emit TYPE_QUALIFIER_FIXEDOUTPUT .or\n"
"	.if (parsing_builtin != 0) \"__fixed_input\" .emit TYPE_QUALIFIER_FIXEDINPUT;\n"
"type_specifier_space\n"
" \"void\" .emit TYPE_SPECIFIER_VOID .or\n"
" \"float\" .emit TYPE_SPECIFIER_FLOAT .or\n"
" \"int\" .emit TYPE_SPECIFIER_INT .or\n"
" \"bool\" .emit TYPE_SPECIFIER_BOOL .or\n"
" \"vec2\" .emit TYPE_SPECIFIER_VEC2 .or\n"
" \"vec3\" .emit TYPE_SPECIFIER_VEC3 .or\n"
" \"vec4\" .emit TYPE_SPECIFIER_VEC4 .or\n"
" \"bvec2\" .emit TYPE_SPECIFIER_BVEC2 .or\n"
" \"bvec3\" .emit TYPE_SPECIFIER_BVEC3 .or\n"
" \"bvec4\" .emit TYPE_SPECIFIER_BVEC4 .or\n"
" \"ivec2\" .emit TYPE_SPECIFIER_IVEC2 .or\n"
" \"ivec3\" .emit TYPE_SPECIFIER_IVEC3 .or\n"
" \"ivec4\" .emit TYPE_SPECIFIER_IVEC4 .or\n"
" \"mat2\" .emit TYPE_SPECIFIER_MAT2 .or\n"
" \"mat3\" .emit TYPE_SPECIFIER_MAT3 .or\n"
" \"mat4\" .emit TYPE_SPECIFIER_MAT4 .or\n"
" \"mat2x3\" .emit TYPE_SPECIFIER_MAT23 .or\n"
" \"mat3x2\" .emit TYPE_SPECIFIER_MAT32 .or\n"
" \"mat2x4\" .emit TYPE_SPECIFIER_MAT24 .or\n"
" \"mat4x2\" .emit TYPE_SPECIFIER_MAT42 .or\n"
" \"mat3x4\" .emit TYPE_SPECIFIER_MAT34 .or\n"
" \"mat4x3\" .emit TYPE_SPECIFIER_MAT43 .or\n"
" \"sampler1D\" .emit TYPE_SPECIFIER_SAMPLER1D .or\n"
" \"sampler2D\" .emit TYPE_SPECIFIER_SAMPLER2D .or\n"
" \"sampler3D\" .emit TYPE_SPECIFIER_SAMPLER3D .or\n"
" \"samplerCube\" .emit TYPE_SPECIFIER_SAMPLERCUBE .or\n"
" \"sampler1DShadow\" .emit TYPE_SPECIFIER_SAMPLER1DSHADOW .or\n"
" \"sampler2DShadow\" .emit TYPE_SPECIFIER_SAMPLER2DSHADOW .or\n"
" \"sampler2DRect\" .emit TYPE_SPECIFIER_SAMPLER2DRECT .or\n"
" \"sampler2DRectShadow\" .emit TYPE_SPECIFIER_SAMPLER2DRECTSHADOW .or\n"
" type_name .emit TYPE_SPECIFIER_TYPENAME;\n"
"type_specifier_nospace\n"
" struct_specifier .emit TYPE_SPECIFIER_STRUCT;\n"
"struct_specifier\n"
" \"struct\" .and struct_specifier_1 .and optional_space .and lbrace .error LBRACE_EXPECTED .and\n"
" struct_declaration_list .and rbrace .emit FIELD_NONE;\n"
"struct_specifier_1\n"
" struct_specifier_2 .or .true .emit '\\0';\n"
"struct_specifier_2\n"
" space .and identifier;\n"
"struct_declaration_list\n"
" struct_declaration .and .loop struct_declaration .emit FIELD_NEXT;\n"
"struct_declaration\n"
" struct_declaration_nospace .or struct_declaration_space;\n"
"struct_declaration_space\n"
" type_specifier_space .and space .and struct_declarator_list .and semicolon .emit FIELD_NONE;\n"
"struct_declaration_nospace\n"
" type_specifier_nospace .and struct_declarator_list .and semicolon .emit FIELD_NONE;\n"
"struct_declarator_list\n"
" struct_declarator .and .loop struct_declarator_list_1 .emit FIELD_NEXT;\n"
"struct_declarator_list_1\n"
" comma .and struct_declarator;\n"
"struct_declarator\n"
" identifier .and struct_declarator_1;\n"
"struct_declarator_1\n"
" struct_declarator_2 .emit FIELD_ARRAY .or .true .emit FIELD_NONE;\n"
"struct_declarator_2\n"
" lbracket .and constant_expression .and rbracket;\n"
"initializer\n"
" assignment_expression .and .true .emit OP_END;\n"
"declaration_statement\n"
" declaration;\n"
"statement\n"
" compound_statement .or simple_statement;\n"
"statement_space\n"
" compound_statement .or statement_space_1;\n"
"statement_space_1\n"
" space .and simple_statement;\n"
"simple_statement\n"
" .if (parsing_builtin != 0) __asm_statement .emit OP_ASM .or\n"
" selection_statement .or\n"
" iteration_statement .or\n"
" jump_statement .or\n"
" expression_statement .emit OP_EXPRESSION .or\n"
" declaration_statement .emit OP_DECLARE;\n"
"compound_statement\n"
" compound_statement_1 .emit OP_BLOCK_BEGIN_NEW_SCOPE .and .true .emit OP_END;\n"
"compound_statement_1\n"
" compound_statement_2 .or compound_statement_3;\n"
"compound_statement_2\n"
" lbrace .and rbrace;\n"
"compound_statement_3\n"
" lbrace .and statement_list .and rbrace;\n"
"statement_no_new_scope\n"
" compound_statement_no_new_scope .or simple_statement;\n"
"compound_statement_no_new_scope\n"
" compound_statement_no_new_scope_1 .emit OP_BLOCK_BEGIN_NO_NEW_SCOPE .and .true .emit OP_END;\n"
"compound_statement_no_new_scope_1\n"
" compound_statement_no_new_scope_2 .or compound_statement_no_new_scope_3;\n"
"compound_statement_no_new_scope_2\n"
" lbrace .and rbrace;\n"
"compound_statement_no_new_scope_3\n"
" lbrace .and statement_list .and rbrace;\n"
"statement_list\n"
" statement .and .loop statement;\n"
"expression_statement\n"
" expression_statement_1 .or expression_statement_2;\n"
"expression_statement_1\n"
" semicolon .emit OP_PUSH_VOID .emit OP_END;\n"
"expression_statement_2\n"
" expression .and semicolon .emit OP_END;\n"
"selection_statement\n"
" \"if\" .emit OP_IF .and lparen .error LPAREN_EXPECTED .and expression .and\n"
" rparen .error RPAREN_EXPECTED .emit OP_END .and selection_rest_statement;\n"
"selection_rest_statement\n"
" statement .and selection_rest_statement_1;\n"
"selection_rest_statement_1\n"
" selection_rest_statement_2 .or .true .emit OP_EXPRESSION .emit OP_PUSH_VOID .emit OP_END;\n"
"selection_rest_statement_2\n"
" \"else\" .and optional_space .and statement;\n"
"condition\n"
" condition_1 .emit OP_DECLARE .emit DECLARATION_INIT_DECLARATOR_LIST .or\n"
" condition_3 .emit OP_EXPRESSION;\n"
"condition_1\n"
" condition_1_nospace .or condition_1_space;\n"
"condition_1_nospace\n"
" fully_specified_type_nospace .and condition_2;\n"
"condition_1_space\n"
" fully_specified_type_space .and space .and condition_2;\n"
"condition_2\n"
" identifier .emit VARIABLE_IDENTIFIER .and equals .emit VARIABLE_INITIALIZER .and\n"
" initializer .and .true .emit DECLARATOR_NONE;\n"
"condition_3\n"
" expression .and .true .emit OP_END;\n"
"iteration_statement\n"
" iteration_statement_1 .or iteration_statement_2 .or iteration_statement_3;\n"
"iteration_statement_1\n"
" \"while\" .emit OP_WHILE .and lparen .error LPAREN_EXPECTED .and condition .and\n"
" rparen .error RPAREN_EXPECTED .and statement;\n"
"iteration_statement_2\n"
" \"do\" .emit OP_DO .and statement_space .and \"while\" .and lparen .error LPAREN_EXPECTED .and\n"
" expression .and rparen .error RPAREN_EXPECTED .emit OP_END .and semicolon;\n"
"iteration_statement_3\n"
" \"for\" .emit OP_FOR .and lparen .error LPAREN_EXPECTED .and for_init_statement .and\n"
" for_rest_statement .and rparen .error RPAREN_EXPECTED .and statement_no_new_scope;\n"
"for_init_statement\n"
" expression_statement .emit OP_EXPRESSION .or declaration_statement .emit OP_DECLARE;\n"
"conditionopt\n"
" condition .or\n"
" .true .emit OP_EXPRESSION .emit OP_PUSH_BOOL .emit 2 .emit '1' .emit '\\0' .emit OP_END;\n"
"for_rest_statement\n"
" conditionopt .and semicolon .and for_rest_statement_1;\n"
"for_rest_statement_1\n"
" for_rest_statement_2 .or .true .emit OP_PUSH_VOID .emit OP_END;\n"
"for_rest_statement_2\n"
" expression .and .true .emit OP_END;\n"
"jump_statement\n"
" jump_statement_1 .or jump_statement_2 .or jump_statement_3 .or jump_statement_4 .or\n"
" .if (shader_type == 1) jump_statement_5;\n"
"jump_statement_1\n"
" \"continue\" .and semicolon .emit OP_CONTINUE;\n"
"jump_statement_2\n"
" \"break\" .and semicolon .emit OP_BREAK;\n"
"jump_statement_3\n"
" \"return\" .emit OP_RETURN .and optional_space .and expression .and semicolon .emit OP_END;\n"
"jump_statement_4\n"
" \"return\" .emit OP_RETURN .and semicolon .emit OP_PUSH_VOID .emit OP_END;\n"
"jump_statement_5\n"
" \"discard\" .and semicolon .emit OP_DISCARD;\n"
"__asm_statement\n"
" \"__asm\" .and space .and identifier .and space .and asm_arguments .and semicolon .emit OP_END;\n"
"asm_arguments\n"
" asm_argument .and .true .emit OP_END .and .loop asm_arguments_1;\n"
"asm_arguments_1\n"
" comma .and asm_argument .and .true .emit OP_END;\n"
"asm_argument\n"
" var_with_field .or\n"
" variable_identifier .or\n"
" floatconstant;\n"
"var_with_field\n"
" variable_identifier .and dot .and field_selection .emit OP_FIELD;\n"
"translation_unit\n"
" optional_space .emit REVISION .and external_declaration .error INVALID_EXTERNAL_DECLARATION .and\n"
" .loop external_declaration .and optional_space .and\n"
" '\\0' .error INVALID_EXTERNAL_DECLARATION .emit EXTERNAL_NULL;\n"
"external_declaration\n"
" precision_stmt .emit DEFAULT_PRECISION .or\n"
" invariant_stmt .emit INVARIANT_STMT .or\n"
" function_definition .emit EXTERNAL_FUNCTION_DEFINITION .or\n"
" declaration .emit EXTERNAL_DECLARATION;\n"
"precision_stmt\n"
" \"precision\" .and space .and precision .error INVALID_PRECISION .and space .and prectype .error INVALID_PRECISION_TYPE .and semicolon;\n"
"precision\n"
" \"lowp\" .emit PRECISION_LOW .or\n"
" \"mediump\" .emit PRECISION_MEDIUM .or\n"
" \"highp\" .emit PRECISION_HIGH;\n"
"prectype\n"
" \"int\" .emit TYPE_SPECIFIER_INT .or\n"
" \"float\" .emit TYPE_SPECIFIER_FLOAT .or\n"
" \"sampler1D\" .emit TYPE_SPECIFIER_SAMPLER1D .or\n"
" \"sampler2D\" .emit TYPE_SPECIFIER_SAMPLER2D .or\n"
" \"sampler3D\" .emit TYPE_SPECIFIER_SAMPLER3D .or\n"
" \"samplerCube\" .emit TYPE_SPECIFIER_SAMPLERCUBE .or\n"
" \"sampler1DShadow\" .emit TYPE_SPECIFIER_SAMPLER1DSHADOW .or\n"
" \"sampler2DShadow\" .emit TYPE_SPECIFIER_SAMPLER2DSHADOW .or\n"
" \"sampler2DRect\" .emit TYPE_SPECIFIER_SAMPLER2DRECT .or\n"
" \"sampler2DRectShadow\" .emit TYPE_SPECIFIER_SAMPLER2DRECTSHADOW;\n"
"invariant_stmt\n"
" \"invariant\" .and space .and identifier .and semicolon;\n"
"function_definition\n"
" function_prototype .and compound_statement_no_new_scope;\n"
"digit_oct\n"
" '0'-'7';\n"
"digit_dec\n"
" '0'-'9';\n"
"digit_hex\n"
" '0'-'9' .or 'A'-'F' .or 'a'-'f';\n"
"id_character_first\n"
" 'a'-'z' .or 'A'-'Z' .or '_';\n"
"id_character_next\n"
" id_character_first .or digit_dec;\n"
"identifier\n"
" id_character_first .emit * .and .loop id_character_next .emit * .and .true .emit '\\0';\n"
"float\n"
" float_1 .or float_2;\n"
"float_1\n"
" float_fractional_constant .and float_optional_exponent_part;\n"
"float_2\n"
" float_digit_sequence .and .true .emit '\\0' .and float_exponent_part;\n"
"float_fractional_constant\n"
" float_fractional_constant_1 .or float_fractional_constant_2 .or float_fractional_constant_3;\n"
"float_fractional_constant_1\n"
" float_digit_sequence .and '.' .and float_digit_sequence;\n"
"float_fractional_constant_2\n"
" float_digit_sequence .and '.' .and .true .emit '\\0';\n"
"float_fractional_constant_3\n"
" '.' .emit '\\0' .and float_digit_sequence;\n"
"float_optional_exponent_part\n"
" float_exponent_part .or .true .emit '\\0';\n"
"float_digit_sequence\n"
" digit_dec .emit * .and .loop digit_dec .emit * .and .true .emit '\\0';\n"
"float_exponent_part\n"
" float_exponent_part_1 .or float_exponent_part_2;\n"
"float_exponent_part_1\n"
" 'e' .and float_optional_sign .and float_digit_sequence;\n"
"float_exponent_part_2\n"
" 'E' .and float_optional_sign .and float_digit_sequence;\n"
"float_optional_sign\n"
" float_sign .or .true;\n"
"float_sign\n"
" '+' .or '-' .emit '-';\n"
"integer\n"
" integer_hex .or integer_oct .or integer_dec;\n"
"integer_hex\n"
" '0' .and integer_hex_1 .emit 0x10 .and digit_hex .emit * .and .loop digit_hex .emit * .and\n"
" .true .emit '\\0';\n"
"integer_hex_1\n"
" 'x' .or 'X';\n"
"integer_oct\n"
" '0' .emit 8 .emit * .and .loop digit_oct .emit * .and .true .emit '\\0';\n"
"integer_dec\n"
" digit_dec .emit 10 .emit * .and .loop digit_dec .emit * .and .true .emit '\\0';\n"
"boolean\n"
" \"true\" .emit 2 .emit '1' .emit '\\0' .or\n"
" \"false\" .emit 2 .emit '0' .emit '\\0';\n"
"type_name\n"
" identifier;\n"
"field_selection\n"
" identifier;\n"
"floatconstant\n"
" float .emit OP_PUSH_FLOAT;\n"
"intconstant\n"
" integer .emit OP_PUSH_INT;\n"
"boolconstant\n"
" boolean .emit OP_PUSH_BOOL;\n"
"optional_space\n"
" .loop single_space;\n"
"space\n"
" single_space .and .loop single_space;\n"
"single_space\n"
" white_char .or c_style_comment_block .or cpp_style_comment_block;\n"
"white_char\n"
" ' ' .or '\\t' .or new_line .or '\\v' .or '\\f';\n"
"new_line\n"
" cr_lf .or lf_cr .or '\\n' .or '\\r';\n"
"cr_lf\n"
" '\\r' .and '\\n';\n"
"lf_cr\n"
" '\\n' .and '\\r';\n"
"c_style_comment_block\n"
" '/' .and '*' .and c_style_comment_rest;\n"
"c_style_comment_rest\n"
" .loop c_style_comment_char_no_star .and c_style_comment_rest_1;\n"
"c_style_comment_rest_1\n"
" c_style_comment_end .or c_style_comment_rest_2;\n"
"c_style_comment_rest_2\n"
" '*' .and c_style_comment_rest;\n"
"c_style_comment_char_no_star\n"
" '\\x2B'-'\\xFF' .or '\\x01'-'\\x29';\n"
"c_style_comment_end\n"
" '*' .and '/';\n"
"cpp_style_comment_block\n"
" '/' .and '/' .and cpp_style_comment_block_1;\n"
"cpp_style_comment_block_1\n"
" cpp_style_comment_block_2 .or cpp_style_comment_block_3;\n"
"cpp_style_comment_block_2\n"
" .loop cpp_style_comment_char .and new_line;\n"
"cpp_style_comment_block_3\n"
" .loop cpp_style_comment_char;\n"
"cpp_style_comment_char\n"
" '\\x0E'-'\\xFF' .or '\\x01'-'\\x09' .or '\\x0B'-'\\x0C';\n"
"ampersandampersand\n"
" optional_space .and '&' .and '&' .and optional_space;\n"
"barbar\n"
" optional_space .and '|' .and '|' .and optional_space;\n"
"bang\n"
" optional_space .and '!' .and optional_space;\n"
"bangequals\n"
" optional_space .and '!' .and '=' .and optional_space;\n"
"caretcaret\n"
" optional_space .and '^' .and '^' .and optional_space;\n"
"colon\n"
" optional_space .and ':' .and optional_space;\n"
"comma\n"
" optional_space .and ',' .and optional_space;\n"
"dot\n"
" optional_space .and '.' .and optional_space;\n"
"equals\n"
" optional_space .and '=' .and optional_space;\n"
"equalsequals\n"
" optional_space .and '=' .and '=' .and optional_space;\n"
"greater\n"
" optional_space .and '>' .and optional_space;\n"
"greaterequals\n"
" optional_space .and '>' .and '=' .and optional_space;\n"
"lbrace\n"
" optional_space .and '{' .and optional_space;\n"
"lbracket\n"
" optional_space .and '[' .and optional_space;\n"
"less\n"
" optional_space .and '<' .and optional_space;\n"
"lessequals\n"
" optional_space .and '<' .and '=' .and optional_space;\n"
"lparen\n"
" optional_space .and '(' .and optional_space;\n"
"minus\n"
" optional_space .and '-' .and optional_space;\n"
"minusequals\n"
" optional_space .and '-' .and '=' .and optional_space;\n"
"minusminus\n"
" optional_space .and '-' .and '-' .and optional_space;\n"
"plus\n"
" optional_space .and '+' .and optional_space;\n"
"plusequals\n"
" optional_space .and '+' .and '=' .and optional_space;\n"
"plusplus\n"
" optional_space .and '+' .and '+' .and optional_space;\n"
"question\n"
" optional_space .and '?' .and optional_space;\n"
"rbrace\n"
" optional_space .and '}' .and optional_space;\n"
"rbracket\n"
" optional_space .and ']' .and optional_space;\n"
"rparen\n"
" optional_space .and ')' .and optional_space;\n"
"semicolon\n"
" optional_space .and ';' .and optional_space;\n"
"slash\n"
" optional_space .and '/' .and optional_space;\n"
"slashequals\n"
" optional_space .and '/' .and '=' .and optional_space;\n"
"star\n"
" optional_space .and '*' .and optional_space;\n"
"starequals\n"
" optional_space .and '*' .and '=' .and optional_space;\n"
".string string_lexer;\n"
"string_lexer\n"
" lex_first_identifier_character .and .loop lex_next_identifier_character;\n"
"lex_first_identifier_character\n"
" 'a'-'z' .or 'A'-'Z' .or '_';\n"
"lex_next_identifier_character\n"
" 'a'-'z' .or 'A'-'Z' .or '0'-'9' .or '_';\n"
"err_token\n"
" '~' .or '`' .or '!' .or '@' .or '#' .or '$' .or '%' .or '^' .or '&' .or '*' .or '(' .or ')' .or\n"
" '-' .or '+' .or '=' .or '|' .or '\\\\' .or '[' .or ']' .or '{' .or '}' .or ':' .or ';' .or '\"' .or\n"
" '\\'' .or '<' .or ',' .or '>' .or '.' .or '/' .or '?' .or err_identifier;\n"
"err_identifier\n"
" id_character_first .and .loop id_character_next;\n"
""
