#include <stdio.h>
#include "calc3.h"
#include "./lexyacc-code/y.tab.h"

static int lbl;

int ex(nodeType *p)
{
    int lbl1 = 0;
    int lbl2 = 0;

    if (!p)
        return 0;
    switch (p->type)
    {
        case typeCon:
            printf("\tpushq\t$%d\n", p->con.value);
            break;
        case typeId:
            printf("\tpushq\t%c\n", p->id.i + 'a');
            break;
        case typeOpr:
            switch (p->opr.oper)
            {
                case WHILE:
                    printf("L%03d:\n", lbl1 = lbl++);
                    ex(p->opr.op[0]);
                    printf("\tjz\tL%03d\n", lbl2 = lbl++);
                    ex(p->opr.op[1]);
                    printf("\tjmp\tL%03d\n", lbl1);
                    printf("L%03d:\n", lbl2);
                    break;
                case IF:
                    ex(p->opr.op[0]);
                    if (p->opr.nops > 2)
                    {
                        /* if else */
                        ex(p->opr.op[1]);
                        printf("\tjmp\tL%03d\n", lbl2 = lbl++);
                        printf("L%03d:\n", lbl1);
                        ex(p->opr.op[2]);
                        printf("L%03d:\n", lbl2);
                    }
                    else
                    {
                        /* if */
                        printf("\tjz\tL%03d\n", lbl1 = lbl++);
                        ex(p->opr.op[1]);
                        printf("L%03d:\n", lbl1);
                    }
                    break;
                case PRINT:
                    ex(p->opr.op[0]);
                    printf("\tcall\tprintf\n");
                    break;
                case '=':
                    ex(p->opr.op[1]);
                    printf("\tpopq\t%c\n", p->opr.op[0]->id.i + 'a');
                    break;
                case UMINUS:
                    ex(p->opr.op[0]);
                    printf("\tnegq\n");
                    break;
                case FACT:
                    ex(p->opr.op[0]);
                    printf("\tcall\tfact\n");
                    break;
                case LNTWO:
                    ex(p->opr.op[0]);
                    printf("\tcall\tlntwo\n");
                    break;
                default:
                    ex(p->opr.op[0]);
                    ex(p->opr.op[1]);
                    switch (p->opr.oper)
                    {
                        case GCD:
                            printf("\tcall\tgcd\n");
                            break;
                        case '+':
                            printf("\taddq\n"); // TODO: operands
                            break;
                        case '-':
                            // a = a - b
                            // movq b, %rdi
                            // subq %rdi, a
                            printf("\tmovq\t%c, %%rdi\n", p->opr.op[1]->id.i + 'a');
                            printf("\tsubq\t%%rdi, %c\n", p->opr.op[0]->id.i + 'a'); // TODO: operands
                            break;
                        case '*':
                            printf("\timulq\n"); // TODO: operands
                            break;
                        case '/':
                            printf("\tdivq\n"); // TODO: operands
                            break;
                        case '<': // TODO: cmpq & jlt?
                            printf("\tcmp\t%c, %c\n", p->opr.op[1]->id.i + 'a', p->opr.op[0]->id.i + 'a');
                            printf("\tjl\tL%03d\n", lbl1 = lbl++);
                            break;
                        case '>': // cmpq & jgt?
                            printf("\tcompGT\n");
                            printf("\tjg\tL%03d\n", lbl1 = lbl++);
                            break;
                        case GE: // cmpq & jge?
                            printf("\tcmp\t%c, %c\n", p->opr.op[1]->id.i + 'a', p->opr.op[0]->id.i + 'a');
                            printf("\tjge\tL%03d\n", lbl1 = lbl++);
                            break;
                        case LE: // cmpq & jle?
                            printf("\tcmp\t%c, %c\n", p->opr.op[1]->id.i + 'a', p->opr.op[0]->id.i + 'a');
                            printf("\tjle\tL%03d\n", lbl1 = lbl++);
                            break;
                        case NE: // cmpq & jne?
                            printf("\tmovq\t%c, %%rdi\n", p->opr.op[1]->id.i + 'a');
                            printf("\tsubq\t%%rdi, %c\n", p->opr.op[0]->id.i + 'a');
                            printf("\tjne\tL%03d\n", lbl1 = lbl++);
                            break;
                        case EQ: // cmpq & je?
                            printf("\tcmp\t%c, %c\n", p->opr.op[1]->id.i + 'a', p->opr.op[0]->id.i + 'a');
                            printf("\tje\tL%03d\n", lbl1 = lbl++);
                            break;
                    }
            }
    }
    return 0;
}

// TODO: handle generated push instructions
// generated code seem to always push variables before cmp/sub/call
// * cmp needs two arguments in instruction (cmpq s2,s1)
// * sub needs two arguments in instruction (subq S,D)
// * call needs arguments in registers (rdi, rsi, ...)

// * constant
//       - "push"
// * identifier
//       - "push"
// * operand
//       * WHILE
//       * IF
//       * PRINT
//       * FACT
//       * LNTWO
//       * UNARY MINUS
//       * ASSIGN
//       * default
//             * GCD
//             * SUB
//             * ADD
//             * MULT
//             * DIV
//             * LT
//             * LE
//             * GT
//             * GE
//             * NE
//             * EQ
