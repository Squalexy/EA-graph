#include <bits/stdc++.h>
using namespace std;

int N, op_process_time, op_num_dependencies, num_operation, statistic, first_node, min_amount_time, best;

bool is_first_node;

map<int, pair<vector<int>, int>> graph;
vector<bool> visited;
vector<vector<int>> nodes;
vector<vector<int>> parent_nodes;
vector<int> nodes_order;
vector<int> dp;
vector<bool> is_on_stack;

// set<int>white;
// set<int>black;

int flag = 0;
int bottleneck_count = 0;
int num_last_nodes = 0;

bool check_last_node(){

    queue <int> Q; 
    Q.push(first_node);

    while (!Q.empty()){

        int node = Q.front();
        Q.pop();
        
        if (!nodes[node].empty()){
            for (auto next_node: nodes[node]){

                if (!visited[next_node] ) {
                    if (nodes[next_node].empty()) num_last_nodes++;
                    visited[next_node] = true;
                    Q.push(next_node);
                }

                if (num_last_nodes > 1) return false;
            }
        }
    }

    return true;
}

bool isCyclic(int node){

    if (!visited[node]){

        visited[node] = true;
        is_on_stack[node] = true;

        if(!nodes[node].empty()){
            for (auto next_node: nodes[node]){
                if (!visited[next_node] && isCyclic(next_node)) return true;
                else if (is_on_stack[next_node]) return true;
            }
        }
    }

    is_on_stack[node] = false;
    return false;
}

bool check_cycle(){

    /*
    white.erase(node);
    black.insert(node);

    if (!nodes[node].empty()){
        for (int next_node: nodes[node]){
            if (white.find(next_node) != white.end()) check_cycle(next_node);
            if (black.find(next_node) != black.end()) flag = 1;
        }
    }

    black.erase(node);
    */

   for (int i = 1; i <= N; i++){
       if (!visited[i] && isCyclic(i)) return false;
   }

   return true;
}

void statistic1(){

        nodes_order.push_back(first_node);
        visited[first_node] = true;
        min_amount_time += graph[first_node].second;

        priority_queue <int, vector<int>, greater<int>> pQ(nodes[first_node].begin(), nodes[first_node].end());

        while (!pQ.empty()){

            int node = pQ.top();

            if (parent_nodes[node].size() > 1){
                for (int parent: parent_nodes[node]){
                    if (!visited[parent]) {
                        visited[node] = false;
                        pQ.pop();
                        node = pQ.top();
                    }
                }
            }

            min_amount_time += graph[node].second;
            nodes_order.push_back(node);
            pQ.pop();

            if (!nodes[node].empty()){
                for (auto next_node: nodes[node]){
                    if (!visited[next_node]) {
                        pQ.push(next_node);
                        visited[next_node] = true;
                    }
                }
            }
        }

        cout << min_amount_time << endl;
        for (int node: nodes_order) cout << node << endl;
    
        return;
}

void statistic2(int node){

    visited[node] = true;
    dp[node] = graph[node].second;

    for (int next_node: nodes[node]){
        if (!visited[next_node])statistic2(next_node);
        dp[node] = max(dp[node], graph[node].second + dp[next_node]);
    }

    best = max(best, dp[node]);
    return;
}

void statistic3(){
    
    queue <int> Q;
    Q.push(first_node);
    visited[first_node] = true;

    while (!Q.empty()){

        /*
        cout << ":: ";
        queue <int> q2 = Q;
        while(!q2.empty()){
            cout << q2.front() << " <- ";
            q2.pop();
        }
        cout << ":: " << endl;
        */

        int node = Q.front();

        if (nodes[node].empty()) {
            nodes_order.push_back(node);
            return;
        }

        if (parent_nodes[node].size() > 1) {

            bottleneck_count -= parent_nodes[node].size();

            if (bottleneck_count != 0){

                int aux_flag = 0;

                if (nodes[node].size() > 0){
                    for (auto next_node: nodes[node]){
                        if (!visited[next_node]){
                            bottleneck_count += parent_nodes[node].size();
                            Q.push(node);
                            Q.pop();
                            flag = 1;
                            break;
                        }
                    }
                }

                if (aux_flag == 1) continue;
            }
        }

        if (bottleneck_count == 0 && Q.size() == 1) nodes_order.push_back(node);

        if (!nodes[node].empty()){
            if (nodes[node].size() > 1) bottleneck_count += nodes[node].size();
        }

        Q.pop();

        if (!nodes[node].empty()){
            for (auto next_node: nodes[node]){
                if (!visited[next_node]){
                    Q.push(next_node);
                    visited[next_node] = true;
                }
            }
        }
    }

    return;

}

int main(){

    cin >> N;

    is_first_node = false;
    visited = vector<bool>(N+1, false);
    nodes = vector<vector<int>>(N+1);
    parent_nodes = vector<vector<int>>(N+1);
    is_on_stack =  vector<bool>(N+1, false);
    
    // for (int i = 1; i <= N; i++) white.insert(i);

    for (int i = 1; i <= N; i++)
    {
        cin >> op_process_time >> op_num_dependencies;
        graph[i].second = op_process_time;

        // ------- INVALID OPERATION: more than 1 first node or node is disconnected ------- //
        if (op_num_dependencies == 0)
        {
            if (is_first_node)
            {
                // cout << "[ERROR] too many first nodes or node is disconnected" << endl;
                cout << "INVALID" << endl;
                return 0;
            }
            else {
                first_node = i;
                is_first_node = true;
            }
        }

        else if (op_num_dependencies != 0)
        {
            for (int j = 0; j < op_num_dependencies; j++)
            {
                cin >> num_operation;
                nodes[num_operation].push_back(i);
                graph[num_operation].first.push_back(i);
                graph[i].first.push_back(num_operation);
                parent_nodes[i].push_back(num_operation);
            }
        }
    }

    if (!check_last_node())
    {
        // cout << "[ERROR] too many last nodes" << endl;
        cout << "INVALID" << endl;
        return 0;
    }

    visited = vector<bool>(N+1, false);
    if (!check_cycle()){
        //cout << "[ERROR] cycle" << endl;
        cout << "INVALID" << endl;
        return 0;
    }

    else{

        cin >> statistic;

        if (statistic == 0) {
            cout << "VALID" << endl;
            return 0;
        }
        
        else {

            min_amount_time = 0;
            nodes_order = vector<int>();
            visited = vector<bool>(N+1, false);
            parent_nodes[first_node].push_back(0);

            if (statistic == 1) statistic1();

            else if (statistic == 2) {
                dp = vector<int>(N+1);
                statistic2(first_node);
                cout << best << endl;
                return 0;
            }

            else if (statistic == 3){
                statistic3();
                for (int bottleneck: nodes_order) cout << bottleneck << endl;
                return 0;
            }
        }
    }
        
    return 0;
}