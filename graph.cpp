#include <bits/stdc++.h>
using namespace std;

int N, op_process_time, op_num_dependencies, num_operation, statistic, first_node, min_amount_time;

bool is_first_node;

map<int, pair<vector<int>, int>> graph;
vector<bool> visited;
vector<bool> visited_stat3;
vector<vector<int>> nodes;
vector<vector<int>> parent_nodes;
vector<int> nodes_order;
vector<int> dp;
vector<bool> is_on_stack;
vector<int> bottlenecks;
vector<int> bottleneck_count;

int flag = 0;
int num_last_nodes = 0;
int best = 0;
int node_bottleneck;

bool check_last_node(){

    queue <int> Q; 
    Q.push(first_node);

    while (!Q.empty()){

        int node = Q.front();
        Q.pop();
        
        if (!nodes[node].empty()){
            for (auto next_node: nodes[node]){

                if (!visited[next_node] ) {
                    // if the node hasn't been visited yet and doesn't have any following nodes, then it should be the last one
                    if (nodes[next_node].empty()) num_last_nodes++;
                    visited[next_node] = true;
                    Q.push(next_node);
                }

                // if there is more than 1 node that should be the last one, then it's impossible
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
            for (auto next_node: nodes[node]){
                if (!visited[next_node] && is_cyclic(next_node)) return true;
                // if the node already was on the stack, then it's pointing to a cycle
                else if (is_on_stack[next_node]) return true;
            }
        }
    }

    is_on_stack[node] = false;
    return false;
}

bool check_cycle(){

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

            int node = pQ.top();

            // flag to control if all dependencies have been solved
            int flagstat1 = 1;

            if (parent_nodes[node].size() > 1){
                for (int parent: parent_nodes[node]){
                    // if dependencies of actual node haven't been solved yet, I can't proceed with this one
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

            // if all dependencies have been solved, then everything proceeds as normal
            min_amount_time += graph[node].second;
            nodes_order.push_back(node);
            is_in_queue[node] = false;
            pQ.pop();

            if (!nodes[node].empty()){
                // if the next nodes haven't been visited yet, we're gonna put them in the queue
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

    // dp bottom-up approach

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

void search_parents(int node, int b_count){

    // recursive function looking every parent of every parent... of this node

    visited_stat3[node] = true;
    bottleneck_count[b_count] ++;

    for (auto parent: parent_nodes[node]){
        if (!visited_stat3[parent]) {
            search_parents(parent, b_count);
        }
    }
}

void search_children(int node, int b_count){

    // recursive function looking every child of every child... of this node

    visited_stat3[node] = true;
    bottleneck_count[b_count] ++;

    for (auto child: nodes[node]){
        if (!visited_stat3[child]) {
            search_children(child, b_count);
        }
    }

}

void statistic3(){

    // reusing statistic1 function
    // in statistic1 function we already control the right order of the bottlenecks

    vector <bool> is_in_queue(N+1);
    visited[first_node] = true;
    bottlenecks.push_back(first_node);

    if (!nodes[first_node].empty()){

        priority_queue <int, vector<int>, greater<int>> pQ(nodes[first_node].begin(), nodes[first_node].end());
        for (auto next_node: nodes[first_node]) {
            visited[next_node] = true;
            is_in_queue[next_node] = true;
        }

        while (!pQ.empty()){

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
            is_in_queue[node] = false;
            pQ.pop();

            //------------- stat3 algorithm here -------------//
            // verify if the node can access to every node in the graph
            // if it's possible to access all of them, then it's a bottleneck
            visited_stat3 = vector<bool>(N+1, false);
            visited_stat3[node] = true;

            for (auto parent: parent_nodes[node]){
                if (!visited_stat3[parent]) {
                    search_parents(parent, node);
                }
            }
            
            for (auto child: nodes[node]){
                if (!visited_stat3[child]) {
                    search_children(child, node);
                }
            }

            if (bottleneck_count[node] == N) bottlenecks.push_back(node);
            //------------------------------------------//

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
}

int main(){

    cin >> N;

    is_first_node = false;                      // checks if a node is disconnected or if there's more than one initial node
    visited = vector<bool>(N+1, false);         // list of nodes that have been visited
    nodes = vector<vector<int>>(N+1);           // list of child nodes of every node
    parent_nodes = vector<vector<int>>(N+1);    // list of parent nodes of every node
    is_on_stack =  vector<bool>(N+1, false);    // used to check if there's a cycle

    for (int i = 1; i <= N; i++)
    {
        cin >> op_process_time >> op_num_dependencies;
        graph[i].second = op_process_time;

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
                parent_nodes[i].push_back(num_operation);

                graph[num_operation].first.push_back(i);
                graph[i].first.push_back(num_operation);
            }
        }
    }

    if (!check_last_node())
    {
        cout << "INVALID" << endl;
        return 0;
    }

    visited = vector<bool>(N+1, false);
    if (!check_cycle()){
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
            
            if (statistic == 1) {
                parent_nodes[first_node].push_back(0);
                statistic1();
            }

            else if (statistic == 2) {
                dp = vector<int>(N+1);
                statistic2(first_node);
                cout << best << endl;
                return 0;
            }

            else if (statistic == 3){
                bottlenecks = vector<int>();
                bottleneck_count = vector<int>(N+1, 1);
                statistic3();
                for (int bottleneck: bottlenecks) cout << bottleneck << endl;
                return 0;
            }
        }
    }
        
    return 0;
}