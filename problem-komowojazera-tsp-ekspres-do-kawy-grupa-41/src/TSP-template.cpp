//
// Created by Dominika on 01.11.2020.
// Edited by :
// Mikołaj Depta 401964
// Radosław Szpot 400188
//

#include "TSP.hpp"

#include <algorithm>
#include <stack>
#include <optional>

std::ostream& operator<<(std::ostream& os, const CostMatrix& cm) {
    for (std::size_t r = 0; r < cm.size(); ++r) {
        for (std::size_t c = 0; c < cm.size(); ++c) {
            const auto& elem = cm[r][c];
            os << (is_inf(elem) ? "INF" : std::to_string(elem)) << " ";
        }
        os << "\n";
    }
    os << std::endl;

    return os;
}

/* PART 1 */

/**
 * Create path from unsorted path and last 2x2 cost matrix.
 * @return The vector of consecutive vertex.
 */
path_t StageState::get_path() {
    vertex_t vertex;
    for (std::size_t it_row = 0; it_row < matrix_.size(); it_row++) {
        for (std::size_t it_col = 0; it_col < matrix_.size(); it_col++) {
            if (!is_inf(matrix_[it_row][it_col])) {
                vertex.col = it_col;
                vertex.row = it_row;
                update_lower_bound(matrix_[it_row][it_col]);
                append_to_path(vertex);
                matrix_[it_row][it_col] = INF;
                matrix_[it_col][it_row] = INF;
            }
        }
    }
    path_t path{0};
    std::size_t first = 0;
    for (std::size_t it = 0; it < matrix_.size(); it++){
        if (path.size() == matrix_.size()){ break; }
        for (const auto &vert: unsorted_path_) {
            if (vert.row == first) {
                path.push_back(vert.col);
                first = vert.col;
                if (path.size() == matrix_.size()){ break; }
            }
        }
    }

    return path;
}

/**
 * Get minimum values from each row and returns them.
 * @return Vector of minimum values in row.
 */
std::vector<cost_t> CostMatrix::get_min_values_in_rows() const {
    std::vector<cost_t> vec_min_values;
    for (const auto&  row: matrix_) {
        auto min = *std::min_element(row.begin(), row.end());
        if (!is_inf(min)) {
            vec_min_values.push_back(min);
        }
        else{
            vec_min_values.push_back(0);
        }
    }
    return vec_min_values;
}

/**
 * Reduce rows so that in each row at least one zero value is present.
 * @return Sum of values reduced in rows.
 */
cost_t CostMatrix::reduce_rows() {
    std::vector<cost_t> min_values = get_min_values_in_rows();
    for(std::size_t it_row = 0; it_row < matrix_.size(); it_row++){
        for(std::size_t it_col = 0; it_col < matrix_.size(); it_col++){
            if(!is_inf(matrix_[it_row][it_col])){
                matrix_[it_row][it_col] -= min_values[it_row];
            }
        }
    }
    for(std::size_t it = 0; it < matrix_.size(); it++){ matrix_[it][it] = INF; }
    return std::accumulate(min_values.begin(), min_values.end(), 0);
}

/**
 * Get minimum values from each column and returns them.
 * @return Vector of minimum values in columns.
 */
std::vector<cost_t> CostMatrix::get_min_values_in_cols() const {
    std::vector<cost_t> vec_min_values;
    std::vector<cost_t> col;
    for (std::size_t i = 0; i < matrix_.size(); i++) {
        col.clear();
        for (std::size_t j = 0; j < matrix_.size(); j++){
            col.push_back(matrix_[j][i]);
        }
        int min = *std::min_element(col.begin(), col.end());
        if (!is_inf(min)) {
            vec_min_values.push_back(min);
        }
        else{
            vec_min_values.push_back(0);
        }
    }
    return vec_min_values;
}

/**
 * Reduces rows so that in each column at least one zero value is present.
 * @return Sum of values reduced in columns.
 */
cost_t CostMatrix::reduce_cols() {
    std::vector<cost_t> min_values = get_min_values_in_cols();
    for(std::size_t it_cols = 0; it_cols < matrix_.size(); it_cols++){
        for(std::size_t it_value = 0; it_value < matrix_.size(); it_value++){
            if(!is_inf(matrix_[it_value][it_cols])){
                matrix_[it_value][it_cols] -= min_values[it_cols];
            }
        }
    }
    for(std::size_t it = 0; it < matrix_.size(); it++){ matrix_[it][it] = INF; }
    return std::accumulate(min_values.begin(), min_values.end(), 0);
}

/**
 * Get the cost of not visiting the vertex_t (@see: get_new_vertex())
 * @param row
 * @param col
 * @return The sum of minimal values in row and col, excluding the intersection value.
 */
cost_t CostMatrix::get_vertex_cost(std::size_t row, std::size_t col) const {
    std::vector<cost_t> reduced_row;
    for(std::size_t it = 0; it < matrix_.size(); it++){
        if(it != col) {
            if (!is_inf(matrix_[row][it])){
                reduced_row.push_back(matrix_[row][it]);
            }
        }
    }

    std::vector<cost_t> reduced_col;
    for(std::size_t it = 0; it < matrix_.size(); it++){
        if(it != row) {
            if( !is_inf(matrix_[it][col])){
                reduced_col.push_back(matrix_[it][col]);
            }
        }
    }
    cost_t min_row = *std::min_element(reduced_row.begin(), reduced_row.end());
    cost_t min_col = *std::min_element(reduced_col.begin(), reduced_col.end());
    return min_row + min_col;
}

/* PART 2 */

/**
 * Choose next vertex to visit:
 * - Look for vertex_t (pair row and column) with value 0 in the current cost matrix.
 * - Get the vertex_t cost (calls get_vertex_cost()).
 * - Choose the vertex_t with maximum cost and returns it.
 * @param cm
 * @return The coordinates of the next vertex.
 */
NewVertex StageState::choose_new_vertex() {
    std::vector< NewVertex> zero_vertex_cost;
    for(std::size_t it_row = 0; it_row < matrix_.size(); it_row++){
        for(std::size_t it_col = 0; it_col < matrix_.size(); it_col++){
            if(!matrix_[it_row][it_col]) {
                NewVertex curr_vertex(vertex_t(it_row, it_col), matrix_.get_vertex_cost(it_row, it_col));
                zero_vertex_cost.push_back(curr_vertex);
            }
        }
    }
    NewVertex max_v = NewVertex();
    for(const auto& elem: zero_vertex_cost){
        if (elem.cost > max_v.cost){ max_v = elem;}
    }
    return max_v;
}

/**
 * Update the cost matrix with the new vertex.
 * @param new_vertex
 */
void StageState::update_cost_matrix(vertex_t new_vertex) {
    // wykreslanie wiersza i klumny
    for(std::size_t it = 0; it < matrix_.size(); it++){
        matrix_[it][new_vertex.col] = INF;
        matrix_[new_vertex.row][it] = INF;
    }
    // definiowanie wektorów z już wybranymi wierzchołkami
    std::vector<std::size_t> vect_r;
    std::vector<std::size_t> vect_c;
    std::vector<std::size_t> chain_r;
    std::vector<std::size_t> chain_c;

    for(const auto& vertex: get_unsorted_path()){
        vect_r.push_back(vertex.row);
        vect_c.push_back(vertex.col);
    }

    for(std::size_t r = 0; r < vect_r.size(); r++){
        for(std::size_t c = 0; c < vect_c.size(); c++){
            if (vect_r[r] == vect_c[c]){
                matrix_[vect_c[r]][vect_r[c]] = INF;
                chain_r.push_back(vect_c[r]);
                chain_c.push_back(vect_r[c]);
            }
        }
    }
    // blokady wewnątrz łancuchów
    for(const auto& r: chain_r){
        for(const auto& c: chain_c){
            matrix_[r][c] = INF;
        }
    }

    // zabraniamy oczywistego przejscia powrotnego
    matrix_[new_vertex.col][new_vertex.row] = INF;
}

/**
 * Reduce the cost matrix.
 * @return The sum of reduced values.
 */
cost_t StageState::reduce_cost_matrix() {
    cost_t reduced_rows = matrix_.reduce_rows();
    cost_t reduced_cols = matrix_.reduce_cols();
    return reduced_cols + reduced_rows;
}



/**
 * Given the optimal path, return the optimal cost.
 * @param optimal_path
 * @param m
 * @return Cost of the path.
 */
cost_t get_optimal_cost(const path_t& optimal_path, const cost_matrix_t& m) {
    cost_t cost = 0;

    for (std::size_t idx = 1; idx < optimal_path.size(); ++idx) {
        cost += m[optimal_path[idx - 1]][optimal_path[idx]];
    }

    // Add the cost of returning from the last city to the initial one.
    cost += m[optimal_path[optimal_path.size() - 1]][optimal_path[0]];

    return cost;
}

/**
 * Create the right branch matrix with the chosen vertex forbidden and the new lower bound.
 * @param m
 * @param v
 * @param lb
 * @return New branch.
 */
StageState create_right_branch_matrix(cost_matrix_t m, vertex_t v, cost_t lb) {
    CostMatrix cm(m);
    cm[v.row][v.col] = INF;
    return StageState(cm, {}, lb);
}

/**
 * Retain only optimal ones (from all possible ones).
 * @param solutions
 * @return Vector of optimal solutions.
 */
tsp_solutions_t filter_solutions(tsp_solutions_t solutions) {
    cost_t optimal_cost = INF;
    for (const auto& s : solutions) {
        optimal_cost = (s.lower_bound < optimal_cost) ? s.lower_bound : optimal_cost;
    }

    tsp_solutions_t optimal_solutions;
    std::copy_if(solutions.begin(), solutions.end(),
                 std::back_inserter(optimal_solutions),
                 [&optimal_cost](const tsp_solution_t& s) { return s.lower_bound == optimal_cost; }
    );

    return optimal_solutions;
}

/**
 * Solve the TSP.
 * @param cm The cost matrix.
 * @return A list of optimal solutions.
 */
tsp_solutions_t solve_tsp(const cost_matrix_t& cm) {

    StageState left_branch(cm);

    // The branch & bound tree.
    std::stack<StageState> tree_lifo;

    // The number of levels determines the number of steps before obtaining
    // a 2x2 matrix.
    std::size_t n_levels = cm.size() - 2;

    tree_lifo.push(left_branch);   // Use the first cost matrix as the root.

    cost_t best_lb = INF;
    tsp_solutions_t solutions;

    while (!tree_lifo.empty()) {

        left_branch = tree_lifo.top();
        tree_lifo.pop();

        while (left_branch.get_level() != n_levels && left_branch.get_lower_bound() <= best_lb) {
            // Repeat until a 2x2 matrix is obtained or the lower bound is too high...

            if (left_branch.get_level() == 0) {
                left_branch.reset_lower_bound();
            }
            // 1. Reduce the matrix in rows and columns.
            cost_t new_cost = left_branch.reduce_cost_matrix();

            // 2. Update the lower bound and check the break condition.
            left_branch.update_lower_bound(new_cost);
            if (left_branch.get_lower_bound() > best_lb) {
                break;
            }


            // 3. Get new vertex and the cost of not choosing it.
            NewVertex new_vertex = left_branch.choose_new_vertex();

            // 4. Update the path - use append_to_path method.
            left_branch.append_to_path(new_vertex.coordinates);

            // 5. Update the cost matrix of the left branch.
            left_branch.update_cost_matrix(new_vertex.coordinates);

            // 6. Update the right branch and push it to the LIFO.
            cost_t new_lower_bound = left_branch.get_lower_bound() + new_vertex.cost;
            tree_lifo.push(create_right_branch_matrix(cm, new_vertex.coordinates,
                                                      new_lower_bound));
        }

        if (left_branch.get_lower_bound() <= best_lb) {
            // If the new solution is at least as good as the previous one,
            // save its lower bound and its path.
            best_lb = left_branch.get_lower_bound();
            path_t new_path = left_branch.get_path();
            path_t new_path_real;
            for (const auto &vec: new_path) {
                new_path_real.push_back(vec + 1);
            }
            solutions.push_back({get_optimal_cost(new_path, cm), new_path_real});
        }
    }

    return filter_solutions(solutions); // Filter solutions to find only optimal ones.
}
