//
//  maxFlow.cpp
//  genic
//
//  Created by sanshanxiashi on 2017/3/23.
//  Copyright © 2017年 sanshanxiashi. All rights reserved.
//

#include "maxFlow.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <stack>
#include <map>
#include <queue>
#include <deque>
#include <stdio.h>
#include <cstring>
#include <cmath>
#include <ctime>
#include <stdlib.h>
#include <functional>
#define maxn 10000

using namespace std;


    
    //初始化
    void MCMF:: init(int n,int netNode,int consumeNode,int serverCost){
        this->n = n;
        this->netNode= netNode;
        this->consumeNode= consumeNode;
        this->serverCost= serverCost;
        this->numOfserver = 0;
        
        this->s= netNode + consumeNode;
        this->t= this->s + 1;
        
        for(int i=0; i<n; i++)  res[i].clear(), outFlow.clear() ;
        memset(isNetToCons, -1 ,sizeof(isNetToCons));
        
        size = 0;
        memset(net, -1, sizeof(net));
    }

    //加边
    void MCMF:: AddEdge(int from , int to, int cap , int cost) {
        E.push_back(EDGE(to, cap, cost, net[from] , 0));
        net[from] = size++;
        E.push_back(EDGE(from, 0, -cost, net[to] , 0));
        net[to] = size++;
    }


//调整
bool MCMF:: adjust()
{
    int v, min = INF;
    for (int i = 0; i < n; i++)
    {
        if (!vis[i])
            continue;
        for (int j = net[i]; v = E[j].v, j != -1; j = E[j].next)
            if (E[j].cap > E[j].flow)
                if (!vis[v] && dis[v] - dis[i] + E[j].cost < min)
                    min = dis[v] - dis[i] + E[j].cost;
    }
    if (min == INF)
        return false;
    for (int i = 0; i < n; i++)
        if (vis[i])
        {
            cur[i] = net[i];
            vis[i] = false;
            dis[i] += min;
        }
    return true;
}

//找增广路
int MCMF:: augment(int i, int flow)
{
    if (i == t)
    {
        mincost += dis[s] * flow;
        maxflow += flow;
        return flow;
    }
    vis[i] = true;
    
    for (int j = cur[i], v; v = E[j].v, j != -1; j = E[j].next)
        if (E[j].cap > E[j].flow){
            
            if (vis[v] || dis[v] + E[j].cost != dis[i]) continue;
            if(v==t){
                conflow[i-netNode]+= flow;
                //printf("i=%d , flow =%d\n", i-netNode, flow);
            }
            int delta = augment(v, std::min(flow, E[j].cap - E[j].flow));
            if (delta)
            {
                E[j].flow += delta;
                E[j ^ 1].flow -= delta;
                cur[i] = j;
                return delta;
            }
        }
    
    return 0;
}



    //最小费用流算法
    int MCMF:: zkw(int _s, int _t, int need)
    {
        s = _s, t = _t;
    
        mincost = maxflow = 0;
        for (int i = 0; i < n; i++){
            vis[i] = false;
            cur[i] = net[i];
        }
    
        do
        {
            while (augment(s, INF))
                memset(vis, false, sizeof(vis));
            
        } while (adjust());
        if (maxflow < need)
            return -1;
       // printf("Path cost =%d ,TotCost= %d\n", mincost , mincost+serverCost*numOfserver);
        mincost += serverCost*numOfserver;
        return mincost;
    }


    void MCMF:: OutputPath(string& str){
        char c[50];
        
       // printf("outCnt = %d\n\n", outCnt);
        sprintf(c, "%d\n\n", outCnt);
        str += c;
        
        for(int i=0; i< outCnt ;i++){
            for(int j= (int)res[i].size()-1 ; j>0; j--)
            {
         //       printf("%d ",res[i][j]>=netNode?(res[i][j]-netNode):res[i][j]);
                sprintf(c,"%d ",res[i][j]>=netNode?(res[i][j]-netNode):res[i][j]);
                str +=c;
            }
           // printf("%d\n",outFlow[i]);
            sprintf(c, "%d\n",outFlow[i]);
            str +=c;
        }
    }



    void MCMF:: update_flow(int minflow , int fa[]){
        //从s开始
        int cnt = outCnt;
        // printf("cnt=%d: minflow=%d\n",cnt, minflow);
        int u=t;
        while(u!=s){
            
            //流量更新，减掉minflow
            //  if(p[u]==s) printf("p_edeg=%d ,s.flow=%d ",p_edge[u], edges[p_edge[u]].flow);
            //edges[p_edge[u]].flow -=minflow;
            E[p_edge[u]].flow -=minflow;
            if(u != s) res[cnt].push_back(u);
            //   if(p[u]==s) printf("^^^^ , s.flow=%d \n",edges[p_edge[u]].flow);
            u = fa[u];
        }
        outFlow.push_back(minflow);
        
        
        find_flow += minflow;
        outCnt++;
    }


//cur表示当前点
void MCMF:: dfs_findPath(int cur, int minflow){
    
    if(ok) return;
    
    //从s正方向找
    if(cur == t){
        
        //更新整条路上的 流量
        update_flow(minflow, fa);
        ok=1;
        return ;
    }
    
    for (int i = net[cur] , v; v = E[i].v, i != -1; i = E[i].next){
        
        if(E[i].flow<=0) continue; //反向边不搜索
        if(find_vis[v]) continue;
        
        fa[v] = cur;
        p_edge[v] = i;
        find_vis[v] = 1;
        
        if(cur ==0 && E[i].v==119)
        {
            //printf("cur=%d , i= %d , E[i].flow =%d\n",cur, i ,E[i].flow);
         //   while (1) {
          //      printf("cao");
          //  }
        }
        dfs_findPath(v , min(minflow, E[i].flow));
        
        find_vis[v] = 0;
        
        if(ok) return;
        
    }
    
}




    void MCMF::search_path(){

        printf("search start\n");
        outCnt = 0;
        for(int i=0; i<1500;i++)
        {
            res[i].clear();
            outFlow.clear();
        }
        find_flow = 0;
        fa[s] = -1;
        while(find_flow< k ){
            
            // for(int i=0; i<(int)G[st].size(); i++){
            for (int i = net[s] , v; v = E[i].v, i != -1; i = E[i].next){
                //puts("ss");
                // printf("i=%d, e.flow =%d e.from =%d, e.to =%d\n",i, e.flow , e.flow , e.to);
                fa[v] = s;
                p_edge[v] = i;
                
                if(E[i].flow>0) {  //搜路径 ,初始为服务点
                    ok=0;
                    memset(find_vis, 0, sizeof(find_vis));
                    find_vis[s] = 1;
                    dfs_findPath(v , INF);
                  //   printf("find_flow =%d , k=%d\n",find_flow, k);
                }
                
                
            }
        }
    }



// -------------------------------------------------------------


//当前方案的最大流 ,hua为实参
int maxFlow(struct MCMF& hua){
    
    hua.outCnt = 0;
    int res = hua.zkw(hua.s, hua.t, hua.k);
  //  printf("res =%d\n", res);
    return res;
}





//获得服务点 并返回最小费用 ,hua为形参 ,huaReal为实参
int getServersAndGetMincost(vector<int>& serverList, int& serverNum ,struct MCMF hua ,struct MCMF& huaReal , string& str,  int printPath){
    int len,ret;
    
    len = (int)serverList.size();
    
    hua.numOfserver=len;
    //建立 源点 与 服务点的连线
    for(int i=0; i< len ; i++)
    {
        //源点与服务点连
        hua.AddEdge(hua.s, serverList[i], INF, 0);
        //消费点与汇点相连
     //   int Ccap = hua.edges[hua.consumeEdge[serverList[i]]].cap;
     //   hua.AddEdge(serverList[i] + hua.netNode, hua.t, Ccap, 0);
    }
    
    
    
    //清空之前数据
    huaReal.outFlow.clear();
    for(int i=0; i<hua.outCnt; i++)
        hua.res[i].clear();
    
    
    //对当前结果 判优，并进行保存
    ret = maxFlow(hua);

    if(printPath)
    {
        hua.search_path();
        hua.OutputPath(str);
       // printf("sss\n");
    }
    
    return ret;
}



//添加了消费点列表
//获得服务点 并返回最小费用 ,hua为形参 ,huaReal为实参
int newGetServersAndGetMincost(vector<int>& serverList,vector<int> consumeList,int& serverNum,MCMF hua,MCMF& huaReal)
{
    int len,ret;
    len = serverNum;
    
    hua.numOfserver= (int)serverList.size();
    //建立 源点 与 服务点的连线
    for(int i=0; i< serverList.size() ; i++)
    {
        //源点与服务点连
        hua.AddEdge(hua.s, serverList[i], INF, 0);
        
    }
    
    //记录t点的总流量
    int tot_k=0;
    //消费点与汇点相连
    for(int i=0; i<consumeList.size(); i++){
       // int Ccap = hua.edges[hua.consumeEdge[consumeList[i]]].cap;
        int Ccap = hua.E[hua.consumeEdge[consumeList[i]]].cap;
        tot_k +=Ccap;
      //  printf("cL: %d , %d\n",consumeList[i], Ccap);
        hua.AddEdge(consumeList[i] + hua.netNode, hua.t, Ccap, 0);
    }
    hua.k=tot_k;
    
    //清空之前数据
    huaReal.outFlow.clear();
    for(int i=0; i<hua.outCnt; i++)
        hua.res[i].clear();
    
    
    //对当前结果 判优，并进行保存
    ret = maxFlow(hua);
    
    
    if(ret>0){ //正确解
        if(huaReal.costNow<0 || huaReal.costNow>ret){
            // printf("^^^^^^ huaReal.costNow =%d\n",huaReal.costNow);
            huaReal.outFlow.assign(hua.outFlow.begin(), hua.outFlow.end());
            for(int i=0; i<hua.outCnt; i++)
                huaReal.res[i].assign(hua.res[i].begin(), hua.res[i].end());
            huaReal.outCnt = hua.outCnt;
            huaReal.costNow = hua.costNow;
        }
    }
    
    //  huaReal.OutputPath(str);
    
    return ret;
}



int newGetServersAndGetMincost2(vector<int>& serverList,vector<int> consumeList,int& serverNum,MCMF& hua , string& str)
{
    int len,ret;
    len = (int)serverList.size();
    
    hua.numOfserver= (int)serverList.size();
    //建立 源点 与 服务点的连线
    for(int i=0; i< serverList.size() ; i++)
    {
        //源点与服务点连
        hua.AddEdge(hua.s, serverList[i], INF, 0);
        
    }
    
    //记录t点的总流量
    int tot_k=0;
    //消费点与汇点相连
    for(int i=0; i<consumeList.size(); i++){
        //int Ccap = hua.edges[hua.consumeEdge[consumeList[i]]].cap;
        int Ccap = hua.E[hua.consumeEdge[consumeList[i]]].cap;
        tot_k +=Ccap;
        //  printf("cL: %d , %d\n",consumeList[i], Ccap);
        hua.AddEdge(consumeList[i] + hua.netNode, hua.t, Ccap, 0);
    }
    hua.k=tot_k;
    
    //清空之前数据
    for(int i=0; i<hua.outCnt; i++)
        hua.res[i].clear();
    
    
    //对当前结果 判优，并进行保存
    ret = maxFlow(hua);

    return ret;
}










