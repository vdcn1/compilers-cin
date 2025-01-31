#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "visitor.h"

#define printm(...) printf(__VA_ARGS__)

int isStatBlock = 0;

int globalAddrC = 0;

char buffer[256];
int str_count = 0;
char* type_func;
void print_addr(){
	printf("\t %%%d = ", ++globalAddrC);
	/*if(expr->expr.binary_expr.operation == '+'){
		printf("add");
	}
	if(expr->expr.binary_expr.operation == '-'){
		printf("sub");
	}
	if(expr->expr.binary_expr.operation == '*'){
		printf("mul");
	}
	if(expr->expr.binary_expr.operation == '/'){
		printf("sdiv");
	}
	if(expr->expr.binary_expr.operation == '%'){
		printf("srem");
	}*/
	//when ending resolve, \n
}
void visit_file (AST *root) {
	//////printm(">>> file\n");
	//////printm("file has %d declarations\n", root->list.num_items);

	for (ListNode *ptr = root->list.first; ptr != NULL; ptr = ptr->next) {
		switch (ptr->ast->decl.type) {
		case FUNCTION_DECLARATION:
			visit_function_decl(ptr->ast); break;
		case VARIABLE_DECLARATION:
			visit_var_decl(ptr->ast); break;
		default:
			fprintf(stderr, "UNKNOWN DECLARATION TYPE %c\n", ptr->ast->decl.type);
			break;
		}
	}
	// //////printm("<<< file\n");
}

void visit_function_decl (AST *ast) {
	//////printm(">>> function_decl\n");
	AST *params = ast->decl.function.param_decl_list;
	type_func = ast->decl.function.type == 1 ? "void" : "int";
	printf("define dso_local %s @%s", type_func, ast->decl.function.id->id.string);
	printf("(");
	if (params != NULL) {
		ListNode *ptr;
		AST *reg;
		int i = 0;
		for (ptr = params->list.first; ptr != NULL; ptr = ptr->next, i++) {
			if(i) printf(", ");
			if(ptr->ast->decl.variable.type == 2)
				printf("i32");
			//ptr->ast->decl.variable.id->id.type = LLIR_REGISTER; 
			ptr->ast->decl.variable.id->id.ssa_register = i;
			//ptr->ast->decl.variable.id->id.int_value = i+1;
			//params->id.ssa_register = i+1;
			//params->id.type = LLIR_REGISTER;
			//printf("registrador: %ld", ptr->ast->decl.variable.id->id.ssa_register);
			//////printm("  param");
		}
		//////printm("\n");
	}
	printf(")");
	printf(" local_unnamed_addr #0 ");
	if (ast->decl.function.stat_block != NULL) {
		visit_stat_block(ast->decl.function.stat_block, params, ast->decl.function.type);
	}
	// //////printm("<<< function_decl\n");
}

// what is surrounded by { }
ExprResult visit_stat_block (AST *stat_block, AST *params, int return_type) {
	//////printm(">>> stat_block\n");
	printf("{\n");
	ExprResult ret = { 0, TYPE_VOID };
	isStatBlock = 1;
	globalAddrC = params != NULL ? params->list.num_items : 0;
	for (ListNode *ptr = stat_block->list.first; ptr != NULL; ptr = ptr->next) {
		ret = visit_stat(ptr->ast);
	}
	char* typ = "void";
	if(type_func == typ)
		printf("\t ret void\n");
	printf("}\n");
	isStatBlock = 0;
	// //////printm("<<< stat_block\n");
	return  ret;
}


ExprResult visit_stat (AST *stat) {
	//////printm(">>> statement\n");
	ExprResult ret = { 0, TYPE_VOID };
	switch (stat->stat.type) {
	case VARIABLE_DECLARATION:
		visit_var_decl(stat); break;
	case ASSIGN_STATEMENT:
		visit_assign_stat(stat);
		//stat->id.ssa_register = globalAddrC; 
		break;
	case RETURN_STATEMENT:
		ret = visit_return_stat(stat); break;
	case EXPRESSION_STATEMENT:
		visit_expr(stat->stat.expr.expr); break;
		default: fprintf(stderr, "UNKNOWN STATEMENT TYPE %c\n", stat->stat.type); break;
	}
	return ret;
	// //////printm("<<< statement\n");
}

void visit_var_decl (AST *ast) {
	//////printm(">>> var_decl\n");
	AST *id = ast->decl.variable.id;

	if (ast->decl.variable.expr != NULL) {
		ExprResult expr = visit_expr(ast->decl.variable.expr);
			if(!isStatBlock) //print global variables
			printf("@%s = dso_local local_unnamed_addr global i32 %ld, allign 4\n", ast->decl.variable.id->id.string,
																	   ast->decl.variable.expr->expr.literal.int_value);
			else{
				//	AST *params = ast->decl.function.param_decl_list;
					// print_addr();
					//++globalAddrC;
					//id->id.type = LLIR_REGISTER;
					//id->id.ssa_register = globalAddrC - 1;
			}
	}
	else{
			if(!isStatBlock) //print global variables
			printf("@%s = dso_local local_unnamed_addr global i32 0, allign 4\n", ast->decl.variable.id->id.string);
	}
	// ////printm("<<< var_decl\n");
}


ExprResult visit_return_stat (AST *ast) {
	////printm(">>> return stat\n");
	ExprResult ret = { 0, TYPE_VOID };
	if (ast->stat.ret.expr) {
		ret = visit_expr(ast->stat.ret.expr);
	}
	char * empty = "";
	if(type_func == empty)
		printf("\t ret void %%%d\n", globalAddrC);
	else
		printf("\t ret i32 %%%d\n",	 globalAddrC);
	return ret;
	// ////printm("<<< return stat\n");
}

ExprResult visit_assign_stat (AST *assign) {
	////printm(">>> assign stat\n");
	ExprResult expr = visit_expr(assign->stat.assign.expr);
	expr.ssa_register = globalAddrC;
	assign->stat.assign.id->id.ssa_register = globalAddrC;
	//expr.type = LLIR_REGISTER;
	return expr;
	// ////printm("<<< assign stat\n");
}

ExprResult visit_expr (AST *expr) {
	////printm(">>> expression\n");
	ExprResult ret = {};
	switch (expr->expr.type) {
	case BINARY_EXPRESSION:
		switch (expr->expr.binary_expr.operation) {
		case '+':
			ret = visit_add(expr);
			break;
		case '-':
			ret = visit_sub(expr); 
			break;
		case '*':
			ret = visit_mul(expr); 
			break;
		case '/':
			ret = visit_div(expr); 
			break;
		case '%':
			ret = visit_mod(expr); 
			break;
		default:
			fprintf(stderr, "UNKNOWN OPERATOR %c\n", expr->expr.binary_expr.operation); break;
		}
		//printf("fim da linha\n");
		break;
	// ////printm("<<< expression\n");
	case UNARY_MINUS_EXPRESSION:
		ret = visit_unary_minus(expr);
		break;
	case LITERAL_EXPRESSION:
		ret = visit_literal(expr); break;
	case IDENTIFIER_EXPRESSION:
		ret = visit_id(expr->expr.id.id);
		break;
	case FUNCTION_CALL_EXPRESSION:
		ret = visit_function_call(expr); break;
	default:
		fprintf(stderr, "UNKNOWN EXPRESSION TYPE %c\n", expr->expr.type);
		break;
	}
	return ret;
}

// não implementar
ExprResult visit_function_call (AST *ast) {
	////printm(">>> function_call\n");
	ExprResult ret = {}, arg_expr[20]; //instead of alloca
	AST *arg_list = ast->expr.function_call.expr_list;

	if (arg_list != NULL) {
		int i = 0;
		for (ListNode *ptr = arg_list->list.first; ptr != NULL; ptr = ptr->next) {
			arg_expr[i++] = visit_expr(ptr->ast);
		}
	}
	// ////printm("<<< function_call\n");
	return ret;
}

ExprResult visit_id (AST *ast) {
	////printm(">>> identifier\n");
	ExprResult ret = {}; // armazenar aqui
	if (ast->id.type == TYPE_INT) {
		ret.int_value = ast->id.int_value;
		//ret.ssa_register = globalAddrC;
		ret.type = LLIR_REGISTER;
		ast->id.ssa_register = globalAddrC;
		//printf("%%%ld ", ast->id.ssa_register);
	} else if (ast->id.type == TYPE_FLOAT) {
		ret.float_value = ast->id.float_value;
		ret.type = TYPE_FLOAT;
		ast->id.ssa_register = globalAddrC;
	}
	// ////printm("<<< identifier\n");
	return ret;
}

ExprResult visit_literal (AST *ast) {
	////printm(">>> literal\n");
	ExprResult ret = {};
	ast->id.type = LITERAL_EXPRESSION;
	ast->id.int_value = ast->expr.literal.int_value;
	ret.type = LITERAL_EXPRESSION;
	ret.int_value = ast->expr.literal.int_value;
	// printf("meu literal: %ld\n", ast->expr.literal.int_value);
	// printf("%ld", ast->expr.literal.int_value);
	// ////printm("<<< literal\n");
	return ret;
}

ExprResult visit_unary_minus (AST *ast) {
	////printm(">>> unary_minus\n");
	ExprResult expr, ret = {};
	expr = visit_expr(ast->expr.unary_minus.expr);
	print_addr();
	expr.ssa_register = globalAddrC;
	expr.type = LLIR_REGISTER;
	printf("sub i32 0, %%%d\n", globalAddrC);
	// ////printm("<<< unary_minus\n");
	return expr;
}

ExprResult visit_add (AST *ast) {
	////printm(">>> add\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	print_addr();
	printf("add nsw i32 ");
	if(left.type == LLIR_REGISTER)
		printf("%%%ld, ", left.ssa_register);
	else
		printf("%ld, ", left.int_value);
	if(right.type == LLIR_REGISTER)
		printf("%%%ld\n", right.ssa_register);
	else
		printf("%ld\n", right.int_value);
	ret.ssa_register = globalAddrC;
	ret.type = LLIR_REGISTER;
	// ////printm("<<< add\n");
	return ret;
}

ExprResult visit_sub (AST *ast) {
	////printm(">>> sub\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	print_addr();
	printf("sub i32 ");
	if(left.type == LLIR_REGISTER)
		printf("%%%ld, ", left.ssa_register);
	else
		printf("%ld, ", left.int_value);
	if(right.type == LLIR_REGISTER)
		printf("%%%ld\n", right.ssa_register);
	else
		printf("%ld\n", right.int_value);
	ret.ssa_register = globalAddrC;
	ret.type = LLIR_REGISTER;
	// ////printm("<<< sub\n");
	return ret;
}

ExprResult visit_mul (AST *ast) {
	////printm(">>> mul\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	print_addr();
	printf("mul i32 ");
	if(left.type == LLIR_REGISTER)
		printf("%%%ld, ", left.ssa_register);
	else
		printf("%ld, ", left.int_value);
	if(right.type == LLIR_REGISTER)
		printf("%%%ld\n", right.ssa_register);
	else
		printf("%ld\n", right.int_value);
	ret.ssa_register = globalAddrC;
	ret.type = LLIR_REGISTER;
	// ////printm("<<< mul\n");
	return ret;
}

ExprResult visit_div (AST *ast) {
	////printm(">>> div\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	print_addr();
	printf("sdiv i32 ");
	if(left.type == LLIR_REGISTER)
		printf("%%%ld, ", left.ssa_register);
	else
		printf("%ld, ", left.int_value);
	if(right.type == LLIR_REGISTER)
		printf("%%%ld\n", right.ssa_register);
	else
		printf("%ld\n", right.int_value);
	ret.ssa_register = globalAddrC;
	ret.type = LLIR_REGISTER;
	// ////printm("<<< div\n");
	return ret;
}

ExprResult visit_mod (AST *ast) {
	////printm(">>> mod\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	print_addr();
	printf("srem i32 ");
	if(left.type == LLIR_REGISTER)
		printf("%%%ld, ", left.ssa_register);
	else
		printf("%ld, ", left.int_value);
	if(right.type == LLIR_REGISTER)
		printf("%%%ld\n", right.ssa_register);
	else
		printf("%ld\n", right.int_value);
	ret.ssa_register = globalAddrC;
	ret.type = LLIR_REGISTER;
	// //////printm("<<< mod\n");
	return ret;
}

