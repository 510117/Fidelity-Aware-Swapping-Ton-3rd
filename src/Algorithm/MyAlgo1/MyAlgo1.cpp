#include "MyAlgo1.h"

MyAlgo1::MyAlgo1(Graph graph, vector<pair<int, int>> requests, map<SDpair, vector<Path>> paths):
    AlgorithmBase(graph, requests, paths) {
    algorithm_name = "MyAlgo1";

}

void MyAlgo1::variable_initialize() {
    // m = i + vt
    // x(i, m) = 0
    // delta = (1 + eps)((1 + eps)m)^(-1/eps)
    // alpha(i) = delta 
    // beta(v, t) = delta / C(v)

    epsilon = (0.9);
    double m = requests.size() + (double)graph.get_num_nodes() * (double)graph.get_time_limit();
    double delta = (1 + epsilon) * (1.0 / pow((1 + epsilon) * m, 1.0 / epsilon));
    obj = m * delta;
    // cerr << "[MyAlgo1] delta = " << delta << endl;
    x.clear();
    alpha.clear();
    beta.clear();
    x.resize(requests.size());
    alpha.resize(requests.size(), delta);
    beta.resize(graph.get_num_nodes(), vector<double>(graph.get_time_limit()));

    for(int i = 0; i < graph.get_num_nodes(); i++) {
        for(int t = 0; t < graph.get_time_limit(); t++) {
            if(graph.get_node_memory_at(i, t) == 0) beta[i][t] = INF;
            else beta[i][t] = delta / graph.get_node_memory_at(i, t);
        }
    }
}

Shape_vector MyAlgo1::separation_oracle() {
    Shape_vector min_shape;
    double min_value = INF;
    for(int i = 0; i < (int)requests.size(); i++) {
        int src = requests[i].first, dst = requests[i].second;
        // cerr << "[MyAlgo1] " << "path len = " << graph.get_path(src, dst).size() << endl;
        auto result = find_min_shape(src, dst, alpha[i]);
        Shape_vector shape = result.first;
        double value = result.second;
        if(value < min_value) {
            min_shape = shape;
            min_value = value;
        }
        // cerr << "[MyAlgo1] " << "find shape" << endl;
    }
    return min_shape;
}
pair<Shape_vector, double> MyAlgo1::find_min_shape(int src, int dst, double alp) {
    vector<Path> paths = get_paths(src, dst);
    
    Shape_vector best_shape;
    double best_cost = INF;
    for(Path path : paths) {
        dp.clear();
        dp.resize(path.size());
        par.clear();
        par.resize(path.size());
        caled.clear();
        caled.resize(path.size());
        for(int i = 0; i < (int)path.size(); i++) {
            dp[i].resize(path.size());
            par[i].resize(path.size());
            caled[i].resize(path.size());
            for(int j = 0; j < (int)path.size(); j++) {
                dp[i][j].resize(time_limit);
                par[i][j].resize(time_limit, -2);
                caled[i][j].resize(time_limit, false);
            }
        }

        double best = INF;
        int best_time = -1;
        for(int t = 0; t < time_limit; t++) {
            double result = recursion_calculate_min_shape(0, path.size() - 1, t, path);
            result = (result + alp) / graph.path_Pr(path);
            if(best > result) {
                best = result;
                best_time = t;
            }
        }

        if(best_time == -1) continue;

        if(best < best_cost) {
            best_shape = recursion_find_shape(0, (int)path.size() - 1, best_time, path);
            best_cost = best;
        }
    }

    if(best_cost == INF) return {{}, INF};
    return {best_shape, best_cost};
}
double MyAlgo1::recursion_calculate_min_shape(int left, int right, int t, vector<int> &path) {
    if(t <= 0) return INF;
    // cerr << left << " " << right << " " << t << " " << (int)path.size() << endl;

    int left_id = path[left], right_id = path[right];
    if(left == right - 1) {
        return beta[left_id][t - 1] + beta[left_id][t] + beta[right_id][t - 1] + beta[right_id][t];
    }

    if(caled[left][right][t]) return dp[left][right][t];
    
    double best = recursion_calculate_min_shape(left, right, t - 1, path);
    int best_k = -1;
    int best_dis_mid = (right - left + 1);
    for(int k = left + 1; k < right; k++) {
        double left_result = recursion_calculate_min_shape(left, k, t - 1, path);
        double right_result = recursion_calculate_min_shape(k, right, t - 1, path);
        double result = left_result + right_result;
        int dis_mid = abs((left + right) / 2 - k);
        if(result + EPS < best || (fabs(result - best) <= EPS && dis_mid < best_dis_mid)) {
            best = result;
            best_k = k;
            best_dis_mid = dis_mid;
        }
    }

    caled[left][right][t] = true;
    par[left][right][t] = best_k;
    return dp[left][right][t] = best + beta[left_id][t] + beta[right_id][t];
}
Shape_vector MyAlgo1::recursion_find_shape(int left, int right, int t, vector<int> &path) {
    int left_id = path[left], right_id = path[right], k = par[left][right][t];
    if(left == right - 1 && k == -2) {
        Shape_vector result;
        result.push_back({left_id, {{t - 1, t}}});
        result.push_back({right_id, {{t - 1, t}}});
        return result;    
    }

    if(k == -1) {
        Shape_vector last_time = recursion_find_shape(left, right, t - 1, path);
        if(DEBUG) {
            assert(last_time.front().first == left_id);
            assert(last_time.front().second[0].second == t - 1);
            assert(last_time.back().first == right_id);
            assert(last_time.back().second[0].second == t - 1);
        }
        last_time.front().second[0].second++;
        last_time.back().second[0].second++;
        return last_time;
    }

    assert(k >= 0);
    Shape_vector left_result, right_result, result;

    int k_id = path[k];
    left_result = recursion_find_shape(left, k, t - 1, path);
    right_result = recursion_find_shape(k, right, t - 1, path);

    if(DEBUG) {
        assert(left_result.front().first == left_id);
        assert(left_result.front().second[0].second == t - 1);
        assert(left_result.front().second.size() == 1);
        assert(left_result.back().first == k_id);
        assert(right_result.front().first == k_id);
        assert(right_result.back().first == right_id);
        assert(right_result.back().second[0].second == t - 1);
        assert(left_result.back().second.size() == 1);
    }

    for(int i = 0; i < (int)left_result.size(); i++) {
        result.push_back(left_result[i]);
    }

    result.back().second.push_back(right_result.front().second.front());

    for(int i = 1; i < (int)right_result.size(); i++) {
        result.push_back(right_result[i]);
    }

    result.front().second[0].second++;
    result.back().second[0].second++;
    return result;
}

void MyAlgo1::run() {

    // q = min (1, C(v) / theta(v, t)) for all v, t

    // primal:
    // x(i, m) = x(i, m) + q

    // dual:
    // alpha(v) = alpha(v)(1 + eps(q / ahpla(v))
    // beta(v, t) = beta(v, t)(1 + eps(q / beta(v, t))

    int round = 1;
    while(round-- && !requests.empty()) {
        variable_initialize();
        while(obj < 1.0) {
            // cerr << "obj = " << obj << endl;
            Shape_vector shape = separation_oracle();
            if(shape.empty()) break;
            double q = 1;
            for(int i = 0; i < (int)shape.size(); i++) {
                map<int, int> need_amount; // time to amount
                for(pair<int, int> rng : shape[i].second) {
                    int left = rng.first, right = rng.second;
                    for(int t = left; t <= right; t++) {
                        need_amount[t]++;
                    }
                }

                for(pair<int, int> P : need_amount) {
                    int t = P.first;
                    double theta = P.second;
                    q = min(q, graph.get_node_memory_at(i, t) / theta);
                }
            }
            if(q <= 1e-10) break;

            int request_index = -1;
            for(int i = 0; i < (int)requests.size(); i++) {
                if(requests[i] == make_pair(shape.front().first, shape.back().first)) {
                    if(request_index == -1 || alpha[request_index] > alpha[i]) {
                        request_index = i;
                    }
                }
            }

            x[request_index][shape] += q;
        
            double ori = alpha[request_index];
            alpha[request_index] = alpha[request_index] * (1 + epsilon * q);
            obj += (alpha[request_index] - ori);

            for(int i = 0; i < (int)shape.size(); i++) {
                map<int, int> need_amount; // time to amount
                for(pair<int, int> rng : shape[i].second) {
                    int left = rng.first, right = rng.second;
                    for(int t = left; t <= right; t++) {
                        need_amount[t]++;
                    }
                }

                for(pair<int, int> P : need_amount) {
                    int t = P.first;
                    int node_id = shape[i].first;
                    double theta = P.second;
                    double original = beta[node_id][t];
                    if(graph.get_node_memory_at(node_id, t) == 0) {
                        beta[node_id][t] = INF;
                    } else {
                        beta[node_id][t] = beta[node_id][t] * (1 + epsilon * (q / (graph.get_node_memory_at(node_id, t) / theta)));
                    }
                    obj += (beta[node_id][t] - original) * theta;
                }
            }

            // cerr << "[MyAlgo1] obj = " << obj << endl;
        }


        vector<pair<double, Shape_vector>> shapes;

        for(int i = 0; i < (int)requests.size(); i++) {
            for(auto P : x[i]) {
                shapes.push_back({P.second, P.first});
                // shapes.push_back({Shape(P.first).get_fidelity(A, B, n, T, tao), P.first});
            }
        }

        // sort(shapes.begin(), shapes.end(), [](pair<double, Shape_vector> left, pair<double, Shape_vector> right) {
        //     if(fabs(left.first - right.first) >= EPS) return left.first > right.first;
        //     if(left.second.size() != right.second.size()) return left.second.size() < right.second.size();
        //     return left.second < right.second;
        // });
        sort(shapes.begin(), shapes.end(), [](pair<double, Shape_vector> left, pair<double, Shape_vector> right) {
            return left.first > right.first;
        });
        // cerr << "[MyAlgo1] " << shapes.size() << endl;
        vector<bool> used(requests.size(), false);
        vector<int> finished;
        for(pair<double, Shape_vector> P : shapes) {
            Shape shape = Shape(P.second);
            int request_index = -1;
            for(int i = 0; i < (int)requests.size(); i++) {
                if(used[i] == false && requests[i] == make_pair(shape.get_node_mem_range().front().first, shape.get_node_mem_range().back().first)) {
                    request_index = i;
                }
            }

            if(request_index == -1 || used[request_index]) continue;
            if(graph.check_resource(shape)) {
                used[request_index] = true;
                // cerr << "[MyAlgo1] " << P.first << " " << P.second.size() << endl;
                graph.reserve_shape(shape);
                finished.push_back(request_index);
            }
        }

        sort(finished.rbegin(), finished.rend());
        for(auto fin : finished) {
            requests.erase(requests.begin() + fin);
        }
    }

    update_res();
    cerr << "[" << algorithm_name << "] end" << endl;
}