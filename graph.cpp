#include <bits/stdc++.h>
using namespace std;

int N, op_process_time, op_num_dependencies, num_operation, statistic, first_node, min_amount_time;

bool is_first_node;

map<int, pair<vector<int>, int>> graph;
vector<bool> visited;
vector<vector<int>> nodes;
vector<vector<int>> parent_nodes;
vector<int> nodes_order;
vector<int> dp;
vector<bool> is_on_stack;
vector<int> bottlenecks;

// set<int>white;
// set<int>black;

int flag = 0;
int bottleneck_count = 0;
int num_last_nodes = 0;
int best = 0;

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

bool is_cyclic(int node){

    if (!visited[node]){

        visited[node] = true;
        is_on_stack[node] = true;

        if(!nodes[node].empty()){
            for (auto neighbor: nodes[node]){
                if (!visited[neighbor] && is_cyclic(neighbor)) return true;
                else if (is_on_stack[neighbor]) return true;
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
       if (is_cyclic(i)) return false;
   }

   return true;
}

void statistic1(){

    vector <bool> is_in_queue(N+1);

    nodes_order.push_back(first_node);
    visited[first_node] = true;
    min_amount_time += graph[first_node].second;

    if (!nodes[first_node].empty()){

        priority_queue <int, vector<int>, greater<int>> pQ(nodes[first_node].begin(), nodes[first_node].end());
        for (auto next_node: nodes[first_node]) {
            visited[next_node] = true;
            is_in_queue[next_node] = true;
        }

        while (!pQ.empty()){

            priority_queue <int, vector<int>, greater<int>> Q = pQ;

            /*
            cout << ":: ";
            while (!Q.empty()){
                cout << Q.top() << " <- ";
                Q.pop();
            }
            cout << ":: " << endl;

            */

            int node = pQ.top();

            int flagstat1 = 1;

            if (parent_nodes[node].size() > 1){


                for (int parent: parent_nodes[node]){
                    if (!visited[parent] || is_in_queue[parent]) {
                        visited[node] = false;
                        is_in_queue[node] = false;
                        pQ.pop();
                        flagstat1 = 0;
                        break;
                    }
                }
            }

            if (flagstat1 == 0) continue;

            min_amount_time += graph[node].second;
            nodes_order.push_back(node);
            is_in_queue[node] = false;
            pQ.pop();

            if (!nodes[node].empty()){
                for (auto next_node: nodes[node]){
                    if (!visited[next_node]) {
                        pQ.push(next_node);
                        is_in_queue[next_node] = true;
                        visited[next_node] = true;
                    }
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
    

    if (!nodes[node].empty()){

        for (int next_node: nodes[node]){
            if (!visited[next_node])statistic2(next_node);
            dp[node] = max(dp[node], graph[node].second + dp[next_node]);
        }

    }
    
    best = max(best, dp[node]);
    return;
}

void statistic3(){

    vector <bool> is_in_queue(N+1);
    bottlenecks.push_back(first_node);
    visited[first_node] = true;

    if (!nodes[first_node].empty()){

        priority_queue <int, vector<int>, greater<int>> pQ(nodes[first_node].begin(), nodes[first_node].end());
        for (auto next_node: nodes[first_node]) {
            visited[next_node] = true;
            is_in_queue[next_node] = true;
            bottleneck_count++;
        }

        while (!pQ.empty()){

            
            priority_queue <int, vector<int>, greater<int>> Q = pQ;
            cout << ":: ";
            while (!Q.empty()){
                cout << Q.top() << " <- ";
                Q.pop();
            }
            cout << ":: " << endl;
            

            int node = pQ.top();
            int flagstat1 = 1;

            if (parent_nodes[node].size() > 1){

                for (int parent: parent_nodes[node]){
                    if (!visited[parent] || is_in_queue[parent]) {
                        visited[node] = false;
                        is_in_queue[node] = false;
                        pQ.pop();
                        flagstat1 = 0;
                        break;
                    }
                }
            }

            if (flagstat1 == 0) continue;

            for (int parent: parent_nodes[node]){
                if (visited[parent] && !is_in_queue[parent]) bottleneck_count --;
            }

            is_in_queue[node] = false;
            pQ.pop();

            cout << "[bottleneck count] " << bottleneck_count << endl;
            if (bottleneck_count == 0 && pQ.empty()) bottlenecks.push_back(node);

            if (!nodes[node].empty()){
                for (auto next_node: nodes[node]){
                    if (!visited[next_node]) {
                        pQ.push(next_node);
                        is_in_queue[next_node] = true;
                        visited[next_node] = true;
                    }
                    bottleneck_count++;
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
                bottlenecks = vector<int>();
                statistic3();

                for (int bottleneck: bottlenecks) cout << bottleneck << endl;
                return 0;
            }
        }
    }
        
    return 0;
}