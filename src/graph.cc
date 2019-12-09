/* source code courtesy of Frank Thomas Braun */

/* Generation of the graph of the syntax tree */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <functional>

#include "common.h"

const unordered_map<int, const char *> tokenToStr = {{'+', "[+]"},
                                                     {'-', "[-]"},
                                                     {'*', "[*]"},
                                                     {'/', "[/]"},
                                                     {'<', "[<]"},
                                                     {'>', "[>]"},
                                                     {'=', "[=]"},
                                                     {'%', "[%]"},
                                                     {token::GE, "[>=]"},
                                                     {token::LE, "[<=]"},
                                                     {token::EQ, "[==]"},
                                                     {token::NE, "[!=]"},
                                                     {token::WHILE, "while"},
                                                     {token::IF, "if"},
                                                     {token::PRINT, "print"},
                                                     {token::IFX, ""},
                                                     {token::ELSE, "else"},
                                                     {token::UMINUS, "[_]"}};

int del = 1; /* distance of graph columns */
int eps = 3; /* distance of graph lines */

/* interface for drawing (can be replaced by "real" graphic using GD or other)
 */
void graphInit(void);
void graphFinish();
void graphBox(char *s, int *w, int *h);
void graphDrawBox(char *s, int c, int l);
void graphDrawArrow(int c1, int l1, int c2, int l2);
void exNode(const nodeType &p, int c, int l, int *ce, int *cm);

/*****************************************************************************/
void before_processing() {}
void after_processing() {}
/* main entry point of the manipulation of the syntax tree */
void ex(nodeType &p) {
    int rte, rtm;

    graphInit();
    exNode(p, 0, 0, &rte, &rtm);
    graphFinish();
}

/*c----cm---ce---->                       drawing of leaf-nodes
 l leaf-info
 */

/*c---------------cm--------------ce----> drawing of non-leaf-nodes
 l            node-info
 *                |
 *    -------------     ...----
 *    |       |               |
 *    v       v               v
 * child1  child2  ...     child-n
 *        che     che             che
 *cs      cs      cs              cs
 *
 */

void exNode(const nodeType &p, int c, int l, /* start column and line of node */
            int *ce, int *cm /* resulting end column and mid of node */
) {
    int w, h;     /* node width and height */
    int cbar;     /* "real" start column of node (centred above subnodes) */
    int k;        /* child number */
    int che, chm; /* end column and mid of children */
    int cs;       /* start column of children */

    auto tag = visit(
        overloaded{[](const constantNode &conNode) -> string {
                       return visit(overloaded{[](const int32_t val) -> string {
                                                   stringstream ss;
                                                   ss << "c(" << val << ')';
                                                   return ss.str();
                                               },
                                               [](const double val) -> string {
                                                   stringstream ss;
                                                   ss << "c(" << val << ')';
                                                   return ss.str();
                                               }},
                                    conNode.innerValue);
                   },
                   [](const operatorNode &oprNode) -> string {
                       return tokenToStr.at(oprNode.operatorToken);
                   },
                   [](const symbolNode &symNode) -> string {
                       stringstream ss;
                       ss << "id(" << symNode.symbol << ')';
                       return ss.str();
                   },
                   [](const vector<unique_ptr<nodeType>> &nodes) -> string {
                       if (nodes.size() == 0) {
                           return "empty";
                       }
                       return "stmts";
                   }},
        p.innerNode);

    /* construct node text box */
    graphBox(tag.data(), &w, &h);
    cbar = c;
    *ce = c + w;
    *cm = c + w / 2;

    const auto leaves = visit(
        overloaded{
            [](const constantNode &conNode)
                -> vector<reference_wrapper<const nodeType>> {
                return vector<reference_wrapper<const nodeType>>();
            },
            [](const operatorNode &oprNode)
                -> vector<reference_wrapper<const nodeType>> {
                if (!oprNode.operands.has_value()) {
                    return vector<reference_wrapper<const nodeType>>();
                }
                return visit(
                    overloaded{
                        [](const unique_ptr<nodeType> &opr1)
                            -> vector<reference_wrapper<const nodeType>> {
                            const vector<reference_wrapper<const nodeType>>
                                ret = {*opr1};
                            return ret;
                        },
                        [](const pair<unique_ptr<nodeType>,
                                      unique_ptr<nodeType>> &opr)
                            -> vector<reference_wrapper<const nodeType>> {
                            const vector<reference_wrapper<const nodeType>>
                                ret = {*opr.first, *opr.second};
                            return ret;
                        },
                        [](const tuple<unique_ptr<nodeType>,
                                       unique_ptr<nodeType>,
                                       unique_ptr<nodeType>> &opr)
                            -> vector<reference_wrapper<const nodeType>> {
                            const auto &[opr1, opr2, opr3] = opr;
                            vector<reference_wrapper<const nodeType>> ret = {
                                *opr1, *opr2, *opr3};
                            return ret;
                        },
                    },
                    oprNode.operands.value());
            },
            [](const symbolNode &symNode)
                -> vector<reference_wrapper<const nodeType>> {
                return vector<reference_wrapper<const nodeType>>();
            },
            [](const vector<unique_ptr<nodeType>> &nodes)
                -> vector<reference_wrapper<const nodeType>> {
                vector<reference_wrapper<const nodeType>> ret;
                for (const auto &node : nodes) {
                    ret.push_back(*node);
                }
                return ret;
            }},
        p.innerNode);
    /* node is leaf */
    if (leaves.size() == 0) {
        graphDrawBox(tag.data(), cbar, l);
        return;
    }

    /* node has children */
    cs = c;
    for (const auto &leaf : leaves) {
        exNode(leaf, cs, l + h + eps, &che, &chm);
        cs = che;
    }

    /* total node width */
    if (w < che - c) {
        cbar += (che - c - w) / 2;
        *ce = che;
        *cm = (c + che) / 2;
    }

    /* draw node */
    graphDrawBox(tag.data(), cbar, l);

    /* draw arrows (not optimal: children are drawn a second time) */
    cs = c;
    for (const auto &leaf : leaves) {
        exNode(leaf, cs, l + h + eps, &che, &chm);
        graphDrawArrow(*cm, l + h, chm, l + h + eps - 1);
        cs = che;
    }
}

/* interface for drawing */

#define lmax 200
#define cmax 200

char graph[lmax][cmax]; /* array for ASCII-Graphic */
int graphNumber = 0;

void graphTest(int l, int c) {
    int ok;
    ok = 1;
    if (l < 0)
        ok = 0;
    if (l >= lmax)
        ok = 0;
    if (c < 0)
        ok = 0;
    if (c >= cmax)
        ok = 0;
    if (ok)
        return;
    printf("\n+++error: l=%d, c=%d not in drawing rectangle 0, 0 ... %d, %d", l,
           c, lmax, cmax);
    exit(1);
}

void graphInit(void) {
    int i, j;
    for (i = 0; i < lmax; i++) {
        for (j = 0; j < cmax; j++) {
            graph[i][j] = ' ';
        }
    }
}

void graphFinish() {
    int i, j;
    for (i = 0; i < lmax; i++) {
        for (j = cmax - 1; j > 0 && graph[i][j] == ' '; j--)
            ;
        graph[i][cmax - 1] = 0;
        if (j < cmax - 1)
            graph[i][j + 1] = 0;
        if (graph[i][j] == ' ')
            graph[i][j] = 0;
    }
    for (i = lmax - 1; i > 0 && graph[i][0] == 0; i--)
        ;
    printf("\n\nGraph %d:\n", graphNumber++);
    for (j = 0; j <= i; j++)
        printf("\n%s", graph[j]);
    printf("\n");
}

void graphBox(char *s, int *w, int *h) {
    *w = strlen(s) + del;
    *h = 1;
}

void graphDrawBox(char *s, int c, int l) {
    int i;
    graphTest(l, c + strlen(s) - 1 + del);
    for (i = 0; i < strlen(s); i++) {
        graph[l][c + i + del] = s[i];
    }
}

void graphDrawArrow(int c1, int l1, int c2, int l2) {
    int m;
    graphTest(l1, c1);
    graphTest(l2, c2);
    m = (l1 + l2) / 2;
    while (l1 != m) {
        graph[l1][c1] = '|';
        if (l1 < l2)
            l1++;
        else
            l1--;
    }
    while (c1 != c2) {
        graph[l1][c1] = '-';
        if (c1 < c2)
            c1++;
        else
            c1--;
    }
    while (l1 != l2) {
        graph[l1][c1] = '|';
        if (l1 < l2)
            l1++;
        else
            l1--;
    }
    graph[l1][c1] = '|';
}
