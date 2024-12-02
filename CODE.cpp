#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <list>
#include <string>
#include <deque>
#include <climits>
#include <chrono>  // Include the chrono library for timing
#include <cstring>
#include <cmath> // For A* algorithm heuristic
#include <ctime> // For time functionality
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

// Color codes
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"

// Function to set console color (Windows)
void setConsoleColor(int color)
{
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#endif
}

// Custom sleep function for cross-platform support
void sleepMilliseconds(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000); // Convert milliseconds to microseconds
#endif
}

// Loading screen animation
void loadingScreen()
{
    system("cls || clear");
    cout << CYAN << "\n\n\n\t\t\t WELCOME TO JAYPEE AIRLINES" << RESET << endl;
    cout << "\t\t\t";

    for (int i = 0; i < 20; i++)
    {
        cout << GREEN << "." << RESET;
        cout.flush();
        sleepMilliseconds(100); // Use custom sleep function
    }

    cout << endl;
    sleepMilliseconds(1000); // Sleep for 1 second
}

class Airport {
public:
    string name;

    Airport(string name) {
        this->name = name;
    }
};

typedef Airport* PAirport;
typedef pair<int, PAirport> myPair;

map<PAirport, int> dist;
map<PAirport, PAirport> parent;

class Flight {
public:
    PAirport origin;
    PAirport destination;
    int distance;

    Flight(PAirport origin, PAirport destination, int distance) {
        this->origin = origin;
        this->destination = destination;
        this->distance = distance;
    }
};

class Graph
{
    map<PAirport, vector<pair<PAirport, int>>> adj;
    void printAllUtils(PAirport src, PAirport dest, map<PAirport, bool> &visit, vector<string> &path, int &);

public:
    vector<PAirport> listOfAirports;
    vector<Flight> flights; // Store flights

    void addFlight(PAirport u, PAirport v, int w, bool bidirected);
    void shortestPath(PAirport s, PAirport d);
    void aStarPath(PAirport s, PAirport d); // New A* Algorithm function
    void addAirport(PAirport a);
    void viewMap();
    void PrintAllPaths(PAirport src, PAirport dest);
    PAirport SearchAirport(string abc);
    void viewFlights(); // View all flights
    void scheduleFlight(); // New feature to schedule a flight
    void cancelFlight(); // New feature to cancel a flight
};

void Graph::PrintAllPaths(PAirport src, PAirport dest)
{
    map<PAirport, bool> visit;
    vector<string> path(100);
    int path_index = 0;

    for (auto itr : listOfAirports)
    {
        visit[itr] = false;
    }
    printAllUtils(src, dest, visit, path, path_index);
}

void Graph::printAllUtils(PAirport src, PAirport dest, map<PAirport, bool> &visit, vector<string> &path, int &path_index)
{
    visit[src] = true;
    path[path_index] = src->name;
    path_index++;

    if (src->name.compare(dest->name) == 0)
    {
        for (int i = 0; i < path_index; i++)
            cout << path[i] << " -> ";
        cout << endl;
    }
    else
    {
        for (pair<PAirport, int> i : adj[src])
        {
            if (!visit[i.first])
            {
                printAllUtils(i.first, dest, visit, path, path_index);
            }
        }
    }
    path_index--;
    visit[src] = false;
}

PAirport Graph::SearchAirport(string abc)
{
    for (auto itr : listOfAirports)
    {
        if (itr->name.compare(abc) == 0)
        {
            return itr;
        }
    }
    return NULL;
}

void Graph::addFlight(PAirport u, PAirport v, int w, bool bidirected)
{
    if (bidirected) {
        adj[u].push_back(make_pair(v, w));
        adj[v].push_back(make_pair(u, w));
    } else {
        adj[u].push_back(make_pair(v, w));
    }

    // Store the flight information
    flights.push_back(Flight(u, v, w));
}

void Graph::addAirport(PAirport a) {
    listOfAirports.push_back(a);
}

bool doesPathExist(PAirport dest) {
    return dist[dest] != INT_MAX;
}

void Graph::shortestPath(PAirport src, PAirport dest)
{
    using namespace std::chrono; // For timing

    // Start measuring time
    auto start = high_resolution_clock::now();

    // Initialize distances to infinity
    for (auto v : listOfAirports) {
        dist[v] = INT_MAX;
    }

    priority_queue<myPair, vector<myPair>, greater<myPair>> pq;

    pq.push(make_pair(0, src));
    dist[src] = 0;
    parent[src] = nullptr;

    // Dijkstra's algorithm to find the shortest path
    while (!pq.empty())
    {
        PAirport u = pq.top().second;
        pq.pop();

        for (pair<PAirport, int> i : adj[u]) {
            PAirport v = i.first;
            int weight = i.second;

            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                pq.push(make_pair(dist[v], v));
                parent[v] = u;
            }
        }
    }

    // End measuring time
    auto stop = high_resolution_clock::now();

    // Calculate the duration the algorithm took to compute the shortest path
    auto duration = duration_cast<milliseconds>(stop - start);

    if (doesPathExist(dest)) {
        // Output the shortest distance
        cout << endl << "Shortest Flight Distance between " << src->name << " and " << dest->name << " is " << dist[dest] << " km." << endl;

        // Output the shortest route
        cout << "Shortest Route using Dijkstra's Algorithm is: ";
        
        // Reconstruct the path from the destination to the source
        vector<string> path;
        PAirport current = dest;

        while (current != nullptr) {
            path.push_back(current->name);  // Add the airport to the path
            current = parent[current];      // Move to the parent airport
        }

        // Print the path from source to destination (reverse the order)
        for (int i = path.size() - 1; i > 0; --i) {
            cout << path[i] << " -> ";  // Print all airports except the source
        }
        cout << path[0] << endl;  // Print the source airport last

        // Show the time it took to compute the shortest path
        cout << "Time taken to compute the shortest path: " << duration.count() << " milliseconds." << endl;
    } else {
        cout << "No Path Exists between " << src->name << " and " << dest->name << endl << endl;
    }
}



// A* algorithm heuristic function
int heuristic(PAirport a, PAirport b) {
    return abs(static_cast<int>(a->name.length()) - static_cast<int>(b->name.length()));
}

void Graph::aStarPath(PAirport src, PAirport dest)
{
    using namespace std::chrono; // For timing

    // Start measuring time
    auto start = high_resolution_clock::now();

    map<PAirport, int> g_score, f_score;
    for (auto v : listOfAirports) {
        g_score[v] = INT_MAX;
        f_score[v] = INT_MAX;
    }

    priority_queue<myPair, vector<myPair>, greater<myPair>> openSet;
    openSet.push(make_pair(0, src));
    g_score[src] = 0;
    f_score[src] = heuristic(src, dest);
    parent[src] = nullptr;

    // A* algorithm to find the optimal path
    while (!openSet.empty()) {
        PAirport current = openSet.top().second;
        openSet.pop();

        if (current == dest) {
            // End measuring time
            auto stop = high_resolution_clock::now();

            // Calculate the duration the algorithm took to compute the path
            auto duration = duration_cast<milliseconds>(stop - start);

            // Output the A* route
            cout << "\nA* Path from " << src->name << " to " << dest->name << " found with cost " << g_score[dest] << endl;
            cout << "A* Route is: ";

            // Reconstruct the path from destination to source
            vector<string> path;
            int totalWeight = 0;  // Variable to accumulate the total distance (weight)
            PAirport cur = dest;

            while (cur != nullptr) {
                path.push_back(cur->name);
                if (parent[cur] != nullptr) {
                    // Add the distance between the current airport and its parent
                    // Look up the distance from the adjacency list (adj)
                    for (const auto& pair : adj[parent[cur]]) {
                        if (pair.first == cur) {
                            totalWeight += pair.second;  // Add the weight (distance)
                            break;
                        }
                    }
                }
                cur = parent[cur];
            }

            // Print the path from source to destination (reverse the order)
            for (int i = path.size() - 1; i > 0; --i) {
                cout << path[i] << " -> ";  // Print all airports except the destination
            }
            cout << path[0] << endl;  // Print the source airport last

            // Show the total weight of the path
            cout << "Total Weight (Distance): " << totalWeight << " km" << endl;

            // Show the time it took to compute the path
            cout << "Time taken to compute the A* path: " << duration.count() << " milliseconds." << endl;

            return;
        }

        // Explore neighbors
        for (pair<PAirport, int> i : adj[current]) {
            PAirport neighbor = i.first;
            int tentative_gScore = g_score[current] + i.second;

            if (tentative_gScore < g_score[neighbor]) {
                parent[neighbor] = current;
                g_score[neighbor] = tentative_gScore;
                f_score[neighbor] = g_score[neighbor] + heuristic(neighbor, dest);
                openSet.push(make_pair(f_score[neighbor], neighbor));
            }
        }
    }

    cout << "No A* Path Exists between " << src->name << " and " << dest->name << endl;
}



void Graph::viewMap()
{
    for (auto itr : listOfAirports)
    {
        cout << "\n\n" << itr->name << "\n" << endl;
    }
}

void Graph::viewFlights() {
    cout << "\nList of Flights:\n";
    for (const auto& flight : flights) {
        cout << "Flight from " << flight.origin->name << " to " << flight.destination->name << " with distance " << flight.distance << " km\n";
    }
}

void Graph::scheduleFlight() {
    string originName, destName;
    int distance;
    cout << "Enter Origin Airport Name: ";
    cin >> originName;
    cout << "Enter Destination Airport Name: ";
    cin >> destName;
    cout << "Enter Distance: ";
    cin >> distance;

    PAirport origin = SearchAirport(originName);
    PAirport destination = SearchAirport(destName);
    if (origin && destination) {
        addFlight(origin, destination, distance, true);
        cout << "Flight scheduled successfully!\n";
    } else {
        cout << "Invalid airports entered!\n";
    }
}

void Graph::cancelFlight() {
    string originName, destName;
    cout << "Enter Origin Airport Name: ";
    cin >> originName;
    cout << "Enter Destination Airport Name: ";
    cin >> destName;

    for (auto it = flights.begin(); it != flights.end(); ++it) {
        if (it->origin->name == originName && it->destination->name == destName) {
            flights.erase(it);
            cout << "Flight cancelled successfully!\n";
            return;
        }
    }
    cout << "Flight not found!\n";
}

vector<PAirport>* getSourceAndDestinationAirports(Graph &g) {
    auto len = g.listOfAirports.size();
    bool validInput = false;
    int src = -1, dest = -1;

    while (!validInput) {
        cout << "List of Source Airports" << endl;

        for (int i = 0; i < g.listOfAirports.size(); i++) {
            cout << i + 1 << " " << g.listOfAirports[i]->name << endl;
        }
        cout << "Select Source Airport(1-" << len << ") : " << endl;
        cin >> src;

        cout << "List of Destination Airports" << endl;
        for (int i = 0; i < g.listOfAirports.size(); i++) {
            cout << i + 1 << " " << g.listOfAirports[i]->name << endl;
        }
        cout << "Select Destination Airport(1-" << len << ") : " << endl;
        cin >> dest;

        if (src == dest) {
            cout << "Source and Destination cannot be the same!" << endl;
            validInput = false;
            continue;
        }

        if (src >= 1 && src <= len && dest >= 1 && dest <= len) {
            validInput = true;
        }
    }

    PAirport srcAirport = g.listOfAirports[src - 1];
    PAirport destAirport = g.listOfAirports[dest - 1];

    vector<PAirport>* srcAndDest = new vector<PAirport>;
    srcAndDest->push_back(srcAirport);
    srcAndDest->push_back(destAirport);

    return srcAndDest;
}

int boxDelivering(vector<vector<int>>& boxes, int portsCount, int maxBoxes, int maxWeight) {
    deque<int> q;
    int n = boxes.size();
    vector<int> dp(n + 1);
    dp[n] = 0;
    int cur_box_count = 0;
    int sum_w = 0;
    int r = n - 1;

    cur_box_count += 1;
    sum_w += boxes[n - 1][1];
    dp[n - 1] = 2;

    for (int i = n - 2; i >= 0; --i) {
        auto& box = boxes[i];
        cur_box_count += 1;
        sum_w += box[1];

        if (i + 1 <= r && boxes[i][0] != boxes[i + 1][0]) {
            q.push_front(i);
        }
        while (cur_box_count > maxBoxes || sum_w > maxWeight) {
            cur_box_count -= 1;
            sum_w -= boxes[r][1];
            --r;
            if (!q.empty() && (q.back() + 1) > r) {
                q.pop_back();
            }
        }
        if (q.size() >= 1) {
            dp[i] = q.size() + 1 + dp[q.back() + 1];
        } else {
            dp[i] = 0x7fffffff;
        }
        dp[i] = min(dp[i], (int)q.size() + 2 + dp[r + 1]);
    }
    return dp[0];
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int dp[5001][5011];
int mn[5001];
int N;
vector<int> A;

int solve(int i, int mx) {
    if (i == N) return 0;
    if (dp[i][mx] >= 0) return dp[i][mx];
    int mi = mn[i], ret = 0;

    if (A[i] > A[mx]) // A[i] > Y
        ret = max(ret, solve(i + 1, mx));
    else if (A[i] > A[mi]) { // X < A[i] <= Y
        ret = max(ret, solve(i + 1, mx));
        ret = max(ret, 1 + solve(i + 1, i));
    } else { // A[i] <= X
        ret = max(ret, 1 + solve(i + 1, mx));
        ret = max(ret, 1 + solve(i + 1, mi));
    }

    return dp[i][mx] = ret;
}

// Prints the items which are put in a knapsack of capacity W
void printknapSack(int W, int wt[], int n) {
    int i, w;
    int K[n + 1][W + 1];

    // Build table K[][] in a bottom-up manner
    for (i = 0; i <= n; i++) {
        for (w = 0; w <= W; w++) {
            if (i == 0 || w == 0)
                K[i][w] = 0;  // Base case: no weight or no items
            else if (wt[i - 1] <= w)
                K[i][w] = max(wt[i - 1] + K[i - 1][w - wt[i - 1]], K[i - 1][w]);
            else
                K[i][w] = K[i - 1][w];
        }
    }

    int res = K[n][W]; // Maximum weight that can be carried
    if (res > W) {
        cout << "Error: Bag limit exceeded!" << endl;
        return;
    }

    // Print the total weight that fits into the knapsack
    cout << "Total weight: " << res << endl;

    // Print the items selected
    w = W;
    cout << "Bags allowed:";
    for (i = n; i > 0 && res > 0; i--) {
        if (res == K[i - 1][w]) {
            // Item was not included
            continue;
        } else {
            // This item is included
            cout << " " << wt[i - 1];
            // Deduct the item's weight and value from the total
            res -= wt[i - 1];
            w -= wt[i - 1];
        }
    }
    cout << endl;
}

int main()
{
    loadingScreen(); // Show loading screen

    Graph g;

    PAirport a1 = new Airport("New Delhi");
    PAirport a2 = new Airport("Mumbai");
    PAirport a3 = new Airport("Lucknow");
    PAirport a4 = new Airport("Chennai");
    g.addAirport(a1);
    g.addAirport(a2);
    g.addAirport(a3);
    g.addAirport(a4);

    g.addFlight(a1, a2, 500, true);
    g.addFlight(a2, a3, 150, true);
    g.addFlight(a1, a3, 100, true);
    g.addFlight(a1, a4, 600, true);

    char choice = 'y';
    do
    {
        int ch;
        cout << BLUE
             << R"(
    +----------------------------------------+
    |         JAYPEE AIRLINES MENU           |
    +----------------------------------------+
    | 1. To Get shortest Flight Distance to  |
    |    your destination.                   |
    |                                        |
    | 2. To Add a new Airport.               |
    |                                        |
    | 3. To View all Routes.                 |
    |                                        |
    | 4. To Add a new Flight.                |
    |                                        |
    | 5. To View the Airport List.           |
    |                                        |
    | 6. To find the number of bags allowed  |
    |    with optimal weight.                |
    |                                        |
    | 7. To View all Flights.                |
    |                                        |
    | 8. To Schedule a Flight.               |
    |                                        |
    | 9. To Cancel a Flight.                 |
    |                                        |
    | 10. To Get shortest Flight Distance to |
    |    your destination.(A*)               |
    |                                        |
    | 11. To Exit.                           |
    +----------------------------------------+
    )" << RESET << endl;

    cout << GREEN << "Enter your choice: " << RESET;
        
        cin >> ch;
        if (ch == 11)
        {
            exit(0);
        }
        else
        {
            switch (ch)
            {
                case 1:
                {
                    system("cls || clear");
                    vector<PAirport>* srcAndDest = getSourceAndDestinationAirports(g);
                    PAirport srcAirport = (*srcAndDest)[0];
                    PAirport destAirport = (*srcAndDest)[1];
                    g.shortestPath(srcAirport, destAirport);
                    break;
                }
                case 2:
                {
                    system("cls || clear");
                    cout <<GREEN "\n\n Enter Name of the New Airport:\n" << endl;
                    string st;
                    cin >> st;
                    PAirport a5 = new Airport(st);
                    g.addAirport(a5);
                    dist[a5] = INT_MAX;
                    break;
                }
                case 3:
                {
                    system("cls || clear");
                    vector<PAirport>* srcAndDest = getSourceAndDestinationAirports(g);
                    PAirport srcAirport = (*srcAndDest)[0];
                    PAirport destAirport = (*srcAndDest)[1];

                    if (doesPathExist(destAirport)) {
                        g.PrintAllPaths(srcAirport, destAirport);
                    } else {
                        cout << "No Path Exists between " << srcAirport->name << " and " << destAirport->name << endl;
                    }

                    break;
                }
                case 4:
                {
                    system("cls || clear");
                    g.scheduleFlight(); // Call the function to schedule a flight
                    break;
                }
                case 5:
                {
                    system("cls || clear");
                    g.viewMap();
                    break;
                }
                /*case 6:
                {
                    system("cls || clear");
                    int num;
                    int n;
                    cout <<GREEN "Enter the number of flights:";
                    cin >> n;
                    cout << GREEN"Enter the flight number and the units of luggage:" << endl;
                    vector<vector<int>> boxes;
                    for (int i = 0; i < n; i++) {
                        vector<int> temp;
                        for (int j = 0; j < 2; j++) {
                            cin >> num;
                            temp.push_back(num);
                        }
                        boxes.push_back(temp);
                    }
                    int maxBoxes, maxWeight, portsCount;
                    cout << "maxBoxes:" << endl;
                    cin >> maxBoxes;
                    cout << "maxWeight:" << endl;
                    cin >> maxWeight;
                    cout << "portsCount:" << endl;
                    cin >> portsCount;
                    int result = boxDelivering(boxes, portsCount, maxBoxes, maxWeight);
                    cout << result;
                    break;
                }*/
                /*case 6:
                {
                    system("cls || clear");
                    cout <<GREEN "Enter the number of flights:";
                    cin >> N;
                    A.resize(N + 1);
                    for (int i = 0; i < N; i++) {
                        cout <<GREEN "Enter the speed of airplane" << " " << i + 1 << ":";
                        cin >> A[i];
                        A[N] = INT_MAX;
                    }
                    int m = INT_MAX, mi = -1;
                    for (int i = 0; i < N; i++) {
                        if (A[i] < m) m = A[i], mi = i;
                        mn[i] = mi;
                    }
                    memset(dp, -1, sizeof dp);
                    int res = 1 + solve(1, N);
                    cout << GREEN"No. of flights that need to be rescheduled are: " << N - res << endl;
                    break;
                }*/
                case 6:
                {
                    system("cls || clear");
                    int n;
                    cout << GREEN"Enter no. of items: ";
                    cin >> n;
                    cout << GREEN"Enter weights of " << n << " items: ";
                    int wt[n];
                    for (int i = 0; i < n; i++) {
                        cin >> wt[i];
                    }
                    int W = 30;
                    printknapSack(W, wt, n);
                    break;
                }
                case 7:
                {
                    system("cls || clear");
                    g.viewFlights(); // Call to view all flights
                    break;
                }
                case 8:
                {
                    system("cls || clear");
                    g.scheduleFlight(); // Schedule a flight
                    break;
                }
                case 9:
                {
                    system("cls || clear");
                    g.cancelFlight(); // Cancel a flight
                    break;
                }
                case 10:
                {
                    system("cls || clear");
                    vector<PAirport>* srcAndDest = getSourceAndDestinationAirports(g);
                    PAirport srcAirport = (*srcAndDest)[0];
                    PAirport destAirport = (*srcAndDest)[1];
                    g.aStarPath(srcAirport, destAirport);
                    break;
                }
            }
        }
        cout <<GREEN "\n\n\nDo you want to go to the main page or not?(Y/N)\n";
        cin >> choice;
        system("cls || clear");

    } while (choice == 'y' || choice == 'Y');

    for (auto &v : g.listOfAirports) {
        delete v;
    }

    return 0;
}
