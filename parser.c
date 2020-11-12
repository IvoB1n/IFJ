#include "scanner.h"
#include "symtable.h"
#include "error.h"

#include <stdbool.h>

void get_next_token(Token *token) {
	token = DLActive(token_list);
	DLSucc(token_list);
}

int return_types_rule(Token *token, FuncNodePtr func_node) {
	return 0;
}

int param_rule(Token *token, FuncNodePtr func_node) {
	return 0;
}

int statement_rule(Token *token) {
	return 0;
}

int func_def_rule(Token *token) {
	get_next_token(token);
	if (token->type == FUNC) {
		get_next_token(token);
		if (token->type == ID) {
			token->type = ID_FUNC;
			FuncNodePtr func_node = malloc(sizeof(struct func_node));
			if (!func_node) {
				return INTERNAL_ERROR;
			}
			func_node->name = token->data;
			get_next_token(token);
			if (token->type != ROUND_BR_L) {
				free(func_node);
				return SYNTAX_ERROR;
			}
			get_next_token(token);
			if (param_rule(token, func_node)) {
				free(func_node);
				return SYNTAX_ERROR;
			}
			get_next_token(token);
			if (token->type != ROUND_BR_R) {
				free(func_node);
				return SYNTAX_ERROR;
			}
			get_next_token(token);
			if (return_types_rule(token, func_node)) {
				free(func_node);
				return SYNTAX_ERROR;
			}
			get_next_token(token);
			if (token->type != CURLY_BR_L)  {
				free(func_node);
				return SYNTAX_ERROR;
			}
			get_next_token(token);
			if (token->type != END_OF_LINE) {
				free(func_node);
				return SYNTAX_ERROR;
			}
			DLInsertLast(func_list, func_node, FUNCTYPE);
			get_next_token(token);
			statement_rule(token);
			get_next_token(token);
			if (token->type != CURLY_BR_R) {
				return SYNTAX_ERROR;
			}
			get_next_token(token);
			if (token->type != END_OF_LINE && token_list->Act != token_list->Last) {
				return SYNTAX_ERROR;
			}
			return 0;
		}
	}
	return SYNTAX_ERROR;
}

int func_def_next_rule(Token *token) {
	if (token_list->Act == token_list->Last) {
		return 0;
	}
	if (!func_def_rule(token)) {
		if (!func_def_next_rule(token)) {
			return 0;
		}
	}
	return SYNTAX_ERROR;
}

int functions_rule(Token *token) {
	if (!func_def_rule(token)) {
		if (!func_def_next_rule(token) {
			return 0;
		}
	}
	return SYNTAX_ERROR;
}

int package_rule(Token *token) {
	get_next_token(token);
	if (token->type == PACKAGE) {
		get_next_token(token);
		if (!strcmp(token->data, "main") {
			return SYNTAX_ERROR;
		}
		get_next_token(token);
		if (token->type == END_OF_LINE) {
			return 0;
		}
	}
	return SYNTAX_ERROR;
}

int parse() {
	Token token;
	if (!package_rule(&token)) {
		if (!functions_rule(&token))
			return 0;
	}
	return SYNTAX_ERROR;
}