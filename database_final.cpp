#include <bits/stdc++.h>
using namespace std;
int M = 4;
unordered_map<string, long long>vis_ctr, q_ctr;

struct person{
    string name;
    int age;
    string country;

    person(string nname, int aage, string ccountry)
    {
        name = nname;
        age = aage;
        country = ccountry;
    }
};

template <typename key>
struct bpnode{
    bool is_leaf;
    vector<key>keys;
    vector<bpnode<key>*>children;
    vector<vector<person*>>data_ptr;
    bpnode<key>*next_leaf;

    bpnode(bool leaf){
        is_leaf = leaf;
        next_leaf = nullptr;
    }
};

template<typename key>
void clear_the_tree(bpnode<key>*st)
{
    if(st == nullptr) return;
    if(st->is_leaf == true) for(int i = 0; i < st->data_ptr.size(); i++) for(int j = 0; j < st->data_ptr[i].size(); j++) delete(st->data_ptr[i][j]);
    else for(int i = 0; i < st->children.size(); i++) clear_the_tree(st->children[i]);
    delete st;
}

template <typename key>
vector<person*> point_query(bpnode<key>*st, key target_key, long long &vis_ctr)
{
    if(st == nullptr) return {};
    vis_ctr++;
    if(st->is_leaf == true)
    {
        auto itr = lower_bound(st->keys.begin(), st->keys.end(), target_key);
        if(itr == st->keys.end() || *itr != target_key) return {};
        return st->data_ptr[itr-st->keys.begin()];
    }

    int ind = upper_bound(st->keys.begin(), st->keys.end(), target_key)-(st->keys).begin();
    if(ind == st->keys.size() && st->children.size() == st->keys.size()) return {}; 
    if(ind == st->keys.size()) return point_query((st->children)[ind], target_key, vis_ctr);
    return point_query((st->children)[ind], target_key, vis_ctr);
}

template <typename key>
bpnode<key>* range_query_assist(bpnode<key>* st, key target_key, long long &vis_ctr)
{
    vis_ctr++;
    if(st->is_leaf == true) return st;

    int ind = upper_bound(st->keys.begin(), st->keys.end(), target_key)-(st->keys).begin();
    if(ind == st->keys.size() && st->children.size() == st->keys.size()) return nullptr;
    return range_query_assist((st->children)[ind], target_key, vis_ctr);
}

template <typename key>
vector<vector<person*>> range_query(bpnode<key>*st, key minkey, key maxkey, long long &vis_ctr)
{
    if(st == nullptr) {vector<vector<person*>>tmp; return tmp;} 
    bpnode<key>* curr = range_query_assist(st, minkey, vis_ctr);
    if(curr == nullptr) return {};
    vector<vector<person*>>poss;
    int innode = lower_bound(curr->keys.begin(), curr->keys.end(), minkey)-curr->keys.begin();

    for(;;)
    {
        if(curr == nullptr) break;    
        while(innode < curr->keys.size() && curr->keys[innode] >= minkey && curr->keys[innode] <= maxkey)
        {
            vis_ctr++;
            poss.push_back(curr->data_ptr[innode]);
            innode++;
        }
        if(innode == curr->keys.size()){curr = curr->next_leaf; innode = 0;}
        if(curr == nullptr || curr->keys[innode] > maxkey) break;
    }
    return poss;
}

template <typename key>
pair<key, bpnode<key>*> insert_in_graph_assist(bpnode<key>* st, key givenkey, person* thisptr)
{
    if(st->is_leaf == true)
    {
        if(binary_search(st->keys.begin(), st->keys.end(), givenkey) == true){
            st->data_ptr[lower_bound(st->keys.begin(), st->keys.end(), givenkey)-st->keys.begin()].push_back(thisptr);
            return {key(), nullptr};
        }
        else if(st->keys.size() < M)
        {
            int ind = lower_bound(st->keys.begin(), st->keys.end(), givenkey)-st->keys.begin();
            st->keys.insert(ind+st->keys.begin(), givenkey);
            st->data_ptr.insert(ind+st->data_ptr.begin(), {thisptr});
            return {key(), nullptr};
        }
        else{
            vector<pair<key, vector<person*>>>tmp;
            for(int i = 0; i < st->keys.size(); i++) tmp.push_back({st->keys[i], st->data_ptr[i]});
            tmp.push_back({givenkey, {thisptr}});
            sort(tmp.begin(), tmp.end());

            bpnode<key>* anotherleaf = new bpnode<key>(true);
            for(int i = tmp.size()/2; i < tmp.size(); i++) {anotherleaf->keys.push_back(tmp[i].first); anotherleaf->data_ptr.push_back(tmp[i].second);}
            st->keys.clear();
            st->data_ptr.clear();
            for(int i = 0; i < tmp.size()/2; i++) {st->keys.push_back(tmp[i].first); st->data_ptr.push_back(tmp[i].second);}
            anotherleaf->next_leaf = st->next_leaf;
            st->next_leaf = anotherleaf;
            return {tmp[tmp.size()/2].first, anotherleaf};
        }
    }

    int ind = upper_bound(st->keys.begin(), st->keys.end(), givenkey)-st->keys.begin();
    pair<key, bpnode<key>*> retv = insert_in_graph_assist(st->children[ind], givenkey, thisptr);
    if(retv.second == nullptr) return retv;
    else if(st->keys.size() < M)
    {
        // int indtmp = lower_bound(st->keys.begin(), st->keys.end(), retv.first)-st->keys.begin();
        st->keys.insert(st->keys.begin()+ind, retv.first);
        st->children.insert(st->children.begin()+ind+1, retv.second);
        return {key(), nullptr};
    }
    vector<pair<key, bpnode<key>*>>tmp;
    tmp.push_back(retv);
    for(int i = 0; i < st->keys.size(); i++)tmp.push_back({st->keys[i], st->children[i+1]});
    sort(tmp.begin(), tmp.end());
    st->keys.clear();
    bpnode<key>*firstptr = st->children[0];
    st->children.clear();
    st->children.push_back(firstptr);
    for(int i = 0; i < tmp.size()/2; i++){st->keys.push_back(tmp[i].first); st->children.push_back(tmp[i].second);}

    bpnode<key>* anothernode = new bpnode<key>(false);
    anothernode->children.push_back(tmp[tmp.size()/2].second);
    for(int i = tmp.size()/2+1; i < tmp.size(); i++) {anothernode->keys.push_back(tmp[i].first); anothernode->children.push_back(tmp[i].second);}
    return {tmp[tmp.size()/2].first, anothernode};
}

template <typename key>
bpnode<key>* insert_in_graph(bpnode<key>*st, key givenkey, person thisperson)
{
    person* thisptr = new person(thisperson.name, thisperson.age, thisperson.country);
    if(st == nullptr){
        bpnode<key>* startleaf = new bpnode<key>(true);
        startleaf->keys.push_back(givenkey);
        startleaf->data_ptr.push_back({thisptr});
        return startleaf;
    }
    else{
        pair<key, bpnode<key>*> retv = insert_in_graph_assist(st, givenkey, thisptr);
        if(retv.second == nullptr) return st;
        bpnode<key>* newstart = new bpnode<key>(false);
        newstart->keys.push_back(retv.first);
        newstart->children.push_back(st);
        newstart->children.push_back(retv.second);
        return newstart;
    }
}

int tell_age(string &s)
{
    int ct = 0, p = 1;
    for(int i = s.size()-1; i >= 0; i--)
    {
        ct+=(p*(s[i]-'0'));
        p*=10;
    }
    return ct;
}

bpnode<string>* implement_name_tree(vector<person>&inp)
{
    bpnode<string>*starter = nullptr;
    for(int i = 0; i < inp.size(); i++) starter = insert_in_graph(starter, inp[i].name, inp[i]);
    return starter;
}

bpnode<int>* implement_age_tree(vector<person>&inp)
{
    bpnode<int>*starter = nullptr;
    for(int i = 0; i < inp.size(); i++) starter = insert_in_graph(starter, inp[i].age, inp[i]);
    return starter;
}

bpnode<string>* implement_country_tree(vector<person>&inp)
{
    bpnode<string>*starter = nullptr;
    for(int i = 0; i < inp.size(); i++) starter = insert_in_graph(starter, inp[i].country, inp[i]);
    return starter;
}

void clean(string &s)
{
    while((s[s.size()-1] == '\n' || s[s.size()-1] == '\r' || s[s.size()-1] == '\t' || s[s.size()-1] == ' ')){s.erase(s.end()-1);}
    while((s[0] == '\n' || s[0] == '\r' || s[0] == '\t' || s[0] == ' ')){s.erase(s.begin());}
}

vector<person*> point_query_intermediate_name(bpnode<string>* st, string from, vector<person*>&inp2)
{
    if(st != nullptr) return point_query(st, from, vis_ctr["name"]);
    vis_ctr["name"]+=inp2.size();
    vector<person*>ans;
    for(int i = 0; i < inp2.size(); i++) if(inp2[i]->name == from) ans.push_back(inp2[i]);
    return ans;
}

vector<person*> point_query_intermediate_age(bpnode<int>* st, int from, vector<person*>&inp2)
{
    if(st != nullptr) return point_query(st, from, vis_ctr["age"]);
    vis_ctr["age"]+=inp2.size();
    vector<person*>ans;
    for(int i = 0; i < inp2.size(); i++) if(inp2[i]->age == from) ans.push_back(inp2[i]);
    return ans;
}

vector<person*> point_query_intermediate_country(bpnode<string>* st, string from, vector<person*>&inp2)
{
    if(st != nullptr) return point_query(st, from, vis_ctr["country"]);
    vis_ctr["country"]+=inp2.size();
    vector<person*>ans;
    for(int i = 0; i < inp2.size(); i++) if(inp2[i]->country == from) ans.push_back(inp2[i]);
    return ans;
}

vector<person*> range_query2_name(string from, string to, vector<person*>&inp2)
{
    vector<person*>ans;
    vis_ctr["name"]+=inp2.size();
    for(int i = 0; i < inp2.size(); i++) if(inp2[i]->name >= from && inp2[i]->name <= to) ans.push_back(inp2[i]);
    return ans;
}

vector<person*> range_query2_age(int from, int to, vector<person*>&inp2)
{
    vis_ctr["age"]+=inp2.size();
    vector<person*>ans;
    for(int i = 0; i < inp2.size(); i++) if(inp2[i]->age >= from && inp2[i]->age <= to) ans.push_back(inp2[i]);
    return ans;
}

vector<person*> range_query2_country(string from, string to, vector<person*>&inp2)
{
    vis_ctr["country"]+=inp2.size();
    vector<person*>ans;
    for(int i = 0; i < inp2.size(); i++) if(inp2[i]->country >= from && inp2[i]->country <= to) ans.push_back(inp2[i]);
    return ans;
}

void perform_action(string &action, string &onpara, bpnode<string>*&name_tree, bpnode<int>*&age_tree, bpnode<string>*&country_tree, vector<person>&inp)
{
    if(action == "pass") return;
    if(action == "clear")
    {
        if(onpara == "name") {clear_the_tree(name_tree); name_tree = nullptr;}
        else if(onpara == "age") {clear_the_tree(age_tree); age_tree = nullptr;}
        else if(onpara == "country") {clear_the_tree(country_tree); country_tree = nullptr;}
        else cout << "Wrong parameter received" << endl;
    }
    else if(action =="create"){
        if(onpara == "name") name_tree = implement_name_tree(inp);
        else if(onpara == "age") age_tree = implement_age_tree(inp);
        else if(onpara == "country") country_tree = implement_country_tree(inp);
        else cout << "Wrong parameter received" << endl;
    }
    else cerr << "Wrong action received" << endl;
}

void api_call(bpnode<string>* &name_tree, bpnode<int>* &age_tree, bpnode<string>* &country_tree, int &batch_size_ctr, vector<person>&inp, int &ai_ctr)
{
    cout << "\n";
    cout << "name_vis: " << vis_ctr["name"] << " age_vis: " << vis_ctr["age"] << " country_vis: " << vis_ctr["country"] << "\n";
    cout << "name_q_ct: " << q_ctr["name"] << " age_q_ct: " << q_ctr["age"] << " country_q_ct: " << q_ctr["country"] << "\n";
    cout << "current graphs: ";
    if(name_tree !=  nullptr) cout << "name ";
    if(age_tree != nullptr) cout << "age ";
    if(country_tree != nullptr) cout << "country";
    cout << endl;
    cerr << "1:   " << "\n";//----------------------------------
    if(name_tree !=  nullptr) cerr << "name tree  ";
    if(age_tree != nullptr) cerr << "age tree  ";
    if(country_tree != nullptr) cerr << "country tree";
    cerr << "\n";
    string action1, onpara1, action2, onpara2;
    cin >> action1 >> onpara1 >> action2 >> onpara2;
    perform_action(action1, onpara1, name_tree, age_tree, country_tree, inp), perform_action(action2, onpara2, name_tree, age_tree, country_tree, inp);
    if(name_tree != nullptr && age_tree != nullptr && country_tree != nullptr){cout << "Made more than two B+ trees. Condition violated. Database is going to crash" << endl; exit(0);}
    batch_size_ctr = 0, vis_ctr["name"] = 0, vis_ctr["age"] = 0, vis_ctr["country"] = 0, q_ctr["name"] = 0, q_ctr["age"] = 0, q_ctr["country"] = 0;
    ai_ctr++;
    cerr << "2:   " << "\n"; //-------------------------------
    if(name_tree !=  nullptr) cerr << "name tree  ";
    if(age_tree != nullptr) cerr << "age tree  ";
    if(country_tree != nullptr) cerr << "country tree";
    cerr << endl;
}

int main(void)
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ifstream datacsv("data.csv");
    if(datacsv.is_open() == false)
    {
        cout << "Could not open the input data file" << endl;
        exit(0);
    }

    vector<person>inp;
    vector<person*>inp2;
    string line;
    while(getline(datacsv, line))
    {
        if(line == "")continue;
        person tmp("", 0, "");
        stringstream breaker(line);
        string tmpname, tmpc, tmpa;
        if(getline(breaker, tmpname, ',') && getline(breaker, tmpa, ',') && getline(breaker, tmpc, ','))
        {
            clean(tmpname), clean(tmpa), clean(tmpc);
            tmp.name = tmpname, tmp.age = tell_age(tmpa), tmp.country = tmpc;
        }
        inp.push_back(tmp);
        person* nnew = new person(tmp.name, tmp.age, tmp.country);
        inp2.push_back(nnew);
    }

    bpnode<string>* name_tree = nullptr;
    bpnode<int>* age_tree = nullptr;
    bpnode<string>* country_tree = nullptr;

    ifstream queries("queries.csv");
    if(queries.is_open() == false)
    {
        cout << "Could not open the query file" << endl;
        exit(0);
    }

    auto time_start = chrono::high_resolution_clock::now();
    //--------------------------------------Reading queries-----------------------------------
    int batch_size = 100, batch_size_ctr = 0, ai_ctr = 0;
    while(getline(queries, line))
    {
        if(batch_size_ctr == batch_size)
        {
            api_call(name_tree, age_tree, country_tree, batch_size_ctr, inp, ai_ctr);
        }
        if(line == "" || (line[0] != 'p' && line[0] != 'r')) continue;
        string typeofq, datatype, from, to;
        stringstream breaker(line);
        if(getline(breaker, typeofq, ',') && getline(breaker, datatype, ',') && getline(breaker, from, ','))
        {
            if(typeofq == "p")
            {
                vector<person*> retv;
                clean(from);
                if(datatype == "name") {q_ctr["name"]++; retv = point_query_intermediate_name(name_tree, from, inp2);}
                else if(datatype == "age") {q_ctr["age"]++; retv = point_query_intermediate_age(age_tree, tell_age(from), inp2);}
                else if(datatype == "country") {q_ctr["country"]++; retv = point_query_intermediate_country(country_tree, from, inp2);}
                else retv = {};

                // if(retv.size() == 0) cout << "Person not found" << "\n";
                // else for(int i = 0; i < retv.size(); i++) cout << "name: " << retv[i]->name << "  age: " << retv[i]->age << "  country: " << retv[i]->country << "\n";
                // cout << endl;
            }
            else if(typeofq == "r")
            {
                if(!getline(breaker, to, ',')) {cout << "Invalid range query" << endl; continue;}
                vector<vector<person*>>retv;
                vector<person*>retv2;
                clean(from), clean(to);
                if(datatype == "name") {q_ctr["name"]++; if(name_tree != nullptr) retv = range_query(name_tree, from, to, vis_ctr["name"]); else retv2 = range_query2_name(from, to, inp2);}
                else if(datatype == "age") {q_ctr["age"]++; if(age_tree != nullptr) retv = range_query(age_tree, tell_age(from), tell_age(to), vis_ctr["age"]); else retv2 = range_query2_age(tell_age(from), tell_age(to), inp2);}
                else if(datatype == "country") {q_ctr["country"]++; if(country_tree != nullptr) retv = range_query(country_tree, from, to, vis_ctr["country"]); else retv2 = range_query2_country(from, to, inp2);}
                else retv = {};

                // if(retv.size() == 0 && retv2.size() == 0) cout << "People not found" << "\n";
                // else if(retv.size() == 0) for(int i = 0; i < retv2.size(); i++) cout << "name: " << retv2[i]->name << "  age: " << retv2[i]->age << "  country: " << retv2[i]->country << "\n";                    
                // else for(int i = 0; i < retv.size(); i++) for(int j = 0; j < retv[i].size(); j++) cout << "name: " << retv[i][j]->name << "  age: " << retv[i][j]->age << "  country: " << retv[i][j]->country << "\n";
                // cout << endl;
            }
        }
        batch_size_ctr++;
    }
    
    if(name_tree !=  nullptr) clear_the_tree(name_tree);
    if(age_tree != nullptr) clear_the_tree(age_tree);
    if(country_tree != nullptr) clear_the_tree(country_tree);
    auto time_end = chrono::high_resolution_clock::now();
    for(int i = 0; i < inp2.size(); i++) delete inp2[i];
    cerr << "[C++ writing] api was called: " << ai_ctr  << " times "<< endl;
    cerr << "Program run time is: " << (chrono::duration_cast<chrono::milliseconds>(time_end-time_start)).count() << " milliseconds" << endl;
}