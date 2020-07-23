/*
  main.cpp
  path_finder
  Created by Jim Li on 7/21/20.
  Copyright © 2020 Jim Li. All rights reserved.
  Xcode 11.6 (11E708)
 
 输入：
 line 1: 坐标数量n
 line 2~n+1: 坐标 x y
 
 输出：
 按顺序输出坐标
 
 第一个坐标视为起始点
*/

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <string.h>
#include <cmath>
#include <vector>

#define pow_2(x) ((x)*(x))

using namespace std;

int hole_num, iter;
double phero_0, mean, v_evap=1, beta = 6 , phi = 1;
vector<vector<double>> coord, dist, heru, phero;

struct ant{
    vector<int> path;
    vector<bool> traveled;
    double len;
    int not_traveled, current;
    
    void init(){
        path.assign(hole_num, 0);
        path[0]=0;
        traveled.assign(hole_num, 0);
        traveled[0]=1;
        not_traveled=hole_num-1;
        current=0;
        len=0;
    }
    
    void travel(int a){
        traveled[a]=1;
        current+=1;
        path[current]=a;
        not_traveled-=1;
        len+=dist[path[current-1]][path[current]];
    }
    
    void reset(){
        fill(path.begin(), path.end(), 0);
        path[0]=0;
        fill(traveled.begin(), traveled.end(), 0);
        traveled[0]=1;
        not_traveled=hole_num-1;
        current=0;
        len=0;
    }
    
    void addlen(double l){
        len+=l;
    }
} best_route;

void io(){
    cin>>hole_num;
    vector<double> a;
    a.assign(2, 0);
    coord.assign(hole_num, a);
    for(int i = 0; i < hole_num; i++){
        cin>>coord[i][0];
        cin>>coord[i][1];
    }
}
//计算每个点间的距离
void calc_distance(){
    vector<double> a;
    a.assign(hole_num, 0);
    dist.assign(hole_num, a);
    int total=0;
    for(int i = 0 ; i < hole_num ; i++){
        for(int j = i+1 ; j < hole_num ; j++){
            double n=sqrt(pow_2(coord[i][0]-coord[j][0])+pow_2(coord[i][1]-coord[j][1]));
            dist[i][j]=n;
            dist[j][i]=dist[i][j];
            total+=n;
        }
    }
    mean=total/(pow_2(hole_num)/2);
}
//信息素浓度赋初值
void assign_phero(){
    phero_0=1/(hole_num*mean);
    vector<double> a;
    a.assign(hole_num, phero_0);
    phero.assign(hole_num, a);
}
//计算每条路径的启发值
void assign_heru(){
    vector<double> a;
    a.assign(hole_num, 0);
    heru.assign(hole_num, a);
    for(int i = 0 ; i < hole_num ; i++){
        for(int j = i+1 ; j < hole_num; j ++){
            heru[i][j]=1/dist[i][j];
            heru[j][i]=heru[i][j];
        }
    }
}
//计算基础数据
void init(){
    calc_distance();
    assign_phero();
    assign_heru();
    iter=pow_2(hole_num);
}
//返回分母
double get_fractor(ant ant){
    double sum=0;
    for(int i = 0 ; i < hole_num; i++){
        double a=ant.traveled[i]?0:1;
        sum+=pow(heru[ant.path[ant.current]][i], beta)*pow(phero[ant.path[ant.current]][i], phi)*a;
    }
    return sum;
}
//计算一点所连接边的权重
double* update_prob(ant ant){
    double *prob_arr=new double[hole_num];
    double fractor=get_fractor(ant);
    for(int i = 0 ; i < hole_num ; i++){
        int a = ant.traveled[i]?0:1;
        prob_arr[i]=(pow(heru[ant.path[ant.current]][i], beta)*pow(phero[ant.path[ant.current]][i], phi))*a/fractor;
    }
    return prob_arr;
}
//根据权重随机
int select_rand(double prob[]){
    double rand_n=(double)rand()/(double)RAND_MAX, tmp=0;
    for(int i = 0 ; i < hole_num ; i++){
        tmp+=prob[i];
        if(tmp>=rand_n)
            return i;
    }
    return -1;
}
//建立路径
ant constr_path(ant ant){
    double* a=update_prob(ant);
    int nxt=select_rand(a);
    while(nxt!=-1){
        ant.travel(nxt);
        a=update_prob(ant);
        nxt=select_rand(a);
    }
    return ant;
}
//更新路径信息素值
void update_info(ant ants[]){
    best_route=ants[0];
    for(int i = 1 ; i < hole_num ; i++){
        if(ants[i].len<best_route.len)
            best_route=ants[i];
        ants[i].reset();
    }
    for(int i = 0 ; i < hole_num ; i++){
        for(int j = i+1 ; j < hole_num ; j++){
            phero[i][j]*=(1-v_evap);
            phero[j][i]=phero[i][j];
        }
    }
    for(int i = 1 ; i < hole_num ; i++){
        phero[best_route.path[i-1]][best_route.path[i]]+=1/best_route.len;
        phero[best_route.path[i]][best_route.path[i-1]]=phero[best_route.path[i-1]][best_route.path[i]];
    }
}

void solve(ant ants[]){
    for(int i = 0 ; i < iter ; i++){
        for(int j = 0 ; j < hole_num ; j++){
            ants[j]=constr_path(ants[j]);
        }
        update_info(ants);
    }
    cout<<best_route.path[0]+1;
    for(int i = 1 ; i < hole_num ; i++){
        cout<<"->"<<best_route.path[i]+1;
    }
}

int main(){
    io();
    init();
    ant ants[hole_num];
    for(int i = 0 ; i < hole_num ; i++)
        ants[i].init();
    solve(ants);
    return 0;
}
