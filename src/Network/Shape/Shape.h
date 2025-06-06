#ifndef __SHAPE_H
#define __SHAPE_H

#include "../../config.h"

using namespace std;
using Shape_vector = vector<pair<int, vector<pair<int, int>>>>;

class Shape {
    Shape_vector node_mem_range;    // {id, {{start, end}, ...}}
    double A, B, n, T, tao;
    void recursion_check(int left, int right);
    double recursion_get_fidelity(int left, int right, map<pair<int, int>, double> &F_init);
    double bar(double F);
    double Fswap(double Fa, double Fb);
    double t2F(double t);
    double F2t(double F);
    double pass_tao(double F);
public:
    Shape(Shape_vector _node_mem_range); 
    Shape();
    Shape_vector get_node_mem_range();
    double get_fidelity(double _A, double _B, double _n, double _T, double _tao, map<pair<int, int>, double> F_init);
    void check_valid(); // check the correctness of shape, regardless of resource and graph
    void print();
    bool operator< (Shape &ls);
    bool operator== (Shape &ls);
};

#endif