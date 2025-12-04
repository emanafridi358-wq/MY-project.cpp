#include <bits/stdc++.h>
using namespace std;

// ------------------- USER STRUCT -------------------
struct User {
    string username;
    string password;
    unordered_set<string> skills; // user skills
};

// ------------------- JOB STRUCT -------------------
struct Job {
    string title;
    unordered_map<string,double> required; // skill -> weight
};

// ------------------- SKILL GRAPH -------------------
class SkillGraph {
public:
    unordered_map<string, vector<string>> adj;

    void addEdge(const string &a, const string &b) {
        adj[a].push_back(b);
        if(!adj.count(b)) adj[b]; // ensure node exists
    }

    // BFS for shortest prerequisite path (cycle-safe)
    vector<string> getPath(const unordered_set<string> &userSkills, const string &target) {
        unordered_map<string, vector<string>> rev;

        // Reverse graph
        for(auto &p : adj) {
            for(auto &s : p.second) rev[s].push_back(p.first);
            if(!rev.count(p.first)) rev[p.first];
        }

        queue<string> q;
        unordered_set<string> visited;
        unordered_map<string,string> parent;

        q.push(target);
        visited.insert(target);

        string meet = "";
        bool found = false;

        while(!q.empty()) {
            string cur = q.front(); q.pop();

            if(userSkills.count(cur)) {
                meet = cur;
                found = true;
                break;
            }

            for(auto &pre : rev[cur]) {
                if(visited.count(pre)) continue; // skip already visited
                visited.insert(pre);
                parent[pre] = cur;
                q.push(pre);
            }
        }

        if(!found) return {}; // no path

        vector<string> path;
        string cur = meet;
        path.push_back(cur);
        while(cur != target) {
            cur = parent[cur];
            path.push_back(cur);
        }
        return path;
    }
};

// ------------------- MATCHER -------------------
class Matcher {
public:
    static double score(User &u, Job &j) {
        double total = 0, have = 0;
        for(auto &p : j.required) {
            total += p.second;
            if(u.skills.count(p.first)) have += p.second;
        }
        if(total == 0) return 0;
        return (have/total)*100;
    }

    static vector<pair<double, Job>> topJobs(User &u, vector<Job> &jobs) {
        priority_queue<pair<double,int>> pq;
        for(int i=0;i<jobs.size();i++)
            pq.push({score(u, jobs[i]), i});

        vector<pair<double, Job>> out;
        for(int i=0;i<3 && !pq.empty();i++) {
            auto x = pq.top(); pq.pop();
            out.push_back({x.first, jobs[x.second]});
        }
        return out;
    }
};

// ------------------- CAREER PORTAL -------------------
class CareerPortal {
public:
    vector<User> users;
    vector<Job> jobs;
    SkillGraph graph;

    void signup() {
        string u,p;
        cout << "Choose username: ";
        cin >> u;

        for(auto &x : users)
            if(x.username == u) {
                cout << "Username already exists!\n";
                return;
            }

        cout << "Choose password: ";
        cin >> p;

        User user;
        user.username = u;
        user.password = p;

        users.push_back(user);
        cout << "Signup successful!\n\n";
    }

    int login() {
        string u,p;
        cout << "Username: ";
        cin >> u;
        cout << "Password: ";
        cin >> p;

        for(int i=0;i<users.size();i++) {
            if(users[i].username == u && users[i].password == p) {
                cout << "\nLogin successful!\n\n";
                return i;
            }
        }
        cout << "\nInvalid login!\n\n";
        return -1;
    }

    void userMenu(int uid) {
        while(true) {
            cout << "\n===== USER MENU (" << users[uid].username << ") =====\n";
            cout << "1) View Profile\n";
            cout << "2) Add Skill\n";
            cout << "3) Get Job Recommendations\n";
            cout << "4) Learning Path\n";
            cout << "5) Logout\n";
            cout << "Choose: ";

            int op;
            cin >> op;

            if(op == 1) viewProfile(uid);
            else if(op == 2) addSkill(uid);
            else if(op == 3) recommend(uid);
            else if(op == 4) learning(uid);
            else if(op == 5) break;
            else cout << "Invalid!\n";
        }
    }

    void viewProfile(int id) {
        cout << "\nYour skills: ";
        for(auto &s : users[id].skills) cout << s << " ";
        cout << "\n";
    }

    void addSkill(int id) {
        string s;
        cout << "Enter skill to add: ";
        cin >> s;
        if(users[id].skills.count(s)) {
            cout << "Skill already exists!\n";
            return;
        }
        users[id].skills.insert(s);
        cout << "Skill added!\n";
    }

    void recommend(int id) {
        auto list = Matcher::topJobs(users[id], jobs);
        cout << "\n=== Top Jobs ===\n";
        for(auto &p : list) {
            cout << p.second.title << " → Score: "
                 << fixed << setprecision(2) << p.first << "%\n";
        }
    }

    void learning(int id) {
        string target;
        cout << "Enter target skill: ";
        cin >> target;

        auto path = graph.getPath(users[id].skills, target);

        if(path.empty()) {
            cout << "No learning path found.\n";
            return;
        }

        cout << "\nLearning Path:\n";
        for(int i=0;i<path.size();i++)
            cout << i+1 << ") " << path[i] << "\n";
    }
};

// ------------------- MAIN -------------------
int main() {
    CareerPortal sys;

    // Predefined jobs
    sys.jobs = {
        {"Software Engineer", {{"C++",0.4},{"DSA",0.4},{"Algorithms",0.2}}},
        {"Frontend Dev",     {{"HTML",0.3},{"CSS",0.2},{"JS",0.5}}},
        {"ML Engineer",      {{"Python",0.35},{"ML",0.4},{"Stats",0.25}}}
    };

    // Predefined skill graph
    sys.graph.addEdge("Arrays","DSA");
    sys.graph.addEdge("DSA","Algorithms");
    sys.graph.addEdge("HTML","Frontend");
    sys.graph.addEdge("CSS","Frontend");
    sys.graph.addEdge("JS","Frontend");
    sys.graph.addEdge("LinearAlgebra","ML");
    sys.graph.addEdge("Stats","ML");

    while(true) {
        cout << "\n===== CAREER PORTAL =====\n";
        cout << "1) Login\n";
        cout << "2) Signup\n";
        cout << "3) Exit\n";
        cout << "Choose: ";

        int op;
        cin >> op;

        if(op == 1) {
            int id = sys.login();
            if(id != -1) sys.userMenu(id);
        }
        else if(op == 2) sys.signup();
        else if(op == 3) break;
        else cout << "Invalid!\n";
    }

    cout << "Goodbye!\n";
    return 0;
}
