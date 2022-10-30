#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"

static int lbl;

int ex(nodeType *p) {
    int lbl1 = 0;
    int lbl2 = 0;

    if (!p) return 0;
    switch(p->type) {
    case typeCon:
        printf("\tpushq\t$%d\n", p->con.value);
        break;
    case typeId:
        printf("\tpushq\t%c\n", p->id.i + 'a');
        break;
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:
            printf("L%03d:\n", lbl1 = lbl++);
            lbl2 = lbl;
            ex(p->opr.op[0]);
            ex(p->opr.op[1]);
            printf("\tjmp\t\tL%03d\n", lbl1);
            printf("L%03d:\n", lbl2);
            break;
        case IF:
            lbl1 = lbl;
            ex(p->opr.op[0]);
            if (p->opr.nops > 2) {
                /* if else */
                ex(p->opr.op[1]);
                printf("\tjmp\t\tL%03d\n", lbl2 = lbl++);
                printf("L%03d:\n", lbl1);
                ex(p->opr.op[2]);
                printf("L%03d:\n", lbl2);
            } else {
                /* if */
                ex(p->opr.op[1]);
                printf("L%03d:\n", lbl1);
            }
            break;
        case PRINT:
            ex(p->opr.op[0]);
            printf("\tpopq\t%%rsi\n");
            printf("\tmovq\t$fmt, %%rdi\n");
            printf("\tcall\tprintf\n");
            // TODO: syscall instead of printf for higher grade
            break;
        case '=':
            ex(p->opr.op[1]);
            printf("\tpopq\t%c\n", p->opr.op[0]->id.i + 'a');
            break;
        case UMINUS:
            ex(p->opr.op[0]);
            printf("\tpopq\t%%r8\n");
            printf("\tnegq\t%%r8\n");
            printf("\tpushq\t%%r8\n");
            break;
        case FACT:
            ex(p->opr.op[0]);
            printf("\tpopq\t%%rdi\n");
            printf("\tcall\tfact\n");
            printf("\tpushq\t%%rax\n");
            break;
        case LNTWO:
            ex(p->opr.op[0]);
            printf("\tpopq\t%%rdi\n");
            printf("\tcall\tlntwo\n");
            printf("\tpushq\t%%rax\n");
            break;
        default:
            ex(p->opr.op[0]);
            ex(p->opr.op[1]);
            switch(p->opr.oper) {
            case GCD:
                printf("\tpopq\t%%rsi\n");
                printf("\tpopq\t%%rdi\n");
                printf("\tcall\tgcd\n");
                printf("\tpushq\t%%rax\n");
                break;
            case '+':
                printf("\tpopq\t%%r9\n");
                printf("\tpopq\t%%r8\n");
                printf("\taddq\t%%r9, %%r8\n"); // r8 += r9
                printf("\tpushq\t%%r8\n");
                break;
            case '-':
                printf("\tpopq\t%%r9\n");
                printf("\tpopq\t%%r8\n");
                printf("\tsubq\t%%r9, %%r8\n"); // r8 -= r9
                printf("\tpushq\t%%r8\n");
                break;
            case '*':
                printf("\tpopq\t%%r9\n");
                printf("\tpopq\t%%r8\n");
                printf("\timulq\t%%r9, %%r8\n"); // r8 *= r9
                printf("\tpushq\t%%r8\n");
                break;
            case '/':
                printf("\txor\t\t%%rax, %%rax\n");
                printf("\txor\t\t%%rdx, %%rdx\n");
                printf("\tpopq\t%%r8\n");
                printf("\tpopq\t%%rax\n");
                printf("\tidiv\t%%r8\n");
                printf("\tpushq\t%%rax\n");
                break;

            // When checking logical conditions we want to
            // assert that e.g. 'LT' is true. If so, the following
            // part of the program runs as expected. If false
            // ('GE'), we want to jump. Therefore, we jump if the
            // "opposite" condition is true, meaning that
            // 'compLT' creates a 'jge' instruction, and so on.
            case '<':
                printf("\tpopq\t%%r9\n");       // b
                printf("\tpopq\t%%r8\n");       // a
                printf("\tcmpq\t%%r9, %%r8\n"); // cmpq b, a (a < b)
                printf("\tjge\t\tL%03d\n", lbl1 = lbl++);
                break;
            case '>':
                printf("\tpopq\t%%r9\n");       // b
                printf("\tpopq\t%%r8\n");       // a
                printf("\tcmpq\t%%r9, %%r8\n"); // cmpq b, a (a > b)
                printf("\tjle\t\tL%03d\n", lbl1 = lbl++);
                break;
            case GE:
                printf("\tpopq\t%%r9\n");       // b
                printf("\tpopq\t%%r8\n");       // a
                printf("\tcmpq\t%%r9, %%r8\n"); // cmpq b, a (a >= b)
                printf("\tjl\t\tL%03d\n", lbl1 = lbl++);
                break;
            case LE:
                printf("\tpopq\t%%r9\n");       // b
                printf("\tpopq\t%%r8\n");       // a
                printf("\tcmpq\t%%r9, %%r8\n"); // cmpq b, a (a <= b)
                printf("\tjg\t\tL%03d\n", lbl1 = lbl++);
                break;
            case NE:
                printf("\tpopq\t%%r9\n");       // b
                printf("\tpopq\t%%r8\n");       // a
                printf("\tcmpq\t%%r9, %%r8\n"); // cmpq b, a (a != b)
                printf("\tje\t\tL%03d\n", lbl1 = lbl++);
                break;
            case EQ:
                printf("\tpopq\t%%r9\n");       // b
                printf("\tpopq\t%%r8\n");       // a
                printf("\tcmpq\t%%r9, %%r8\n"); // cmpq b, a (a == b)
                printf("\tjne\t\tL%03d\n", lbl1 = lbl++);
                break;
            }
        }
    }
    return 0;
}
