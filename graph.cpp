#include <bits/stdc++.h>
using namespace std;

int N, op_process_time, op_num_dependencies, num_operation, statistic, first_node, min_amount_time, best;

bool is_first_node;

int num_last_nodes = 0;

map<int, pair<vector<int>, int>> graph;
vector<int> node_color;
vector<bool> visited;
vector<vector<int>> nodes;
vector<vector<int>> parent_nodes;
vector<int> bottlenecks;
vector<int> nodes_order;
vector<int> dependencies;
vector<int> dp;


bool check_cycle_and_last_node(int node){

    queue <int> Q; 

    node_color[node] = 1;
    Q.push(node);

    while (!Q.empty()){

        int t = Q.front();
        Q.pop();

        for (auto u: graph[t].first){

            // checks if there is more than 1 last node
            if (!visited[u] && nodes[u].empty()) num_last_nodes++;

            if (num_last_nodes > 1) return false;

            if (node_color[u] == 0){
                visited[u] = true;
                node_color[u] = 1 - node_color[t];
                Q.push(u);
            }

            // checks if there's a cycle
            if (node_color[u] == node_color[t]) return false;
        }
    }
    return true;
}

void statistic1(){

        nodes_order.push_back(first_node);
        min_amount_time += graph[first_node].second;

        priority_queue <int, vector<int>, greater<int>> pQ(nodes[first_node].begin(), nodes[first_node].end());
        for (int node: nodes[first_node]) visited[node] = true;

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

            for (auto next_node: nodes[node]){
                if (!visited[next_node]) {
                    pQ.push(next_node);
                    visited[next_node] = true;
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

        int node = Q.front();
        long unsigned int visited_parents = 0;
        int aux_node;
 
        if (parent_nodes[node].size() > 1){
            
            for (int parent: parent_nodes[node]){
                if (!visited[parent]) {
                    visited[node] = false;
                }

                else if (visited[parent]) visited_parents ++;
            }

            if (Q.size() == 1 && visited_parents == parent_nodes[node].size()) bottlenecks.push_back(node);
        }

        else if (parent_nodes[node].size() == 1){
            if (Q.size() == 1) bottlenecks.push_back(node);
        }

        if (visited[node] == false){
            aux_node = node;
            Q.pop();
            node = Q.front();
            Q.push(aux_node);
            continue;
        }

        else Q.pop();

        for (auto next_node: nodes[node]){
            if (!visited[next_node]) {
                Q.push(next_node);
                visited[next_node] = true;
            }
        }
    }
}

int main(){

    cin >> N;

    is_first_node = false;
    node_color = vector<int>(N+1, 0);
    visited = vector<bool>(N+1, false);
    nodes = vector<vector<int>>(N+1);
    parent_nodes = vector<vector<int>>(N+1);
    dependencies = vector<int>(N+1);

    for (int i = 1; i <= N; i++)
    {
        cin >> op_process_time >> op_num_dependencies;
        graph[i].second = op_process_time;
        dependencies[i] = op_num_dependencies;

        // ------- INVALID OPERATION: more than 1 first node or node is disconnected ------- //
        if (op_num_dependencies == 0)
        {
            if (is_first_node)
            {
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

    if (!check_cycle_and_last_node(first_node))
    {
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
            bottlenecks = vector<int>();
            nodes_order = vector<int>();
            visited = vector<bool>(N+1, false);
            dp = vector<int>(N+1);
            bottlenecks.push_back(first_node);

            if (statistic == 1) statistic1();

            else if (statistic == 2) {
                statistic2(first_node);
                cout << best << endl;
            }

            else if (statistic == 3){
                statistic3();
                for (int bottleneck: bottlenecks) cout << bottleneck << endl;
            }
        }
    }
        
    return 0;
}