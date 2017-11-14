//
// Created by squarefong on 17-11-13.
//

#ifndef GRAMMARANALYSISLR0_LR0_H
#define GRAMMARANALYSISLR0_LR0_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <iostream>
#include <queue>
#include <iomanip>
#include <stack>
#include <stdlib.h>
using namespace std;

template<typename T>
void cupEqual(set<T> &lhs, set<T> rhs){
    for(auto it = rhs.begin(); it != rhs.end(); ++it){
        lhs.insert(*it);
    }
}

class LR0{
    class state{
    public:
        //Vn -> p, i is the position
        vector<char> Vn;
        vector<string> p;
        vector<int> i;
        set<char> next;
        unsigned long size()const {
            return p.size();
        }
        state(){}
        state(char VN, const string &P, int I=0){
            Vn.push_back(VN);
            p.push_back(P);
            i.push_back(I);
            if(I<P.length())
                next.insert(P[I]);
        }
        void push(char VN, const string &P, int I=0){
            Vn.push_back(VN);
            p.push_back(P);
            i.push_back(I);
            if(I<P.length())
                next.insert(P[I]);
        }
        bool operator<(const state &rhs)const {
            return true;
        }
    };


    //开始字符
    string S;
    //终结符集
    set<char > V_T;
    //非终结符集
    set<string> V_N;
    //V*
    set<string> V;
    //产生式集
    map<string, vector<string>> P;
    vector<string> P_Order;
    vector<char> P_Order_for_V;

    //可空集合
    set<string> nullAble;

    //文法顺序
    vector<string> V_N_Order;
    map<string, int> V_N_Order_;
    //map<string, int> V_N_Order_WithP;


    //LR0分析表
    vector<vector<string>> sheet;
    vector<char > reduceSheet;
public:
    bool readFile(const string &FilePath){
        ifstream openFile(FilePath);
        if(openFile){
            char temp[256];
            bool isFirst(true);
            while (openFile.getline(temp,256)){
                string currentLine(temp);
                for(unsigned it = 1; it < currentLine.size(); ++it){
                    if( currentLine[it] == ' ')
                    {
                        string Vn=currentLine.substr(0,it);
                        string p=currentLine.substr(it+1,currentLine.size()-it-1);
                        if(p == "`") {
                            p = "";
                        }
                        if(p == "")
                            nullAble.insert(Vn);
                        else {
                            for (char i : p) {
                                if(i < 'A' || i > 'Z') {
                                    V_T.insert(i);
                                    string str;
                                    str += i;
                                    V.insert(str);
                                }
                            }
                        }
                        P_Order.push_back(p);
                        P_Order_for_V.push_back(Vn[0]);
                        if(V_N.find(Vn) != V_N.end()) {
                            P[Vn].push_back(p);
                        }
                        else {
                            vector<string> t(1,p);
                            P.insert(make_pair(Vn,t));
                            V_N_Order.push_back(Vn);
                            V_N_Order_.insert(make_pair(Vn,V_N_Order_.size()));
                        }
                        if(isFirst) {
                            isFirst = false;
                            S = Vn;
                        }
                        V_N.insert(Vn);
                        V.insert(Vn);
                        break;
                    }
                }
            }
            return true;
        } else {
            cout << "No such file: " << FilePath << endl;
            return false;
        }
    }

    void show(){
        cout << "S = " << S << endl;

        cout << "Vt : ";
        for(auto it = V_T.begin(); it != V_T.end(); ++it){
            cout << *it << ' ';
        }
        cout << endl;

        cout << "Vn : ";
        for(auto it = V_N_Order.begin(); it != V_N_Order.end(); ++it){
            cout << *it << ' ';
        }
        cout << endl;

        int i(0);
        for(auto it = V_N_Order.begin(); it != V_N_Order.end(); ++it){
            auto p = P[(*it)];
            cout << i++ << ": " << (*it) ;
            for(auto it2 = p.begin(); it2 != p.end(); ++it2) {
                if(it2 == p.begin())
                    cout << "  -> ";
                else
                    cout << i++ << ": " << "    | ";
                if ((*it2) == "") {
                    cout << endl;
                    continue;
                } else {
                    cout << (*it2);
                    cout << endl;
                }
            }
        }

        cout << "Nullable : ";
        for(auto it = nullAble.begin(); it != nullAble.end(); ++it){
            cout << *it << ' ';
        }

        cout << endl;
    }

    bool closure(state &p){
//        set<string> state;
//        state.insert(p);
        unsigned long n= 0;
        set<char> included;
        while (n != p.size()) {
            n= p.size();
            for (auto i = 0; i != p.size(); ++i){
                //若是非终结符，添加
                char next = p.p[i][p.i[i]];
                if((V_T.find(next) == V_T.end()) && (included.find(next) == included.end())) {
                    included.insert(next);
                    string Vn;
                    Vn += next;
                    auto pn = P[Vn];
                    for(unsigned long j(0); j < pn.size(); ++j){
                        p.push(next,pn[j],0);
                    }
                }
            }
        }
        return n != 0;
    }

    bool buildSheet(){
        reduceSheet = vector<char>(100,-1);
        //添加开始状态
        state start;
        string Vn;
        Vn += S;
        auto pn = P[Vn];
        for(unsigned long j(0); j < pn.size(); ++j){
            start.push(S[0],pn[j],0);
        }
        closure(start);
        //构造状态并遍历之
        queue<state> q;
        queue<int> order;
        q.push(start);


        //map<state,int> stateNumber;
        int n=0;
        //pair<state,int> pair1(start, n);
        //stateNumber.insert(make_pair(start, n++));
        order.push(n++);
        while(!q.empty()){
            state &tempState = q.front();
            vector<string> currentLine(128,"");
            //计算所有可能的下一个状态，并且把状态加入队列
            //若next为空，终结或规约
            if(tempState.next.begin() == tempState.next.end()) {
                if(tempState.Vn[0] == S[0]){

                    currentLine['$'] = "accept";
                } else {
                    string temp = "r";
                    auto currentP = tempState.p[0];
                    int num = 0;
                    while (num < P_Order.size()) {
                        if(P_Order[num] == currentP)
                            break;
                        ++num;
                    }
                    //reduceSheet[num] = tempState.Vn[0];
                    temp += intToStr(num);

                    for(int i(0); i<128; ++i){

                        currentLine[i] = temp;
                    }
                }
            }
            else{
                //便利所有可能的下一个字符
                for(auto it = tempState.next.begin(); it != tempState.next.end(); ++it) {
                    char character = (*it);
                    state nexts;
//                    //如果是终结符
//                    if (V_T.find(character) != V_T.end()) {
                        //寻找包含此终结符的产生式
                        for(int i(0); i < tempState.size(); ++i) {
                            //for(int j(0); j < tempState.p[i].size(); ++j){
                                if(tempState.p[i][tempState.i[i]] == character)
                                {
                                    nexts.push(tempState.Vn[i],tempState.p[i],tempState.i[i] + 1);
                                    //break;
                                }
                            //}
                        }
//                    }
//                    //如果是非终结符
//                    else {
//                        for (int i = 0; i < tempState.size(); ++i) {
//                            if (tempState.Vn[i] == character) {
//                                nexts.push(character, tempState.p[i], tempState.i[i] + 1);
//                            }
//                        }

                    closure(nexts);
                    q.push(nexts);
                    currentLine[character] = n;
                    order.push(n++);
                    //stateNumber.insert(make_pair(nexts, n++));

                }
            }
            sheet.push_back(currentLine);
            q.pop();
            order.pop();
        }
		return true;
    }

    string intToStr(int num){
        if(num == 0)
            return "0";
        string number("");
        while (num){
            char t = '0' + num%10;
            number = t + number;
            num /= 10;
        }

        return number;
    }

    void sheetShow(){

        cout << '\t';
        for(auto it = V_T.begin(); it != V_T.end(); ++it){
            cout <<  (*it) << '\t';
        }
        //cout << " |";
        for(auto it = V_N.begin(); it != V_N.end(); ++it){
            cout << *it << '\t';
        }
        cout << endl;
        for(int i(0); i<sheet.size(); ++i){
            cout << i << '\t' ;
            for(auto it = V_T.begin(); it != V_T.end(); ++it){
                cout << (sheet[i][(*it)].length() != 1 ? sheet[i][(*it)] : intToStr(sheet[i][(*it)][0])) << '\t';
            }
            //cout << " | ";
            for(auto it = V_N.begin(); it != V_N.end(); ++it){

                cout << (sheet[i][(*it)[0]].length() != 1 ? (" ") : intToStr(sheet[i][(*it)[0]][0])) << '\t';
            }
            cout << endl;
        }
    }

    bool LR0analyze(vector<char> tokens){
        stack<char> states;
        stack<int> stateNumber;
        states.push('#');
        stateNumber.push(0);
        int i(0);
        while (true) {
            char t = tokens[i];
            int s = stateNumber.top();
			//移进
            if(sheet[s][t].length() == 1){
                states.push(t);
                stateNumber.push(int(sheet[s][t][0]));
                if(tokens.size() - 1 > i)
                    ++i;
            }
			//规约
			else if (sheet[s][t][0] == 'r') {
                //按照第几条产生式规约
                int No_P = atoi(sheet[s][t].substr(1, sheet[s][t].length() - 1).c_str());
                //pop多少步
                int ri = P_Order[No_P].length();
                //规约后push的东西
                char Vn = P_Order_for_V[No_P];
                while (ri--){
                    stateNumber.pop();
                    states.pop();
                }
                states.push(Vn);
                s = stateNumber.top();
                stateNumber.push(int(sheet[s][Vn][0]));
            } else if (sheet[s][t] == "accept" && tokens.size() == i + 1){
                return true;
            } else
                return false;
        }
    }
};


#endif //GRAMMARANALYSISLR0_LR0_H
