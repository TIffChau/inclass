/* parse.c - parsing and parse tree construction */

#include "ntcalc.h"

struct parse_node_st * parse_operand(struct parse_table_st *pt,
                                    struct scan_table_st *st);
struct parse_node_st * parse_expression(struct parse_table_st *pt,
                                        struct scan_table_st *st);

void parse_table_init(struct parse_table_st *pt) {
    pt->len = 0;
    pt->next = 0;
}

struct parse_node_st * parse_node_new(struct parse_table_st *pt) {
    struct parse_node_st *np;

    np = &(pt->table[pt->len]);
    pt->len += 1;

    return np;
}

void parse_error(char *err) {
    printf("parse_error: %s\n", err);
    exit(-1);
}

/*
 * Parse tree pretty printing
 */

char *parse_oper_strings[] = {"PLUS", "MINUS", "MULT", "DIV"};

void parse_tree_print_indent(int level) {
    level *= 2;
    for (int i = 0; i < level; i++) {
        printf(".");
    }
}

void parse_tree_print_expr(struct parse_node_st *np, int level) {
    parse_tree_print_indent(level);
    printf("EXPR ");

    if (np->type == EX_INTVAL) {
        printf("INTVAL %d\n", np->intval.value);
    } else if (np->type == EX_OPER2) {
        printf("OPER2 %s\n", parse_oper_strings[np->oper2.oper]);
        parse_tree_print_expr(np->oper2.left, level+1);
        parse_tree_print_expr(np->oper2.right, level+1);
    }
}

void parse_tree_print(struct parse_node_st *np) {
    parse_tree_print_expr(np, 0);
}

struct parse_node_st * parse_operand(struct parse_table_st *pt,
                                    struct scan_table_st *st) {
    struct scan_token_st *tp;
    struct parse_node_st *np, *np1;

    if (scan_table_accept(st, TK_INTLIT)) {
        tp = scan_table_get(st, -1);
        np = parse_node_new(pt);
        np->type = EX_INTVAL;
        np->intval.value = atoi(tp->value);
    } else {
       parse_error("Bad operand");
    }
    return np;
}

struct parse_node_st * parse_expression(struct parse_table_st *pt,
                                        struct scan_table_st *st) {
    struct scan_token_st *tp;
    struct parse_node_st *np, *np1;

    np = parse_operand(pt, st);
    tp = scan_table_get(st, 0);
    if (tp->id == TK_PLUS) {
        scan_table_accept(st, TK_ANY);
        np1 = parse_node_new(pt);
        np1->type = EX_OPER2;
        np1->oper2.oper = OP_PLUS;
        np1->oper2.left = np;
        np1->oper2.right = parse_operand(pt, st);
        np = np1;
    } else {
        parse_error("Invalid operator");
    }

    return np;
}
